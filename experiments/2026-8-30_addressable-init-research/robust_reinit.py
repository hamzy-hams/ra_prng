import json, math, subprocess, statistics
from pathlib import Path

HERE = Path(".").resolve()
REINIT_SWEEP_TARGET_WORDS = 20_000_000
REINIT_SWEEP_K = [1, 2, 4, 8, 16, 24, 32, 48, 64, 80, 96, 128, 192, 256,
                   512, 1024, 4096, 65536]
REPEATS = 3

def parse_rows(stdout, row_key):
    rows = []
    for line in stdout.strip().splitlines():
        parts = line.split()
        if not parts or parts[0] != row_key:
            continue
        d = {}
        i = 0
        while i < len(parts):
            if parts[i] == row_key:
                d[row_key] = int(parts[i+1]); i += 2; continue
            key, val = parts[i], parts[i+1]
            try: d[key] = int(val)
            except ValueError: d[key] = float(val)
            i += 2
        rows.append(d)
    return rows

def run_reinit_sweep(bin_path, variant):
    args = [str(bin_path), "reinit-sweep", variant, str(REINIT_SWEEP_TARGET_WORDS)] + [str(k) for k in REINIT_SWEEP_K]
    result = subprocess.run(args, capture_output=True, text=True, check=True)
    return parse_rows(result.stdout, "K")

def find_crossover(rows, n_key, addr_key, philox_key):
    prev = None
    for row in rows:
        ratio = row[addr_key] / row[philox_key]
        if prev is not None:
            prev_ratio = prev[addr_key] / prev[philox_key]
            if prev_ratio >= 1.0 > ratio:
                x0, x1 = math.log(prev[n_key]), math.log(row[n_key])
                y0, y1 = prev_ratio, ratio
                frac = (1.0 - y0) / (y1 - y0)
                return math.exp(x0 + frac * (x1 - x0))
        prev = row
    return None

COMBOS = [
    ("tahap5_bench", "v0_baseline"),
    ("tahap5_bench", "v1_rolv"),
    ("tahap5_bench_zmm512", "v0_baseline"),
    ("tahap5_bench_unroll", "v0_baseline"),
]

for binname, variant in COMBOS:
    label = f"{binname}:{variant}"
    all_runs = [run_reinit_sweep(HERE / binname, variant) for _ in range(REPEATS)]
    median_rows = []
    for idx, k in enumerate(REINIT_SWEEP_K):
        addr_vals = [run[idx]["addr_ns_per_word_steadystate"] for run in all_runs]
        philox_vals = [run[idx]["philox_ns_per_word"] for run in all_runs]
        median_rows.append({
            "K": k,
            "addr_ns_per_word_steadystate": statistics.median(addr_vals),
            "philox_ns_per_word": statistics.median(philox_vals),
        })
    k_star = find_crossover(median_rows, "K", "addr_ns_per_word_steadystate", "philox_ns_per_word")
    print(f"{label:35s} K*(median-of-{REPEATS}) = {k_star:.1f}" if k_star else f"{label}: not found")
    Path(f"tahap5_results_reinit-sweep_median_{binname}_{variant}.json").write_text(
        json.dumps({"rows": median_rows, "k_star_estimate_median": k_star, "repeats": REPEATS}, indent=2))
    for r in median_rows:
        print(f"  K={r['K']:6d}  addr={r['addr_ns_per_word_steadystate']:.3f}  philox={r['philox_ns_per_word']:.3f}  ratio={r['addr_ns_per_word_steadystate']/r['philox_ns_per_word']:.3f}")
