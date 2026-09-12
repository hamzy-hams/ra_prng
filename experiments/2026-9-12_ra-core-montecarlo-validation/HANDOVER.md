# Validasi `ra_core` dengan workload Monte Carlo nyata (untuk paper baru)

Status: **SELESAI — Workload A dan B sudah diimplementasikan dan
dijalankan**. Hasil lengkap: `RESULTS.md` di folder yang sama. Dokumen ini
tetap dipertahankan sebagai catatan desain-yang-dikunci-sebelum-implementasi
(per `feedback_ask_more_before_implementing`), sesuai Fase 2 rencana yang
disetujui user (lihat plan file sesi 2026-09-12, "Pre-paper punch list:
validasi ra_core dengan workload nyata Monte Carlo").

## Kenapa dokumen ini ada

Paper baru yang direncanakan (tentang `ra_core`: dual-mode
`orbit`/`singleblock` + Orbit Addressing — BUKAN draft ICCS2026 yang ada,
yang cuma membahas `ra_prng2`) butuh bukti lebih dari microbenchmark
sintetis (`bench_release.c`). Instruksi eksplisit user: **uji tiap mode di
regime aslinya masing-masing**, bandingkan dengan generator pembanding
yang dipakai di pola pemakaian yang SAMA.

Prasyarat yang sudah beres sebelum dokumen ini ditulis (Fase 1, sesi sama):
`docs/ERRATA.md` dibuat, `BENCHMARK_METHODOLOGY_FINDINGS.md` dikoreksi —
kesimpulannya sekarang: **gap ~17-21x antara `singleblock` dan `orbit`
genuine** (bukan artefak benchmark), jadi Workload A & B di bawah TIDAK
dirancang untuk "membuktikan gap-nya salah" — melainkan untuk membuktikan
KEDUANYA benar dan cepat DI REGIME masing-masing, dengan workload nyata,
bukan cuma raw ns/word.

## Generator pembanding yang dipakai ulang (tidak reimplementasi)

Dari `experiments/2026-9-1_family-productionization/bench_release.c`
(KAT-verified, byte-for-byte copy dari implementasi yang sudah diverifikasi
di `bench_ra_core.c`/`diag_init_candidates.c`):
- `philox4x32_10()` + `ra_core_philox()` (baris ~207-233).
- `ra_core_xoshiro256()` + `splitmix64_step()` (baris ~235-264).
- PCG32: belum ada di `bench_release.c`, tapi ada implementasi lama di
  `benchmarks/comparisons/source/pcg.c` — cek dulu apakah itu KAT-verified
  atau perlu diverifikasi ulang sebelum dipakai (lihat
  `feedback_verify_external_baselines`: cek terhadap sumber otoritatif +
  KAT, bukan cuma konsistensi internal repo).

Untuk workload MC, fungsi-fungsi ini TIDAK dipakai apa adanya (mereka
hanya mengembalikan word TERAKHIR lewat `fwrite`/return value) — perlu
varian tipis yang mengeluarkan SETIAP word yang di-generate (dikonsumsi
inline oleh loop simulasi, bukan ditulis ke file/buffer besar dulu, supaya
tidak menambah overhead I/O/memori yang tidak relevan dengan biaya
generate itu sendiri). Rekayasa ulang minimal: ubah `if (raw_stream)
fwrite(...)` jadi callback/pemakaian langsung nilai word di loop pemanggil,
formula generatornya SAMA PERSIS (tidak boleh diubah, supaya tetap
comparable dengan angka `bench_release.c` yang sudah ada).

## Workload A — niche `singleblock`: bulk/reinit-per-batch

**Task**: estimasi Monte Carlo untuk π, dipilih karena (a) closed-form
target diketahui persis, (b) formula galat standarnya well-known, (c)
sudah dipakai sebagai uji sanity di tool `ent` yang project ini pakai
sendiri untuk validasi entropi — jadi konsisten dengan konvensi yang
sudah ada, bukan pilihan ad-hoc.

- **Struktur**: N = 1.000.000 trial independen. Tiap trial di-reseed
  dengan key baru (`key = trial_index * 2654435761u + 1u`, pola yang sama
  dipakai `bench_release.c` untuk key sintetis), lalu generate TEPAT 255
  word (K=255, sweet-spot tervalidasi `singleblock`, dari
  `docs/PITCH.md` §5). Dari 255 word itu, ambil 254 (127 pasang `(x,y)`,
  masing-masing dinormalisasi ke `[-1,1]` dari 32-bit uniform), cek titik
  di dalam unit circle, akumulasi `inside_count`. Word ke-255 dibuang
  (supaya tetap generate persis K=255, bukan 254, biar match sweet-spot).
- **Generator dibandingkan**: `ra_core_singleblock`, `xoshiro256**`,
  `PCG32` — ketiganya dipanggil dengan pola reinit-per-trial yang SAMA
  PERSIS (ini niche ketiganya, bukan yang dipermasalahkan sesi
  sebelumnya).
- **Kriteria penerimaan statistik** (dikunci sebelum run):
  - Total titik = 127 × 1.000.000 = 127.000.000.
  - Standard error gabungan: `SE = sqrt(π(4-π)/n_total) ≈ 1.8146 /
    sqrt(127.000.000) ≈ 1.61×10⁻⁴`.
  - **PASS** kalau `|estimasi_π - π| < 4×SE` (~1.6 CI 99.99%) untuk
    KETIGA generator. Kalau ada yang gagal, itu sinyal bias/korupsi
    statistik pada generator itu — stop, jangan retry seed lain (lihat
    `feedback_no_reseed_on_fail`).
- **Metrik yang dilaporkan**: (a) wall-clock generate+hitung untuk 1.000.000
  trial penuh (real end-to-end, bukan ns/word sintetis), diulang 5x
  (min-of-5, konsisten `bench_real_reinit4`); (b) estimasi π akhir + apakah
  PASS/FAIL kriteria di atas, untuk ketiga generator.

## Workload B — niche `orbit`: continuous/addressed, sedikit alamat panjang

**Task**: sesuai target desain asli Orbit Addressing
(`experiments/2026-8-30_addressable-init-research/HANDOVER.md:108-115`,
"sedikit alamat, masing-masing jalan panjang", eksplisit contoh "1000
model paralel"). Dipilih: **Asian option** (opsi rata-rata harga, payoff
`max(mean(S_t) - K_strike, 0)`) via simulasi Geometric Brownian Motion —
BUKAN European option (yang cuma butuh nilai akhir, bisa "curang" kalau
generatornya dipotong pendek) — Asian option payoff betul-betul butuh
SELURUH lintasan panjang, jadi memaksa pemakaian genuine continuous
stream, bukan sekadar 1 draw per alamat.

- **Struktur**: 1.000 path independen, tiap path T=10.000 timestep (jadi
  ~10.000-20.000 word per path tergantung berapa word/timestep dibutuhkan
  transformasi normal). Tiap path diturunkan langsung dari 1 alamat lewat
  Orbit Addressing: `key = key_base` (Weyl constant `0x9e3779b7`) di-
  increment per path index — pola "chaining across keys, addr_hi fixed"
  yang SUDAH divalidasi (`docs/PITCH.md` §5), BUKAN pola "sequential
  addr_hi chaining untuk 1 key" yang eksplisit sudah diketahui GAGAL
  PractRand (`docs/PITCH.md` §8) — jangan pakai pola itu.
- **Generator dibandingkan**: `ra_core_orbit` (mode continuous, 1 panggilan
  `ra_core_orbit(key_i, addr_hi_fixed, N_words_path, ...)` per path) vs
  `philox4x32-10` di mode counter-addressed native-nya (Philox memang
  didesain untuk lompat ke stream #i lewat counter — pembanding yang adil,
  bukan yang dipermasalahkan sesi sebelumnya) vs `xoshiro256**` (jump-ahead
  standar per stream, atau reseed per stream kalau jump-ahead belum
  diimplementasi di repo ini — cek dulu, jangan asumsikan ada).
- **Kriteria penerimaan statistik**:
  - (b) **Konsistensi lintas-generator**: rata-rata payoff Asian-option
    dari ketiga generator harus saling konsisten dalam batas
    `3× SE_gabungan` (SE dihitung dari varians antar-path per generator,
    standar Monte Carlo, BUKAN dibandingkan ke closed-form — Asian option
    tidak punya closed-form sederhana, jadi kriterianya "generator-generator
    saling setuju", bukan "cocok rumus"). Kalau ada yang menyimpang jauh di
    luar itu, itu sinyal bias — stop dan investigasi, jangan retry seed.
  - (c) **Independensi cross-stream** (mengisi gap eksplisit di
    `docs/PITCH.md` §8): dari 1.000 stream Orbit Addressing, ambil sample
    representatif (mis. 50 pasang stream acak dari C(1000,2) yang
    mungkin), hitung koefisien korelasi Pearson antar-word raw output
    (bukan payoff akhir) untuk tiap pasang. **PASS** kalau semua koefisien
    korelasi di bawah ambang batas null-hypothesis (`≈ 1.96/sqrt(n_words)`
    untuk 95% CI dua-sisi terhadap korelasi nol) — kalau ada yang lolos
    ambang, catat sebagai temuan (bukan otomatis FAIL total, tapi WAJIB
    dilaporkan eksplisit di paper, bukan didiamkan). Opsional
    (kalau waktu ada): jalankan PractRand pada output ter-interleave dari
    sample stream itu sebagai uji tambahan yang lebih kuat.
- **Metrik yang dilaporkan**: (a) wall-clock 1.000 path penuh (5 trial,
  min-of-5); (b) hasil konsistensi lintas-generator; (c) hasil independensi
  cross-stream — PASS/FAIL eksplisit, bukan diasumsikan.

## Urutan implementasi

1. Workload A dulu (lebih sederhana, tidak butuh transformasi normal/GBM,
   tidak butuh cross-stream test) — implementasi + verifikasi statistik +
   `RESULTS.md` parsial untuk Workload A.
2. Cek/verifikasi PCG32 dari `benchmarks/comparisons/source/pcg.c` (KAT
   dulu, per `feedback_verify_external_baselines`) sebelum dipakai di
   Workload A.
3. Workload B (lebih kompleks: GBM multi-step, transformasi normal dari
   uniform 32-bit, cross-stream independence test) — implementasi +
   verifikasi statistik + cross-stream test + `RESULTS.md` untuk Workload B.
4. Gabungkan ke satu `RESULTS.md` akhir untuk folder ini, dengan bagian
   terpisah A dan B.

## Yang TIDAK dilakukan (di luar scope dokumen ini)

- Tidak menguji `ra_prng2`/`ra_prng3` di workload ini (paper baru ini
  fokus `ra_core`).
- Tidak membuat closed-form/analytic ground truth untuk Asian option
  (pakai kriteria konsistensi antar-generator, bukan pembanding rumus).
- Tidak menguji generator lain di luar Philox4x32-10/Xoshiro256**/PCG32
  (3 pembanding standar yang sudah dipakai konsisten di seluruh
  `bench_release.c`/paper ICCS2026).
- Tidak menjalankan di mesin kedua di dokumen ini (itu Fase 3 rencana,
  terpisah, setelah kedua workload ini selesai).
