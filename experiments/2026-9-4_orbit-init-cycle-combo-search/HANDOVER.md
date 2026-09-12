# Handover: search tertarget init-formula × cycle-transform untuk `ra_core_orbit`, wajib dual key-ordering

**Status folder ini: BELUM DIKERJAKAN — dirancang siap-eksekusi, ditulis
supaya sesi lain (atau user) bisa langsung jalan tanpa perlu merancang
ulang dari nol.** Tidak ada kode/hasil di folder ini sama sekali sampai
tulisan ini.

## 1. Kenapa search ini perlu (ringkasan, baca sumber untuk detail lengkap)

- `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`: gate "orbit
  K-small defect CLOSED" (2026-09-04) ternyata cuma pernah divalidasi
  dengan urutan key **Weyl-stepped**. `ra_core_orbit` K=1 dengan urutan
  key **hashed (SplitMix32)** FAIL PractRand parah dan reproducible
  (`FPF-14+6/16` eskalatif, R=+132.3 @8GB, dua base_key independen,
  dikonfirmasi bukan bug harness). K=255 (kedua urutan) dan K=1+Weyl tetap
  bersih.
- `../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`: satu hipotesis fix
  sudah diuji dan **DITOLAK**. Analisis statis menemukan
  `ra_init_state_orbit`'s formula `l_val=(i*const)*keyterm` selalu nol
  persis di `i=0` (`L[0]`/`M[0]` selalu nol untuk key apa pun), dibaca
  sebagai salah satu dari 8 tap `o` di siklus pertama tiap panggilan. Fix
  (geser index jadi `i+1`) **diverifikasi benar-benar menghilangkan** nilai
  nol itu, tapi PractRand hasilnya nyaris identik (R=+135.2 patched vs
  R=+132.3 unpatched @8GB) — tap-nol BUKAN (satu-satunya) mekanisme
  penyebab gap ini.
- Ditemukan sesi ini (grep manual, belum pernah dicatat sebelumnya):
  `../2026-9-2_singleblock-cycle-combo-search/` — combo-search yang
  menghasilkan formula produksi `w8_f10_i0` — **tidak pernah menguji
  urutan key hashed sama sekali**, Tier 0/1/Promotion semuanya
  Weyl-only. Ini celah metodologi yang sama persis yang membuat gap K=1+
  hashed di atas lolos tanpa terdeteksi selama ini.

**Keputusan user (2026-09-04)**: bukan lanjut tebak-satu-hipotesis, dan
bukan juga ablasi total dari nol — search tertarget yang meng-cross axis
**init-formula** (baru, belum pernah dieksplorasi) dengan axis
**cycle-transform** (dipersempit ke kandidat yang sudah terbukti solid di
combo-search lama), **dengan kriteria lolos WAJIB dual-ordering (Weyl DAN
hashed) sejak Tier 1** — bukan dievaluasi ulang belakangan seperti yang
terjadi pada gate lama.

## 2. Scope: fungsi mana yang divariasikan

Target: `ra_core.c` kanonik
(`../2026-9-1_family-productionization/ra_core.c`), fungsi `static`:
- `ra_init_state_orbit` (baris ~197-207) — **axis 1, DSL BARU**.
- `ra_permutation_cycle_orbit` (baris ~217-252) — **axis 2, DSL REUSE
  dipersempit** dari `../2026-9-2_singleblock-cycle-combo-search/recipes.py`.

`ra_hash`/`ra_reseed` (baris 166-183) TIDAK divariasikan — dicopy verbatim
tak berubah (mekanismenya sendiri adalah track terbuka terpisah, lihat
`../2026-9-4_orbit-reseed-postboundary-bcfn/HANDOVER.md`, belum
dieksekusi, di luar scope search ini).

## 3. DSL axis 1: init-formula (8 kandidat)

Formula saat ini (kontrol, wajib byte-identik dengan produksi):

```c
uint32_t keyterm_l = fmix32(GUARD_L ^ (0x9e3779b7u * key));
uint32_t keyterm_m = fmix32(GUARD_M ^ (0x06a0dd9bu * key));
for (int i = 0; i < 256; ++i) {
    uint32_t r = key ^ (uint32_t)i;
    uint32_t l_val = (uint32_t)(IDX(i) * 0x9e3779b7u) * keyterm_l;   // shared
    uint32_t m_val = (uint32_t)(IDX(i) * 0x06a0dd9bu) * keyterm_m;   // shared
    L[i] = rot32(l_val, r);
    M[i] = rot32(m_val, r);
}
```

**Sub-axis `IDX_XFORM`** (pengganti bare `i` di perkalian) — 4 pilihan:

| nama | `IDX(i)` | Catatan |
|---|---|---|
| `linear` | `i` | kontrol = formula produksi saat ini, termasuk cacat M[0]/L[0]=0 |
| `linear_p1` | `i + 1` | **sudah diuji standalone di `2026-9-4_orbit-init-zerotap-fix/`, GAGAL sendirian** — tetap dimasukkan di sini sebagai satu titik data dalam kombinasi, bukan diulang terisolasi |
| `odd` | `2*i + 1` | selalu ganjil untuk semua `i`, beda karakter dari `i+1` (yang genap separuh waktu) |
| `hashed` | `fmix32((uint32_t)i) \| 1` | scalar per-index sepenuhnya diacak, bukan fungsi linear dari `i` — kandidat paling agresif |

**Sub-axis `KEYTERM_MODE`** (uji hipotesis "reuse satu `keyterm` scalar di
256 entry terlalu tipis", disebut sebagai hipotesis belum-diuji di
`2026-9-4_orbit-init-zerotap-fix/RESULTS.md` §"apa yang masih terbuka") —
2 pilihan:

| nama | formula `l_val`/`m_val` | Catatan |
|---|---|---|
| `shared` | seperti di atas — `keyterm_l`/`keyterm_m` dihitung SEKALI, dipakai ulang di 256 entry | kontrol = formula produksi |
| `per_index` | `l_val = fmix32(GUARD_L ^ (0x9e3779b7u * key) ^ IDX_L(i))` (dan analog untuk `m_val`/GUARD_M/0x06a0dd9b) — index dilipat KE DALAM argumen `fmix32`, bukan cuma menskalakan hasil hash bersama. **Tidak ada perkalian luar terpisah** — `fmix32` sendiri sudah menggantikan langkah `* keyterm`. Rotasi `r = key ^ i` tidak berubah. | 1 `fmix32` call per index (256x lebih mahal dari `shared` di loop init), tapi menjawab langsung apakah "berbagi satu scalar" itu sendiri (bukan cuma kasus tepi `i=0`) adalah titik lemahnya |

4 × 2 = **8 kandidat**, penamaan `init_<IDX_XFORM>_<KEYTERM_MODE>`, mis.
`init_linear_shared` (= kontrol/produksi), `init_hashed_per_index`
(paling agresif).

## 4. DSL axis 2: cycle-transform (4 kandidat kurasi, BUKAN full 108)

Ambil langsung dari 11 survivor Promotion Tier
`../2026-9-2_singleblock-cycle-combo-search/RESULTS.md` (semua width=8 —
tap width=8 WAJIB per temuan combo-search itu, 0/6 kandidat width=2/4
lolos Promotion Tier walau lolos Tier 1; jangan buka lagi axis tap-width
di sini). Dipilih 4 yang paling beda struktur (bukan asal 4 teratas),
`c_stmt()` diambil verbatim dari `recipes.py`'s `FINALIZER_RECIPES`:

| id | finalizer | C statement (append setelah `c = rot32((a>>13)^a, b);`) | Alasan dipilih |
|---|---|---|---|
| `w8_f10_i0` | `XORSHIFT(17)` | `c ^= c >> 17u;` | **formula produksi saat ini** — wajib jadi baseline pembanding |
| `w8_f9_i0` | `XORSHIFT(16)` | `c ^= c >> 16u;` | varian shift XORSHIFT tunggal terdekat dari produksi |
| `w8_f28_i0` | `MUL(0xc2b2ae35)->XORSHIFT(13)` | `c = (uint32_t)(c * 0xc2b2ae35u); c ^= c >> 13u;` | kombinasi MUL+XORSHIFT — dulu pernah jadi runner-up pilihan Pareto (`../2026-9-3_combo-winner-pareto-selection/RESULTS.md`), struktur 2-tahap beda dari f10/f9 |
| `w8_f33_i0` | `ADD(b)->ROT(cons)` | `c = (uint32_t)(c + b); c = rot32(c, cons);` | **satu-satunya dari 11 pemenang TANPA xorshift sama sekali** — paling beda struktur, uji apakah interaksi dengan init baru berubah kalau finalizer-nya bukan xorshift-based |

**Catatan wajib**: orbit's `o` SUDAH wide 8-tap secara struktural (lihat
`ra_permutation_cycle_orbit` — tidak seperti singleblock yang punya axis
tap-width terpisah, orbit tidak punya varian tap-width lain untuk
diuji). Jangan tambahkan tap-width sebagai axis ketiga — itu bukan
bagian dari cacat yang sedang diselidiki dan combo-search lama sudah
membuktikan width=8 wajib.

## 5. Total kombinasi Stage 1

8 init × 4 cycle = **32 kombinasi**. Satu di antaranya
(`init_linear_shared` × `w8_f10_i0`) HARUS byte-identik dengan
`ra_core_orbit` produksi — ini titik kontrol wajib (lihat §7).

**Stage 2 (fallback, HANYA kalau Stage 1 nol survivor di Tier 1
dual-ordering)**: perluas axis cycle balik ke 11 survivor penuh (atau
108 kombinasi width/finalizer combo-search asli kalau 11 juga nol),
di-cross cuma dengan 2-3 kandidat init paling menjanjikan dari data
Stage 1 (bukan cross-product penuh 8×108). Kalau Stage 2 juga nol —
STOP, laporkan ke user dengan data lengkap sebelum mempertimbangkan
opsi ablasi total dari nol yang sempat dibahas sebagai alternatif lebih
mahal.

## 6. Struktur kode yang harus dibuat

Ikuti pola `../2026-9-2_singleblock-cycle-combo-search/combo_gen.py`
(baca file itu dulu sebagai starting point literal — TEMPLATE-nya persis
struktur yang harus di-porting ke orbit) dan `combo_prng.py` (oracle
Python). Perbedaan penting dari versi singleblock:

- **Perlu full `ra_core_orbit`-equivalent**, bukan cuma satu putaran
  singleblock — sertakan `ra_hash`/`ra_reseed` (copy verbatim, TIDAK
  divariasikan, lihat §2) dan L[]-swap penuh (`ra_permutation_cycle_orbit`'s
  `L[i]=L[d]; L[d]=tmp;` loop) supaya Promotion Tier bisa menguji K=255
  (butuh reseed dan swap L[] bekerja, tidak seperti singleblock yang cukup
  1 iterasi per panggilan).
- **CLI WAJIB dua urutan key di binary yang sama** (celah yang harus
  ditutup, lihat §1) — bukan cuma `--stream` (Weyl) seperti
  `combo_gen.py`'s TEMPLATE lama:
  - `--stream <base_key> <n> <K>` — Weyl-stepped, `key = base_key +
    block_idx * 0x9E3779B9u` (pola sama seperti semua driver multikey
    lain di repo ini).
  - `--stream-hashed <base_key> <n> <K>` — SplitMix32-hashed
    (`splitmix32()`, lihat `../2026-9-1_family-productionization/multikey_stream_hashed.c`
    atau `../2026-9-4_orbit-kmin-battery/diag_orbit_k1_hashed.c` untuk pola
    generator persis — salin fungsinya, bukan seluruh file).
  - `--single <key> <rng>` — sanity check byte-exact vs oracle Python
    (pola `sanity_check()`/`verify_control_matches_production()` di
    `combo_gen.py`).
- File baru di folder ini (nama disarankan, boleh disesuaikan asal
  konsisten):
  - `recipes_orbit.py` — enumerasi 8×4=32 kombinasi (lebih sederhana dari
    `recipes.py` singleblock karena tidak ada axis tap-width/inject; boleh
    hardcode 4 cycle candidate di atas sebagai list literal alih-alih
    generate dari `FINALIZER_RECIPES` penuh).
  - `combo_prng_orbit.py` — oracle Python murni: implementasi 8 varian
    init + `orbit_round1(key, init_variant, cycle_variant)` untuk K=1,
    dan `orbit_stream_full(key, rng, init_variant, cycle_variant)` untuk
    verifikasi multi-round (Promotion Tier sanity, perlu simulasi
    `ra_reseed`/`ra_hash` juga kalau `rng>255`).
  - `combo_gen_orbit.py` — TEMPLATE C standalone (pola persis di atas),
    `write_and_compile_orbit(init_variant, cycle_variant)`,
    `sanity_check_orbit()`, `verify_control_matches_production_orbit()`.
  - `candidates/` — output codegen (gitignore-kan binary kalau perlu,
    ikuti konvensi folder combo-search lama).

## 7. Pipeline validasi (tiap tahap harus lolos sebelum lanjut ke tahap berikutnya)

1. **Oracle Python vs produksi**: `init_linear_shared` × `w8_f10_i0` di
   `combo_prng_orbit.py` HARUS byte-identik dengan
   `ra_core_orbit(key, 1, ...)` produksi untuk beberapa key uji coba
   (pola `round1()`'s hand-trace di `combo_prng.py`). Lakukan ini SEBELUM
   apa pun lain — kalau oracle sendiri salah, semua hasil di bawahnya
   tidak bisa dipercaya.
2. **Codegen C vs oracle Python**: tiap 32 kombinasi, `--single` output
   binary HARUS cocok `combo_prng_orbit.py`'s oracle byte-for-byte
   (pola `sanity_check()`) SEBELUM dikirim ke PractRand — mencegah bug
   codegen dikira anomali RNG (pelajaran dari sesi 2026-09-04's diagnosis:
   verifikasi patch/kode BENAR-BENAR melakukan yang diklaim, sebelum
   percaya hasil statistiknya).
3. **Tier 0 — avalanche** (pola `tier0_avalanche.py`, K=1, band sama
   LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2 dari
   `../2026-8-26_operation-pruning-research/quality_gate.py`'s
   `avalanche_gate_min_bit`). Buang kombinasi yang gagal band ini di
   K=1 sebelum masuk PractRand (murah, Python-only).
4. **Tier 1 — PractRand K=1, DUA urutan key, skala kecil (1-2GB per
   urutan)** — **INI GATE WAJIB, bukan opsional, beda dari search lama**:
   kombinasi harus lolos Weyl DAN hashed sebelum promote ke tahap
   berikutnya. Kombinasi yang cuma lolos satu urutan **TIDAK LANJUT**,
   dicatat sebagai gagal-parsial di RESULTS.md (informasi berharga, bukan
   dibuang).
5. **Promotion Tier** (pola `promotion_search.py` — baca file itu untuk
   struktur run-and-parse-checkpoint, early-kill on hard FAIL — jalankan
   di VPS, lihat `VPS_ACCESS.md` di root repo): survivor Tier 1 diuji
   penuh — K=1 DAN K=255, KEDUA urutan key (4 run per survivor), skala
   8GB (naikkan ke 16GB kalau K=1 bersih di 8GB, menyamakan bar
   `orbit-kmin-battery`'s gate). **Tidak ada retry seed pada FAIL apa
   pun** (`feedback_no_reseed_on_fail`, memory repo ini).
6. **Benchmark kecepatan** (HANYA untuk survivor final Promotion Tier,
   bukan semua 32 kombinasi): pola `bench_release.c` /
   standar 5-angka (`feedback_orbit_singleblock_benchmark_standard`:
   orbit --multistream K=1/K=255/--stream, dan untuk perbandingan
   langsung juga ukur singleblock K=1/K=255 kalau relevan) — bandingkan
   ns/word vs `w8_f10_i0`+`init_linear_shared` (produksi saat ini).

## 8. Kriteria PASS/lanjut

- Survivor final: bersih (0 FAIL, tanpa pola eskalatif) sampai minimal
  8GB di SEMUA 4 kombinasi (K=1/K=255 × Weyl/hashed).
- Stage 1 nol survivor di Tier 1 → jalankan Stage 2 (§5) sebelum lapor
  ke user sebagai "belum ketemu".
- Stage 1 DAN Stage 2 nol survivor → STOP, laporkan ke user dengan data
  lengkap (kombinasi mana yang paling dekat lolos, pola kegagalan apa,
  apakah init atau cycle axis yang tampak lebih dominan menyebabkan
  gagal) — **jangan lanjut ke opsi ablasi total dari nol tanpa
  persetujuan eksplisit user** (scope terpisah, lebih mahal, sempat
  dibahas sebagai alternatif saat user bertanya "apakah kita perlu
  ablasi/pruning dari awal").
- Kalau ADA survivor: pilih kandidat terbaik (Pareto speed vs jumlah
  perubahan dari produksi — makin sedikit axis yang diubah dari kontrol,
  makin mudah dipercaya/direview, sama prinsipnya dengan
  `../2026-9-3_combo-winner-pareto-selection/`), diskusikan dengan user
  sebelum dipromosikan ke `ra_core.c` kanonik (ra_core.c "saat ini TIDAK
  boleh disentuh tanpa konfirmasi eksplisit user" — konvensi repo).

## 9. Peringatan teknis yang wajib diwarisi (jangan mengulang bug lama)

- **Bug `fmemopen` glibc**: kalau buffer harness apa pun di sini pakai
  `fmemopen` (mis. untuk sanity-check per-word), buffer WAJIB
  `data_size+1` byte, bukan pas — NUL-terminator glibc menimpa byte
  terakhir (little-endian, byte teratas word terakhir jadi nol tiap
  panggilan). Sudah dua kali jadi sumber kebingungan (§4 di
  `../2026-9-4_orbit-reseed-postboundary-bcfn/HANDOVER.md`, dan
  `../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`).
- **No re-seed retry on FAIL** (`feedback_no_reseed_on_fail`): FAIL di
  satu seed/base_key pada kombinasi manapun sudah final — jangan retry
  seed lain untuk "menjelaskan" kegagalan itu. Boleh menguji base_key
  KEDUA yang independen untuk KONFIRMASI reproduksibilitas suatu temuan
  (seperti dilakukan di §1's investigasi gap) — itu beda dari retry untuk
  membatalkan FAIL.
- **Verifikasi patch/kode SEBELUM percaya statistik** — pelajaran
  langsung dari sesi ini (`2026-9-4_orbit-init-zerotap-fix/`): patch
  `linear_p1` diverifikasi dulu benar-benar menghilangkan nilai nol
  (differential test vs oracle Python) SEBELUM PractRand dijalankan.
  Terapkan disiplin yang sama di sini — jangan percaya hasil PractRand
  kombinasi manapun sebelum §7 langkah 1-2 (oracle+codegen sanity) lolos.

## 10. Referensi cepat

- Gap yang memicu search ini:
  `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`.
- Hipotesis fix yang ditolak:
  `../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`.
- Infra yang direuse: `../2026-9-2_singleblock-cycle-combo-search/`
  (`recipes.py`, `combo_prng.py`, `combo_gen.py`, `tier0_avalanche.py`,
  `promotion_search.py`, `RESULTS.md` untuk daftar 11 survivor lengkap).
- Fungsi target: `../2026-9-1_family-productionization/ra_core.c`
  (`ra_init_state_orbit` L197-207, `ra_permutation_cycle_orbit` L217-252,
  `ra_core_orbit` L254-270, `ra_hash`/`ra_reseed` L166-183 — tidak
  divariasikan).
- Pola dual key-ordering: `splitmix32()` di
  `../2026-9-1_family-productionization/multikey_stream_hashed.c` atau
  `../2026-9-4_orbit-kmin-battery/diag_orbit_k1_hashed.c`.
- VPS untuk Promotion Tier: `VPS_ACCESS.md` di root repo (gitignored,
  lihat `CLAUDE.md` repo untuk cara akses).
- Status/tempat mencatat hasil setelah selesai:
  `../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md`
  §2b (update lagi setelah ada hasil, jangan tambah section baru),
  memory `project_singleblock_kmin_defect`. Jalankan `/graphify --update`
  setelah selesai (wajib per `CLAUDE.md` repo ini).
