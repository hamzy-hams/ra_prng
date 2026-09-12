"""HANDOVER.md Tahap B (4.2): gerbang period teoretis, sebelum kompilasi
PractRand/dieharder apa pun -- murah karena cuma hitung-hitungan.

Adaptasi `predicted(n, w)` dari
../2026-8-25_periodicity-heuristic-validation/run_sweep.py (heuristik
lambda ~= 0.7824*sqrt(|S|) dari Purdom-Williams random-mapping theory,
LAMBDA_CONST = 0.7824 verbatim, TIDAK diubah) ke topologi state RIIL
ra_core_param.c (w=32 tetap, sweep atas N, bukan sweep atas (n,w) toy).

Pemetaan bit-entropi mengikuti pecahan bmut/log2_fact yang sama persis
dengan run_sweep.py's predicted(): kata w-bit "raw" (M, cons, it) dihitung
linear (w bit masing-masing slot), sedangkan L dihitung sebagai ENTROPI
PERMUTASI log2(N!) -- bukan N*w bit raw -- karena mekanisme L satu-satunya
yang dipakai produksi adalah swap L[i]<->L[d] ("permute" di toy_prng.py's
MECHANISMS), yang mempertahankan multiset nilai L dan hanya mempermutasi
posisinya. Ini PERSIS asumsi run_sweep.py's `predicted()`, bukan asumsi
baru.

  orbit (2xN word + cons + it persisten lintas siklus reseed dalam satu
  stream tak terbatas):
      bmut   = 32 * (N + 2)      # M: N word, cons: 1, it: 1, semua w=32 raw
      log2S  = bmut + log2_fact(N)   # + L sebagai permutasi N elemen
      lambda_pred = 0.7824 * sqrt(2**log2S)

  singleblock (N word, TANPA L, TANPA reseed/loop -- satu init + satu
  siklus (N-1) langkah lalu selesai; cons==key tetap, ra_reseed dead code):
      bmut   = 32 * N             # M saja, cons/it tidak berevolusi
      log2S  = bmut               # tidak ada suku permutasi (tidak ada L)
      lambda_pred = 0.7824 * sqrt(2**log2S)

CATATAN PENTING (supaya tidak disalahartikan): untuk singleblock,
lambda_pred BUKAN "periode stream" literal -- singleblock tidak pernah
loop/reseed, dibatasi keras rng<=N-1 per desain. Angka ini adalah ukuran
ruang-keadaan M[] semata (seberapa besar keluarga M[] yang bisa dihasilkan
lintas key/addr_hi berbeda), dipakai di sini hanya sebagai pembanding kasar
skala terhadap N-1 (panjang maksimum yang benar-benar dipakai per block) --
BUKAN gerbang periodisitas dalam pengertian orbit. Untuk orbit, lambda_pred
adalah prediksi genuine mean-return-time heuristik atas grafik fungsional
(L,M,cons,it) -> next, relevan langsung terhadap "berapa lama sebelum
stream --stream kontinu mulai berulang".
"""

from __future__ import annotations

import math

LAMBDA_CONST = 0.7824  # verbatim dari run_sweep.py, TIDAK diubah
W = 32  # ra_core.c selalu uint32_t, tidak disweep seperti toy model

NS = [256, 128, 64, 32, 16]

# Budget test yang direncanakan (HANDOVER.md #4.3 Tier1/Tier2), dalam word
# 32-bit, dipakai sbg pembanding skala -- BUKAN ambang pass/fail keras.
TEST_BUDGETS_WORDS = {
    "Tier1 screen (1-2GB)": 2 * 1024**3 // 4,
    "Tier2 deep (16GB)": 16 * 1024**3 // 4,
}


LOG2_LAMBDA_CONST = math.log2(LAMBDA_CONST)


def log2_fact(n: int) -> float:
    return math.lgamma(n + 1) / math.log(2)


def predicted_orbit_log2(n: int) -> tuple[float, float]:
    """Returns (log2|S|, log2(lambda_pred)). lambda_pred itself overflows a
    Python float for any realistic N (log2S is in the thousands of bits), so
    everything stays in log2 space -- only ratios (differences of logs) are
    ever exponentiated back, and only when small enough to matter."""
    bmut = W * (n + 2)
    log2s = bmut + log2_fact(n)
    log2_lam = LOG2_LAMBDA_CONST + log2s / 2
    return log2s, log2_lam


def predicted_singleblock_log2(n: int) -> tuple[float, float]:
    bmut = W * n
    log2s = bmut
    log2_lam = LOG2_LAMBDA_CONST + log2s / 2
    return log2s, log2_lam


def fmt_log2(log2_val: float) -> str:
    """Render a huge log2-scale value as 'a x 10^b' without ever
    materializing the actual float."""
    log10_val = log2_val * math.log10(2)
    exponent = math.floor(log10_val)
    mantissa = 10 ** (log10_val - exponent)
    return f"{mantissa:.3f}e+{exponent:d}"


def main() -> None:
    print("=== Orbit (2xN word + cons + it, L = permutasi entropi) ===")
    print(f"{'N':>5} {'log2|S|':>10} {'lambda_pred (words)':>20}  vs budget (log2 margin)")
    for n in NS:
        log2s, log2_lam = predicted_orbit_log2(n)
        flags = []
        for label, budget in TEST_BUDGETS_WORDS.items():
            log2_budget = math.log2(budget)
            log2_margin = log2_lam - log2_budget  # lambda_pred is 2**log2_margin x budget
            if log2_margin < math.log2(10):
                flags.append(
                    f"WARNING: lambda_pred hanya 2^{log2_margin:.1f}x {label}"
                )
        line = f"{n:>5} {log2s:>10.1f} {fmt_log2(log2_lam):>20}"
        if flags:
            line += "  <-- " + "; ".join(flags)
        print(line)

    print()
    print("=== Singleblock (N word M saja, TANPA L/cons-evolution/loop) ===")
    print(f"{'N':>5} {'log2|S|':>10} {'lambda_pred (M-space)':>20}  vs N-1 (panjang max/block)")
    for n in NS:
        log2s, log2_lam = predicted_singleblock_log2(n)
        log2_block = math.log2(n - 1)
        log2_margin = log2_lam - log2_block
        line = f"{n:>5} {log2s:>10.1f} {fmt_log2(log2_lam):>20}  margin=2^{log2_margin:.1f}x(N-1)"
        if log2_margin < 20:  # heuristik longgar hanya utk N kecil, lihat header note
            line += "  <-- WARNING: M-state-space heuristik mendekati skala panjang-per-block"
        print(line)

    print()
    print("Budget test yang dipakai sbg pembanding:")
    for label, budget in TEST_BUDGETS_WORDS.items():
        print(f"  {label}: {budget:.3e} word (32-bit) = 2^{math.log2(budget):.1f} word")


if __name__ == "__main__":
    main()
