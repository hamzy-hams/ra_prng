#!/usr/bin/env python3
"""Orchestrator for benchmark_all.c -- see HANDOVER.md.

Gate sequencing (HANDOVER.md section 5):
  1. verify   -- every KAT block must PASS. Fatal for the whole run if not
                 (the algorithm itself may be wrong).
  2. crosscheck -- byte-for-byte/ported candidates get --stream diffed
                 against their original source binaries (built on the fly).
                 A failure here is scoped to that ONE candidate: it is
                 marked FAILED_VALIDATION and excluded from benchmarking,
                 the rest of the run continues.
  3. reinit-sweep -- only candidates that passed all applicable gates.
  4. dev_urandom -- flat-throughput only, no gates apply.

Regression (session decision): OLS fit call_ns(K) = a + b*K per candidate
from the sweep's (K, call_ns) pairs. a = fixed seeding-cost estimate (ns),
b = steady-state ns/word. Same formula for every candidate, uniformly.
"""
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
SCRATCH = os.environ.get("SCRATCH_DIR", "/tmp/benchmark_all_scratch")
os.makedirs(SCRATCH, exist_ok=True)

BENCH_BIN = os.path.join(HERE, "benchmark_all")

SWEEP_K = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80, 96, 128, 192, 255,
           256, 512, 1024, 4096, 65536, 1000000]

# max_rng per core (None == uncapped), mirrors CORES[] in benchmark_all.c
MAX_RNG = {
    "paperorig": None, "wiredv2": None, "addrcont": None, "singleblock": 255,
    "philox": None, "xoshiro256": None, "pcg32": None, "chacha20": None,
    "mt19937": None, "splitmix64": None,
}

# Known reference MB/s numbers from HANDOVER.md section 4, for the sanity check.
KNOWN_MBPS = {
    "xoshiro256": 3574.0, "pcg32": 3065.0, "philox": 1304.0,
    "paperorig": 745.6, "chacha20": 555.5,
}

CROSSCHECK_KEYS = [0, 0xFFFFFFFF, 0xDEADBEEF, 1, 2654435769]
CROSSCHECK_LENGTHS = [1, 254, 255, 1000]


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def run_raw(cmd, **kw):
    """Like run(), but keeps stdout as raw bytes -- for --stream binary output."""
    return subprocess.run(cmd, capture_output=True, **kw)


def build_bench_all():
    r = run(["gcc", "-O3", "-march=native", "-std=gnu17", "-include", "stdalign.h",
             os.path.join(HERE, "benchmark_all.c"), "-o", BENCH_BIN, "-Wall", "-Wextra"])
    if r.returncode != 0:
        print("FATAL: benchmark_all.c failed to build:\n" + r.stderr, file=sys.stderr)
        sys.exit(1)


def run_verify():
    r = run([BENCH_BIN, "verify"])
    print(r.stdout)
    if r.returncode != 0:
        print("FATAL: verify (KAT) failed -- refusing to benchmark ANY candidate.\n" + r.stderr,
              file=sys.stderr)
        sys.exit(1)


def run_validate_singleblock():
    r = run([BENCH_BIN, "validate"])
    print(r.stdout)
    if r.returncode != 0:
        print("FATAL: validate (singleblock vs addrcont exhaustive check) failed.\n" + r.stderr,
              file=sys.stderr)
        sys.exit(1)


def build_crosscheck_originals():
    paths = {}
    def build(src, name):
        out = os.path.join(SCRATCH, name)
        r = run(["gcc", "-O3", "-march=native", src, "-o", out])
        if r.returncode != 0:
            print(f"WARNING: could not build crosscheck original {name}: {r.stderr}", file=sys.stderr)
            return None
        return out
    paths["paperorig"] = build(os.path.join(REPO_ROOT, "src/ra_prng2/c/ra_prng2.c"), "ra_prng2_orig")
    paths["wiredv2"] = build(os.path.join(REPO_ROOT, "experiments/2026-8-27_operand-position-search/winner_wired_v2.c"), "winner_wired_v2_orig")
    tahap6 = os.path.join(REPO_ROOT, "experiments/2026-8-30_addressable-init-research/tahap6_bench")
    paths["addrcont"] = tahap6 if os.path.exists(tahap6) else None
    return paths


def stream_original(path, core, key, n):
    """paperorig/wiredv2 originals: `bin --stream key n`. addrcont original
    (tahap6_bench): `bin --stream baseline key n`."""
    if core == "addrcont":
        r = run_raw([path, "--stream", "baseline", str(key), str(n)])
    else:
        r = run_raw([path, "--stream", str(key), str(n)])
    return r.stdout


def stream_new(core, key, n):
    r = run_raw([BENCH_BIN, "--stream", core, str(key), str(n)])
    return r.stdout


def run_crosscheck(originals):
    failed = set()
    for core in ("paperorig", "wiredv2", "addrcont"):
        path = originals.get(core)
        if not path:
            print(f"crosscheck: SKIPPING {core} (original binary unavailable)")
            failed.add(core)
            continue
        ok = True
        for key in CROSSCHECK_KEYS:
            for n in CROSSCHECK_LENGTHS:
                a = stream_original(path, core, key, n)
                b = stream_new(core, key, n)
                if a != b:
                    print(f"crosscheck MISMATCH: core={core} key={key} n={n}")
                    ok = False
        print(f"crosscheck {core}: {'PASS' if ok else 'FAIL'}")
        if not ok:
            failed.add(core)
    return failed


def ols_fit(rows):
    ks = [r["K"] for r in rows]
    ts = [r["call_ns"] for r in rows]
    n = len(ks)
    if n < 2:
        return None
    mk = sum(ks) / n
    mt = sum(ts) / n
    denom = sum((k - mk) ** 2 for k in ks)
    if denom == 0:
        return None
    b = sum((k - mk) * (t - mt) for k, t in zip(ks, ts)) / denom
    a = mt - b * mk
    return {"a_ns": a, "b_ns_per_word": b}


def run_reinit_sweep(core, cycles_target_words=2_000_000):
    max_rng = MAX_RNG[core]
    ks = [k for k in SWEEP_K if max_rng is None or k <= max_rng]
    args = [BENCH_BIN, "reinit-sweep", core, str(cycles_target_words)] + [str(k) for k in ks]
    r = run(args)
    rows = []
    for line in r.stdout.splitlines():
        # core X K k cycles c call_ns v ns_per_word_steadystate w checksum s
        parts = line.split()
        d = {parts[i]: parts[i + 1] for i in range(0, len(parts) - 1, 2)}
        try:
            rows.append({
                "K": int(d["K"]),
                "cycles": int(d["cycles"]),
                "call_ns": float(d["call_ns"]),
                "ns_per_word": float(d["ns_per_word_steadystate"]),
            })
        except (KeyError, ValueError):
            continue
    return rows


def run_urandom_flat(n=50_000_000):
    r = run([BENCH_BIN, "urandom-flat", str(n)])
    for line in r.stdout.splitlines():
        parts = line.split()
        d = {parts[i]: parts[i + 1] for i in range(0, len(parts) - 1, 2)}
        if "mb_per_s" in d:
            return {"ns_per_word": float(d["ns_per_word"]), "mb_per_s": float(d["mb_per_s"])}
    return None


def main():
    build_bench_all()
    run_verify()
    run_validate_singleblock()
    originals = build_crosscheck_originals()
    failed_validation = run_crosscheck(originals)

    results = {}
    for core in MAX_RNG:
        if core in failed_validation:
            results[core] = {"status": "FAILED_VALIDATION"}
            print(f"{core}: FAILED_VALIDATION -- excluded from benchmarking")
            continue
        print(f"benchmarking {core} ...")
        rows = run_reinit_sweep(core)
        fit = ols_fit(rows)
        results[core] = {"status": "OK", "sweep": rows, "regression": fit}

    results["dev_urandom"] = {"status": "OK", "flat": run_urandom_flat()}

    out_path = os.path.join(HERE, "benchmark_all_results.json")
    with open(out_path, "w") as f:
        json.dump(results, f, indent=2)
    print(f"\nwrote {out_path}")

    print("\n=== Seeding-cost summary (OLS fit: call_ns(K) = a + b*K) ===")
    print(f"{'core':<14}{'a_ns (seed cost)':>18}{'b_ns/word':>14}")
    for core, data in results.items():
        if core == "dev_urandom" or data.get("status") != "OK":
            continue
        fit = data.get("regression")
        if fit:
            print(f"{core:<14}{fit['a_ns']:>18.1f}{fit['b_ns_per_word']:>14.3f}")

    print("\n=== MB/s sanity check (largest uncapped K point) vs HANDOVER.md reference ===")
    for core, known in KNOWN_MBPS.items():
        data = results.get(core, {})
        rows = data.get("sweep") or []
        if not rows:
            continue
        largest = max(rows, key=lambda r: r["K"])
        mbps = 4.0 / largest["ns_per_word"] * 1e9 / 1e6
        ratio = mbps / known if known else float("nan")
        flag = "" if 0.4 < ratio < 2.5 else "  <-- CHECK FOR PORTING BUG"
        print(f"{core:<14}{mbps:>10.1f} MB/s   (reference {known:.1f} MB/s, ratio {ratio:.2f}){flag}")

    if results.get("dev_urandom", {}).get("flat"):
        f = results["dev_urandom"]["flat"]
        print(f"{'dev_urandom':<14}{f['mb_per_s']:>10.1f} MB/s   (reference 389.6 MB/s, ratio {f['mb_per_s']/389.6:.2f})")


if __name__ == "__main__":
    main()
