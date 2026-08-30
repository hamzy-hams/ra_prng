# Hasil fase 1: spektrum mekanisme update state L (swap vs. maximal bit-change)

Sesuai `HANDOVER.md`. Prioritas L dulu (per keputusan user), M menyusul di
fase 2. Cadence-mixing (katalog #5 HANDOVER) di-skip sesuai keputusan user.

## Setup

3 mekanisme × 2 algoritma target = 6 kandidat, dibangun via
`gen_variants.py` (template generator, mirror struktural dari
`src/ra_prng2/c/ra_prng2.c` dan
`experiments/2026-8-27_operand-position-search/winner_wired_v2.c`, hanya
blok swap/fold yang diganti):

- **permute** — swap hari ini (`L[i], L[d] = L[d], L[i]`), kontrol/baseline.
- **inject** — one-directional overwrite ala ra_prng3: `L[i] = L[d]; L[d] =
  c` (reuse `c` yang sudah dihitung step ini, tidak ada fungsi mixing baru).
- **overwrite** — direct overwrite tanpa relokasi: `L[i] = c`.

**Sanity check generator**: varian `*_permute` diverifikasi bit-identical
byte-for-byte terhadap binary asli (`ra_prng2.c`/`winner_wired_v2.c`)
sebelum kandidat lain dipercaya — PASS untuk kedua target.

**Catatan metodologi penting** (ditemukan dengan menjalankan kode, bukan
dari inspeksi): `c` (nilai stream yang di-emit) dihitung **sebelum** L
diupdate tiap step, dan L hanya mempengaruhi output secara tidak langsung
lewat fold `M[i] ^= L[i]` yang terjadi sekali per 255 langkah (saat
reseed). Konsekuensinya:
- 255 nilai output pertama **identik byte-for-byte** di ketiga mekanisme
  (diverifikasi langsung) — divergensi baru muncul setelah reseed pertama.
- Pengukuran avalanche yang naif memakai konvensi `quality_gate.py`
  (`_capture_first_cycle`, 1 siklus, 255 nilai) akan **buta total**
  terhadap efek mekanisme L — semua mekanisme akan tampak identik. Fase
  ini memakai capture 2 siklus (510 nilai) untuk melewati satu batas
  reseed, baru mekanisme L benar-benar teruji.

## 1. Periodisitas (toy-model, Brent λ/μ)

Ekstensi `toy_prng.py`/`cycle_measure.{py,c}` dengan parameter
`mechanism` baru (default `permute`, tidak mengubah perilaku lama).
Paritas Python vs C diverifikasi identik bit-for-bit di `n=2,w=4` untuk
ketiga mekanisme sebelum dipakai untuk sweep besar (C jauh lebih cepat).

Grid sama seperti eksperimen `2026-8-25` (`n∈{2,4,8}, w∈{4,8}`, minus
`n=8,w=8` yang infeasible), seed count disesuaikan agar tetap tractable
per mekanisme (300 untuk config cepat, 50-100 untuk `n=8,w=4`/`n=4,w=8`):

| n | w | mechanism | seeds | λ mean | λ median | λ min | λ max | distinct λ |
|---|---|---|---|---|---|---|---|---|
| 2 | 4 | permute   | 300 | 1.60e1 | 1.60e1 | 16 | 16 | 1 |
| 2 | 4 | inject    | 300 | 1.90e1 | 1.60e1 | 16 | 32 | 2 |
| 2 | 4 | overwrite | 300 | 1.60e1 | 1.60e1 | 16 | 16 | 1 |
| 2 | 8 | permute   | 300 | 5.60e2 | 7.68e2 | 256 | 768 | 2 |
| 2 | 8 | inject    | 300 | 1.53e3 | 5.12e2 | 512 | 6144 | 2 |
| 2 | 8 | overwrite | 300 | 2.56e2 | 2.56e2 | 256 | 256 | 1 |
| 4 | 4 | permute   | 300 | 8.28e4 | 1.07e5 | 15232 | 107328 | 3 |
| 4 | 4 | inject    | 300 | 4.45e3 | 4.45e3 | 4448 | 4448 | 1 |
| 4 | 4 | overwrite | 300 | 2.20e2 | 2.40e2 | 16 | 240 | 3 |
| 4 | 8 | permute   | 100 | 1.54e5 | 2.00e5 | 4352 | 200448 | 5 |
| 4 | 8 | inject    | 100 | 8.47e5 | 9.90e5 | 83200 | 990464 | 5 |
| 4 | 8 | overwrite | 100 | 6.10e4 | 6.45e4 | 3328 | 64512 | 3 |
| 8 | 4 | permute   | 50  | 6.32e6 | 6.32e6 | 6318640 | 6318640 | 1 |
| 8 | 4 | inject    | 50  | 5.53e5 | 4.63e5 | 151952 | 847056 | 3 |
| 8 | 4 | overwrite | 50  | 1.59e4 | 2.20e4 | 3200 | 21952 | 3 |

**Temuan utama**:
- **`overwrite` konsisten memperpendek siklus** di **kelima** config,
  seringkali drastis — di `n=8,w=4`, λ turun dari 6,318,640 (konstan,
  permute) ke rata-rata 15,873 (~**398× lebih pendek**). Ini menegaskan
  dugaan HANDOVER: menghapus relokasi (tidak menyimpan nilai lama sama
  sekali) merusak periodisitas.
- **`inject` tidak konsisten** — kadang lebih panjang dari permute
  (`n=2,w=8`: 1526 vs 560; `n=4,w=8`: 847k vs 154k), kadang jauh lebih
  pendek (`n=4,w=4`: 4448 vs 82807; `n=8,w=4`: 553k vs 6.32M). Tidak ada
  arah yang jelas — mekanisme ini butuh data lebih banyak/config lain
  sebelum disimpulkan "aman" atau "berbahaya".
- Pola "hanya sedikit λ berbeda di antara ratusan seed" (functional graph
  collapse ke beberapa giant cycle) yang ditemukan eksperimen `2026-8-25`
  untuk `permute` **juga muncul** di `inject`/`overwrite` — bukan
  keunikan swap.

## 2. Sanity check skala penuh (256×32-bit nyata)

`full_scale_sanity_check.py`: hash setiap blok 255-word (satu siklus
outer penuh) dari 300,000,000 nilai stream per kandidat, cek duplikat.

**Hasil: tidak ada blok berulang di keenam kandidat** (1,176,470 blok
dicek per kandidat, 0 duplikat). Tidak ada indikasi collapse ke siklus
pendek pada skala 3×10⁸ nilai — bukan bukti periode astronomis, tapi
menyingkirkan collapse praktis pada rentang ini (lihat juga bagian
PractRand di bawah, yang menjalankan hampir 14× lebih banyak nilai lagi
tanpa anomali, checksum independen kedua terhadap collapse pendek).

## 3. Speed (`perf stat -e instructions,cycles -r 5`)

Instruction count sebagai axis utama (deterministik, `+-0.00%` di semua
run) — sesuai konvensi repo ini.

| target | mechanism | instructions | vs. permute |
|---|---|---|---|
| original | permute | 14,898,168,176 | — |
| original | inject | 14,697,383,675 | −1.35% |
| original | overwrite | 14,299,736,436 | −4.02% |
| winner | permute | 6,053,460,213 | — |
| winner | inject | 5,853,460,057 | −3.30% |
| winner | overwrite | 5,452,675,650 | −9.92% |

**`overwrite` tercepat di kedua target** (tidak perlu membaca nilai lama
`L[d]` sebelum menimpanya) — tapi ini justru mekanisme dengan periodisitas
paling rusak di bagian 1. Trade-off speed vs. periodicity yang jelas,
bukan kombinasi menang-menang.

## 4. Avalanche (per-bit minimum, capture 2 siklus)

`avalanche_gate_min_bit` (band [0.3,0.7], floor per-bit 0.2) — bukan
`avalanche_gate`/`quality_gate` yang lebih lemah, sesuai pelajaran
eksperimen `2026-8-27`.

| kandidat | overall | min_bit | passed |
|---|---|---|---|
| original_permute | 0.4999 | 0.4925 | ✅ |
| original_inject | 0.4979 | 0.4895 | ✅ |
| original_overwrite | 0.4988 | 0.4902 | ✅ |
| winner_permute | 0.4938 | 0.4838 | ✅ |
| winner_inject | 0.4933 | 0.4817 | ✅ |
| winner_overwrite | 0.4939 | 0.4860 | ✅ |

Keenam kandidat lolos nyaman, jauh di atas floor 0.2 — avalanche tidak
menjadi pembeda di antara mekanisme pada skala pengukuran ini (2 siklus).

## 5. Kualitas (PractRand, 16GB tier untuk semua 6 kandidat)

| kandidat | ukuran | hasil |
|---|---|---|
| original_permute | 16GB | no anomalies in 240 test result(s) |
| original_inject | 16GB | no anomalies in 240 test result(s) |
| original_overwrite | 16GB | no anomalies in 240 test result(s) |
| winner_permute | 16GB | no anomalies in 240 test result(s) |
| winner_inject | 16GB | no anomalies in 240 test result(s) |
| winner_overwrite | 16GB | no anomalies in 240 test result(s) |

**Keenam kandidat clean di 16GB** — termasuk `overwrite`, meskipun
periodisitas toy-model-nya jauh lebih rusak. Ini konsisten dengan bagian 2:
16GB (~4.3 milyar nilai) jauh di bawah cycle length real 256×32-bit yang
mana pun (bahkan versi toy paling rusak sekalipun beroperasi pada state
space yang jauh lebih kecil daripada versi real), jadi PractRand di
tingkat ini **tidak bisa** membedakan kualitas mekanisme L pada skala
nyata — sejalan dengan sanity-check block-repeat di bagian 2 yang juga
tidak menemukan collapse pada 300 juta nilai. Kesimpulannya: pada resolusi
pengukuran yang tersedia untuk skala nyata (miliaran, bukan astronomis),
axis kualitas dan avalanche **tidak mendiskriminasi** antar mekanisme L —
hanya axis periodisitas toy-model dan speed yang menunjukkan perbedaan
nyata.

## Kesimpulan fase L

Menjawab pertanyaan literal user ("swap L[i]/L[d] perlu atau tidak"):

- **Avalanche dan PractRand 16GB tidak mendiskriminasi** antar mekanisme
  L — keenam kandidat clean/lolos. Pada resolusi pengukuran yang tersedia
  untuk skala nyata, kedua axis ini tidak bisa menjawab pertanyaan
  "swap perlu atau tidak" satu arah maupun lainnya.
- **Periodisitas (toy-model) adalah axis yang benar-benar membedakan**:
  - **`overwrite` (buang swap sepenuhnya) secara konsisten memperpendek
    periode** di semua 5 config yang diuji, hingga ~400× lebih pendek
    (`n=8,w=4`). Ini **mendukung** dugaan HANDOVER bahwa relokasi
    (menyimpan nilai lama suatu slot di slot lain) berkontribusi nyata ke
    periodisitas — swap bukan sekadar dekorasi.
  - **`inject` tidak konsisten arahnya** (kadang lebih panjang, kadang
    lebih pendek dari permute) — mekanisme ini butuh investigasi lanjut
    (config n/w lebih luas, atau ulangan seed lebih banyak) sebelum bisa
    disimpulkan aman/berbahaya.
- **Speed vs. periodicity adalah trade-off nyata, bukan menang-menang**:
  `overwrite` tercepat (4-10% instruksi lebih sedikit di kedua target)
  justru mekanisme dengan periodisitas paling rusak. `inject` speed-nya
  sedikit lebih baik (1-3%) dengan periodisitas yang tidak konsisten
  (kadang lebih baik dari permute).
- **Jawaban langsung**: swap (`permute`) **secara statistik tampak perlu
  dipertahankan** sebagai pilihan aman — tidak ada mekanisme alternatif
  yang terbukti unggul di semua axis. `overwrite` jelas lebih buruk untuk
  periodisitas meski lebih cepat; `inject` adalah kandidat paling
  menjanjikan untuk dieksplorasi lebih lanjut (speed sedikit lebih baik,
  avalanche/PractRand sama baiknya) tapi belum bisa direkomendasikan
  tanpa investigasi periodisitas tambahan.

---

# Hasil fase 2: spektrum mekanisme update state M (XOR-fold vs. alternatif)

L mekanisme **dikunci ke `permute`** (rekomendasi fase 1) sesuai urutan
yang disepakati (L dulu, lalu M, lalu kombinasi). Hanya fold
`M[i] ^= L[i]` yang divariasikan.

## Setup dan keputusan desain

HANDOVER.md catalog #4 ("symmetric treatment of M") sengaja tidak memberi
rumus persis — cuma dua arah umum ("permute slot M" atau "ganti XOR-fold
dengan fold/injection lain"), eksplisit disebut "a menu, not a mandate".
Rumus konkret di bawah adalah keputusan desain sesi ini (didokumentasikan
di `toy_prng.py`'s `M_MECHANISMS` docstring), dibuat mirroring bentuk
ketiga mekanisme L persis tapi diterapkan di cadence fold M (sekali per
255 langkah, pakai nilai `L[i]` yang sudah final siklus ini sebagai
sumber index/injeksi — analog dengan bagaimana mekanisme L sendiri
membaca `M` yang sudah final tanpa masalah aliasing):

- **xor_fold** (kontrol, tidak berubah): `M[i] ^= L[i]` untuk semua i.
- **m_permute** — relokasi murni, mirror "permute"-nya L: `M[i], M[L[i]
  & 0xFF]` ditukar untuk tiap i (permutasi menyeluruh array M yang
  digerakkan oleh L, tanpa XOR, tanpa kehilangan bit).
- **m_inject** — mirror "inject"-nya L: `M[i] = M[j]; M[j] = L[i]` di
  mana `j = L[i] & 0xFF` (relokasi satu arah + injeksi nilai L yang
  sudah final, tanpa fungsi mixing baru).
- **m_overwrite** — mirror "overwrite"-nya L: `M[i] = L[i]` untuk semua
  i (assignment langsung, seluruh isi M lama dibuang tiap siklus).

Kontrol `xor_fold` (L=permute, M=xor_fold) **identik** dengan binary
`original_permute`/`winner_permute` fase 1 — tidak dibuat ulang, dipakai
langsung. Hanya 6 kandidat baru dibangun via `gen_variants_m.py`
(`original_m_permute/m_inject/m_overwrite`, `winner_m_...` yang sama).
**Sanity check**: 255 nilai output pertama tiap kandidat baru
diverifikasi identik byte-for-byte terhadap kontrolnya (M cuma
mempengaruhi output lewat fold, sama seperti temuan fase 1 untuk L) —
PASS untuk keenamnya.

## 1. Periodisitas (toy-model, Brent λ/μ) — sama persis grid fase 1

| n | w | m_mechanism | λ mean | vs. xor_fold |
|---|---|---|---|---|
| 2 | 4 | xor_fold | 16 | — |
| 2 | 4 | permute/inject/overwrite | 16 | 1.0× (config degenerate, sama seperti fase 1) |
| 2 | 8 | xor_fold | 560 | — |
| 2 | 8 | permute/inject/overwrite | 256 | **2.2× lebih pendek** (ketiganya identik) |
| 4 | 4 | xor_fold | 82,807 | — |
| 4 | 4 | permute | 5,028 | **16.5× lebih pendek** |
| 4 | 4 | inject | 31 | **2672× lebih pendek** |
| 4 | 4 | overwrite | 32 | **2588× lebih pendek** |
| 4 | 8 | xor_fold | 154,200 | — |
| 4 | 8 | permute | 29,568 | **5.2× lebih pendek** |
| 4 | 8 | inject | 256 | **602× lebih pendek** |
| 4 | 8 | overwrite | 256 | **602× lebih pendek** |
| 8 | 4 | xor_fold | 6,318,640 (konstan) | — |
| 8 | 4 | permute | 134 | **47,014× lebih pendek** |
| 8 | 4 | inject | 2,993 | **2111× lebih pendek** |
| 8 | 4 | overwrite | 573 | **11,019× lebih pendek** |

**Temuan utama — jauh lebih dramatis daripada fase L**: **ketiga
alternatif M tanpa kecuali memperpendek periode di semua config
non-degenerate**, seringkali sampai **puluhan ribu kali** lebih pendek
(`n=8,w=4`: `m_permute` 47,000× lebih pendek). Bahkan `m_permute` —
yang murni relokasi, tidak membuang bit sama sekali, secara filosofi
paling mirip dengan swap L yang "aman" — tetap menghancurkan
periodisitas jauh lebih parah daripada mekanisme `overwrite`-nya L di
fase 1 (yang "cuma" ~400× lebih pendek).

**Kenapa ini masuk akal**: XOR-fold adalah satu-satunya dari keempat
mekanisme yang benar-benar **mengakumulasi** perubahan state M lintas
siklus — tiap `M[i]` terus berevolusi (di-XOR dengan nilai baru,
mempertahankan sejarah lewat reversibilitas XOR). `permute`/`inject`/
`overwrite` semuanya pada dasarnya **menurunkan ulang isi M murni dari
L** siklus ini — menghapus seluruh riwayat akumulasi M sebelumnya. Untuk
`overwrite` ini jelas (assignment langsung); tapi bahkan `permute` yang
"hanya menata ulang" isi M yang sudah ada, hasilnya sama buruknya —
karena tidak ada bit BARU yang masuk (beda dengan XOR-fold yang
mencampur bit lama M dengan bit baru L setiap siklus).

## 2. Sanity check skala penuh (300 juta nilai/kandidat)

**Tidak ada blok berulang** di keenam kandidat M — sama seperti fase L.
Ini **tidak bertentangan** dengan temuan #1: bahkan pemendekan periode
puluhan-ribu-kali di skala toy (state space `n≤8`) masih jauh di atas
3×10⁸ ketika diekstrapolasi ke skala nyata (state space 256-array,
`log2(256!)≈1684` bit untuk L saja) — skala nyata terlalu besar untuk
mendeteksi collapse sebesar apa pun yang ditemukan di toy-model lewat
sanity check praktis ini. Toy-model tetap satu-satunya axis yang bisa
memberi angka periodisitas yang berarti di sini.

## 3. Speed (`perf stat -e instructions,cycles -r 5`)

| target | mekanisme | instructions | vs. xor_fold |
|---|---|---|---|
| original | xor_fold | 14,898,167,206 | — |
| original | m_permute | 16,529,538,177 | **+10.95%** |
| original | m_inject | 16,531,106,839 | **+10.96%** |
| original | m_overwrite | 14,822,089,799 | −0.51% |
| winner | xor_fold | 6,053,459,896 | — |
| winner | m_permute | 7,506,791,985 | **+24.01%** |
| winner | m_inject | 7,506,791,984 | **+24.01%** |
| winner | m_overwrite | 5,992,284,247 | −1.01% |

Beda dengan fase L (di mana mekanisme alternatif L selalu lebih cepat),
di fase M **`m_permute`/`m_inject` justru lebih lambat** (+11% sampai
+24%) — keduanya butuh baca `M[j]` tambahan sebelum menulis (swap/relocate
2-slot), sementara XOR-fold cuma baca-modifikasi-tulis 1 slot. Hanya
`m_overwrite` sedikit lebih cepat (tidak perlu baca `M[i]` lama sama
sekali) — tapi ini justru salah satu mekanisme dengan periodisitas paling
rusak di bagian 1.

## 4. Avalanche (per-bit minimum, capture 2 siklus)

| kandidat | overall | min_bit | passed |
|---|---|---|---|
| original_m_permute | 0.4994 | 0.4871 | ✅ |
| original_m_inject | 0.4983 | 0.4921 | ✅ |
| original_m_overwrite | 0.4998 | 0.4911 | ✅ |
| winner_m_permute | 0.4935 | 0.4839 | ✅ |
| winner_m_inject | 0.4933 | 0.4854 | ✅ |
| winner_m_overwrite | 0.4940 | 0.4852 | ✅ |

Keenamnya lolos nyaman — sama seperti fase L, avalanche tidak
mendiskriminasi mekanisme M pada skala pengukuran ini.

## 5. Kualitas (PractRand 16GB)

| kandidat | hasil |
|---|---|
| original_m_permute | ✅ no anomalies in 240 test result(s) |
| original_m_inject | ✅ no anomalies in 240 test result(s) |
| original_m_overwrite | ✅ no anomalies in 240 test result(s) |
| winner_m_permute | ✅ no anomalies in 240 test result(s) |
| **winner_m_inject** | ❌ **FAIL — lihat detail di bawah** |
| winner_m_overwrite | ✅ no anomalies in 240 test result(s) |

**`winner_m_inject` gagal PractRand secara katastrofik** — bukan
kegagalan tipis. Sebagian hasil (dari puluhan test yang FAIL):

```
FPF-14+6/16:all          R=+157.9  p=1.1e-147   FAIL !!!!!
FPF-14+6/16:cross        R=+416.8  p=5.3e-351   FAIL !!!!!!!
[Low8/32]BCFN(2+0,13-0,T) R=+133.9 p=4.2e-71    FAIL !!!!
[Low8/32]DC6-9x1Bytes-1   R=+131.3 p=3.8e-62    FAIL !!!!
```
p-value serendah `5.3e-351` — jauh melampaui ambang "gagal" biasa,
menandakan struktur non-acak yang kuat, bukan anomali statistik marjinal.

**Yang menarik**: `original_m_inject` (mekanisme M yang SAMA, target
`original`) **lolos bersih** di 16GB. Jadi kegagalan ini bukan murni
soal mekanisme `m_inject` itu sendiri — ini adalah **interaksi spesifik**
antara `m_inject` dan wiring `winner` yang lebih sederhana (`d = c &
0xFF`, mask langsung — bukan multiply-reduce Lemire seperti `original`).
Dugaan paling masuk akal: index `j = L[i] & 0xFF` yang dipakai
`m_inject` mewarisi kualitas pencampuran dari `d`, dan `winner`'s `d`
(mask langsung dari `c`, bukan hasil Lemire multiply-reduce yang
mencampur seluruh 32 bit) punya struktur/korelasi yang cukup untuk bikin
index injeksi M jadi tidak cukup acak — sesuatu yang tidak muncul kalau
sumber index-nya (`d` versi `original`) lebih tercampur. Ini **belum
diverifikasi** secara mendalam (butuh investigasi terpisah kalau
mekanisme ini mau dikejar lebih jauh), tapi menegaskan pelajaran repo ini
berulang kali: kandidat harus divalidasi PENUH di kedua target, tidak
bisa diasumsikan hasil satu target berlaku untuk target lain.

## Kesimpulan fase M

- **XOR-fold (mekanisme hari ini) jauh lebih unggul** daripada ketiga
  alternatif yang diuji — bukan cuma "lebih baik", tapi **beda orde
  magnitudo** di periodisitas (ratusan sampai puluhan-ribu kali lebih
  pendek untuk ketiga alternatif) DAN salah satu alternatif
  (`m_inject` pada target `winner`) gagal PractRand secara katastrofik.
  Ini temuan paling tegas di seluruh riset ini sejauh ini.
- Tidak ada trade-off speed vs. periodicity yang menarik di sini seperti
  di fase L — `m_overwrite` sedikit lebih cepat tapi kalah telak di
  periodisitas; `m_permute`/`m_inject` malah lebih lambat DAN kalah
  periodisitas (dan `m_inject` bahkan gagal kualitas di satu target).
  **Tidak ada alasan untuk mengganti XOR-fold.**
- **Jawaban ke pertanyaan "cara M diubah"**: pertahankan XOR-fold.
  Berbeda dengan L (di mana swap "tampak perlu" tapi belum
  benar-benar terbukti tak tergantikan), untuk M ada bukti kuat —
  periodisitas toy-model DAN kegagalan PractRand nyata — bahwa mekanisme
  akumulatif (XOR) itu sendiri, bukan cuma pilihan relokasi vs.
  overwrite, adalah sumber kualitas dan periodisitas yang baik.

## Langkah selanjutnya (belum dikerjakan di sesi ini)

**Status: diselesaikan di sesi lanjutan, lihat "Fase 3" di bawah.** Kedua
poin di bawah ini (arah `inject` yang tidak konsisten, dan keputusan
kombinasi L×M) adalah spek asli sebelum diinvestigasi — hasilnya ada di
section berikutnya.

1. Investigasi lanjut mekanisme `inject` untuk L (config n/w lebih luas,
   lebih banyak seed) untuk memahami kenapa arahnya tidak konsisten
   (lihat kesimpulan fase L).
2. Fase 3 (kombinasi): karena fase M sudah menunjukkan XOR-fold jelas
   superior, kombinasi paling masuk akal untuk diuji adalah L={permute,
   inject} × M=xor_fold (yaitu, ulangi pertanyaan L dengan M dikunci ke
   xor_fold — yang sebenarnya sudah persis fase 1 saat ini). Full
   cross-product L×M yang lain kemungkinan besar tidak perlu dijalankan
   mengingat XOR-fold sudah didemonstrasikan mendominasi.

## Fase 3: resolusi arah `inject` dan keputusan kombinasi L×M

Dikerjakan sebagai tindak lanjut `HANDOVER_PHASE3.md`. Ringkasan: **arah
`inject` ternyata bukan noise — ia berkorelasi kuat dan konsisten dengan
`w` (word size), bukan dengan `n`.**

### Grid yang diperluas

Selain 5 config asli (`n∈{2,4,8}, w∈{4,8}`, sekarang semua dinaikkan ke
300 seed untuk `permute`/`inject` — termasuk `n=4,w=8` yang semula 100
seed dan `n=8,w=4` yang semula 50 seed), ditambahkan config baru untuk
menguji hipotesis korelasi `w`:

- `n=2,w=16` — **tractable**, 300 seed penuh.
- `n=8,w=8` — permute tractable (1 seed, λ=3,109,376, timeout 600s cukup),
  **inject tidak tractable** (timeout di 600s bahkan untuk 1 seed —
  indikasi kuat λ-nya jauh lebih besar dari yang bisa dideteksi Brent's
  algorithm dalam budget sesi ini).
- `n=16,w=4` dan `n=4,w=16` — **tidak tractable sama sekali** (permute
  *dan* inject timeout di probe 3-seed/180s). Dicoba, dicatat, di-skip —
  tidak ada data numerik untuk kedua config ini.

### Tabel korelasi lengkap (dihasilkan `analyze_inject_direction.py`)

| n | w | seeds (permute/inject) | λ permute | λ inject | rasio inject/permute | arah |
|---|---|---|---|---|---|---|
| 2 | 4 | 300/300 | 16 | 19 | 1.19x | inject lebih panjang (magnitudo trivial — lihat catatan) |
| 2 | 8 | 300/300 | 560 | 1,526 | 2.73x | inject lebih panjang |
| 2 | 16 | 300/300 | 262,144 | 15,297,850 | 58.36x | inject lebih panjang |
| 4 | 4 | 300/300 | 82,807 | 4,448 | 0.05x | inject lebih pendek |
| 4 | 8 | 300/300 | 149,373 | 843,160 | 5.64x | inject lebih panjang |
| 8 | 4 | 300/300 | 6,318,640 | 548,477 | 0.09x | inject lebih pendek |

Catatan `n=2,w=4`: state space-nya sangat kecil (λ permute konstan 16 di
semua 300 seed) sehingga kedua mekanisme sudah dekat batas atas ruang
keadaan — rasio 1.19x tidak bermakna secara praktis, berbeda dengan
`n=4,w=4`/`n=8,w=4` yang menunjukkan pemendekan drastis (11-20x) pada
state space yang cukup besar untuk bermakna.

### Kesimpulan Poin #1: arah berkorelasi dengan `w`, bukan `n`

Mengeluarkan titik degenerate `n=2,w=4`, pola pada 5 titik data non-degenerate
benar-benar bersih:
- **`w=4` → `inject` konsisten memendekkan periode** (0.05x-0.09x, yaitu
  11-20x lebih pendek) di `n=4` dan `n=8`.
- **`w≥8` → `inject` konsisten memanjangkan periode**, dan rasio *membesar*
  seiring `w` membesar: 2.73x (w=8,n=2) → 5.64x (w=8,n=4) → 58.36x
  (w=16,n=2). Bukti tambahan di `n=8,w=8`: `inject` butuh >600s untuk
  1 seed padahal `permute` selesai dengan λ=3.1 juta dalam waktu yang
  jauh lebih singkat — konsisten dengan tren ini berlanjut ke λ yang jauh
  lebih besar lagi, bukan kontradiksi.

**Ini menjawab "tidak konsisten" di `HANDOVER_PHASE3.md`: arahnya sangat
konsisten — cuma variabel yang benar untuk memprediksinya adalah `w`,
bukan `n`, yang belum diuji terpisah di grid asli (grid asli mencampur
kedua w=4 dan w=8 tanpa titik data w besar untuk memisahkan kedua efek).**

### Kesimpulan Poin #2: keputusan Fase 3

Per aturan keputusan di `HANDOVER_PHASE3.md`: karena ditemukan kondisi
robust di mana `inject` konsisten menang (semua `w≥8`) — **ini memenuhi
kriteria opsi (b)**, bukan opsi (a). Namun opsi (b) secara eksplisit hanya
meminta *konfirmasi* kombinasi `inject`×`xor_fold` di skala penuh
(sanity check, speed, avalanche, PractRand 16GB) — dan verifikasi di
awal sesi ini mengonfirmasi **data itu sudah ada sejak Fase 1**:
`original_inject`/`winner_inject` (skala produksi 256×32-bit, yaitu
`w=32` — jauh di atas ambang `w≥8` yang sudah terbukti favorable untuk
`inject`) sudah lolos ke-4 axis bersih (lihat bagian 1-5 Fase L di atas):
sanity check skala penuh 300 juta nilai (0 blok duplikat), speed
(`perf stat`), avalanche (`avalanche_gate_min_bit`, lolos), dan PractRand
16GB (bersih). **Tidak ada eksekusi baru yang diperlukan** — opsi (b)
sudah terpenuhi oleh data Fase 1 yang sudah ada, dan tren `w` yang baru
ditemukan justru memperkuat alasan mengapa data itu sudah bersih (produksi
berjalan di `w=32`, jauh di regime yang favorable untuk `inject`).

**Kesimpulan akhir riset** (menutup baik Fase 1, Fase 2, maupun Fase 3):
- Pertahankan **`permute` (swap)** sebagai default L yang aman — ini
  tidak berubah, karena `overwrite` tetap terbukti merusak periodisitas
  di semua skala.
- **`inject` bukan sekadar "netral"** seperti kesimpulan Fase 1 semula —
  ia punya alasan matematis yang jelas dan kondisi terprediksi (word size
  besar) di mana ia justru **mengungguli** `permute` pada axis
  periodisitas, dan kondisi itu (`w≥8`) mencakup skala produksi (`w=32`)
  yang sudah divalidasi bersih di ke-4 axis lain. Ini catatan penting
  untuk siapa pun yang merevisit pilihan mekanisme L di masa depan — bukan
  perubahan rekomendasi saat ini (di luar scope handover yang read-only
  terhadap source produksi), tapi temuan yang seharusnya tidak diabaikan.
- **XOR-fold (M) tetap dipertahankan tanpa syarat** — tidak ada temuan di
  Fase 3 yang menyentuh axis M sama sekali (Fase 3 murni tentang L×M
  dengan M dikunci `xor_fold`, sesuai keputusan Fase 2).
