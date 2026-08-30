#!/usr/bin/env python3
"""Correlate the permute-vs-inject periodicity direction (HANDOVER_PHASE3.md
point #1) with n and w across the toy-model grid. Reads every
toy_sweep_n{n}_w{w}_mech{0,1}*.csv in this folder; when both a base and a
_300seed variant exist for the same (n, w, mech), the _300seed one wins
(more samples). Prints a markdown summary table -- no CSVs are modified.
"""
import csv
import glob
import re
from pathlib import Path

HERE = Path(__file__).parent
PATTERN = re.compile(r"toy_sweep_n(\d+)_w(\d+)_mech([01])(_300seed)?\.csv$")


def load_means():
    # key: (n, w, mech) -> (path, is_300seed, mean_lambda, n_seeds)
    best = {}
    for path in glob.glob(str(HERE / "toy_sweep_n*_w*_mech*.csv")):
        m = PATTERN.search(path)
        if not m:
            continue
        n, w, mech, suffix = int(m.group(1)), int(m.group(2)), int(m.group(3)), bool(m.group(4))
        with open(path) as f:
            rows = list(csv.DictReader(f))
        if not rows:
            continue
        lambdas = [int(r["lambda"]) for r in rows]
        mean_l = sum(lambdas) / len(lambdas)
        key = (n, w, mech)
        if key not in best or (suffix and not best[key][1]):
            best[key] = (path, suffix, mean_l, len(lambdas))
    return best


def main():
    means = load_means()
    configs = sorted({(n, w) for (n, w, _mech) in means})

    print("| n | w | seeds (permute/inject) | lambda permute | lambda inject | rasio inject/permute | arah |")
    print("|---|---|---|---|---|---|---|")
    for n, w in configs:
        p = means.get((n, w, 0))
        i = means.get((n, w, 1))
        p_str = f"{p[2]:,.0f}" if p else "n/a"
        i_str = f"{i[2]:,.0f}" if i else "n/a (intractable)"
        seeds_str = f"{p[3] if p else '-'}/{i[3] if i else '-'}"
        if p and i:
            ratio = i[2] / p[2]
            arah = "inject LEBIH PANJANG" if ratio > 1.05 else ("inject LEBIH PENDEK" if ratio < 0.95 else "~sama")
            ratio_str = f"{ratio:,.2f}x"
        else:
            ratio_str = "n/a"
            arah = "tidak bisa dibandingkan (salah satu intractable)"
        print(f"| {n} | {w} | {seeds_str} | {p_str} | {i_str} | {ratio_str} | {arah} |")

    print()
    print("Config yang dicoba tapi intractable dalam budget sesi (tidak ada CSV sama sekali):")
    for label in ("n=16,w=4", "n=4,w=16"):
        print(f"- {label}: permute dan inject sama-sama timeout (rc=124) pada probe 3-seed/180s.")
    print("- n=8,w=8 inject: timeout (rc=124) bahkan untuk 1 seed/600s -- permute (1 seed) selesai dengan"
          " lambda=3,109,376 dalam waktu yang sama; lihat toy_probe_n8w8_mech1_1seed.csv (kosong/timeout).")


if __name__ == "__main__":
    main()
