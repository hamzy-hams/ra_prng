#!/usr/bin/env python3
"""tools/battery/practrand_gate.py -- generic Tier 1 PractRand gate.

Generalized port of the 3-stage cheapest-first dual-ordering gate in
../../experiments/2026-9-5_orbit-d0-raw-cons-fix/tier1_gate_d0raw.py
(the most refined template found across experiments/2026-9-{4,5}_orbit-*/),
parametrized over binary path, core, K values, orderings, stage sizes and
base keys instead of being hand-copied per candidate.

Test lanes:
  - one lane per K in --k (default "1,255"): multistream, reinit every K
    words, tested at EVERY ordering in --orderings (default "weyl,hashed").
    A lane survives a stage only if ALL its orderings are clean_to_target.
  - one "continuous" lane if --continuous is given: single key, no reinit
    (--single mode). Orbit only -- ra_core_singleblock hard-aborts above
    rng=255, so --continuous is refused with a warning when --core singleblock.

The K=1 + hashed-ordering lane is the one that has repeatedly caught the
structural defect under active research (see
../../experiments/2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md and
../../experiments/2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md) --
--orderings defaults to BOTH weyl and hashed specifically so this axis is
never silently skipped.

Stages (cheapest-first, early-kill on hard FAIL, no retry on FAIL --
feedback_no_reseed_on_fail):
  A (screen):  base_key #1, up to --sizes[0]  (default 256M)
  B (confirm): Stage-A survivors, base_key #1, up to --sizes[1] (default 2G)
  C (repro):   Stage-B survivors, base_key #2, up to --sizes[2] (default 2G)

Usage:
    python tools/battery/practrand_gate.py --binary ./candidate --core orbit \\
        --k 1,255 --continuous --outdir .
    python tools/battery/practrand_gate.py --binary ./candidate --core orbit --quick

Exit code 0 if at least the requested lanes were all CLEAN through Stage C,
1 otherwise, 2 on usage/runtime error.
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import time
import warnings
from pathlib import Path

DEFAULT_PRACTRAND_BIN = Path.home() / "Documents/research/PractRand/RNG_test"

# addr_hi derivation (2026-09-11, HANDOVER.md state-size-pareto-frontier
# Tahap D fix): driver_main_template.c's --single/--stream/--stream-hashed
# gained a mandatory addr_hi argument on 2026-09-06 (Kandidat D 64-bit
# address widening) -- this module predates that and called the binary
# without it, which the driver now rejects as a usage error. Fixed by
# deriving a base_addr_hi from base_key via a fixed XOR constant (distinct
# from base_key, matching the "addr_hi must be varied INDEPENDENTLY from
# key" requirement in driver_main_template.c's own --stream/--stream-hashed
# docstring -- --stream then steps key/addr_hi by different Weyl deltas per
# block and --stream-hashed re-seeds two independent splitmix32 chains from
# different XOR constants, so this single distinguishing XOR is enough for
# the two to diverge from the very first block).
ADDR_HI_XOR = 0x5EEDA55D


def derive_addr_hi(base_key: int) -> int:
    return (base_key ^ ADDR_HI_XOR) & 0xFFFFFFFF


DEFAULT_K = [1, 255]
DEFAULT_ORDERINGS = ["weyl", "hashed"]
DEFAULT_SIZES = ["256M", "2G", "2G"]
QUICK_SIZES = ["64M", "256M", "256M"]
DEFAULT_BASE_KEYS = [0x12345, 0xABCDEF01]
STAGE_TAGS = ["A", "B", "C"]

FAIL_RE = re.compile(r"\bFAIL\b|very suspicious")
SOFT_ANOMALY_RE = re.compile(r"\bmildly suspicious\b|\bsuspicious\b|\bunusual\b")
LENGTH_RE = re.compile(r"length=\s*([^,]+),")
BYTES_EXP_RE = re.compile(r"\(2\^(\d+) bytes\)")


def parse_size(s: str) -> int:
    s = s.strip().upper()
    m = re.match(r"^(\d+)(K|M|G)B?$", s)
    if not m:
        raise ValueError(f"cannot parse size: {s!r}")
    n, unit = int(m.group(1)), m.group(2)
    mult = {"K": 1024, "M": 1024 ** 2, "G": 1024 ** 3}[unit]
    return n * mult


def iter_checkpoints(stream):
    """Yield one full checkpoint block (str) at a time from RNG_test's
    stdout, as lines arrive (live, not buffered at the end)."""
    current = []
    for raw in stream:
        line = raw.decode(errors="replace") if isinstance(raw, bytes) else raw
        if line.strip() == "":
            if current:
                yield "".join(current)
                current = []
            continue
        current.append(line)
    if current:
        yield "".join(current)


class Lane:
    """One test lane: either a multistream (K, ordering) pair or the
    continuous (--single) mode. A lane's "sub-checks" are the orderings it
    must clear at a given stage (continuous has exactly one, K=None)."""

    def __init__(self, lane_id: str, K: int | None, orderings: list[str | None]):
        self.lane_id = lane_id
        self.K = K
        self.orderings = orderings

    def gen_cmd(self, binary: Path, base_key: int, n_words: int, ordering: str | None,
                core_prefix: str | None = None) -> list[str]:
        # Resolve to an absolute path before exec: str(Path("./binary")) strips
        # the "./" (becomes "binary"), and subprocess with no "/" in the name
        # searches $PATH instead of cwd -- a bare relative binary would then
        # FileNotFoundError even though it exists right there.
        binary_str = str(Path(binary).resolve())
        prefix = [binary_str, core_prefix] if core_prefix else [binary_str]
        base_addr_hi = derive_addr_hi(base_key)
        if self.K is None:  # continuous
            return [*prefix, "--single", str(base_key), str(base_addr_hi), str(n_words)]
        stream_flag = "--stream" if ordering == "weyl" else "--stream-hashed"
        return [*prefix, stream_flag, str(base_key), str(base_addr_hi), str(n_words), str(self.K)]


def build_lanes(k_values: list[int], orderings: list[str], continuous: bool, core: str) -> list[Lane]:
    lanes = [Lane(f"K{k}", k, list(orderings)) for k in k_values]
    if continuous:
        if core == "singleblock":
            warnings.warn(
                "--continuous ignored for --core singleblock: "
                "ra_core_singleblock hard-aborts above rng=255, "
                "continuous (--single) mode is orbit-only."
            )
        else:
            lanes.append(Lane("continuous", None, [None]))
    return lanes


def run_one(cid: str, binary: Path, lane: Lane, ordering: str | None, base_key: int,
            practrand_bin: Path, tlmin: str, tlmax: str, stage_tag: str,
            logs_dir: Path, core_prefix: str | None = None) -> dict:
    target_bytes = parse_size(tlmax)
    n_words = target_bytes // 4
    # +1 word safety margin for the continuous (--single) lane only
    # (2026-09-11, HANDOVER.md state-size-pareto-frontier Tahap D): discovered
    # ra_core_orbit (production src/ra_core/c/ra_core.c, reproduces identically
    # in the parametrized core -- pre-existing, not introduced by this
    # experiment) writes exactly ONE WORD FEWER than requested whenever
    # rng == k*(RA_N-1)+1 for some k>=1 (i.e. rng lands exactly 1 past a
    # reseed-cycle boundary): ra_permutation_cycle_orbit's per-word `*count`
    # decrement and ra_core_orbit's post-cycle `if (count<=1) return` both
    # treat count==1 as "already done" without that last word ever having
    # been written. For RA_N=16 this silently collides with EVERY power-of-2
    # PractRand target size (16-1=15 divides 2^4-1, so 2^(4k) mod 15 == 1),
    # which was truncating the continuous lane's stdout exactly 4 bytes short
    # and killing PractRand mid-run with "error reading standard input" (a
    # harness EOF artifact, NOT a statistical anomaly). Requesting one extra
    # word costs nothing and makes `--single` always emit >= target_bytes
    # regardless of whether this boundary is hit. The underlying ra_core_orbit
    # defect itself is left untouched here (out of scope, unrelated to state-
    # size parameterization) and reported in this experiment's RESULTS.md.
    if lane.K is None:
        n_words += 1
    cmd = lane.gen_cmd(binary, base_key, n_words, ordering, core_prefix)
    log_path = logs_dir / f"{cid}_{ordering or 'single'}_{base_key:#010x}_{stage_tag}.log"

    t0 = time.time()
    gen = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    test = subprocess.Popen(
        [str(Path(practrand_bin).resolve()), "stdin32", "-tlmin", tlmin, "-tlmax", tlmax, "-multithreaded"],
        stdin=gen.stdout, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, bufsize=1,
    )
    gen.stdout.close()

    checkpoints = []
    killed_early = False
    with log_path.open("w") as logf:
        for block in iter_checkpoints(test.stdout):
            logf.write(block + "\n\n")
            logf.flush()
            m = LENGTH_RE.search(block)
            length_str = m.group(1).strip() if m else "?"
            bm = BYTES_EXP_RE.search(block)
            block_bytes = (2 ** int(bm.group(1))) if bm else 0
            hard_fail = bool(FAIL_RE.search(block))
            soft_hits = sorted(set(SOFT_ANOMALY_RE.findall(block)))
            checkpoints.append({
                "length": length_str, "bytes": block_bytes,
                "hard_fail": hard_fail, "soft_anomaly_kinds": soft_hits,
            })
            is_final = block_bytes >= target_bytes
            if hard_fail and not is_final:
                test.terminate()
                gen.terminate()
                killed_early = True
                logf.write(f"\n[EARLY KILL after {length_str}: hard FAIL detected]\n")
                break

    test.stdout.close()
    if test.poll() is None:
        test.wait(timeout=10)
    if gen.poll() is None:
        gen.wait(timeout=10)
    elapsed = time.time() - t0

    last = checkpoints[-1] if checkpoints else {
        "length": "?", "bytes": 0, "hard_fail": True, "soft_anomaly_kinds": [],
    }
    result = {
        "id": cid, "K": lane.K, "ordering": ordering, "base_key": base_key,
        "stage": stage_tag, "checkpoints": checkpoints, "killed_early": killed_early,
        "final_length": last["length"], "final_hard_fail": last["hard_fail"],
        "clean_to_target": (not killed_early) and (not last["hard_fail"]) and last["bytes"] >= target_bytes,
        "elapsed_s": elapsed,
    }
    verdict = "FAIL" if (killed_early or last["hard_fail"]) else "CLEAN"
    print(f"  [{stage_tag}] {cid} {ordering or 'single'} base={base_key:#x}: "
          f"{verdict} @ {last['length']} ({elapsed:.0f}s)", flush=True)
    return result


def run_stage(lanes: list[Lane], binary: Path, base_key: int, practrand_bin: Path,
              tlmin: str, tlmax: str, stage_tag: str, logs_dir: Path,
              core_prefix: str | None = None) -> tuple[list[Lane], list[dict]]:
    survivors, all_results = [], []
    for lane in lanes:
        clean = True
        for ordering in lane.orderings:
            r = run_one(lane.lane_id, binary, lane, ordering, base_key,
                        practrand_bin, tlmin, tlmax, stage_tag, logs_dir, core_prefix)
            all_results.append(r)
            if not r["clean_to_target"]:
                clean = False
        if clean:
            survivors.append(lane)
    return survivors, all_results


def run(binary: Path, core: str, k_values: list[int], orderings: list[str],
        continuous: bool, sizes: list[str], base_keys: list[int],
        practrand_bin: Path, outdir: Path, binary_mode: str = "single") -> dict:
    outdir.mkdir(parents=True, exist_ok=True)
    logs_dir = outdir / "practrand_logs"
    logs_dir.mkdir(exist_ok=True)
    core_prefix = core if binary_mode == "dual" else None

    lanes = build_lanes(k_values, orderings, continuous, core)
    if not lanes:
        raise RuntimeError("no test lanes (empty --k and no usable --continuous)")

    stage_sizes = [(sizes[0], sizes[0]), (sizes[0], sizes[1]), (sizes[1], sizes[2])]
    # Stage A: tlmin=tlmax=sizes[0] would over-report; use a smaller tlmin
    # floor (1/4 of target) so PractRand's own checkpoint cadence still
    # shows intermediate progress, matching tier1_gate_d0raw.py's 64MB/256MB
    # (roughly n/4) precedent.
    def tlmin_for(target: str) -> str:
        n = parse_size(target)
        quarter = max(n // 4, 1024 * 1024)
        for suffix, mult in (("G", 1024**3), ("M", 1024**2), ("K", 1024)):
            if quarter >= mult:
                return f"{quarter // mult}{suffix}"
        return f"{quarter}"

    all_stage_results = {}
    survivors = lanes
    base_key_a, base_key_b = base_keys[0], base_keys[1]
    stage_base_keys = [base_key_a, base_key_a, base_key_b]

    for stage_tag, target, base_key in zip(STAGE_TAGS, sizes, stage_base_keys):
        if not survivors:
            break
        print(f"=== Stage {stage_tag}: {len(survivors)} lane(s), "
              f"base_key={base_key:#x}, up to {target} ===", flush=True)
        tlmin = tlmin_for(target)
        survivors, results = run_stage(survivors, binary, base_key, practrand_bin,
                                        tlmin, target, stage_tag, logs_dir, core_prefix)
        all_stage_results[stage_tag] = results
        print(f"Stage {stage_tag} done: {len(survivors)} lane(s) survived\n", flush=True)

    clean_ids = [lane.lane_id for lane in survivors] if len(all_stage_results) == len(sizes) else []
    summary = {
        "binary": str(binary), "core": core, "binary_mode": binary_mode,
        "k_values": k_values,
        "orderings": orderings, "continuous": continuous, "sizes": sizes,
        "base_keys": [base_key_a, base_key_b],
        "all_lanes": [lane.lane_id for lane in lanes],
        "clean_dual_survivors": clean_ids,
        "stage_results": all_stage_results,
    }
    (outdir / "practrand_summary.json").write_text(json.dumps(summary, indent=2))
    return summary


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0],
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--binary", required=True, type=Path)
    ap.add_argument("--core", required=True, choices=["orbit", "singleblock"])
    ap.add_argument("--k", default="1,255", help="comma-separated K values (default 1,255)")
    ap.add_argument("--orderings", default="weyl,hashed",
                     help="comma-separated orderings to test (default weyl,hashed -- "
                          "do not narrow to weyl-only, the researched defect is hashed-only)")
    ap.add_argument("--continuous", action="store_true",
                     help="also test the continuous (--single) lane; orbit only")
    ap.add_argument("--sizes", default=None, help="Stage A,B,C target sizes, e.g. 256M,2G,2G")
    ap.add_argument("--quick", action="store_true",
                     help=f"use smaller stage sizes for fast iteration ({','.join(QUICK_SIZES)})")
    ap.add_argument("--base-keys", default=None,
                     help="two base keys A,B (hex or decimal), e.g. 0x12345,0xABCDEF01")
    ap.add_argument("--practrand-bin", type=Path, default=DEFAULT_PRACTRAND_BIN)
    ap.add_argument("--outdir", type=Path, default=Path("."))
    ap.add_argument("--binary-mode", choices=["single", "dual"], default="single",
                     help="'dual' prefixes the binary's argv with --core's value "
                          "(driver_main_template.c's CORE_INIT/CORE_TRANSFORM "
                          "convention); 'single' (default) matches the plain "
                          "CORE_FN convention, unchanged.")
    args = ap.parse_args(argv)

    if not args.binary.exists():
        print(f"error: binary not found: {args.binary}", file=sys.stderr)
        return 2
    if not args.practrand_bin.exists():
        print(f"error: PractRand RNG_test not found: {args.practrand_bin}", file=sys.stderr)
        return 2

    k_values = [int(x) for x in args.k.split(",")] if args.k else []
    orderings = [x.strip() for x in args.orderings.split(",")] if args.orderings else []
    sizes = args.sizes.split(",") if args.sizes else (QUICK_SIZES if args.quick else DEFAULT_SIZES)
    if len(sizes) != 3:
        print("error: --sizes must have exactly 3 comma-separated values", file=sys.stderr)
        return 2
    base_keys = ([int(x, 0) for x in args.base_keys.split(",")] if args.base_keys
                 else DEFAULT_BASE_KEYS)
    if len(base_keys) != 2:
        print("error: --base-keys must have exactly 2 comma-separated values", file=sys.stderr)
        return 2

    try:
        summary = run(args.binary, args.core, k_values, orderings, args.continuous,
                      sizes, base_keys, args.practrand_bin, args.outdir, args.binary_mode)
    except (RuntimeError, ValueError) as e:
        print(f"error: {e}", file=sys.stderr)
        return 2

    print("=== TIER 1 DONE ===")
    print(f"Lanes tested: {summary['all_lanes']}")
    print(f"CLEAN through Stage C (both base keys): {summary['clean_dual_survivors']}")
    all_clean = set(summary["clean_dual_survivors"]) == set(summary["all_lanes"])
    return 0 if all_clean else 1


if __name__ == "__main__":
    raise SystemExit(main())
