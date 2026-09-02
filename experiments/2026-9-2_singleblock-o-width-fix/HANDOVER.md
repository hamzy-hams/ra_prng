# HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup

Status: **spec-only, belum diimplementasikan.** Sesi ini berhenti di tahap
plan (disetujui secara desain lewat diskusi dengan user, tapi eksekusi
sengaja ditunda ke sesi lain — ikuti pola handover-then-execute yang sudah
dipakai di `experiments/2026-9-1_family-productionization/HANDOVER.md`).

## Context

Battery gate `experiments/2026-9-1_production-candidate-battery/` menemukan
defect struktural di `ra_core_singleblock`: PractRand BCFN FAIL tumbuh
monoton untuk K (jumlah word per block/key) kecil sampai K=72, baru bersih
total mulai K≈96 — lihat
`experiments/2026-9-2_singleblock-k-threshold-characterization/RESULTS.md`
untuk tabel lengkap per-K.

Root cause (diverifikasi langsung dari kode,
`experiments/2026-9-1_family-productionization/ra_core.c`,
`ra_permutation_cycle_singleblock` baris 214-236): state awal tiap block
SELALU `a=cons=key, b=it=0, c=0, d=0` — identik untuk semua key/block,
tidak bergantung K. Iterasi pertama (`i=255`) cuma satu putaran mixing
dangkal:

```c
o = M[(uint8_t)(i+6)]<<6 ^ M[(uint8_t)(i+7)]<<7;   // cuma 2 slot M[] tetap
a = (d ^ o) ^ (cons + a);                           // = o ^ (key+key) di iterasi pertama
b = (cons + a) ^ (o + d);
c = rot32((a >> 13) ^ a, b);                        // <- word PERTAMA yang keluar
```

Word pertama ini SELALU lemah, berapa pun K. Menaikkan K (bar aman K≥96,
lihat karakterisasi) kemungkinan besar cuma **mendilusi** proporsi word
lemah itu di stream agregat (1 dari K word per block) — bukan
menghilangkan defect-nya secara struktural. Hipotesis dilusi ini konsisten
dengan pola R yang tumbuh lambat seiring K naik di tabel karakterisasi,
tapi **belum diverifikasi lebih jauh** (mis. lewat uji PractRand skala
lebih besar di K=96, atau uji yang mengisolasi khusus word-pertama-per-block
lintas banyak key) — kalau mau, itu bisa jadi langkah verifikasi tambahan
sebelum/sesudah eksperimen fix di bawah.

## Ide fix dari user (2 tahap, verbatim diparafrase)

1. **Perlebar `o` dari 2 M[] slot ke 8 M[] slot.** User mengingat bahwa di
   algoritma ASLI (sebelum riset pruning), `o` direkonstruksi dari 8 slot
   M[] berbeda, bukan 2. Diverifikasi langsung ke
   `experiments/2026-8-26_operation-pruning-research/pruned_prng.c`,
   `build_o()` baris 73-79:
   ```c
   static inline uint32_t build_o(const uint32_t *M, uint32_t i, uint32_t ops) {
       uint32_t o = 0;
       for (uint32_t e = 0; e < 8; ++e) {
           if (has(ops, (int)e)) o ^= (M[(uint8_t)(i + e)] << e);
       }
       return o;
   }
   ```
   Dengan semua TAP0-7 ON: `o = XOR_{e=0..7} M[(i+e)&0xFF] << e`. Riset
   pruning 2026-8-26 memangkas ini jadi 2 tap (TAP6+TAP7 — persis formula
   sekarang di `ra_core.c`) karena cukup untuk target validasi SAAT ITU
   (single-key/long-stream) — **tapi tidak pernah diuji terhadap pola
   K-kecil/aggressive-reinit** yang baru ditemukan Tahap 6 (September).
   Hipotesis: `o` lebih lebar (8 sumber M[] berbeda, bukan 2) mungkin bikin
   word pertama per block cukup terdifusi meski cuma satu putaran, karena
   entropi masuknya jauh lebih kaya.

2. **Fallback kalau (1) gagal/belum cukup**: internal warm-up — jalankan
   N putaran dibuang (tidak ditulis ke stream, tidak dihitung terhadap
   `rng`) untuk mengevolusi `a,b,c,d` dari state dingin, lalu lanjutkan
   loop utama seperti biasa memakai `a,b,c,d` hasil warm-up itu sebagai
   starting state. Ini MIRIP percobaan warm-up lama yang sudah ditolak
   (butuh ≥128/255 putaran, overhead 5.5-13x tak bisa diterima,
   lihat [[project_multikey_remix_search]]) — bedanya di sini
   dikombinasikan dengan `o` lebar dari (1), dengan harapan difusi
   per-putaran yang lebih kuat berarti N yang dibutuhkan jauh lebih kecil,
   sehingga overhead jadi layak. **Kombinasi `o`-lebar + warm-up-N-kecil
   ini belum pernah dicoba** — riset warm-up lama pakai `o` sempit yang
   sekarang, riset pruning 2026-8-26 pakai `o` lebar tapi tidak pernah
   dikombinasikan dengan warm-up atau diuji di pola K-kecil.

## Rencana eksekusi (belum dijalankan)

### Kandidat A — perlebar `o` ke 8-tap (isolasi 1 variabel)

Buat `diag_wideo_singleblock.c` (self-contained, JANGAN extern-link ke
`ra_core_nomain.o` karena fungsi yang mau diubah itu `static` — pola sama
seperti `diag_init_keyterm_mul.c`/`diag_prereseed.c` sebelumnya):

- Copy verbatim: `rot32`, `fmix32`, `GUARD_L`/`GUARD_M`,
  `ra_init_state_singleblock` (`ra_core.c` baris 205-212, TIDAK diubah —
  fix key=0 yang sudah tervalidasi tetap dipakai apa adanya, supaya hanya
  `o` yang jadi variabel eksperimen).
- `ra_permutation_cycle_singleblock_wideo`: SAMA PERSIS dengan versi
  sekarang (`ra_core.c` baris 214-236) KECUALI baris `o = ...` diganti jadi
  8-tap XOR persis `build_o()` di atas (semua TAP0-7 ON). Jangan ubah
  update formula `a,b,c,d`, jangan tambah ROT_A/ROT_B/SHL9/dst dari
  `pruned_prng.c` — HANYA lebar `o` yang berubah, supaya hasil bisa
  diatribusikan murni ke variabel itu.
- CLI driver meniru
  `experiments/2026-9-2_singleblock-k-threshold-characterization/multikey_stream_k.c`:
  `--stream <base_key> <n> <K>`, reseed key tiap block via Weyl increment
  `0x9E3779B9`, tulis SEMUA word tiap block ke stdout (tidak ada discard).

**Uji K=1 dulu** (paling murah, paling sensitif — baseline lama FAIL di
R≈1069 @1GB): `RNG_test stdin32` checkpoint 256MB→1GB→2GB→4GB.

- BERSIH sampai minimal 4GB (idealnya 8GB, skala yang dulu FAIL): lanjut
  K=2,4,8,16 (1-2GB masing-masing, cukup untuk screening). Kalau semua
  bersih, ukur dampak kecepatan (microbench ns/word `o`-2-tap vs `o`-8-tap
  di K=1 dan K=255, reuse pola `bench_ra_core.c`) — 8 tap = ~4x lebih
  banyak load+shift+xor per putaran, perlu diukur bukan diasumsikan.
- MASIH FAIL tapi R mengecil dari baseline: catat "membaik tapi belum
  cukup", lanjut Kandidat B sebagai KOMBINASI (bukan pengganti).
- R sama sekali tidak berubah: `o` lebar tidak relevan untuk defect ini,
  lanjut LANGSUNG ke Kandidat B dengan `o` SEMPIT (formula sekarang) supaya
  hasil warm-up-nya apple-to-apple dengan percobaan warm-up lama.

### Kandidat B (fallback, hanya jalan jika Kandidat A gagal/belum cukup)

`diag_wideo_warmup_singleblock.c` (extend dari Kandidat A):

- Sebelum loop utama yang menulis output, jalankan N putaran warm-up
  (struktur identik, pakai `o` dari Kandidat A) TANPA menulis ke
  `raw_stream` dan TANPA menghitung terhadap `count`/`rng` — cuma
  mengevolusi `a,b,c,d` dari `(cons,0,0,0)`. Setelah N putaran, lanjutkan
  loop utama seperti biasa memakai `a,b,c,d` hasil warm-up sebagai starting
  state (bukan reset lagi).
- Sweep N kecil dulu (murah, banyak titik): N ∈ {1,2,4,8,16,24,32,48,64} di
  K=1, PractRand 1-2GB per titik untuk triase cepat. Titik yang lolos
  triase dikonfirmasi ke skala penuh (4-8GB, sama seperti Kandidat A).
- **Hitung overhead eksplisit** untuk N minimum yang lolos: di K=1, total
  putaran = N+1 untuk 1 word keluar — overhead factor (N+1)x dibanding
  tanpa warm-up. Bandingkan terhadap microbench nyata (ns/word) dan
  terhadap ambang "5.5-13x tidak bisa diterima" yang jadi alasan penolakan
  percobaan warm-up lama — laporkan angka mentah, JANGAN putuskan sendiri
  soal "layak" — itu keputusan user berikutnya.

### RESULTS.md (ditulis di akhir, folder ini)

Tabel status per kandidat/config (K, N kalau ada, hasil PractRand per
checkpoint, R value BCFN kalau relevan), microbench ns/word, dan bagian
"Rekomendasi" eksplisit menyatakan kandidat mana (jika ada) yang siap
dipertimbangkan untuk diterapkan ke `ra_core.c` — TANPA menerapkannya.
Penerapan ke `ra_core.c` dan resume battery gate
(`2026-9-1_production-candidate-battery/`) tetap keputusan terpisah yang
butuh konfirmasi user eksplisit, sama seperti pola guard-XOR fix
sebelumnya di sesi ini.

## Non-goals (eksplisit)

- TIDAK mengubah `ra_core.c` sama sekali di eksperimen ini.
- TIDAK menjalankan ulang battery gate penuh (avalanche/dieharder/cross-
  correlation/shuffle) — itu nanti setelah kandidat pemenang dipilih dan
  diterapkan, sebagai langkah terpisah.
- TIDAK menambah op lain dari `pruned_prng.c` (ROT_A/ROT_B/SHL9 dst.) —
  hanya lebar `o` yang jadi variabel, sesuai permintaan eksplisit user.

## Cara lanjut di sesi berikutnya

1. Baca handover ini + `RESULTS.md` di
   `experiments/2026-9-2_singleblock-k-threshold-characterization/` untuk
   konteks ambang K.
2. Delegasikan ke 1 fork background (pola sama seperti 4 fork diagnostic
   sesi ini) — kompilasi + banyak run PractRand adalah kerja berat/lama.
3. Fork wajib: build dari
   `../2026-9-1_family-productionization/ra_core.c` sebagai referensi
   (baca saja, jangan copy/link biner lama), verifikasi `ra_core.c` TIDAK
   berubah di akhir kerja (`git diff --stat`), dan jalankan
   `/graphify --update` di folder ini sebelum melapor selesai.
4. Battery gate `2026-9-1_production-candidate-battery/` Step 4-8 tetap
   PAUSED sampai salah satu kandidat di sini terbukti bersih dan diterapkan
   ke `ra_core.c` dengan konfirmasi user eksplisit.
