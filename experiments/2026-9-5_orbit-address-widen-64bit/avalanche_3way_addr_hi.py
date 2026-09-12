"""experiments/2026-9-5_orbit-address-widen-64bit -- Tahap 2: avalanche
3-arah untuk kandidat A/B/C (widen ra_core_orbit/singleblock ke addr_hi
64-bit).

Reuse `quality_gate.avalanche_stats()` (band [0.3,0.7], per-bit floor 0.2)
tanpa modifikasi -- sama seperti
../2026-9-4_orbit-kmin-battery/avalanche_orbit_kmin.py dan
../2026-9-1_production-candidate-battery/avalanche_ra_core_singleblock_k1.py
/avalanche_orbit_singleblock_k255.py. Yang baru: target binary kandidat di
candidates/ (kontrak argv `--stream <core> <key> <addr_hi> <n>`), dan 3
arah per HANDOVER.md §5.1:

  1. Flip 1 bit key, addr_hi tetap (BASE_ADDR_HI).
  2. Flip 1 bit addr_hi, key tetap (BASE_KEY).
  3. Flip 1 bit GABUNGAN -- bit posisi sama di key DAN addr_hi SEKALIGUS.
     Ini bukan cuma union arah 1+2: karena `r = key ^ addr_hi ^ i` dipakai
     di ketiga kandidat, membalik bit b di KEDUA key dan addr_hi pada saat
     yang sama membuat XOR itu SALING MENIADAKAN (r tidak berubah sama
     sekali untuk bit itu) -- risiko blind-spot spesifik yang tidak
     tertangkap arah 1 atau 2 sendirian, terutama fatal untuk Kandidat C
     (addr_hi HANYA masuk lewat r).

Diimplementasikan dengan trik yang sama seperti avalanche_stats() sendiri:
avalanche_stats(capture_fn, base_seed) membalik bit dari SATU argumen
`seed`. Untuk arah 3, capture_fn menerima `pattern` dan meng-XOR-kannya ke
KEDUA key dan addr_hi sekaligus (base_seed=0), sehingga bit `b` dari
`pattern` == bit `b` yang dibalik di key DAN addr_hi bersamaan.
"""

from __future__ import annotations

import subprocess
import struct
import sys
from pathlib import Path

HERE = Path(__file__).parent
CANDIDATES_DIR = HERE / "candidates"

sys.path.insert(0, str(HERE.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

LOW, HIGH, MIN_BIT_FLOOR = 0.3, 0.7, 0.2

BASE_KEY = 1
BASE_ADDR_HI = 0x9E3779B9

CANDIDATES = ["candidate_d", "candidate_e"]
MODES = [("orbit", 1), ("orbit", 255), ("singleblock", 1), ("singleblock", 255)]


def capture(binary: Path, core: str, key: int, addr_hi: int, n_words: int) -> list[int]:
    out = subprocess.run(
        [str(binary), "--stream", core,
         str(key & 0xFFFFFFFF), str(addr_hi & 0xFFFFFFFF), str(n_words)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) == n_words * 4, f"expected {n_words * 4} bytes, got {len(out)}"
    return list(struct.unpack(f"<{n_words}I", out))


def run_direction(binary: Path, core: str, n_words: int, direction: str) -> dict:
    if direction == "key_only":
        cfn = lambda seed: capture(binary, core, seed, BASE_ADDR_HI, n_words)
        base_seed = BASE_KEY
    elif direction == "addr_hi_only":
        cfn = lambda seed: capture(binary, core, BASE_KEY, seed, n_words)
        base_seed = BASE_ADDR_HI
    elif direction == "combined":
        cfn = lambda pattern: capture(binary, core, BASE_KEY ^ pattern, BASE_ADDR_HI ^ pattern, n_words)
        base_seed = 0
    else:
        raise ValueError(direction)

    stats = avalanche_stats(cfn, base_seed=base_seed)
    fraction = stats["overall_mean_hamming_fraction"]
    passed = (LOW <= fraction <= HIGH) and (stats["min_bit_fraction"] >= MIN_BIT_FLOOR)
    return {"direction": direction, "passed": passed, **stats}


def main():
    overall_pass = True
    results = []
    for cand_name in CANDIDATES:
        binary = CANDIDATES_DIR / cand_name
        assert binary.exists(), f"{binary} not found -- compile it first"
        for core, n_words in MODES:
            for direction in ("key_only", "addr_hi_only", "combined"):
                r = run_direction(binary, core, n_words, direction)
                overall_pass &= r["passed"]
                results.append((cand_name, core, n_words, r))
                print(f"=== {cand_name} | {core} K={n_words} | arah={direction} ===")
                print(f"  overall_mean_hamming_fraction: {r['overall_mean_hamming_fraction']:.6f} "
                      f"(band [{LOW},{HIGH}])")
                print(f"  min_bit_fraction: {r['min_bit_fraction']:.6f} (floor {MIN_BIT_FLOOR})")
                print(f"  PASSED: {r['passed']}")
                if not r["passed"]:
                    worst = min(range(32), key=lambda b: r["per_bit_fractions"][b])
                    print(f"  weakest bit: {worst} (fraction {r['per_bit_fractions'][worst]:.6f})")

    print("\n=== RINGKASAN ===")
    header = f"{'kandidat':<12} {'mode':<12} {'K':<5} {'arah':<14} {'mean':<8} {'min_bit':<8} {'PASS'}"
    print(header)
    for cand_name, core, n_words, r in results:
        print(f"{cand_name:<12} {core:<12} {n_words:<5} {r['direction']:<14} "
              f"{r['overall_mean_hamming_fraction']:<8.4f} {r['min_bit_fraction']:<8.4f} {r['passed']}")

    sys.exit(0 if overall_pass else 1)


if __name__ == "__main__":
    main()
