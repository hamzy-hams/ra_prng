# Handover: Tahap 5 — Optimasi Biaya Init Addressable

Ditulis untuk sesi Claude Code baru yang dipicu frasa "lanjutkan
addressible" / "lanjutkan riset addressable". **Baca file ini SEBAGAI TITIK
MASUK untuk Tahap 5** (bukan `HANDOVER.md` utama — baca itu hanya untuk
konteks Tahap 0-4, yang sudah dirangkum penuh di sini juga).

Ini dokumen persiapan: riset pendahuluan (struktur kode + disassembly) sudah
dikerjakan dan hasilnya final di bawah. **Tahap 5 sendiri belum dikerjakan**
— tidak ada kode/file baru untuk Tahap 5 di disk, tidak ada compile/benchmark
Tahap 5 yang sudah jalan. File ini murni rencana + data yang sudah dikumpulkan
supaya sesi eksekusi tidak perlu menggali ulang.

## 1. Status & tujuan

Tahap 0-4 SELESAI (2026-08-30, detail lengkap di `RESULTS.md`/`HANDOVER.md`
di folder yang sama):

- Kandidat 5 (formula `ra_init_state_addressable`, lihat §2) dibekukan
  Tahap 1, diimplementasi C Tahap 2, tervalidasi statistik penuh Tahap 3
  (128GB PractRand bersih, 0 collision sampai 500.000 key, 0 pasangan
  cross-correlation ter-flag).
- Tahap 4 mengukur kecepatan vs Philox4x32-10 **produksi** (setelah bug
  konstanta Philox di 3 file repo dikoreksi khusus untuk benchmark ini —
  detail di `RESULTS.md`, tidak relevan lagi untuk Tahap 5, jangan diulang):
  - Biaya init murni: `ra_init_state_addressable()` **49.87 ns/call** vs
    satu panggilan `philox4x32_10()` **10.06 ns** → rasio **4.96x**.
  - Break-even throughput sweep: **N\* ≈ 49 kata** 32-bit/alamat.
  - Break-even reinit-frequency empiris: **K\* ≈ 71 kata** 32-bit/alamat.
  - Di atas ~50-70 kata/alamat, mode reuse/continue addressable menang
    **~15-22% sustained** atas Philox produksi.

**Tujuan Tahap 5**: turunkan biaya init (~49.87ns) supaya N\*/K\* turun lebih
jauh — makin murah init, makin luas skenario (makin pendek lifetime per
alamat) di mana desain addressable menang atas Philox. **Ini optimasi
implementasi (formula-preserving), BUKAN redesign formula** — lihat gerbang
validasi di §6 untuk batas tegas antara keduanya.

## 2. Formula & fakta struktural (sudah final, tidak perlu digali ulang)

Sumber: `winner_wired_addressable.c` baris 35-60 (disalin byte-for-byte juga
ke `tahap4_bench.c` baris 44-65 — kedua file ini **read-only precedent**,
lihat §7):

```c
static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

#define ADDR_L_MIX_CONST 0x9e3779b7u  // Tahap 1, lihat HANDOVER.md

static void ra_init_state_addressable(uint32_t *L, uint32_t *M, uint32_t key) {
    for (int i = 0; i < 256; ++i) {
        uint32_t r = key ^ (uint32_t)i;
        uint32_t l_val = (uint32_t)(i * ADDR_L_MIX_CONST + 0x9e3779b7u * key);
        uint32_t m_val = (uint32_t)(i * 0x06a0dd9bu + 0x06a0dd9bu * key);
        L[i] = rot32(l_val, r);
        M[i] = rot32(m_val, r);
    }
}
```

Fakta kunci:
- **Ke-256 iterasi sepenuhnya independen** — tidak ada iterasi yang membaca
  `L`/`M` dari iterasi lain (pure write), hanya fungsi dari `i` dan `key`
  tetap. Tidak ada gather/scatter yang dibutuhkan untuk vektorisasi.
- `rot32`'s shift amount `r = (key^i) & 31` bervariasi per-lane (bukan
  shift tetap) — butuh instruksi variable-shift untuk SIMD (`sllv`/`srlv`
  atau rotate-langsung).
- `L[256]`/`M[256]` dipakai penuh 0-255 di `ra_permutation_cycle`/
  `ra_reseed` (mis. `M[(uint8_t)(i+6)]`, indeks byte penuh) — **tidak bisa**
  mengurangi jumlah iterasi tanpa mengubah formula. Lihat §5 "Ruled out".

## 3. Temuan disassembly (sudah dijalankan read-only, sesi persiapan 2026-08-30)

Dijalankan `objdump -d` atas `tahap4_bench` (binary yang SUDAH ADA di disk,
dikompilasi `gcc -O3 -march=native -std=gnu17 -include stdalign.h`) — **tidak
ada file yang diubah/dikompilasi ulang untuk temuan ini**. Sesi Tahap 5
sebaiknya konfirmasi ulang secara independen di build sendiri sebelum
mempercayai penuh (mungkin ada drift compiler/flag).

- CPU mesin uji: **Intel Core i3-1115G4 (Tiger Lake, 2C/4T, AVX-512-capable)**.
- GCC `-O3 -march=native` **sudah auto-vectorize** loop `ra_init_state_addressable`
  ke AVX-512VL, 8-lane (YMM/256-bit), termasuk:
  - `vpternlogd $0x28` — fusi 3-input boolean untuk `(i^key)&31` dalam satu
    instruksi (bukan XOR lalu AND terpisah).
  - `vpmulld` ×2/iterasi untuk suku perkalian.
  - `vpsrlvd`/`vpsllvd` (variable-shift) + `vpord` untuk `rot32` — compiler
    SUDAH pakai instruksi variable-shift yang tadinya dikira perlu ditulis
    manual.
  - `vmovdqa` aligned store langsung ke `L`/`M` — `alignas(64)` sudah
    berfungsi, tidak ada win alignment tersisa.
- **Preseden identik**: `experiments/2026-8-29_parallelization-research/RESULTS.md`
  bagian Q4 — kernel serupa (independent-lane, variable-rotate) di-hand-SIMD
  pakai AVX2 intrinsics, hasilnya **15% LEBIH LAMBAT, 20% lebih banyak
  instruksi** daripada auto-vectorization compiler yang memakai fusi
  `vpternlogq` serupa, di mesin yang sama. **Jangan mengulang rewrite SIMD
  penuh sebagai langkah pertama** — lihat Rank 4 di §4.
- **Faktorisasi aljabar sudah optimal**: formula `i*C + key*C` (konstanta
  sama untuk kedua suku) sudah difaktorkan compiler jadi `C*(i+key)`
  (`vpaddd` sekali, lalu 2x `vpmulld` reuse) — **tidak ada win
  precompute/cache lagi**, ini jumlah perkalian minimum yang mungkin untuk
  bentuk formula ini.
- **Satu celah nyata dan sempit**: CPU punya AVX-512F/VL sehingga instruksi
  rotate-langsung **`vprolvd`/`vprorvd`** (1 instruksi, bukan 3) tersedia di
  hardware (`_mm256_rolv_epi32`/`_mm256_rorv_epi32` di C), tapi GCC **tidak**
  memakainya — tetap pola `sllv+srlv+or` (dikonfirmasi: 0 hit `vprolv`/
  `vprorv` di seluruh disassembly). Ini target optimasi paling
  terbukti/berisiko-rendah.
- `-mprefer-vector-width=512` **belum pernah dicoba**. Kode sekarang pakai
  YMM 256-bit walau semua opcode-nya instruksi AVX-512VL yang bisa jalan di
  ZMM 512-bit (GCC default cap lebar untuk hindari downclocking di client
  part). Beban kerja init ini adalah **burst pendek** (32 trip loop per
  panggilan, diapit kode scalar/branchy lain), bukan sustained heavy —
  risiko downclocking Q4 mungkin tidak berlaku sama di sini, tapi ini WAJIB
  diuji empiris, jangan diasumsikan menang atau kalah.

## 4. Kandidat optimasi, berperingkat

Semua kandidat berikut **formula-preserving** (output bit-identik) kecuali
disebut lain — lihat gerbang validasi §6.

### Rank 1 — `-mprefer-vector-width=512` (flag saja, tanpa ubah kode)

Recompile dengan flag ini menggandakan lane/iterasi (16 vs 8), separuh trip
loop (16 vs 32). Effort: trivial. Risiko: downclocking AVX-512 di beban
sustained (preseden Q4) — **WAJIB diukur end-to-end** lewat
`mode_throughput`/`mode_reinit_sweep` (bukan cuma `mode_init_cost`
terisolasi), karena kalau downclock terjadi, bagian scalar/branchy
`ra_permutation_cycle` (dominan biaya total) bisa lebih dirugikan daripada
untung yang didapat init.

### Rank 2 — Substitusi `vprolvd`/`vprorvd` intrinsic khusus untuk `rot32`

Tulis varian `ra_init_state_addressable` yang eksplisit pakai
`_mm256_rolv_epi32` (AVX-512VL, `<immintrin.h>`) untuk rotate, menggantikan
pola `sllv+srlv+or` (3 instruksi → 1 instruksi, hemat ~4 instruksi vektor per
trip 8-lane dari ~13 yang ada — perkiraan 10-20% pengurangan instruksi loop
body). **Beda struktural dari preseden Q4 yang kalah**: Q4 punya cross-lane
dependency (`L[d]` data-dependent per lane, butuh gather/scatter), sedangkan
loop init ini murni independent write — taruhan yang jauh lebih masuk akal.
Catatan: `vprolvd`/`vprorvd` sudah otomatis mod-32 shift count, jadi masking
eksplisit `&31` mungkin bisa dihapus — **wajib verifikasi bit-identik**
kalau ini dilakukan, jangan asumsi. Effort: moderate (tulis+verifikasi 1
fungsi intrinsics, tanpa cross-lane logic). Risiko fungsional rendah (mudah
dicek bit-identik), payoff belum pasti — harus dibenchmark, jangan
diasumsikan menang.

### Rank 3 — Unroll loop

Coba `-funroll-loops` (flag saja) dulu; kalau tidak cukup, unroll manual
2 chunk (dual accumulator independen) untuk memutus rantai dependency
loop-carried tunggal (`vpaddd %ymm9,%ymm3,%ymm3` di disassembly — hanya 1
akumulator `i`, tidak ada software pipelining). Effort: rendah (flag) sampai
moderate (manual). Risiko sangat rendah, tetap wajib verifikasi bit-identik
untuk versi manual (lebih rawan salah daripada flag compiler).

### Rank 4 (deprioritized, referensi saja) — Rewrite SIMD manual penuh

**Jangan dikerjakan sebagai langkah pertama.** Preseden Q4 + temuan §3
(compiler sudah auto-vec ke AVX-512VL dengan fusi setara tulisan tangan)
menunjukkan rewrite penuh kemungkinan besar kalah dari kombinasi Rank 1-3.
Kalau Rank 1-3 sudah habis dan user eksplisit ingin lanjut, baru pertimbangkan
ini — pakai AVX-512 (ZMM 16-lane) langsung, bukan AVX2.

### Ruled out — Mengurangi jumlah iterasi (<256)

**Tidak mungkin tanpa mengubah formula.** `L[256]`/`M[256]` diakses penuh
0-255 di `ra_permutation_cycle`/`ra_reseed` (indeks byte penuh, mis.
`M[(uint8_t)(i+6)]`) — mengurangi ukuran state = formula baru, identik
efeknya dengan mengganti formula sepenuhnya. **Jangan dikerjakan diam-diam
di Tahap 5.** Catat sebagai kemungkinan "Tahap 6" terpisah (redesign state
size + Tahap 0-3 penuh diulang) hanya kalau user eksplisit minta ke sana.

## 5. Rencana file & eksperimen

Ikuti konvensi repo: **setiap tahap = file baru, tidak pernah edit file
tahap sebelumnya in-place** (lihat §7 untuk daftar read-only lengkap).

1. **`tahap5_bench.c`** (baru) — fork struktur `tahap4_bench.c` (mode
   `init-cost`/`throughput`/`reinit-sweep`/`verify`/`--stream` dipertahankan
   verbatim untuk perbandingan apples-to-apples dengan angka Tahap 4).
   Kompilasi beberapa varian `ra_init_state_addressable_vN_*` berdampingan
   dalam satu file (mis. `_v0_baseline` = salinan kontrol, `_v1_rolv` =
   Rank 2, dst.), masing-masing dibenchmark independen lewat `mode_init_cost`
   DAN end-to-end lewat `mode_throughput`/`mode_reinit_sweep`. Untuk Rank 1,
   kompilasi binary terpisah (mis. `tahap5_bench_zmm512`) dengan
   `-mprefer-vector-width=512` ditambahkan ke compile line, supaya efeknya
   terisolasi bersih dari Rank 2/3.
2. **Validasi kebenaran**: mode `--stream <key> <n>` (copy dari
   `tahap4_bench.c`) dibandingkan bit-identik terhadap
   `./winner_wired_addressable --stream <key> <n>` untuk key: `0`,
   `0xFFFFFFFF`, beberapa key acak, dan key `0..31` (cover semua kombinasi
   rotate amount karena `r=(key^i)&31` dan `i` sudah mencakup 0-255 untuk
   key tunggal manapun, tapi test beberapa key beda tetap penting untuk
   suku `+key` di formula). Pakai `cmp`/diff byte, pola sama seperti Tahap 2
   & 4. **Kandidat yang gagal cek ini didiskualifikasi langsung** — jangan
   lanjut ke benchmark kecepatan untuknya.
3. **Validasi kecepatan**: metodologi identik Tahap 4 — `clock_gettime
   (CLOCK_MONOTONIC)`, min-of-trials (`trials_for_n` untuk throughput,
   `TRIALS=3` untuk reinit-sweep), `volatile` checksum sink cegah DCE.
   **Pakai sweep N/K yang SAMA** dengan yang ada di
   `tahap4_results_throughput.json`/`tahap4_results_reinit-sweep.json` (baca
   file itu untuk titik sweep persis) supaya N\*/K\* baru bisa dibandingkan
   apples-to-apples dengan 49/71 lama.
4. **Kalau kandidat diadopsi** (menang jelas, tanpa regresi, bit-identik
   terverifikasi): promosikan ke file baru `winner_wired_addressable_v2.c`
   (ikuti pola `winner_wired.c` → `winner_wired_v2.c`), komentar header
   jelaskan apa yang berubah dari `winner_wired_addressable.c` dan bahwa ini
   formula-preserving + bit-identical-verified, kutip nomor Rank yang
   diadopsi.

## 6. Gerbang validasi statistik (aturan mekanis, bukan penilaian bebas)

- **Kalau output `--stream` bit-identik** terhadap `winner_wired_addressable`
  di semua key yang diuji: fungsi matematisnya identik, cuma lebih cepat.
  **TIDAK perlu mengulang Tahap 3** — jaminan statistik (128GB PractRand, 0
  collision, 0 cross-correlation flag) otomatis berlaku karena byte
  outputnya sama persis. Rank 1, 2, 3 di §4 semuanya dirancang masuk kategori
  ini.
- **Kalau output BERBEDA** (mis. bug masking di substitusi `vprolvd`, atau
  eksperimen apa pun di luar §4 yang mengubah formula, termasuk "Ruled out"
  di §4): **wajib** diperlakukan sebagai formula baru — Tahap 0 penuh
  (avalanche/periodicity smoke test di prototipe Python) lalu Tahap 3 penuh
  (128GB PractRand + collision scan + cross-correlation) sebelum diadopsi.
  **Jangan dilewati/disingkat**, dan jangan biarkan near-miss (bug masking
  halus) lolos seolah-olah formula-preserving — cek bit-identik harus benar-
  benar 100% cocok, bukan "kelihatannya mirip".

## 7. Batasan read-only (berlaku juga untuk Tahap 5)

Jangan pernah mengubah in-place:
- `src/ra_prng2/*`, `src/ra_prng3/*`
- `winner_wired.c`, `winner_wired_v2.c` (di
  `experiments/2026-8-27_operand-position-search/`, termasuk semua varian
  `winner_wired*`)
- `experiments/2026-8-29_parallelization-research/*.py` dan
  `simd_prototype.c`
- 3 file Philox buggy: `benchmarks/comparisons/source/philox.c`,
  `philox_amortized.c`, `benchmarks/comparisons/RNGing_speed/src/philox.c`
  (bug konstanta `PHILOX_M0` diketahui, sengaja tidak diperbaiki — di luar
  scope, lihat `RESULTS.md` Tahap 4)

Per konvensi repo (setiap tahap = file baru), perlakukan juga sebagai
read-only precedent (jangan edit in-place, walau tidak ada larangan eksplisit
tertulis sebelumnya):
- `winner_wired_addressable.c`
- `tahap4_bench.c`

Semua kandidat Tahap 5 wajib jadi file baru di
`experiments/2026-8-30_addressable-init-research/`.

## 8. Kondisi berhenti / pelaporan eksplisit

- Kalau Rank 1 regresi throughput end-to-end (downclocking nyata) → jangan
  adopsi, laporkan sebagai temuan terkonfirmasi (menjawab risiko yang
  sebelumnya ditandai belum dievaluasi di Q4 RESULTS.md), lanjut ke Rank 2/3
  saja.
- Kalau semua kandidat cuma menggeser N\*/K\* sedikit (mis. 49→40) →
  laporkan itu sebagai temuan valid: "compiler sudah dekat batas praktis
  untuk formula ini, penurunan besar butuh formula baru = Tahap 6 terpisah".
  **Jangan memaksa "menang"** dengan mengorbankan formula beku atau memoles
  angka microbenchmark yang rapuh.
- Kalau tidak ada kandidat yang menang jelas sama sekali → itu juga temuan
  sah (auto-vectorization sudah near-optimal untuk pola ini), bukan
  kegagalan tugas.
- **Sebelum menutup Tahap 5**: update `RESULTS.md`/`HANDOVER.md` dengan
  hasil (kandidat mana menang/kalah, berapa, N\*/K\* baru atau alasan tidak
  berubah, apakah Tahap 3 re-run terpicu), jalankan `graphify update .`
  (CLI dulu; manual `/graphify --update` fallback dengan `scan_corpus`
  LENGKAP — jangan ulangi pitfall yang didokumentasikan di memory
  `feedback_graphify_update_workflow`), dan update memory
  `project_addressable_init_research.md`.
