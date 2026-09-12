#!/usr/bin/env python3
"""tools/battery/run_battery.py -- one-command battery: Tier 0 avalanche +
Tier 1 PractRand (K=1/K=255/continuous, weyl+hashed).

This is the single entry point for validating a candidate edit to
ra_core_orbit / ra_core_singleblock without hand-writing a fresh
tier0_avalanche_*.py + tier1_gate_*.py pair per experiment (see
tools/battery/README.md for the CLI contract the candidate binary must
implement, and driver_main_template.c for the reusable main() that gives
it to you for free).

Flow per core: Tier 0 avalanche first (cheap, seconds). REJECT -> stop that
core's testing immediately, do not spend hours on PractRand for a candidate
that already fails the cheapest check. PASS -> full Tier 1 PractRand across
every requested K, both orderings, plus --continuous if given. No retry on
FAIL anywhere (feedback_no_reseed_on_fail) -- a FAIL is final for that lane.

Two ways to invoke:
  --core orbit|singleblock   single-core CORE_FN convention (default,
                             unchanged from before): one binary, one core.
  --dual-core                driver_main_template.c's CORE_INIT/CORE_TRANSFORM
                             convention: ONE binary implements BOTH cores
                             (`<binary> orbit ...` / `<binary> singleblock
                             ...`). Runs Tier 0+1 for BOTH cores against the
                             SAME binary and prints one consolidated table --
                             the point being you edit your shared transform
                             once and immediately see which (core, lane)
                             combination breaks, without re-running anything
                             by hand. A REJECT/FAIL on one core does NOT stop
                             testing of the other core -- the goal is to see
                             everywhere it fails in one pass, not to bail out
                             at the first problem.

Usage:
    python tools/battery/run_battery.py --binary ./candidate --core orbit \\
        --k 1,255 --continuous
    python tools/battery/run_battery.py --binary ./candidate --core singleblock --quick
    python tools/battery/run_battery.py --binary ./dual_candidate --dual-core \\
        --k 1,255 --continuous --quick

Exit code 0 if everything requested came back clean, 1 if anything
REJECTed/FAILed, 2 on usage/runtime error.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import avalanche  # noqa: E402
import practrand_gate  # noqa: E402


def run_one_core(binary: Path, core: str, args, outdir: Path, binary_mode: str) -> dict:
    """Run Tier 0 (unless --skip-tier0) then Tier 1 for one core. Returns a
    result dict; never raises for a REJECT/FAIL -- only for usage/runtime
    errors (caller decides whether that's fatal)."""
    result = {"core": core, "binary_mode": binary_mode}

    if not args.skip_tier0:
        print(f"=== [{core}] TIER 0: avalanche gate ===", flush=True)
        tier0_core = core if binary_mode == "dual" else None
        tier0_result = avalanche.run(
            binary, args.avalanche_cycle_words, args.avalanche_base_seed,
            core=tier0_core,
        )
        result["tier0"] = tier0_result
        status = "PASS" if tier0_result["passed"] else "REJECT"
        print(f"[{core}] Tier 0: {status} (mean_fraction={tier0_result['overall_mean_hamming_fraction']:.4f} "
              f"min_bit_fraction={tier0_result['min_bit_fraction']:.4f})\n")
        if not tier0_result["passed"]:
            result["verdict"] = "REJECTED at Tier 0"
            result["tier1"] = None
            return result
    else:
        print(f"=== [{core}] TIER 0: skipped (--skip-tier0) ===\n")
        result["tier0"] = None

    print(f"=== [{core}] TIER 1: PractRand dual-ordering gate ===", flush=True)
    k_values = [int(x) for x in args.k.split(",")] if args.k else []
    orderings = [x.strip() for x in args.orderings.split(",")] if args.orderings else []
    sizes = (args.sizes.split(",") if args.sizes
             else (practrand_gate.QUICK_SIZES if args.quick else practrand_gate.DEFAULT_SIZES))
    base_keys = ([int(x, 0) for x in args.base_keys.split(",")] if args.base_keys
                 else practrand_gate.DEFAULT_BASE_KEYS)

    tier1_result = practrand_gate.run(
        binary, core, k_values, orderings, args.continuous,
        sizes, base_keys, args.practrand_bin, outdir, binary_mode,
    )
    result["tier1"] = tier1_result

    all_lanes = set(tier1_result["all_lanes"])
    clean_lanes = set(tier1_result["clean_dual_survivors"])
    all_clean = all_lanes == clean_lanes
    result["verdict"] = "CLEAN" if all_clean else f"FAILED lanes: {sorted(all_lanes - clean_lanes)}"
    return result


def print_core_summary(core: str, result: dict) -> bool:
    """Print this core's rows in the final table. Returns True if fully clean."""
    tier0 = result.get("tier0")
    tier1 = result.get("tier1")
    if tier0 is not None:
        print(f"  [{core}] Tier 0 avalanche:            {'PASS' if tier0['passed'] else 'REJECT'}")
    if tier1 is None:
        return tier0 is None or tier0["passed"]
    all_lanes = set(tier1["all_lanes"])
    clean_lanes = set(tier1["clean_dual_survivors"])
    for lane in sorted(all_lanes):
        print(f"  [{core}] Tier 1 {lane:<12}:          {'CLEAN' if lane in clean_lanes else 'FAIL'}")
    return all_lanes == clean_lanes


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0],
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--binary", required=True, type=Path)
    core_group = ap.add_mutually_exclusive_group(required=True)
    core_group.add_argument("--core", choices=["orbit", "singleblock"],
                             help="single-core CORE_FN convention: test just this core")
    core_group.add_argument("--dual-core", action="store_true",
                             help="dual-core CORE_INIT/CORE_TRANSFORM convention: "
                                  "test BOTH orbit and singleblock against this one binary")
    ap.add_argument("--k", default="1,255")
    ap.add_argument("--orderings", default="weyl,hashed")
    ap.add_argument("--continuous", action="store_true")
    ap.add_argument("--sizes", default=None)
    ap.add_argument("--quick", action="store_true")
    ap.add_argument("--base-keys", default=None)
    ap.add_argument("--practrand-bin", type=Path, default=practrand_gate.DEFAULT_PRACTRAND_BIN)
    ap.add_argument("--outdir", type=Path, default=Path("."))
    ap.add_argument("--skip-tier0", action="store_true",
                     help="skip the avalanche gate and go straight to PractRand "
                          "(e.g. re-running Tier 1 after Tier 0 already passed)")
    ap.add_argument("--avalanche-cycle-words", type=int, default=avalanche.DEFAULT_CYCLE_WORDS)
    ap.add_argument("--avalanche-base-seed", type=lambda s: int(s, 0),
                     default=avalanche.DEFAULT_BASE_SEED)
    args = ap.parse_args(argv)

    if not args.binary.exists():
        print(f"error: binary not found: {args.binary}", file=sys.stderr)
        return 2

    args.outdir.mkdir(parents=True, exist_ok=True)
    cores = ["orbit", "singleblock"] if args.dual_core else [args.core]
    binary_mode = "dual" if args.dual_core else "single"

    per_core = {}
    try:
        for core in cores:
            # Separate subdirectory per core when dual-core, so orbit's and
            # singleblock's practrand_summary.json/logs don't clobber each other.
            core_outdir = (args.outdir / core) if args.dual_core else args.outdir
            per_core[core] = run_one_core(args.binary, core, args, core_outdir, binary_mode)
    except (RuntimeError, ValueError) as e:
        print(f"error: {e}", file=sys.stderr)
        return 2

    battery_summary = {
        "binary": str(args.binary), "binary_mode": binary_mode,
        "cores": per_core,
    }
    (args.outdir / "battery_summary.json").write_text(json.dumps(battery_summary, indent=2))

    print("\n=== BATTERY SUMMARY ===")
    print(f"binary={args.binary} binary_mode={binary_mode}")
    all_clean = True
    for core in cores:
        clean = print_core_summary(core, per_core[core])
        all_clean = all_clean and clean
    print(f"Verdict: {'CLEAN' if all_clean else 'SEE ABOVE FOR FAILING (core, lane) COMBINATIONS'}")
    print(f"Full summary: {args.outdir / 'battery_summary.json'}")

    return 0 if all_clean else 1


if __name__ == "__main__":
    raise SystemExit(main())
