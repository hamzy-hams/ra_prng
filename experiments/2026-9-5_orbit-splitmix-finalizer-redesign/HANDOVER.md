# Handover: redesign arsitektur "Weyl-add + finalizer 32-bit murni" untuk defect K-small `ra_core_orbit`

**Status folder ini: BELUM DIKERJAKAN — dirancang siap-eksekusi, ditulis
supaya sesi lain (atau user) bisa langsung jalan tanpa perlu merancang
ulang dari nol.** Tidak ada kode/hasil di folder ini sama sekali sampai
tulisan ini.

**PENTING — baca §0 sebelum mulai implementasi apa pun.** Desain di bawah
adalah proposal siap-eksekusi, BUKAN keputusan final — ada checkpoint
wajib ke user sebelum beberapa langkah. Ini juga BUKAN sekadar "kandidat
fix ke-9" dalam pola 8 sesi sebelumnya — ini REDESIGN ARSITEKTUR, scope
dan risikonya beda kelas, baca §0.1 dulu.

## 0. WAJIB: checkpoint ke user sebelum implementasi

1. **Ini redesign arsitektur, bukan patch formula.** 8 kandidat sebelumnya
   (lihat §1) semuanya memodifikasi SATU titik di dalam
   `ra_permutation_cycle_orbit` yang sudah ada (nilai seed, operator
   combine, dsb.) sambil mempertahankan loop 255-round dan tabel
   `L[256]`/`M[256]`. Desain di handover ini **tidak memakai
   `ra_permutation_cycle_orbit`, `ra_init_state_orbit`, atau tabel
   `L[]`/`M[]` sama sekali** — generator baru dari nol berbasis Weyl-
   sequence state advance + finalizer 32-bit. Sebelum mulai, konfirmasi
   ke user: apakah ini dimaksudkan sebagai KANDIDAT PENGGANTI orbit untuk
   K-small (kalau lolos semua tahap), atau VARIAN PARALEL/eksperimental
   yang hidup terpisah selamanya? Jangan pilih nama fungsi/folder yang
   menyiratkan "v2" atau "pengganti resmi" secara sepihak — lihat
   `feedback_arch_specific_baseline` memory (jangan deprecate baseline
   yang sudah proven secara kasual). Nama folder `2026-9-5_orbit-
   splitmix-finalizer-redesign` dan nama fungsi di bawah (`weyl_fmix32`,
   dst.) TENTATIF, boleh diganti sebelum/saat implementasi.

2. **Kekuatan finalizer `fmix32` (3 multiply) untuk kasus ini BELUM
   TERBUKTI.** Di `ra_core.c`, `fmix32` sejauh ini cuma dipakai SEKALI-
   PAKAI (derive `keyterm_l`/`keyterm_m` dari key di
   `ra_init_state_orbit`), bukan sebagai finalizer yang di-apply berulang
   ke input yang PURELY LINEAR/incrementing (pola Weyl-sequence yang
   diusulkan di sini). ***Koreksi penting***: fungsi `splitmix32()` yang
   sudah dipakai di SEMUA driver eksperimen sesi-sesi K-small sebelumnya
   (disalin dari `../2026-9-1_family-productionization/
   multikey_stream_hashed.c` L16-22) **BUKAN bukti bahwa pola ini aman di
   32-bit** — `splitmix32()` itu sebenarnya SplitMix64 (state `uint64_t`,
   Weyl constant 64-bit `0x9E3779B97F4A7C15`, perkalian 64x64
   `z*0xBF58476D1CE4E5B9ULL`) yang outputnya dipotong ke 32-bit atas.
   Lebar 32-bit (bukan 64-bit) punya birthday-bound yang jauh lebih dekat
   (~2^16 state sebelum kolisi mulai muncul secara statistik, vs ~2^32 di
   64-bit) — ada risiko nyata 3-multiply `fmix32` TIDAK cukup kuat untuk
   input linear murni di lebar sesempit ini. **WAJIB siapkan §4
   (`triple32`, fallback lebih kuat) sebagai Kandidat B, uji dua-duanya,
   jangan asumsikan A otomatis cukup.**
3. **Pemetaan `key` (address) → word ke-`j` dalam satu blok** — dua opsi
   di §3, BEDA secara struktural (opsi (a) "SplitMix-literal": satu state
   dasar dari `key`, lalu increment; opsi (b) "counter-style" mirip
   Philox: kombinasi `key` dan `j` langsung tanpa state berjalan). Pilih
   SATU untuk diprioritaskan dulu bersama user sebelum codegen dimulai —
   HANDOVER ini merekomendasikan (a) sebagai default (lebih dekat
   precedent SplitMix yang proven), tapi ini bukan keputusan final.
4. **Skala Tier 1** (2GB vs 8GB) — sama seperti precedent handover lain
   di repo ini, tanya user dulu, jangan asumsikan salah satu.
5. **Implikasi benchmark** — desain ini kemungkinan MENGHILANGKAN
   kebutuhan init 256-word `L[]`/`M[]` sama sekali (state awal cuma
   perlu `fmix32(key)`, bukan mengisi 2×256 array). Potensi speedup besar
   untuk K=1 addressable (relevan ke `project_state_size_pareto_frontier`
   memory — bisa mengubah keseluruhan framing "orbit vs singleblock
   crossover di K≈20-21"), TAPI ini WAJIB dibuktikan lewat benchmark, bukan
   diasumsikan. Jalankan benchmark standar 5-angka
   (`feedback_orbit_singleblock_benchmark_standard`: orbit --multistream
   K=1/K=255/--stream, singleblock K=1/K=255) SEBAGAI PEMBANDING, plus
   angka kandidat baru untuk K=1/K=255/--stream yang setara — jangan
   klaim "lebih cepat" tanpa data sisi-bersisi.
6. **Kalau kandidat A dan B dua-duanya CLEAN-DUAL** — tanya user mana
   yang mau dilanjutkan/didiskusikan lebih lanjut (lebih murah/A vs lebih
   ada margin keamanan/B), jangan otomatis pilih salah satu.
7. **Kalau keduanya FAIL** — JANGAN langsung lompat ke opsi lebih berat
   (mis. desain counter-based yang lebih kompleks lagi, atau balik lagi
   ke patch orbit yang ada). Laporkan dengan data lengkap, biarkan user
   putuskan arah berikutnya (`feedback_ask_more_on_design_decisions`).

Secara umum: perlakukan handover ini sebagai starting point untuk
didiskusikan ulang di fase plan sesi berikutnya, bukan spec yang tinggal
dieksekusi buta — ini levelnya lebih besar dari kandidat-kandidat
sebelumnya, pantas dapat lebih banyak checkpoint, bukan lebih sedikit.

## 1. Kenapa handover ini ditulis (ringkasan, baca sumber untuk detail lengkap — JANGAN re-derive dari nol)

Defect: `ra_core_orbit` K=1 dengan key ordering hashed (SplitMix32/64)
FAIL PractRand (`FPF-14+6/16`) parah; K=255 dan K=1+Weyl-ordering tetap
bersih. **8 axis independen sudah dicoba sesi-sesi sebelumnya (2026-09-04
s/d 2026-09-05), SEMUA GAGAL**:

1. Varying/mixing `c` (finalizer) — 32-combo search, 0/32 survivor.
2. Zero-tap, init × cycle 8×4 combo search — 0/32 survivor.
3. `d0_hash` (d nonzero, hash dari key) — FAIL PractRand K=1+hashed @512MB.
4. `d0_realround` (d dari 1 ronde asli dibuang) — REJECT Tier 0.
5. Weyl-constant-resonance (ganti step constant test-harness) — hipotesis REFUTED, tidak relevan sebagai fix.
6. `fmix32(o)` init-only / every-round — PASS Tier0, FAIL PractRand K=1+hashed.
7. `a=(d*o)^(cons+a)` (MUL-combine) — REJECT Tier 0.
8. `d0_raw_cons` (d nonzero, raw cons, tanpa hash) — FAIL PractRand K=1+hashed @512MB.

Plus **diagnosis posisi** (`experiments/2026-9-5_orbit-round1-collision-
position-isolation/RESULTS.md`): isolasi round1 (`i=255`) vs round2
(`i=254`) di K=2 menunjukkan KEDUA posisi FAIL dengan magnitude nyaris
sama — merefuting hipotesis "satu titik collision `a=cons` di round-1"
sebagai penjelasan tunggal. Kesimpulan: radius yang tercemar mencakup
minimal 2 posisi round dari init segar, dan TIDAK SATU PUN modifikasi
titik tunggal (nilai `d`, `o`, `c`, atau operator combine) terbukti cukup
menutupnya.

**Kesimpulan sesi yang menulis handover ini**: masalahnya bukan di titik
seed/init/nilai tunggal manapun di dalam loop `ra_permutation_cycle_orbit`
— ada di ARSITEKTUR loop itu sendiri. Diffusinya terakumulasi bertahap
per-posisi-round (round dekat init segar secara statistik lemah, round
jauh dari init sudah "matang"/bersih) — properti sekuensial, bukan bug
lokal. Referensi lengkap: `../2026-9-1_family-productionization/
PRODUCTION_READINESS_HANDOVER.md` §2b (semua 8 update tercatat inline),
`project_orbit_key_avalanche_diagnosis` memory.

## 2. Insight arsitektural (dari diskusi sesi ini) dan scope

Desain PRNG yang teruji kebal terhadap kelas defect "diffusi terakumulasi
per-posisi" memisahkan **kualitas output di posisi tertentu** dari
**berapa banyak round sudah berjalan secara historis**. Tiga pola umum
dibahas sesi ini:

1. **Counter-based fixed-round** (Philox, ThreeFry): setiap output dari
   `(key, counter)` lewat fungsi TETAP N-round identik untuk index
   manapun — tidak ada state sekuensial sama sekali.
2. **PCG-style separated output permutation**: state advance murah
   (boleh lemah sendirian) dipisah dari fungsi output kuat yang di-apply
   SETIAP baca, independen dari sejarah.
3. **ARX block cipher** (ChaCha-like): N round tetap per blok, block
   penuh dihitung langsung dari input.

User tertarik ke arah (2)/(1) hybrid, dengan syarat GPU-friendly: HINDARI
perkalian lebar >32-bit (64×64 atau lebih emulasi mahal di banyak GPU;
32×32→32 truncating atau 32×32→64 widening murah/native). Pola konkret
yang dipilih untuk dieksplorasi: **Weyl-sequence state advance (ADD
32-bit murni, TANPA perkalian sama sekali untuk advance) + finalizer
32×32→32-multiply-only** yang diterapkan FRESH ke tiap state, bukan
diakumulasi lewat loop. Ini secara struktural = pola SplitMix, TAPI
diimplementasikan benar-benar 32-bit (bukan seperti `splitmix32()` yang
sudah ada di repo, yang ternyata 64-bit berkedok nama — lihat §0.2).

**Scope**: folder eksperimen baru, TIDAK menyentuh `ra_core.c`, TIDAK
memakai `ra_permutation_cycle_orbit`/`ra_init_state_orbit`/`L[]`/`M[]`.
Dua kandidat finalizer (§3, §4) × dua opsi pemetaan key→state (§0.3) =
ruang desain untuk dieksplorasi, bukan satu kandidat tunggal siap-jalan.

## 3. Kandidat A — `weyl_fmix32` (finalizer termurah, precedent sudah ada di repo)

```c
// State per-key/per-block, 32-bit MURNI, TIDAK ada tabel L[]/M[] apa pun.

#define WEYL32 0x9e3779b7u   // konstanta L-multiplier ra_core.c yang sudah ada
                              // -- ATAU 0x9E3779B9u (Weyl test-harness constant di
                              // driver --stream, beda 2 dari yang di atas) --
                              // PILIH SATU SADAR, catat alasannya di kode (bukan
                              // dua-duanya dipakai sembarang tanpa pertimbangan)

// fmix32 -- SUDAH ADA di ra_core.c L149-156, salin verbatim, JANGAN diubah:
static inline uint32_t fmix32(uint32_t h) {
    h ^= h >> 16; h *= 0x85ebca6bu;
    h ^= h >> 13; h *= 0xc2b2ae35u;
    h ^= h >> 16;
    return h;
}

// Opsi (a) pemetaan key->state, "SplitMix-literal" (REKOMENDASI DEFAULT §0.3):
uint32_t state0 = fmix32(key);
uint32_t state_j = state0 + (uint32_t)(j * WEYL32);   // j = 0..rng-1
uint32_t output_j = fmix32(state_j);

// Opsi (b) pemetaan key->state, "counter-style" (mirip Philox, TANPA state berjalan):
// #define WEYL32_2 0x85ebca6bu   // beda konstanta dari WEYL32, supaya key-term
//                                  // dan index-term tidak collide struktural
// uint32_t state_j = key ^ (uint32_t)(j * WEYL32_2);
// uint32_t output_j = fmix32(state_j);
```

**Wajib**: kalau opsi (b) dipakai, `fmix32(key ^ 0)` untuk `key=0,j=0`
adalah kasus tepi `fmix32(key)` polos tanpa guard — cek dulu apakah
`fmix32(0)=0` masih jadi masalah di jalur ini (riwayat bug ini ada di
`project_multikey_remix_search` memory, sudah pernah jadi alasan
`GUARD_L`/`GUARD_M` ditambahkan ke `ra_init_state_orbit`). Kalau perlu,
tambahkan guard-XOR (pola sama: `fmix32(GUARD_X ^ key)`) sebelum dipakai,
BUKAN `fmix32(key)` telanjang.

## 4. Kandidat B — `weyl_triple32` (finalizer lebih kuat, fallback kalau A gagal Tier 1)

```c
// Chris Wellons, hash-prospector project, hasil exhaustive/statistical bias
// search khusus untuk finalizer 32-bit (https://github.com/skeeto/hash-prospector
// -- BARU di repo ini, BUKAN disalin dari ra_core.c, WAJIB diverifikasi ulang
// terhadap sumber aslinya sebelum dipakai, bukan diketik dari ingatan tanpa cek).
static inline uint32_t triple32(uint32_t x) {
    x ^= x >> 17; x *= 0xed5ad4bbu;
    x ^= x >> 11; x *= 0xac4c1b51u;
    x ^= x >> 15; x *= 0x31848babu;
    x ^= x >> 14;
    return x;
}
// Sama pemetaan key->state seperti §3, ganti fmix32 -> triple32 di kedua titik
// (state0 dan output final).
```

4 multiply (vs 3 di `fmix32`) — tetap 32×32→32 murni, tetap GPU-cheap,
avalanche per publikasi asal lebih kuat dari `fmix32`/MurmurHash3
finalizer. **Kandidat cadangan** — uji A dulu (lebih murah, precedent
kode sudah ada), B hanya kalau A FAIL di Tier 1 (§0.2).

**Peringatan verifikasi**: konstanta `triple32` di atas ditulis dari
ingatan pola publikasi Chris Wellons — SEBELUM dipakai, sesi eksekusi
WAJIB cross-check ulang terhadap sumber aslinya (repo hash-prospector
atau publikasi yang sama), sama seperti disiplin
`feedback_verify_external_baselines` memory (jangan percaya replikasi
dari ingatan tanpa verifikasi terhadap sumber otoritatif).

## 5. Struktur kode yang harus dibuat

Reuse pola CLI (`--single`, `--stream`, `--stream-hashed`) dan struktur
pipeline dari sesi-sesi K-small sebelumnya (rujuk
`../2026-9-4_orbit-init-warmup-fix/` sebagai template detail, BUKAN
disalin verbatim — generator ini beda arsitektur total, tidak ada
`ra_init_state_orbit`/`L[]`/`M[]`/`ra_reseed` yang perlu dibawa):

- `candidates/weyl_fmix32.c`, `candidates/weyl_triple32.c` — 2 binary,
  masing-masing implementasi §3/§4, dengan opsi pemetaan key→state yang
  disepakati di §0.3.
- Oracle Python (`combo_prng_weylfinal.py`) — replika byte-exact kedua
  kandidat, mask 0xFFFFFFFF eksplisit tiap step.
- Tier 0 avalanche gate — port `tier0_avalanche_*.py` pola yang sudah
  ada, band SAMA (`LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2`, K=1,
  `BASE_SEED=1`).
- Tier 1 PractRand dual-ordering — port `tier1_gate_*.py` pola yang
  sudah ada, K=1 DAN K=255, Weyl+hashed, staged (64MB→256MB→skala §0.4),
  dual base_key (`0x12345`, `0xABCDEF01`) untuk reproducibility,
  early-kill on hard FAIL.
- **Benchmark 5-angka** (§0.5, langkah BARU tidak ada di kandidat-kandidat
  sebelumnya) — reuse pola `orbit_bench.c`/`bench_release.c` kalau ada
  (`../2026-9-3_orbit-continuous-bench/`, `../2026-9-1_family-
  productionization/`) untuk perbandingan throughput sisi-bersisi.

## 6. Pipeline validasi (urutan wajib, disiplin sama seperti semua sesi K-small sebelumnya)

1. **Oracle vs codegen byte-exact** (`--single key rng` vs oracle Python,
   byte-for-byte, beberapa skala termasuk yang exercise banyak block)
   WAJIB lolos sebelum PractRand apa pun — disiplin berulang di semua 8
   sesi sebelumnya, jangan dilewati.
2. **Tier 0 avalanche** (K=1) untuk kedua kandidat A dan B.
3. **Tier 1 dual-ordering, K=1 DAN K=255** (skala dikonfirmasi §0.4),
   staged screen→confirm→reproducibility, early-kill on FAIL.
4. **Benchmark 5-angka** (§0.5) — HANYA untuk kandidat yang lolos Tier 1,
   bandingkan head-to-head dengan orbit/singleblock produksi.
5. **No re-seed retry on FAIL** (`feedback_no_reseed_on_fail`) — FAIL di
   satu base_key sudah final, jangan retry base_key lain untuk
   "menjelaskan" kegagalan.

## 7. Kriteria hasil

- Kandidat CLEAN-DUAL (K=1 dan K=255, kedua ordering, kedua base_key,
  skala yang disepakati) DAN benchmark tidak lebih lambat dari orbit
  produksi untuk K=1 → layak didiskusikan sebagai kandidat serius. Tetap
  BUKAN keputusan otomatis untuk mengganti/menggabungkan ke `ra_core.c`
  — laporkan, tunggu konfirmasi user terpisah (§0.1).
- A gagal, B (`triple32`) CLEAN-DUAL → `fmix32` 3-multiply memang kurang
  kuat untuk pola input linear murni ini, `triple32` cukup — catat
  sebagai temuan positif untuk arah desain PRNG masa depan juga.
- Keduanya gagal → arsitektur "sekuensial vs counter-based" BUKAN
  penjelasan lengkap sendirian (atau implementasinya salah — cross-check
  dulu byte-exact/avalanche sebelum menyimpulkan arsitektur yang salah).
  JANGAN langsung lompat ke desain lebih berat lagi tanpa diskusi user
  (`feedback_ask_more_on_design_decisions`).

## 8. Peringatan teknis yang wajib diwarisi (jangan mengulang bug lama)

- **No re-seed retry on FAIL** (`feedback_no_reseed_on_fail`).
- **Verifikasi kode SEBELUM percaya statistik**: byte-exact oracle WAJIB
  lolos sebelum PractRand — pelajaran berulang paling sering di repo ini.
- **`fmix32(0)` weak-key pitfall** — lihat §3 catatan wajib, jangan pakai
  `fmix32(key)`/`triple32(key)` telanjang kalau `key=0` bisa terjadi di
  opsi pemetaan (b) tanpa guard.
- **Verifikasi konstanta eksternal (`triple32`) terhadap sumber asli**
  sebelum dipakai, bukan dari ingatan (`feedback_verify_external_baselines`).
- **`splitmix32()` yang sudah ada di repo BUKAN 32-bit murni** (§0.2) —
  jangan dipakai sebagai referensi "sudah terbukti 32-bit", dan jangan
  dipakai LANGSUNG sebagai bagian dari kandidat A/B (itu tetap generator
  64-bit).

## 9. Referensi cepat

- Histori 8 axis gagal + diagnosis posisi: `../2026-9-1_family-
  productionization/PRODUCTION_READINESS_HANDOVER.md` §2b (semua update
  inline, jangan tambah section baru di sana — update lagi setelah ada
  hasil sesi ini).
- `project_orbit_key_avalanche_diagnosis` memory (diagnosis lengkap,
  linked ke semua sesi terkait).
- Template struktur handover + pipeline validasi yang reusable:
  `../2026-9-4_orbit-init-warmup-fix/HANDOVER.md` dan `RESULTS.md`-nya.
- `fmix32`: `../2026-9-1_family-productionization/ra_core.c` L149-156
  (verbatim reuse). `ra_init_state_orbit` L197-207 — TIDAK dipakai
  desain baru ini, hanya referensi nilai konstanta `0x9e3779b7`.
- `splitmix32()` (CATATAN: 64-bit, JANGAN jadi basis "32-bit murni"):
  `../2026-9-1_family-productionization/multikey_stream_hashed.c` L16-22.
- `project_state_size_pareto_frontier` memory — relevan kalau benchmark
  §0.5 menunjukkan speedup besar (bisa mengubah framing crossover
  orbit-vs-singleblock K≈20-21).
- `feedback_orbit_singleblock_benchmark_standard` memory — format 5-angka
  benchmark yang wajib dipakai.
- Setelah selesai (hasil apa pun): update `PRODUCTION_READINESS_HANDOVER.md`
  §2b (inline) dan memory terkait. **Jangan jalankan `/graphify --update`**
  kecuali user secara eksplisit minta lagi di sesi itu (instruksi standing
  dari sesi 2026-09-04, cek apakah masih berlaku di sesi eksekusi).
