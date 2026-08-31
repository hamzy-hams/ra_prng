# Handover: PRNG family benchmark (ra_prng semua varian vs PRNG terkenal)

Ditulis untuk sesi Claude Code baru yang mengimplementasikan dan
menjalankan benchmark ini (dipicu frasa "lanjutkan prng-family-benchmark"
atau serupa). **Status: BELUM DIKERJAKAN** — dokumen ini murni spesifikasi
(handover), ditulis di sesi yang tidak mengimplementasikan kodenya sendiri
(user akan menjalankan ini di sesi lain). Baca dokumen ini sampai selesai
sebelum menulis kode apa pun — semua keputusan desain yang genuinely
open-ended (roster, path source, gate validasi, metodologi) sudah diputuskan
di sini supaya sesi eksekusi tidak perlu riset ulang atau bertanya balik.

## 1. Tujuan

Bandingkan, dalam **satu benchmark yang metodologinya sama untuk semua
kandidat**, seluruh spektrum "ra_prng" (dari baseline paper paling awal
sampai fast-path addressable paling agresif) bersebelahan dengan PRNG-PRNG
non-cryptographic yang umum dipakai sebagai rujukan literatur/industri.
Motivasi: user menemukan bahwa fast-path addressable tanpa `L[]`
(`ra_core_singleblock`, lihat `experiments/2026-8-30_addressable-init-research/HANDOVER_TAHAP6.md`)
memberi percepatan besar untuk kasus reinit sering ("addressable agresif")
— sekarang ingin melihat di mana posisi SELURUH keluarga ra_prng relatif
terhadap dunia luar, bukan cuma relatif terhadap Philox seperti riset-riset
sebelumnya.

## 2. Roster (12 kandidat, path source persis)

| # | Nama | Sumber (read-only, copy byte-for-byte / port) | Kategori validasi |
|---|---|---|---|
| 1 | `ra_prng2` original (paper-exact) | `src/ra_prng2/c/ra_prng2.c` — `ra_core(uint32_t seed, size_t rng, FILE*)`, `ra_init_state(L,M)` **tanpa parameter key** (seed cuma masuk lewat `cons=seed` di `ra_core`, init-nya fixed/seed-independent), `ra_permutation_cycle`, `ra_reseed`. Ini "titik nol" historis proyek — **BUKAN** `benchmarks/comparisons/source/ra_prng2.c` (itu tool shuffle file-based berbeda "ZepFold", jangan tertukar). | Copy byte-for-byte |
| 2 | `winner_wired_v2` (best non-addressable) | `experiments/2026-8-27_operand-position-search/winner_wired_v2.c` — signature sama persis (`ra_core`, `ra_init_state(L,M)` juga tanpa key), recurrence sudah di-pruning+wiring (~6% instruksi lebih sedikit dari original). | Copy byte-for-byte |
| 3 | addressable "continuing" (dengan `L`, reseed, rng bebas) | `experiments/2026-8-30_addressable-init-research/tahap6_bench.c` fungsi `ra_core_baseline` (dan `ra_init_state_addressable_baseline`, `ra_permutation_cycle`, `ra_reseed` di file yang sama) — ini sama dengan `winner_wired_addressable.c`'s `ra_core`. | Copy byte-for-byte |
| 4 | addressable agresif-minimal, K=1 kata/init | `ra_core_singleblock` (file sama dengan #3), dipanggil dengan `rng=1` di titik sweep | Copy byte-for-byte (sudah divalidasi Tahap 6, 9.945/9.945 bit-identik) |
| 5 | addressable agresif-minimal, K=255 kata/init (maksimal tanpa reseed) | `ra_core_singleblock` (sumber sama), dipanggil dengan `rng=255` — **titik sweep 255 WAJIB ada eksplisit** di array sweep (jangan cuma sampai 192 seperti `tahap6_benchmark.py` lama) | Copy byte-for-byte |
| 6 | Philox4x32-10 | `experiments/2026-8-30_addressable-init-research/tahap6_bench.c` — fungsi `philox4x32_10`/`philox4x32_round`/`philox_ctr_increment` **dengan `PHILOX_M0` sudah terkoreksi** (`0xD2511F53U`) dan `run_kat_checks()` sudah ada. **JANGAN** copy dari `benchmarks/comparisons/source/philox.c` atau `benchmarks/comparisons/RNGing_speed/src/philox.c` — keduanya masih punya bug (`PHILOX_M0=0xD256D193U`, itu konstanta Philox**2x32**, bukan Philox4x32). | Copy byte-for-byte (sudah KAT-validated, Random123 `kat_vectors`) |
| 7 | xoshiro256** | Port matematika dari `benchmarks/comparisons/RNGing_speed/src/xoshiro256.c` (raw-throughput-only, tanpa file I/O — lebih gampang diadaptasi ke harness baru daripada `benchmarks/comparisons/source/xoshiro256.c` yang full-CLI). **State-based — seeding-nya (bukan cuma generate) HARUS masuk region yang di-timer reinit-sweep** (lihat §4). | **Wajib KAT baru** — referensi resmi Blackman & Vigna (`xoshiro256starstar.c`, test vectors dari `vigna.di.unimi.it/xorshift/` atau repo resmi `xoshiro`/`xoroshiro` David Blackman & Sebastiano Vigna) |
| 8 | pcg32 | Port dari `benchmarks/comparisons/RNGing_speed/src/pcg32.c`. Seeding pcg32 nyaris gratis (2 multiply-add, mirip Philox) — TIDAK perlu perlakuan khusus seperti xoshiro/MT19937. | **Wajib KAT baru** — referensi resmi O'Neill (`pcg-random.org`, reference `pcg_basic.c`/test vectors resmi) |
| 9 | chacha20 | Port dari `benchmarks/comparisons/RNGing_speed/src/chacha20.c`. CSPRNG — seeding (key+nonce setup) juga bukan nol, masukkan ke region timer sama seperti xoshiro. | **Wajib KAT baru** — RFC 8439 test vectors |
| 10 | `/dev/urandom` | Port dari `benchmarks/comparisons/RNGing_speed/src/dev_urandom.c` (`fopen("/dev/urandom","rb")`+`fread`). **HANYA flat throughput** — tidak ikut reinit-sweep sama sekali (tidak ada konsep "reseed" yang bermakna untuk system entropy; "reinit" di sini cuma re-`fopen`, bukan comparable dengan reseed PRNG deterministik). Konvensi repo: tetap dimasukkan sebagai referensi dunia-nyata (`README.md:16,50`, `docs/STRUCTURE.md:23`), bukan dikecualikan. | Tidak perlu validasi korektnes (baca byte asli OS) |
| 11 | MT19937 | **Implementasi baru dari nol** (tidak ada di repo). State 624-word `uint32_t`, seeding loop non-trivial (sebanding urutan besar dengan `ra_init_state_full`'s loop 256-word) — **wajib** masuk region timer reinit-sweep. | **Wajib KAT** — referensi resmi Matsumoto & Nishimura (`mt19937ar.c`/`mt19937ar.out`, situs resmi `www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt19937ar.html`) |
| 12 | SplitMix64 | **Implementasi baru dari nol** (repo cuma punya versi Python sebagai internal mixer di `experiments/2026-8-30_addressable-init-research/tahap0_prototype.py:119-166`, `splitmix64_seed()`/`splitmix64_next()` — boleh dipakai sebagai referensi struktur logika, TAPI tetap wajib KAT C-level sendiri, bukan asumsi port Python otomatis benar). Seeding nyaris gratis (1 word 64-bit). | **Wajib KAT** — referensi Vigna `splitmix64.c` (`vigna.di.unimi.it`) |

## 3. Arsitektur harness (perluasan pola `tahap5_bench.c`/`tahap6_bench.c`)

Satu file baru, misal `benchmark_all.c` di folder ini (folder ini masih
kosong kecuali `HANDOVER.md` — tulis file baru, JANGAN edit file manapun
di folder lain).

Pola dispatch yang sudah terbukti di `tahap5_bench.c` (variant table untuk
init) dan `tahap6_bench.c` (core table untuk core lengkap) — generalize ke
12 entri:

```c
typedef uint32_t (*core_fn_t)(uint32_t key, size_t rng, FILE *raw_stream);
typedef struct { const char *name; core_fn_t fn; long max_rng; } core_entry_t;
// max_rng = -1 untuk kandidat tanpa batas (semua kecuali #4/#5 yang capped 255)
```

**Poin desain paling penting**: tiap `core_fn_t` implementasi HARUS melakukan
**seed + generate `rng` kata, sekaligus, di dalam satu panggilan** — persis
pola yang sudah dipakai `ra_core_singleblock`/`ra_core_baseline` (init
dipanggil di awal fungsi, bukan di luar loop timer). Ini krusial supaya
biaya seeding kandidat *state-based* (xoshiro256**, MT19937, chacha20) ikut
terhitung wajar di reinit-sweep — BUKAN ditiru dari pola lama Philox di
`tahap6_bench.c`'s `mode_reinit_sweep` yang manual reset `ctr`/`key` di
LUAR fungsi generate (itu valid untuk Philox karena reseed-nya memang cuma
assignment beberapa word, tapi kalau ditiru mentah-mentah untuk
xoshiro/MT19937/chacha20 hasilnya bias — bikin cost model yang salah untuk
sesi ini, jangan diulang).

## 4. Metodologi benchmark

Reuse metodologi `reinit-sweep` (`tahap4/5/6_bench.c`): `CLOCK_MONOTONIC`,
min-of-N-trials (N=3, sama seperti Tahap 4-6), volatile checksum sink
(cegah dead-code-elimination), steady-state `ns/word = best_time / (cycles*K)`.

**Titik sweep K**: reuse array lama
`[1,2,4,8,16,24,32,48,64,80,96,128,192]` **ditambah `255` eksplisit**
(wajib untuk item #5), **DAN diperluas ke atas** (mis. `256, 512, 1024,
4096, 65536, 1000000`) untuk kandidat tanpa batas (semua kecuali #4/#5)
supaya perilaku asimtotik/flat mereka juga terlihat — di titik sweep besar,
`ns/word` steady-state pada dasarnya SAMA dengan throughput flat biasa
(biaya seed teramortisasi habis), jadi tidak perlu benchmark flat terpisah:
cukup **konversi titik sweep terbesar ke MB/s** (`4 bytes / ns_per_word *
1e9 / 1e6`) untuk dibandingkan langsung ke tabel MB/s historis
(`README.md`, `benchmarks/comparisons/RNGing_speed/*.txt` —
xoshiro256**=3574MB/s, pcg32=3065MB/s, philox=1304MB/s,
ra_prng2=745.6MB/s, chacha20=555.5MB/s, dev_urandom=389.6MB/s — pakai ini
sebagai sanity-check arah: kalau angka baru jauh melenceng dari ini untuk
kandidat yang overlap, curigai bug porting sebelum lapor hasil).

Item #4/#5 (addressable agresif-minimal) otomatis berhenti muncul di
titik sweep `K>255` — generalize guard `core->max_rng` yang sudah ada di
`tahap6_bench.c`'s `mode_reinit_sweep` (skip/print pesan jelas, jangan
crash diam-diam).

Item #10 (`/dev/urandom`) HANYA dilaporkan sebagai satu angka flat
throughput (baca N besar sekali, hitung MB/s) — tidak masuk tabel
reinit-sweep sama sekali (kolomnya kosong/N/A di semua baris K).

## 5. Gate validasi (wajib, berlapis per kategori sumber)

Urutan: **verify dulu, baru boleh benchmark** — pola persis
`tahap6_benchmark.py` (jalankan semua KAT dulu, refuse lanjut kalau ada
yang gagal).

- **Copy byte-for-byte (#1, #2, #3, #4, #5, #6)**: spot-check `--stream`
  mode + `cmp` terhadap binary compile-langsung dari sumber aslinya
  (`ra_prng2.c` yang asli, `winner_wired_v2.c` yang asli,
  `winner_wired_addressable.c`/`tahap6_bench.c` yang asli) — beberapa key
  (0, 0xFFFFFFFF, beberapa key acak), beberapa panjang termasuk edge case
  (1, 254, 255 untuk #4/#5). Pola persis yang sudah dipakai
  `HANDOVER_TAHAP6.md` §5 (spot-check, bukan exhaustive lagi — sudah
  exhaustive-validated di Tahap 6 untuk #3/#4/#5, dan `run_kat_checks`
  untuk #6).
- **Port matematika (#7 xoshiro256**, #8 pcg32, #9 chacha20)**: DUA lapis —
  (a) KAT terhadap reference resmi pihak ketiga (lihat kolom kategori
  validasi di roster §2, pola persis `run_kat_checks()` Philox: array
  `{input, expected_output}` hardcoded, `memcmp`, PASS/FAIL per vector,
  exit code nonzero kalau ada yang gagal); (b) **tambahan** spot-check
  `--stream`+`cmp` terhadap binary `benchmarks/comparisons/RNGing_speed/src/*.c`
  yang sudah ada di repo, khusus untuk menangkap bug transkripsi porting
  (KAT resmi memvalidasi algoritmanya benar secara umum, cross-check ini
  memvalidasi PORT-nya ke harness baru tidak salah ketik).
- **Implementasi baru dari nol (#11 MT19937, #12 SplitMix64)**: KAT wajib
  terhadap reference resmi, TIDAK ada cross-check ke repo (tidak ada
  implementasi lama untuk dibandingkan) — jangan percaya angka
  benchmark apa pun dari kandidat ini sebelum KAT-nya PASS.
- **`/dev/urandom` (#10)**: tidak perlu gate korektnes.

Kalau ADA yang gagal validasi: JANGAN lanjut ke benchmark untuk kandidat
itu, laporkan sebagai kandidat gagal-validasi terpisah, bukan menghapus
diam-diam dari tabel akhir.

## 6. File yang TIDAK BOLEH diubah (read-only precedent)

Semua sumber di §2: `src/ra_prng2/c/ra_prng2.c`,
`experiments/2026-8-27_operand-position-search/winner_wired_v2.c`,
`experiments/2026-8-30_addressable-init-research/tahap6_bench.c` (dan
`tahap4_bench.c`/`tahap5_bench.c`/`winner_wired_addressable.c` yang jadi
precedent-nya), `benchmarks/comparisons/RNGing_speed/src/*.c`,
`benchmarks/comparisons/source/*.c`. Semua kerja baru masuk file baru di
folder `experiments/2026-8-31_prng-family-benchmark/` ini saja.

## 7. Non-goals (eksplisit, di luar scope)

- Tidak menyentuh level shuffle/scramble-tool (`scrambler_addressable.c`
  dkk) — fokus generator murni saja.
- Tidak promosi kandidat manapun ke `src/`.
- Tidak menambah PRNG lain di luar 12 kandidat ini kecuali user memutuskan
  lagi secara eksplisit di sesi eksekusi (mis. WyRand, SFC64 sempat
  disinggung sebagai opsi tapi TIDAK dipilih user — jangan tambahkan
  tanpa konfirmasi baru).
- Tidak mengubah formula/recurrence apa pun (semua kandidat "ra_prng" di
  sini adalah copy byte-for-byte dari hasil riset yang sudah divalidasi
  sebelumnya, bukan formula baru).

## 8. Penutup sesi

`CLAUDE.md` proyek ini mewajibkan `/graphify --update` sebelum menutup
tugas riset. **Catatan**: di sesi tempat handover ini ditulis (2026-08-31),
skill `graphify` **tidak terdaftar** di daftar skill yang tersedia — cek
ulang di sesi eksekusi apakah sudah tersedia; kalau belum, flag ke user
alih-alih mengasumsikan langkah ini otomatis selesai.
