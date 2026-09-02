# RESULTS: `ra_core_singleblock` cycle-operation combo search

Status: **in progress — Promotion Tier (VPS) still running.** Tier 0 and
Tier 1 are complete and final. This file will be updated with the Promotion
Tier table and final recommendation once the VPS run finishes.

## Konteks

Lihat `HANDOVER.md` untuk latar belakang lengkap. Ringkas: `ra_core_singleblock`
(`../2026-9-1_family-productionization/ra_core.c`) punya defect struktural
BCFN pada K kecil (FAIL sampai K≈72, bersih mulai K≈96 — lihat
`../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md`). Dua
percobaan fix via init/seed sudah GAGAL:

1. `../2026-9-2_singleblock-prereseed-experiment/` — pre-mixing M[]/cons
   via `ra_hash` → tidak signifikan beda dari baseline.
2. `../2026-9-2_singleblock-o-width-fix/` — wide-o (Kandidat A) dan
   wide-o+warmup (Kandidat B) → keduanya hanya memindahkan K mana yang FAIL
   (non-monoton) di 16GB, tidak menghilangkan defect.

Keputusan eksplisit user: STOP fix via init/seed. `ra_init_state_singleblock`
tidak disentuh. Eksperimen ini mencari kombinasi operasi per-round yang
lebih kaya di dalam `ra_permutation_cycle_singleblock` (tap width `o`,
finalizer 0-2 stage ala `fmix32` pada `c`, extra-inject tambahan).

## Metodologi

### Search space (DSL)

```
o = tap_combo(M, i)                        # slot 1: tap width {2,4,8}
a = (d ^ o) ^ (cons + a)
b = (cons + a) ^ (o + d)
c = rot32((a >> 13) ^ a, b)
c = finalizer(c)                            # slot 2: 36 resep + kontrol kosong
[extra_inject into a or b]                  # slot 3: 7 pilihan
d = c & 0xFFu
```

Ruang penuh DSL: 3 × 36 × 7 = **756 kombinasi** (dinyatakan eksplisit, tidak
dipangkas diam-diam untuk masuk ke "low hundreds" seperti estimasi awal
HANDOVER.md).

Detail katalog lengkap (6 konstanta MUL, 4 shift XORSHIFT, 3 sumber ROT, 4
operand ADD, 8 kombinasi 2-stage XORSHIFT→MUL, 4 MUL→XORSHIFT, 4 ADD→ROT, 2
ROT→XORSHIFT) ada di `recipes.py`.

### Temuan struktural: slot 3 (extra-inject) inert di K=1

**Dibuktikan, bukan diasumsikan** (lihat `recipes.py` docstring +
`combo_prng.py`): `extra_inject` menulis ke `a`/`b` SETELAH `c` (output round
ini) selesai dihitung dan SEBELUM `d = c & 0xFFu`. Pada K=1,
`ra_permutation_cycle_singleblock` hanya menjalankan SATU iterasi loop per
panggilan (count dimulai di 1, `*count<=1` langsung `break`), dan
`a,b,c,d` direset segar tiap panggilan baru (key baru per block di harness
multikey). Jadi tulisan `extra_inject` ke `a`/`b` hanya bisa memengaruhi
putaran KEDUA — yang tidak pernah terjadi di K=1. Diverifikasi empiris:
compile 7 varian inject dari kandidat yang sama, semua 7 menghasilkan output
`--single` byte-identik di K=1.

**Konsekuensi**: Tier 0 dan Tier 1 (K=1) hanya menguji **108 kombinasi**
(3 tap-width × 36 finalizer, inject tetap "off") — bukan 756 — karena
menguji 7 varian inject di sana pasti menghasilkan stream byte-identik.
Slot 3 baru observable di K≥2, jadi Promotion Tier (K sampai 96) meng-cross
tiap survivor Tier 1 dengan varian inject yang relevan.

### Tier 0 — avalanche gate (min-per-bit)

Reuse threshold band terkalibrasi dari
`../2026-8-26_operation-pruning-research/quality_gate.py`'s
`avalanche_gate_min_bit`: `LOW=0.3, HIGH=0.7, MIN_BIT_FLOOR=0.2`. Capture
K=1 (satu word, bukan 255-word cycle seperti gate lama) — flip 32 bit seed,
ukur fraksi Hamming distance per bit vs baseline, syarat rata-rata dalam
band DAN bit terlemah ≥ floor.

**Keterbatasan yang dinyatakan eksplisit**: ini filter "tolak yang jelas
rusak", BUKAN proxy defect sebenarnya. `singleblock-prereseed-experiment`
sudah menunjukkan input/seed yang lebih kaya ke transform yang sama TIDAK
memperbaiki defect PractRand — defect adalah korelasi lintas-key/block,
yang tidak bisa dideteksi oleh avalanche satu panggilan tunggal secara
konstruksi.

### Tier 1 — multikey PractRand staged (K=1)

Stage 1 (256MB) → Stage 2 (2GB), `base_key=111222` (konsisten dengan semua
eksperimen singleblock sebelumnya di line ini), harness multikey (Weyl
increment `0x9E3779B9` antar block/key, pola `multikey_stream_k.c`).

**Alasan ukuran staged**:
- 256MB: `k-threshold-characterization/RESULTS.md` menunjukkan defect
  UNFIXED untuk K∈{1,2,4,8} sudah menghasilkan R besar (puluhan-ribuan)
  jauh sebelum 1GB — kandidat yang masih rusak parah harusnya sudah
  menunjukkan sinyal kuat di 256MB. Terukur empiris ~8-10s/kandidat di
  mesin lokal.
- 2GB: reuse langsung dari tahap triage Kandidat B di
  `../2026-9-2_singleblock-o-width-fix/` — presedan paling relevan (defect/
  topologi sama: multikey K=1, singleblock). **Eksplisit BUKAN cukup untuk
  vonis final**: tahap triage yang sama lolos bersih 9/9 nilai N di 2GB, dan
  3 di antaranya kemudian FAIL di 16GB — 2GB di sini murni filter volume
  sebelum Promotion Tier yang mahal.

**Bugfix anomaly-detection** (ditemukan saat membaca kode lama sebelum
implementasi): `quality_gate.py` dan `operand_search.py` grep string
`"SUSPICIOUS"` uppercase-only, padahal output asli `RNG_test` memakai
lowercase (`mildly suspicious`, `unusual`, `suspicious`, `very suspicious`)
— dikonfirmasi dengan grep langsung ke file `.log` PractRand repo ini. Gate
lama itu secara diam-diam TIDAK PERNAH menangkap sinyal suspicious-tier,
hanya `FAIL`. **Fix di sini scoped HANYA ke script baru** (`tier1_search.py`,
`promotion_search.py`) per keputusan eksplisit user (2026-09-02) — script
lama TIDAK disentuh. Regex baru: `\bFAIL\b|very suspicious` untuk reject;
`mildly suspicious`/`suspicious`/`unusual` dicatat (tidak auto-reject) untuk
review manusia.

**No re-seed retry on FAIL**: satu FAIL di stage manapun bersifat final,
tidak ada rerun dengan seed berbeda untuk "menjelaskan" hasilnya — di
seluruh eksperimen ini (Tier 0, Tier 1, Promotion Tier).

## Hasil Tier 0

**108/108 kandidat lolos** (0 ditolak). Sesuai ekspektasi metodologi (lihat
"Keterbatasan" di atas) — Tier 0 memang bukan proxy defect sebenarnya,
sebagian besar resep finalizer adalah local diffuser yang layak. Semua 108
lanjut ke Tier 1.

## Hasil Tier 1

**30/108 kandidat (27.8%) lolos Stage 1 (256MB) + Stage 2 (2GB) di K=1.**

| Tap width | Stage1 pass | Stage1 fail | Stage2 pass (survivor) | Stage2 fail |
|---|---|---|---|---|
| 2 | 15/36 | 21 | 3 | 12 |
| 4 | 20/36 | 16 | 3 | 17 |
| 8 | 26/36 | 10 | **24** | 2 |

Tap width 8 (wide-o) mendominasi survivor Tier 1 — konsisten dengan temuan
`o-width-fix` bahwa melebarkan `o` memperbaiki sinyal PractRand jangka
pendek (screening awal 1-8GB), meski `o-width-fix` juga sudah membuktikan
itu TIDAK cukup sampai 16GB untuk wide-o saja (lihat "Hasil Promotion Tier"
di bawah).

30 survivor (id, deskripsi):

```
w2_f14_i0  width=2 finalizer=[ADD(cons)]
w2_f17_i0  width=2 finalizer=[ADD(b)]
w2_f31_i0  width=2 finalizer=[ADD(cons)->ROT(i)]
w4_f22_i0  width=4 finalizer=[XORSHIFT(16)->MUL(0x85ebca6b)]
w4_f25_i0  width=4 finalizer=[XORSHIFT(16)->MUL(0x06a0dd9b)]
w4_f29_i0  width=4 finalizer=[MUL(0xc2b2ae35)->XORSHIFT(16)]
w8_f0_i0   width=8 finalizer=[empty]                          <- lihat catatan di bawah
w8_f1_i0   width=8 finalizer=[MUL(0x85ebca6b)]
w8_f2_i0   width=8 finalizer=[MUL(0xc2b2ae35)]
w8_f3_i0   width=8 finalizer=[MUL(0x9e3779b7)]
w8_f4_i0   width=8 finalizer=[MUL(0x06a0dd9b)]
w8_f5_i0   width=8 finalizer=[MUL(0xff51afd7)]
w8_f8_i0   width=8 finalizer=[XORSHIFT(15)]
w8_f9_i0   width=8 finalizer=[XORSHIFT(16)]
w8_f10_i0  width=8 finalizer=[XORSHIFT(17)]
w8_f11_i0  width=8 finalizer=[ROT(b)]
w8_f12_i0  width=8 finalizer=[ROT(i)]
w8_f14_i0  width=8 finalizer=[ADD(cons)]
w8_f17_i0  width=8 finalizer=[ADD(b)]
w8_f22_i0  width=8 finalizer=[XORSHIFT(16)->MUL(0x85ebca6b)]
w8_f23_i0  width=8 finalizer=[XORSHIFT(16)->MUL(0xc2b2ae35)]
w8_f24_i0  width=8 finalizer=[XORSHIFT(16)->MUL(0x9e3779b7)]
w8_f25_i0  width=8 finalizer=[XORSHIFT(16)->MUL(0x06a0dd9b)]
w8_f26_i0  width=8 finalizer=[MUL(0x85ebca6b)->XORSHIFT(13)]
w8_f27_i0  width=8 finalizer=[MUL(0x85ebca6b)->XORSHIFT(16)]
w8_f28_i0  width=8 finalizer=[MUL(0xc2b2ae35)->XORSHIFT(13)]
w8_f29_i0  width=8 finalizer=[MUL(0xc2b2ae35)->XORSHIFT(16)]
w8_f31_i0  width=8 finalizer=[ADD(cons)->ROT(i)]
w8_f33_i0  width=8 finalizer=[ADD(b)->ROT(cons)]
w8_f34_i0  width=8 finalizer=[ROT(b)->XORSHIFT(16)]
```

Log lengkap (semua 108 trial, stage1+stage2): `combo_search_log.jsonl`.
Survivor Tier 0: `tier0_survivors.jsonl`.

### Catatan: `w8_f0_i0` dikecualikan dari Promotion Tier

`w8_f0_i0` (width=8, finalizer kosong) secara struktural **identik** dengan
"Kandidat A" (wide-o murni, tanpa warm-up) di
`../2026-9-2_singleblock-o-width-fix/`, yang SUDAH diuji sampai 16GB di
eksperimen itu dan **GAGAL**: K=1 FAIL (TMFn, bukan BCFN, R~=+13.6 @16GB),
K=2 FAIL BCFN R=+22.2 @16GB, K=4 FAIL BCFN R=+30.7/+25.8 @16GB (lihat
`../2026-9-2_singleblock-o-width-fix/RESULTS.md` bagian "Kandidat A — hasil
per K"). Menjalankan ulang di sini akan membuang compute VPS untuk hasil
yang sudah diketahui — data lama dikutip langsung di tabel Promotion Tier
di bawah, bukan dijalankan ulang.

## Hasil Promotion Tier

Dijalankan di VPS, 29 kandidat × 8 K-values × 16GB (1 invokasi PractRand
kontinu per (kandidat,K), early-kill saat hard FAIL sebelum checkpoint
16GB — lihat `promotion_search.py` untuk metodologi). Total wall time
**9.18 jam**, 2 kandidat paralel. `w8_f0_i0` (width=8, finalizer kosong)
sengaja tidak dijalankan ulang — dikutip dari
`../2026-9-2_singleblock-o-width-fix/RESULTS.md` (Kandidat A: FAIL K=1
TMFn, K=2/4 BCFN di 16GB).

**Hasil akhir: 11/29 kandidat FULLY CLEAN di seluruh 8 K @16GB — semuanya
width=8.** Tidak ada satupun kandidat width=2 (0/3) atau width=4 (0/3)
yang lolos, walau ketiganya lolos Tier 1 (K=1, 2GB).

Tabel status per K (`OK` = bersih sampai 16GB penuh; `KILL@X` = di-kill
dini setelah checkpoint X hard FAIL; `FAILfinal` = jalan sampai checkpoint
terakhir yang dicapai lalu FAIL, tanpa early-kill karena FAIL baru
terdeteksi di checkpoint yang juga jadi checkpoint akhir sebelum batas):

| id | K=1 | K=2 | K=4 | K=8 | K=16 | K=32 | K=64 | K=96 |
|---|---|---|---|---|---|---|---|---|
| w2_f14_i0 | OK | KILL@1G | KILL@4G | FAIL | OK | OK | OK | OK |
| w2_f17_i0 | KILL@4G | KILL@1G | KILL@4G | FAIL | OK | OK | OK | OK |
| w2_f31_i0 | OK | KILL@4G | OK | OK | OK | OK | OK | OK |
| w4_f22_i0 | KILL@4G | KILL@4G | KILL@8G | FAIL | OK | OK | OK | OK |
| w4_f25_i0 | KILL@4G | KILL@4G | KILL@8G | OK | OK | OK | OK | OK |
| w4_f29_i0 | KILL@4G | FAIL | FAIL | OK | OK | OK | OK | OK |
| w8_f1_i0 | FAIL | KILL@8G | OK | OK | OK | OK | OK | OK |
| w8_f2_i0 | FAIL | KILL@8G | FAIL | OK | OK | OK | OK | OK |
| w8_f3_i0 | FAIL | KILL@8G | OK | OK | OK | OK | OK | OK |
| w8_f4_i0 | FAIL | KILL@8G | OK | OK | OK | OK | OK | OK |
| w8_f5_i0 | FAIL | KILL@8G | FAIL | FAIL | OK | OK | OK | OK |
| **w8_f8_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| **w8_f9_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| **w8_f10_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| w8_f11_i0 | KILL@8G | OK | KILL@2G | KILL@8G | OK | OK | OK | FAIL |
| w8_f12_i0 | FAIL | KILL@8G | KILL@4G | FAIL | OK | OK | OK | OK |
| w8_f14_i0 | FAIL | FAIL | OK | OK | OK | OK | OK | OK |
| w8_f17_i0 | FAIL | OK | OK | OK | OK | OK | OK | OK |
| **w8_f22_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| w8_f23_i0 | KILL@4G | FAIL | OK | OK | OK | OK | OK | OK |
| **w8_f24_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| **w8_f25_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| **w8_f26_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| **w8_f27_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| **w8_f28_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| **w8_f29_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| w8_f31_i0 | KILL@8G | FAIL | OK | OK | OK | OK | OK | OK |
| **w8_f33_i0** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** | **OK** |
| w8_f34_i0 | FAIL | OK | OK | OK | OK | OK | OK | OK |

Observasi tambahan: pada seluruh 18 kandidat yang FAIL, kegagalan **selalu
di K∈{1,2,4,8}** — tidak ada satupun kegagalan di K∈{16,32,64,96} untuk
kandidat manapun dalam set survivor Tier 1 ini. Konsisten dengan
[[project_singleblock_kmin_defect]] (K besar historisnya aman); di sini
batasnya tampak lebih ketat lagi (K≥16 cukup, bukan hanya K≥96) untuk
kandidat-kandidat yang sudah lolos saringan Tier 0/1 duluan — tapi ini
BUKAN klaim umum untuk sembarang finalizer/tap-width (kandidat yang gagal
total di K kecil belum tentu polanya sama untuk kombinasi lain di luar 108
yang diuji).

**Pola finalizer pemenang (11 kandidat, semua width=8):**

| id | finalizer |
|---|---|
| w8_f8_i0 | `XORSHIFT(15)` |
| w8_f9_i0 | `XORSHIFT(16)` |
| w8_f10_i0 | `XORSHIFT(17)` |
| w8_f22_i0 | `XORSHIFT(16)->MUL(0x85ebca6b)` |
| w8_f24_i0 | `XORSHIFT(16)->MUL(0x9e3779b7)` |
| w8_f25_i0 | `XORSHIFT(16)->MUL(0x06a0dd9b)` |
| w8_f26_i0 | `MUL(0x85ebca6b)->XORSHIFT(13)` |
| w8_f27_i0 | `MUL(0x85ebca6b)->XORSHIFT(16)` |
| w8_f28_i0 | `MUL(0xc2b2ae35)->XORSHIFT(13)` |
| w8_f29_i0 | `MUL(0xc2b2ae35)->XORSHIFT(16)` |
| w8_f33_i0 | `ADD(b)->ROT(cons)` |

Pengamatan pola (deskriptif, bukan bukti kausal — lihat catatan di bawah):
- **Tap width=8 wajib**: 0/6 kandidat width=2/4 lolos Promotion Tier
  walau lolos Tier 1; `w8_f0_i0` (width=8 tanpa finalizer) sendiri juga
  FAIL (data lama). Jadi tap width=8 perlu tapi TIDAK cukup — butuh
  finalizer juga.
- **10/11 pemenang mengandung `XORSHIFT`** (shift 13/15/16/17) di salah
  satu tahap — baik sebagai satu-satunya operasi (f8/f9/f10) atau
  dikombinasi dengan MUL di urutan manapun (f22/f24/f25/f26/f27/f28/f29).
  Sebagai perbandingan, MUL sendirian (f1-f5, 5/5 FAIL), ROT sendirian
  (f11/f12, 2/2 FAIL), dan ADD sendirian (f14/f17, 2/2 FAIL) semuanya
  gagal — xorshift tampak jadi bahan yang paling konsisten.
- **Tapi bukan mutlak wajib**: `w8_f33_i0` (`ADD(b)->ROT(cons)`) lolos
  tanpa xorshift sama sekali, padahal ADD sendirian dan ROT sendirian
  sama-sama gagal terpisah — jadi kombinasi 2-tahap tertentu di luar
  xorshift juga bisa cukup, bukan cuma xorshift yang "istimewa".
  Sebaliknya `w8_f31_i0` (`ADD(cons)->ROT(i)`), kombinasi ADD->ROT lain,
  tetap FAIL — jadi bukan "ADD->ROT apa saja", spesifik ke pasangan
  operand tertentu.
- **Konstanta MUL tidak seragam**: di antara 4 varian `XORSHIFT(16)->MUL(c)`
  yang diuji, 3 lolos (`0x85ebca6b`, `0x9e3779b7`, `0x06a0dd9b`) dan 1 FAIL
  (`0xc2b2ae35`, f23) — tidak ada pola aljabar sederhana (mis. "harus
  konstanta fmix32 asli") yang menjelaskan kenapa justru konstanta itu
  yang gagal di posisi ini.

**Batas metodologi penting**: pola di atas murni observasi post-hoc atas
11 pemenang dari 108 kandidat yang diuji dalam DSL search space ini —
BUKAN hasil ablation study terkontrol (mis. tidak ada uji sistematis
"XORSHIFT(16) sendirian vs XORSHIFT(16)+setiap kombinasi operand lain").
Tidak disimpulkan sebab-akibat definitif soal *kenapa* xorshift/kombinasi
tertentu menutup defect ini — hanya korelasi empirik dari kombinasi yang
kebetulan ada di 756-ruang DSL awal. Untuk klaim kausal yang lebih kuat
perlu eksperimen susulan yang secara eksplisit memvariasikan satu dimensi
(shift amount, posisi MUL, dst.) sambil mengunci yang lain.

## Microbench (overhead)

Diukur lokal (mesin dev), `--bench 200000000 1` (K=1, dispatch lewat
function pointer `volatile` — lihat catatan metodologi di bawah), 3 trial
min-of-trials, dibandingkan baseline `narrow` (production
`ra_core_singleblock`, via `diag_wideo_singleblock --bench narrow`):

**Catatan metodologi penting**: versi awal harness `--bench` di
`combo_gen.py` memanggil fungsi kandidat secara LANGSUNG (bukan lewat
function pointer seperti `diag_wideo_singleblock.c`'s `core_fn_t fn`),
sehingga GCC bisa inline+auto-vectorize khusus untuk tap-width=2 dan
menghasilkan angka ~1 ns/word yang TIDAK merepresentasikan overhead nyata
(production `ra_core_singleblock` sendiri selalu dipanggil lewat function
pointer via `ra_core.c`'s `CORES[]` dispatch table). Ditemukan & diperbaiki
sebelum tabel di bawah ini diambil — lihat `combo_gen.py`'s `combo_fn`
komentar untuk detail.

| Baseline | ns/word |
|---|---|
| `narrow` (production, K=1) | 40.10 |

| Kandidat | ns/word | vs baseline |
|---|---|---|
| w2_f14_i0 | 37.50 | 0.94x |
| w2_f17_i0 | 39.60 | 0.99x |
| w2_f31_i0 | 41.79 | 1.04x |
| w4_f22_i0 | 41.73 | 1.04x |
| w4_f25_i0 | 40.56 | 1.01x |
| w4_f29_i0 | 39.46 | 0.98x |
| w8_f0_i0 | 40.16 | 1.00x |
| w8_f1_i0 | 39.88 | 0.99x |
| w8_f2_i0 | 40.02 | 1.00x |
| w8_f3_i0 | 39.74 | 0.99x |
| w8_f4_i0 | 42.00 | 1.05x |
| w8_f5_i0 | 40.22 | 1.00x |
| w8_f8_i0 | 39.75 | 0.99x |
| w8_f9_i0 | 40.59 | 1.01x |
| w8_f10_i0 | 40.54 | 1.01x |
| w8_f11_i0 | 38.98 | 0.97x |
| w8_f12_i0 | 39.69 | 0.99x |
| w8_f14_i0 | 37.27 | 0.93x |
| w8_f17_i0 | 40.02 | 1.00x |
| w8_f22_i0 | 39.68 | 0.99x |
| w8_f23_i0 | 42.70 | 1.06x |
| w8_f24_i0 | 39.75 | 0.99x |
| w8_f25_i0 | 40.37 | 1.01x |
| w8_f26_i0 | 41.01 | 1.02x |
| w8_f27_i0 | 39.81 | 0.99x |
| w8_f28_i0 | 41.78 | 1.04x |
| w8_f29_i0 | 40.43 | 1.01x |
| w8_f31_i0 | 37.65 | 0.94x |
| w8_f33_i0 | 36.05 | 0.90x |
| w8_f34_i0 | 40.01 | 1.00x |

Semua 30 survivor Tier 1 berada di kisaran **0.90x-1.06x** overhead vs
baseline — jauh di bawah batas "beberapa-x" yang disyaratkan HANDOVER,
dan jauh di bawah fix warm-up yang ditolak sebelumnya (5.5-13x). Perbedaan
antar kandidat berada dalam noise pengukuran (K=1 didominasi biaya init
`M[256]`, bukan finalizer 0-2 instruksi tambahan) — overhead BUKAN faktor
pembeda untuk memilih di antara survivor manapun yang lolos Promotion Tier.

## Avalanche heatmap K=255 (full cycle) untuk 11 pemenang

Tier 0 hanya menangkap K=1 (satu word), yang murah tapi tidak bisa
mendeteksi kelas defect "bit mati" seperti yang ditemukan di
`operand-position-search` (seed bit 5/6 pada `pruned_winner` nyaris tidak
avalanche di 253/255 posisi output — fraction rata-rata per-bit 0.0078,
lolos gate skalar lama tapi ketahuan lewat heatmap K=255 penuh, lihat
`quality_gate.py`'s `avalanche_stats()` docstring). Untuk memastikan 11
pemenang Promotion Tier tidak membawa defect sejenis yang tersembunyi dari
Tier 0, dijalankan ulang heatmap K=255 (`avalanche_heatmap_winners_k255.py`,
port dari `../2026-8-27_operand-position-search/avalanche_heatmap_winner.py`)
memakai `--single <key> 255` (satu cycle penuh per key, tanpa re-keying
multikey) untuk tiap kandidat.

**Hasil: tidak ada bit mati di kesebelas pemenang.** `min_bit_fraction`
berkisar 0.481-0.492 (jauh di atas ambang 0.2, dan jauh dari pola
dead-bit historis ~0.008), `overall` semua ~0.499-0.501. Heatmap visual
(`avalanche_heatmap_winners_k255.png`) menunjukkan pola noise seragam di
seluruh 32×255 sel untuk semua 11 kandidat, tanpa garis horizontal gelap
(yang jadi tanda bit mati di kasus `pruned_winner`).

| id | overall (K=255) | min_bit (K=255) | bit terlemah |
|---|---|---|---|
| w8_f8_i0 | 0.499 | 0.486 | bit 1 |
| w8_f9_i0 | 0.501 | 0.487 | bit 22 |
| w8_f10_i0 | 0.499 | 0.481 | bit 17 |
| w8_f22_i0 | 0.501 | 0.492 | bit 5 |
| w8_f24_i0 | 0.500 | 0.488 | bit 11 |
| w8_f25_i0 | 0.498 | 0.486 | bit 18 |
| w8_f26_i0 | 0.499 | 0.492 | bit 5 |
| w8_f27_i0 | 0.501 | 0.488 | bit 8 |
| w8_f28_i0 | 0.499 | 0.486 | bit 0 |
| w8_f29_i0 | 0.500 | 0.489 | bit 4 |
| w8_f33_i0 | 0.500 | 0.488 | bit 21 |

## Rekomendasi

**Kandidat ditemukan — 11 dari 756 kombinasi DSL bersih PractRand untuk
K∈{1,2,4,8,16,32,64,96} @16GB, overhead 0.90x-1.06x (jauh di bawah batas
"beberapa-x" HANDOVER, dan jauh di bawah fix warm-up yang ditolak
sebelumnya, 5.5-13x).** "No acceptable candidate" TIDAK jadi outcome —
hasil aktualnya positif.

Rekomendasi konkret, kalau ada rencana lanjut ke promosi ke `ra_core.c`:

- **`w8_f9_i0`** (tap width=8, finalizer `c ^= c >> 16;`, tanpa MUL) sebagai
  kandidat pilihan pertama: overhead terendah kedua di antara 11 pemenang
  (1.01x), paling sederhana (1 instruksi finalizer, tanpa konstanta MUL
  tambahan untuk diaudit), dan shift=16 pas di tengah lebar word 32-bit —
  paling mudah dijelaskan/didiokumentasikan dibanding kombinasi 2-tahap.
- Alternatif: `w8_f8_i0`/`w8_f10_i0` (shift 15/17, hampir identik) kalau
  ingin variasi shift; `w8_f33_i0` (overhead terendah, 0.90x, tapi
  strukturnya paling tidak biasa — ADD+ROT tanpa xorshift — dan berdiri
  sendiri sebagai satu-satunya pemenang non-xorshift, jadi risiko generalisasi
  lebih tinggi kalau dipakai di luar 96 K yang sudah diuji di sini).
- **TIDAK direkomendasikan mempromosikan otomatis ke `ra_core.c`** —
  di luar scope task ini per HANDOVER.md, butuh konfirmasi eksplisit
  user terpisah. Sebelum promosi, sebaiknya:
  1. Uji ulang dengan seed/base_key berbeda (repo ini tidak pernah
     re-seed setelah FAIL, tapi kandidat yang CLEAN juga belum divalidasi
     silang dengan base_key lain di luar 111222).
  2. Uji dieharder (battery gate yang di-PAUSE per
     [[project_ra_prng_family_productionization]]) untuk kandidat
     terpilih, bukan cuma PractRand.
  3. Crossing dengan 7 varian extra-inject (slot 3) di Promotion Tier,
     yang sengaja belum diuji di run ini (semua 29 kandidat pakai
     inject_idx=0/off) — kalau extra-inject akan tetap dipakai di
     produksi, perlu diverifikasi tidak merusak kebersihan yang baru
     ditemukan ini di K≥2 (slot 3 terbukti inert di K=1 tapi TIDAK inert
     di K lebih besar, lihat `recipes.py`).

## Verifikasi

- `git diff --stat -- ../2026-9-1_family-productionization/ra_core.c`:
  kosong sepanjang eksperimen ini (diverifikasi, lihat shell log sesi).
- Tidak ada write/perubahan ke `../2026-9-1_production-candidate-battery/`.
- Kandidat kontrol (width=2, finalizer kosong, inject off) direproduksi
  byte-identik terhadap `ra_core_singleblock(key=1, rng=1)` produksi
  (2452187902) — satu-satunya hand-trace sejati; kandidat lain divalidasi
  via cross-check C-vs-`combo_prng.py` (methodology yang sama dipakai
  `other_winners_gen.py`), bukan hand-trace individual (dinyatakan eksplisit
  karena skala kandidat besar).
- Tidak ada FAIL (Tier 0/1/Promotion) yang di-rerun dengan seed berbeda
  untuk "menjelaskan" hasilnya, di seluruh eksperimen ini.
- `ra_init_state_singleblock` tidak dimodifikasi di manapun dalam
  eksperimen ini — semua kandidat copy verbatim dari `ra_core.c`.
- Promotion Tier VPS run selesai total 2026-09-03 (mulai 2026-09-02),
  9.18 jam wall time, 232/232 (kandidat,K) pasangan tuntas, 0 error.
  `promotion_log.jsonl` dan `promotion_logs/*.log` (232 file) disalin ke
  folder ini dari VPS untuk reproducibility.
- Re-verifikasi `git diff --stat -- ../2026-9-1_family-productionization/ra_core.c`
  di akhir task (setelah semua tulis-file di atas): tetap kosong.
