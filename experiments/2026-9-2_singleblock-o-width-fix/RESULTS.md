# RESULTS: fix defect K-kecil `ra_core_singleblock` — perlebar `o` (Kandidat A) + internal warm-up (Kandidat B)

Status: **eksperimen selesai. Tidak ada kandidat yang direkomendasikan untuk
diterapkan ke `ra_core.c`.** `ra_core.c` dan
`experiments/2026-9-1_production-candidate-battery/` TIDAK disentuh sama
sekali di eksperimen ini (lihat §Verifikasi non-goals di akhir).

## Konteks

Battery gate (`experiments/2026-9-1_production-candidate-battery/`)
menemukan defect struktural di `ra_core_singleblock`: PractRand BCFN FAIL
tumbuh monoton untuk K (word per block/key) kecil sampai K=72, baru bersih
total mulai K≈96
(`experiments/2026-9-2_singleblock-k-threshold-characterization/RESULTS.md`).
Root cause: state awal tiap block SELALU `a=cons=key, b=0, c=0, d=0`
(`ra_core.c` `ra_permutation_cycle_singleblock`, baris 214-236), dan putaran
pertama cuma mixing dangkal lewat `o` 2-tap (`M[i+6]<<6 ^ M[i+7]<<7`).
Baseline pembanding narrow-o K=1: **FAIL BCFN R≈+1028 s/d +1069 @1GB**.

Spec eksekusi lengkap ada di `HANDOVER.md` (folder ini). Dua kandidat diuji
berurutan sesuai spec, mode checkpoint-per-fork-point (konfirmasi user di
tiap titik keputusan).

## Metodologi

- File diagnostik self-contained (pola `diag_prereseed.c`): copy verbatim
  `rot32`/`fmix32`/`GUARD_L`/`GUARD_M`/`ra_init_state_singleblock` dari
  `../2026-9-1_family-productionization/ra_core.c`, TIDAK diubah.
  `ra_permutation_cycle_singleblock` di-reimplementasi inline (fungsi
  `static` di `ra_core.c`, tidak bisa extern-link).
- `diag_wideo_singleblock.c` — Kandidat A: varian `narrow` (kontrol,
  byte-identik produksi) vs `wideo` (HANYA baris `o=...` diganti 8-tap XOR,
  formula `a/b/c/d` tidak berubah).
- `diag_wideo_warmup_singleblock.c` — Kandidat B: extend Kandidat A, satu
  loop `i=255..1` di mana N putaran pertama dibuang (update `a,b,c,d` saja,
  tanpa `fwrite`/tanpa decrement `count`), lanjut putaran berikutnya seperti
  biasa memakai state hasil warm-up (bukan reset). Sanity check wajib:
  `N=0` byte-identik dengan output `diag_wideo_singleblock` varian yang
  sama — **IDENTIK**, diverifikasi lokal & VPS.
- Semua stream PractRand: base_key=111222, Weyl/golden-ratio key increment
  `0x9E3779B9` antar block (pola `multikey_stream_k.c`), `RNG_test stdin32
  -multithreaded`.
- Run besar (16GB/titik) dijalankan di VPS riset (lihat `VPS_ACCESS.md`,
  gitignored) via `nohup ... & disown`, log live-flushed.
- **Prinsip seed (ditetapkan user eksplisit sesi ini):** tidak ada
  percobaan ulang dengan seed berbeda untuk "mengonfirmasi" FAIL. PRNG yang
  baik harus menghasilkan output acak pada seed apapun — FAIL pada satu
  seed sudah cukup untuk menyatakan algoritma gagal, tanpa perlu direplikasi
  di seed lain.

## Kandidat A — hasil per K

| K | Skala | Checkpoint bersih terakhir | Anomali BCFN tertinggi | Log |
|---|---|---|---|---|
| 1 | 8GB (lokal) | 1GB | mildly susp. R=+10.4 @2GB, unusual R=+8.6 @4-8GB | `practrand_A_wideo_K1.log` |
| 1 | 16GB (VPS, lanjutan) | — | ...+ suspicious R=+13.6 @16GB, **TMFn(2+4):wl FAIL p~=7e-15 @16GB** (bukan BCFN — test lain) | `practrand_A_wideo_K1_16GB.log` |
| 2 | 2GB (lokal) | 2GB (bersih total) | tidak ada | `practrand_A_wideo_K2.log` |
| 2 | 16GB (VPS) | 1GB | BCFN mildly susp. R=+10.7 @2-8GB → **BCFN FAIL R=+22.2 @16GB**, + FPF-14 FAIL | `practrand_A_wideo_K2_16GB.log` |
| 4 | 2GB (lokal) | 1GB | mildly susp. R=+10.9, suspicious R=+12.0 @2GB | `practrand_A_wideo_K4.log` |
| 4 | 16GB (VPS) | 1GB | ...tumbuh very susp. R=+16-18 @4-8GB → **BCFN FAIL R=+30.7/+25.8 @16GB** | `practrand_A_wideo_K4_16GB.log` |
| 8 | 2GB (lokal) | 2GB (bersih total) | tidak ada | `practrand_A_wideo_K8.log` |
| 8 | 16GB (VPS) | 8GB | very susp. R=+16.6, unusual R=+8.2 @16GB (belum FAIL) | `practrand_A_wideo_K8_16GB.log` |
| 16 | 2GB (lokal) | 2GB (bersih total) | tidak ada | `practrand_A_wideo_K16.log` |
| 16 | 16GB (VPS) | 8GB | unusual R=+7.9, suspicious R=+11.4 @16GB (belum FAIL) | `practrand_A_wideo_K16_16GB.log` |

**Kesimpulan Kandidat A:** screening awal (1-8GB) tampak bersih/hampir
bersih untuk semua K — tapi diperpanjang ke 16GB, defect BCFN **kembali
muncul dan tumbuh untuk SEMUA K**, eksplisit FAIL untuk K=1 (test lain,
TMFn), K=2, dan K=4; tren naik jelas untuk K=8 dan K=16. `o` lebar
**menunda dan mengecilkan** defect (dibanding baseline narrow-o R≈1069),
tapi tidak menghilangkannya secara struktural.

**Cabang keputusan:** (b) "membaik tapi belum cukup" — lanjut Kandidat B
sebagai kombinasi dengan `wideo` (bukan pengganti), dikonfirmasi user.

## Microbench

Lokal (mesin dev, via `diag_wideo_singleblock`, N/A untuk warm-up):

| Variant | K | ns/word |
|---|---|---|
| narrow | 1 | 36.31 |
| wideo | 1 | 40.03 (1.10x) |
| narrow | 255 | 0.162 |
| wideo | 255 | 0.159 (0.98x) |

VPS (mesin yang sama dengan semua run PractRand di atas, via
`diag_wideo_warmup_singleblock`, N=0 = Kandidat A murni):

| Variant | K | N | ns/word | vs narrow N=0 |
|---|---|---|---|---|
| narrow | 1 | 0 | 46.39 | 1.00x |
| wideo | 1 | 0 | 53.17 | 1.15x |
| wideo | 1 | 1 | 60.53 | 1.31x |
| wideo | 1 | 4 | 80.71 | 1.74x |
| wideo | 255 | 0 | 12.21 | — |
| wideo | 255 | 1 | 17.09 | 1.40x (vs wideo N=0) |
| wideo | 255 | 4 | 11.34 | ~1.0x (noise, fixed cost dominan) |

## Kandidat B — triage (N ∈ {1,2,4,8,16,24,32,48,64}, K=1, wide-o, 2GB)

| N | BCFN | FAIL | Anomali lain |
|---|---|---|---|
| 1 | tidak ada | tidak ada | **bersih total** (0/4 checkpoint) |
| 2 | tidak ada | tidak ada | FPF-14+6/16:cross R=+8.1 very susp. |
| 4 | tidak ada | tidak ada | tidak ada |
| 8 | tidak ada | tidak ada | tidak ada |
| 16 | tidak ada | tidak ada | tidak ada |
| 24 | tidak ada | tidak ada | tidak ada |
| 32 | tidak ada | tidak ada | Gap-16:B R=-5.7 mildly susp. |
| 48 | tidak ada | tidak ada | Low8/32 DC6-9x1Bytes R=-5.2 unusual |
| 64 | tidak ada | tidak ada | Low1/32 Gap-16:B R=-4.7 unusual |

Log: `practrand_B_wideo_warmup_N{N}_K1_triage.log`, status:
`practrand_B_triage_status.log`. Semua 9 titik lolos triage 2GB — dipilih
N=1 (overhead terkecil, 100% bersih) dan N=4 (margin ekstra) untuk
konfirmasi skala penuh, dikonfirmasi user.

## Kandidat B — konfirmasi skala penuh (N ∈ {1,4}, K ∈ {1,2,4,8,16}, 16GB)

| N | K | BCFN tertinggi | Status | Log |
|---|---|---|---|---|
| 1 | 1 | tidak ada | **bersih total sampai 16GB** (FPF-14 FAIL, bukan BCFN) | `practrand_B_wideo_warmup_N1_K1_16GB.log` |
| 1 | 2 | unusual R=+8.9 | bersih (BCFN); FPF-14 FAIL terpisah | `practrand_B_wideo_warmup_N1_K2_16GB.log` |
| 1 | 4 | suspicious R=+12.6 → **FAIL R=+25.3** | **GAGAL** | `practrand_B_wideo_warmup_N1_K4_16GB.log` |
| 1 | 8 | very susp. R=+13.6 → **FAIL R=+24.1** | **GAGAL** | `practrand_B_wideo_warmup_N1_K8_16GB.log` |
| 1 | 16 | suspicious R=+12.2 | bersih (belum FAIL) | `practrand_B_wideo_warmup_N1_K16_16GB.log` |
| 4 | 1 | tidak ada | **bersih total sampai 16GB** (FPF-14 FAIL, bukan BCFN) | `practrand_B_wideo_warmup_N4_K1_16GB.log` |
| 4 | 2 | tidak ada | **bersih total** | `practrand_B_wideo_warmup_N4_K2_16GB.log` |
| 4 | 4 | very susp. R=+15.0 → **FAIL R=+26.5** | **GAGAL** | `practrand_B_wideo_warmup_N4_K4_16GB.log` |
| 4 | 8 | unusual R=+8.4 | bersih (belum FAIL) | `practrand_B_wideo_warmup_N4_K8_16GB.log` |
| 4 | 16 | mildly susp./unusual R=+7.9-10.5 | bersih (belum FAIL) | `practrand_B_wideo_warmup_N4_K16_16GB.log` |

Log status: `practrand_B_confirm_16gb_status.log`.

**Temuan kunci:** K=1 — kasus target utama HANDOVER, paling parah gagal di
baseline (R≈1069) dan masih gagal di Kandidat A murni (16GB, test TMFn) —
**bersih total** untuk N=1 maupun N=4. Tapi defect **tidak hilang
seluruhnya**: ia muncul kembali di K=4 (FAIL untuk N=1 dan N=4) dan K=8
(FAIL untuk N=1, ringan untuk N=4). Pola ini **non-monoton terhadap K**,
berbeda dari Kandidat A murni yang monoton (parah di K kecil, mereda di K
besar) — warm-up menggeser di mana defect muncul, bukan menghilangkannya
secara universal.

Overhead terukur (K=1, lihat tabel microbench): N=1 → 1.31x, N=4 → 1.74x
vs `narrow` produksi. Overhead struktural putaran `(N+1)x` = 2x (N=1) atau
5x (N=4), jauh lebih besar dari overhead ns/word terukur karena sebagian
besar biaya per-word adalah init `M[]` tetap (256 iterasi), bukan cycle
itu sendiri.

## Rekomendasi

**Tidak ada kandidat (Kandidat A saja, atau A+B dengan N manapun yang
diuji) yang direkomendasikan untuk diterapkan ke `ra_core.c`.** Sesuai
prinsip seed di atas: FAIL pada K=4 (N=1 dan N=4) dan K=8 (N=1) di skala
16GB sudah cukup untuk menyatakan kedua kandidat gagal sebagai fix
universal — regardless of K=1 dan K=2 tampak bersih. Kandidat B menutup
celah K=1 tapi membuka/mempertahankan celah di K=4/K=8; tidak ada N yang
diuji (1 atau 4) menutup seluruh rentang K yang relevan (K-threshold lama
sudah menunjukkan defect signifikan sampai K=72).

Opsi yang TIDAK dijalankan di eksperimen ini (di luar scope, butuh
keputusan/spec terpisah kalau mau dilanjutkan):
- Sweep N lebih besar (8, 16, 24, dst., yang lolos triage 2GB) dikonfirmasi
  khusus di K=4 dan K=8 skala 16GB — mungkin menutup celah itu juga, tapi
  belum diuji di skala penuh.
- Kombinasi struktural lain (mis. state awal per-block tidak selalu
  identik, bukan cuma warm-up putaran).
- Menaikkan K produksi ke ambang aman lama (K≥96, dari
  `2026-9-2_singleblock-k-threshold-characterization`) tetap jadi mitigasi
  yang sudah terverifikasi terpisah, tidak bergantung hasil eksperimen ini.

Battery gate (`2026-9-1_production-candidate-battery/`) **tetap PAUSED**
sampai ada kandidat fix K-kecil yang terbukti bersih di seluruh rentang K
relevan pada skala 16GB+, dengan konfirmasi user eksplisit untuk
menerapkannya ke `ra_core.c`.

## Verifikasi non-goals

- `ra_core.c` TIDAK diubah — lihat `git diff --stat` di akhir sesi.
- `experiments/2026-9-1_production-candidate-battery/` TIDAK dijalankan
  ulang/disentuh.
- Tidak ada op lain dari `pruned_prng.c` (ROT_A/ROT_B/SHL9 dst.) yang
  ditambahkan — hanya lebar `o` (Kandidat A) dan warm-up-N-putaran
  (Kandidat B) yang jadi variabel eksperimen, sesuai spec `HANDOVER.md`.
- Tidak ada percobaan ulang dengan seed berbeda untuk kasus yang FAIL.
