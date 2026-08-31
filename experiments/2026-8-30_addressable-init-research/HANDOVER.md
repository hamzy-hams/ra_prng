# Handover: Riset Addressable State Init (gaya Philox) untuk winner_wired_v2

Ditulis untuk sesi Claude Code baru (atau user) yang mengambil ini nanti.
Status per 2026-08-30: user sudah menempelkan Kandidat 5 (lihat Lampiran di
bawah) — spek sekarang lengkap untuk mulai Tahap 0, tapi **implementasi
(prototipe Python, harness, data) masih belum dikerjakan** di folder ini.

## Konteks: kenapa riset ini muncul

Lahir dari sesi devil's-advocate yang menantang nilai riset paralelisasi
sebelumnya (`experiments/2026-8-29_parallelization-research/`): dibanding
Philox/Threefry (independensi antar-stream terbukti dari konstruksi
counter-mode, dipakai default JAX via Threefry2x32) dan xoshiro256**
(jump()/long_jump() provably non-overlapping), validasi paralelisasi
`winner_wired_v2.c` yang sudah ada sifatnya **empiris** (diuji sampai
500.000 seed acak/128GB PractRand, bukan dibuktikan dari struktur).

Tapi diskusi lanjutan menemukan celah struktural yang Philox **tidak**
punya: Philox itu `f(key, counter)` murni stateless — tiap panggilan
reinit dari nol, bagus untuk random-access tapi tidak dirancang untuk
"lanjut generate sangat panjang dari satu alamat tanpa biaya reinit
berulang" (walau counter-increment sekuensial biasa tetap murah). Kalau
`winner_wired_v2.c` bisa dibuat *addressable* seperti Philox (alamat
= kombinasi seed/key/counter yang deterministik dan tidak collision)
**sekaligus** tetap bisa dilanjutkan generate sekuensial murah dari alamat
itu (reuse state, tanpa reinit), itu kombinasi yang tidak dimiliki Philox
(reinit-heavy per alamat) atau xoshiro/MT (tidak addressable sama sekali).

## Fakta arsitektural kunci yang jadi dasar hipotesis

Dari `experiments/2026-8-27_operand-position-search/winner_wired_v2.c`:

- `ra_init_state()` (baris 73-78) mengisi `L[256]`/`M[256]` dengan formula
  tetap **identik untuk semua seed**. Satu-satunya input seed sekarang
  adalah skalar `cons` (baris 124: `cons = seed`).
- Di `ra_permutation_cycle()` (baris 81-104), `L` **tidak pernah dibaca**
  untuk menghitung `a/b/c/d` — cuma posisinya ditukar (baris 100-102:
  `tmp=L[i]; L[i]=L[d]; L[d]=tmp;`). Konsekuensi: **multiset nilai `L`
  invariant sepanjang eksekusi**, untuk seed apa pun — cuma urutannya yang
  berubah (dipermutasi oleh jejak nilai `d`, yang diturunkan dari `cons`).
- `M` berubah drastis tiap langkah lewat `o`, `a`, `b`, `c` (avalanche
  tinggi) DAN di-reseed tiap 255 langkah lewat `ra_reseed()` (baris
  106-115): `M[i] ^= L[i]` untuk semua `i`, lalu di-hash jadi `cons` baru.

**Hipotesis inti riset ini**: kalau multiset awal `L` (bukan cuma `cons`)
dibuat unik per "alamat" (kombinasi seed/key/counter), maka:
1. `M`/`cons` tetap menyediakan avalanche/kualitas statistik seperti sekarang
   (mekanisme itu tidak disentuh).
2. `L` menyimpan "fingerprint alamat" yang invariant terhadap permutasi, dan
   terus disuntik ulang ke `M` tiap reseed (bukan pengaruh awal yang luntur)
   — inilah yang diharapkan mencegah dua alamat berbeda konvergen ke
   lintasan yang sama dalam jangka panjang.

**Peringatan yang sudah diidentifikasi (jangan diabaikan)**: klaim "avalanche
cepat" dan "alamat harus tetap unik" berpotensi **berlawanan arah** — makin
cepat pengaruh `L` awal "dilupakan" dinamika `M`/`cons`, makin besar risiko
dua alamat berbeda konvergen jangka panjang. Argumen invariant-multiset di
atas adalah jawaban terhadap kekhawatiran ini, tapi **belum dibuktikan** —
itulah yang harus divalidasi di Tahap 0 di bawah, sebelum apa pun lain.

## Constraint read-only (diwarisi dari riset-riset sebelumnya)

- **Never modify** `src/ra_prng2/*`, `src/ra_prng3/*`, atau
  `winner_wired_v2.c` (termasuk semua varian `winner_wired*.c` di
  `2026-8-27_operand-position-search/`) in place. Varian addressable adalah
  **file/binary baru di folder ini**, bukan modifikasi in-place.
- Mode "lanjut generate" (tanpa reinit) harus tetap identik perilakunya
  dengan `winner_wired_v2.c` yang sekarang — supaya perbandingan speed/
  kualitas tetap adil dan baseline yang sudah tervalidasi (128GB PractRand,
  dsb.) tetap relevan sebagai pembanding.

## Flow riset yang disepakati (5 tahap, urutan ini penting)

**Tahap 0 — Uji mekanisme inti, murah, TANPA sentuh C.**
Prototipe cepat (Python cukup) yang mengeksplorasi kandidat formula `L[i]`
(lihat kandidat di bawah) dengan `seed`/`cons` tetap, jalankan **banyak
siklus reseed** per konfigurasi (skala setara jutaan-milyaran output, bukan
cuma ratusan kata — meniru cakupan `collision_scan.py`/`cross_correlation.py`
dari riset paralelisasi, tapi menyapu ruang `(L_multiplier, counter)`,
bukan `seed`). Tujuan: buktikan atau patahkan hipotesis inti di atas —
apakah dua alamat berbeda benar-benar tidak pernah konvergen jangka
panjang, dan apakah ada pasangan parameter yang kebetulan menghasilkan
multiset `L` yang tumpang tindih/berkorelasi (simetri aritmetika modulo
2³²: `mult` vs `2^32-mult`, kelipatan kecil, dll). **Jangan lanjut ke tahap
berikutnya sebelum ini lolos** — kalau gagal di sini, semua tahap
berikutnya percuma.

**Tahap 1 — Bekukan spesifikasi parameter.**
Setelah Tahap 0 lolos: pilih final parameter set (`seed`, `counter` sebagai
address-selector MURNI — tidak dipakai di dalam badan operasi utama
`ra_permutation_cycle`, cuma untuk menurunkan konfigurasi `L`/`M` awal —
dan `L_multiplier`/`M_multiplier` atau turunannya) dan formula pastinya.
Tulis sebagai spec kecil sebelum ngoding.

**Tahap 2 — Implementasi terpisah.**
File C baru (bukan modifikasi `winner_wired_v2.c`), mode "continue" identik
perilakunya dengan sekarang.

**Tahap 3 — Validasi statistik ulang penuh.**
Karena `ra_init_state` berubah, baseline lama (periodicity, avalanche,
PractRand) tidak otomatis berlaku untuk varian baru. Re-run infrastruktur
yang sudah ada (dari riset-riset sebelumnya) dengan target baru:
collision-scan di ruang `(seed, multiplier, counter)` (bukan cuma `seed`),
plus PractRand pada campuran output antar-alamat.

**Tahap 4 — Adu kecepatan (bukan untuk "menang", untuk cari titik impas).**
Dua angka: (a) biaya jump murni (init + reseed pertama) vs satu panggilan
Philox, dan (b) throughput mode-lanjut (reuse) dibanding Philox kalau
sama-sama generate panjang — cari panjang-output minimum di mana biaya
jump "tertutup" oleh reuse. Target bukan "lebih cepat dari Philox", tapi
angka konkret untuk niche: addressable + reuse murah tanpa reinit
berulang, untuk kasus **sedikit alamat yang masing-masing dijalankan
panjang** (mis. Monte Carlo: 1000 model paralel, tiap model jalan lama) —
BUKAN kasus banyak-alamat-pendek-pendek (itu domain Philox/JAX, generator
ini kemungkinan kalah di situ karena biaya jump/init tidak amortisasi).

**Tahap 5 (opsional, kalau Tahap 4 nunjukkin jump jadi bottleneck)** —
optimasi biaya init (mis. percepat loop 256-iterasi, sederhanakan formula)
tanpa merusak hasil Tahap 0/3.

## Kandidat formula awal (usulan asisten — draft starting point, BUKAN final)

User akan menambah kandidat sendiri dari riset lama di luar repo (pernah
mencoba `seed`, `counter`, dan satu `multiplier` untuk salah satu array).
Berikut draf awal untuk Tahap 0, urutan prioritas prototipe:

**Kandidat 1 (prioritas pertama — perubahan minimal, isolasi klaim inti).**
`M` TIDAK diubah sama sekali (formula asli `winner_wired_v2.c` dipertahankan
utuh, supaya semua sifat avalanche yang sudah tervalidasi tetap berlaku).
Hanya `L` yang jadi fungsi `(seed, counter)`, lewat mixer kuat (splitmix64-
style) supaya multiplier/offset tidak degenerate (dipaksa ganjil):
```c
M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);  // ORIGINAL, tak diubah

uint64_t k = splitmix64(((uint64_t)seed << 32) | counter);
uint32_t L_mult = (uint32_t)(splitmix64_next(&k) | 1u);
uint32_t L_off  = (uint32_t)(splitmix64_next(&k) | 1u);
L[i] = (uint32_t)(i * L_mult + L_off);
```
Paling cocok untuk Tahap 0 karena langsung menguji hipotesis inti user
("L bawa keunikan, M/cons bawa avalanche") dengan permukaan-ubah sekecil
mungkin.

**Kandidat 2 (kalau Kandidat 1 kurang cukup distinctness-nya).**
Kedua array dikunci independen (entropi alamat maksimal, tapi permukaan
validasi lebih besar):
```c
uint64_t kL = splitmix64(((uint64_t)seed << 32) | counter);
uint64_t kM = splitmix64(kL ^ 0x9e3779b97f4a7c15ULL);  // domain-separated
uint32_t L_mult = (uint32_t)(splitmix64_next(&kL) | 1u);
uint32_t L_off  = (uint32_t)(splitmix64_next(&kL) | 1u);
uint32_t M_mult = (uint32_t)(splitmix64_next(&kM) | 1u);
uint32_t M_off  = (uint32_t)(splitmix64_next(&kM) | 1u);
L[i] = i * L_mult + L_off;
M[i] = i * M_mult + M_off;
```

**Kandidat 3 (kontrol negatif — dipakai untuk menguji metodologi Tahap 0
itu sendiri, bukan kandidat produksi).** `L` cuma di-*rotasi* oleh
`counter` (multiset TIDAK berubah, cuma reindex), `M` formula asli:
```c
for (int i = 0; i < 256; ++i) {
    int j = (i + counter) & 0xFF;
    L[j] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
}
M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);  // ORIGINAL
```
Dugaan: ini SEHARUSNYA gagal uji distinctness Tahap 0 (karena multiset-nya
sama persis, cuma rotasi). Kalau ternyata malah lolos, itu tanda metodologi
uji Tahap 0-nya kurang sensitif dan perlu diperkuat sebelum dipercaya untuk
menilai Kandidat 1/2.

**Kandidat 4 (paling dekat ke kalimat awal user — "M[i] = i*mult + operasi
lain yang melibatkan counter").**
```c
uint32_t L_mult = (uint32_t)((seed * 0x2545F4914F6CDD1DULL) >> 32) | 1u;
L[i] = (uint32_t)(i * L_mult + (uint32_t)counter * 0x9E3779B1u + 0x9e3779b7UL);
M[i] = ...; // ikut Kandidat 1 atau 2, TBD
```

## Lampiran: formula dari riset lama (Kandidat 5, ditempel 2026-08-30)

User membawa formula konkret dari riset lamanya di luar repo:

```c
L[i] = (rot32(i * multiplier + 0x9e3779b7 * seed, seed ^ i)) & 0xFFFFFFFF;
M[i] = (rot32(i * 0x06a0dd9b + 0x06a0dd9b * seed, seed ^ i)) & 0xFFFFFFFF;
```

`rot32(n, r)` di sini adalah definisi kanonik yang sudah dipakai di seluruh
repo (rotate-**left** 32-bit, `r &= 31` sebelum dipakai) — identik dengan
`src/ra_prng2/python/ra_prng2.py:9`, `src/ra_prng2/c/ra_prng2.c:16`, dan
`winner_wired_v2.c:59` (bukan `rotr32`, bukan implementasi baru).

**Beda dari Kandidat 1-4 di atas**: Kandidat 1-4 memakai `splitmix64` untuk
menghasilkan `mult`/`off` yang lalu dipakai di formula linear
`L[i] = i*mult + off`. Kandidat 5 tidak pakai `splitmix64` sama sekali —
`seed` dipakai langsung dua kali (sebagai pengali di suku aditif, dan
sebagai penentu jumlah rotasi bareng `i`), dan **kedua** `L` maupun `M`
jadi fungsi `seed` (Kandidat 1 sengaja mempertahankan `M` seperti formula
asli tak berubah; Kandidat 5 mengubah keduanya).

**Parameter/asumsi yang perlu dikonfirmasi/disapu di Tahap 0**:
- `multiplier` (khusus array `L` — `M` tetap pakai konstanta asli
  `0x06a0dd9b`): **parameter bebas untuk disapu** (dikonfirmasi user), bukan
  konstanta tetap. Sapu beberapa nilai ganjil, termasuk `0x9e3779b7` sebagai
  default pembanding.
- `counter`: **tidak muncul di formula ini sama sekali** — cuma `(seed, i,
  multiplier)`. Diasumsikan sementara: **address = seed saja** (kandidat ini
  murni fungsi `(seed, i)`, tanpa dimensi counter terpisah). Ini asumsi yang
  perlu dikonfirmasi/dikoreksi user — tapi murah diubah karena formula
  sendiri tidak berubah, cuma cara address di-generate sebelum dipanggil
  formula ini (mis. `seed_efektif = mix(seed_asli, counter)`).

**Risiko struktural yang harus diuji eksplisit di Tahap 0 (bukan
diasumsikan aman)**: karena `rot32` melakukan `r &= 31` secara internal dan
`i` cuma berkisar 0-255, jumlah rotasi efektif untuk `L[i]` adalah
`(seed & 0x1F) ^ (i & 0x1F)` — **hanya bergantung pada 5 bit rendah
`seed`**. Dua seed yang kebetulan sama di 5-bit rendahnya (probabilitas
1/32 per pasangan) akan menghasilkan **pola urutan rotasi yang identik**
untuk seluruh array `L`. Suku `i * multiplier + 0x9e3779b7 * seed` (mod
2³²) tetap berbeda per seed (bergantung pada seluruh 32 bit `seed`, bukan
cuma 5 bit rendah) jadi rotasi yang sama belum tentu menghasilkan `L[i]`
yang sama — tapi ini persis pola simetri aritmetika yang Tahap 0 dirancang
untuk menangkap (lihat "Tahap 0" di atas: "simetri aritmetika modulo 2³²
... kelipatan kecil, dll"). **Jangan lewati pengujian ini** — sapu
pasangan `(seed_a, seed_b)` yang sengaja dipilih berbagi 5-bit rendah yang
sama sebagai kasus uji khusus, di samping sampling acak/sekuensial biasa.

## Status: Tahap 0 SELESAI (2026-08-30) — lolos, lanjut ke Tahap 1

Prototipe Python (`tahap0_prototype.py`) sudah dibuat dan dijalankan (smoke +
full tier). Ringkasan (detail lengkap + tabel angka di `RESULTS.md`):

- Port Python `ra_core`/`ra_permutation_cycle`/`ra_reseed`/`rot32`/`ra_hash`
  divalidasi **bit-identik** terhadap biner `winner_wired_v2` sebelum
  kandidat mana pun diuji.
- Kandidat 1, 2, 4, **5 (formula user)** semua **lolos bersih** di tier full
  (2.000-8.000 alamat, ~19 siklus reseed): 0 collision multiset, 0 collision
  stream, 256/256 nilai `L`/`M` tetap distinct (tidak collapse), avalanche
  sehat (mean ~0.49, min-bit ~0.47-0.49, jauh di atas floor 0.2).
- Kandidat 3 (kontrol negatif) **gagal seperti yang diprediksi** — di tier
  full malah gagal lebih jelas dari perkiraan: 1.999/2.000 collision
  multiset (memang harus, itu maksud kontrol) DAN 1.744/2.000 collision
  stream (baru muncul di tier full, ~19 siklus reseed; di tier smoke yang
  cuma ~2 siklus, stream-nya masih bersih) — bukti bahwa uji prefix pendek
  tidak cukup untuk mendeteksi konvergensi jangka panjang, jadi hasil "0
  collision" Kandidat 1/2/4/5 di tier full adalah sinyal yang jauh lebih
  kuat daripada kalau cuma diuji di prefix pendek.
- Uji stres khusus Kandidat 5 (64 seed berbagi 5-bit-rendah yang sama,
  risiko yang dicatat di Lampiran) juga **bersih** (0 collision) — risiko
  itu tidak jadi kegagalan dominan di skala Tahap 0 ini.
- **Tahap 0b (fallback search) TIDAK dijalankan** — tidak diperlukan karena
  semua kandidat formula lolos.

## Tahap 1: Spesifikasi Final (dibekukan 2026-08-30)

User mengonfirmasi kedua hal yang menggantung dari Tahap 0: **address =
`key` saja** (rename dari `seed` -- lihat catatan penamaan di bawah),
`counter` TIDAK dipakai untuk Kandidat 5. Kandidat yang dibekukan maju ke
Tahap 2 adalah **Kandidat 5** (formula milik user, alasan riset ini ada) --
bukan Kandidat 1/2/4 yang sama-sama lolos Tahap 0, tapi bukan kontribusi
user.

**Penamaan (berlaku mulai Tahap 1, murni rename, tidak ada perubahan
formula/perilaku):**
- `seed` → **`key`** (istilah Philox persis -- perannya memang sebagai key:
  penentu identitas/independensi tiap alamat).
- `multiplier` (di Kandidat 5) → **`L_MIX_CONST`**, dan sekaligus dibekukan
  jadi konstanta tetap (bukan lagi parameter bebas yang disapu). Sengaja
  **bukan** bagian dari vocabulary key/counter karena dia konstanta mixing
  internal, bukan dimensi alamat -- supaya kalau `counter` beneran dipakai
  nanti sebagai dimensi alamat kedua, tidak ambigu dengan konstanta ini.
- `counter`: dicadangkan untuk dimensi alamat kedua di masa depan, TIDAK
  dipakai Kandidat 5 sekarang.

**`L_MIX_CONST` dibekukan ke `0x9e3779b7`** -- nilai default yang sudah diuji
Tahap 0 (satu dari 4 nilai yang disapu, tidak ada beda distinctness di antara
keempatnya pada skala Tahap 0), dan sudah jadi konstanta kanonik di formula
`L` original non-addressable. Ini pilihan yang bisa dikoreksi, bukan
satu-satunya opsi valid yang terbukti optimal.

**Formula final (siap untuk Tahap 2, format C):**
```c
#define ADDR_L_MIX_CONST 0x9e3779b7u

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
`cons` awal di `ra_core` tetap `= key` (nilainya tidak berubah dari `seed`
original, cuma nama parameternya) -- sama seperti yang divalidasi harness
Tahap 0 (`ra_core_from_state()` di `tahap0_prototype.py`, sudah di-rename
mengikuti konvensi ini dan tetap lolos `sanity_check()` bit-identik terhadap
biner `winner_wired_v2`).

**Avalanche min-bit DAN max-bit** (per permintaan user, lihat
`RESULTS.md`): kelima kandidat sehat di kedua ujung -- tidak ada bit seed
yang lemah (min) maupun over-amplified (max) secara sistematis; band
Kandidat 5 adalah [0.4859, 0.5116].

**Status: Tahap 2 SELESAI (2026-08-30).** File baru
`winner_wired_addressable.c` (tidak menyentuh `winner_wired_v2.c`/varian
`winner_wired*` in-place). Kompilasi bersih, tervalidasi bit-identik
terhadap referensi Python (`init_kandidat5()`/`ra_core_from_state()` di
`tahap0_prototype.py`) untuk 6 nilai `key` (termasuk kasus tepi `key=0` dan
`key=2^32-1`), plus properti "mode continue" (generate lebih panjang tidak
pernah reinit/mengganggu prefix yang sudah dihasilkan) dikonfirmasi empiris.
Detail lengkap di `RESULTS.md` bagian "Tahap 2: C Implementation".

**Status: Tahap 3 SELESAI (2026-08-30).** Tiga skrip baru
(`tahap3_cross_correlation.py`, `tahap3_collision_scan.py`,
`tahap3_interleave_practrand.py`) memakai ulang infrastruktur
`experiments/2026-8-29_parallelization-research/` (tidak dimodifikasi
in-place), diarahkan ke `winner_wired_addressable`. Semua tiga metode lolos
bersih, smoke maupun full tier, sequential maupun random key sampling:
- Cross-correlation: 0 pasangan flagged (adjacent maupun kontrol) di
  smoke (K=8) dan full (K=128, 8.128 pasangan).
- Collision-scan: 0 collision di sequential dan random, smoke dan full
  (hingga M=500.000 key acak), probabilitas false-positive digest
  diabaikan (maks ~1.63e-05).
- Interleaved PractRand: smoke (64MB) -> medium (1GB) -> full (16GB, K=8)
  semuanya PASSED, tanpa FAIL/SUSPICIOUS -- hanya tag "unusual" terisolasi
  yang tidak persisten (pola sama seperti baseline `winner_wired_v2`).
  `xlarge` (128GB) juga dijalankan sebagai validasi tambahan setara
  kedalaman baseline lama.

Detail angka lengkap di `RESULTS.md` bagian "Tahap 3: Validasi Statistik".
**Kandidat 5 addressable sekarang tervalidasi setara kedalaman baseline
`winner_wired_v2`.**

**Status: Tahap 4 SELESAI (2026-08-30).** File baru `tahap4_bench.c` +
`tahap4_benchmark.py` (tidak menyentuh `winner_wired_addressable.c` atau
file Philox manapun di `benchmarks/comparisons/` in-place). Metodologi
3-test dibakukan user: (1) adu inisiasi murni, (2) sweep N throughput
sekali-jalan-panjang, (3) sweep K empiris siklus `init;generate K;buang`
berulang.

**Bug ditemukan atas permintaan user untuk verifikasi ekstra**: `PHILOX_M0`
di ketiga file Philox yang ada di repo (`benchmarks/comparisons/source/
philox.c`, `philox_amortized.c`, `RNGing_speed/src/philox.c`) salah --
nilainya (`0xD256D193U`) adalah konstanta Philox**2x32**, bukan Philox4x32
(`0xD2511F53U` yang benar, dikonfirmasi terhadap source resmi Random123).
File lama sengaja TIDAK diperbaiki (keputusan user, di luar scope, akan
merusak hasil benchmark lama tersimpan) -- `tahap4_bench.c` pakai konstanta
yang benar, diverifikasi runtime lewat 3 known-answer-test resmi Random123
(`./tahap4_bench verify`, semua PASS).

**Hasil**: break-even point ~49-71 kata/alamat (Test 2 analitik N*~=49,
Test 3 empirik K*~=71, konsisten satu sama lain). Di bawah titik itu Philox
menang (biaya "landing" nyaris nol per definisi); di atasnya
`winner_wired_addressable` reuse/continue menang, dan keunggulannya
mengarah stabil ke ~15-22% lebih cepat per-kata untuk run panjang.
Detail lengkap + tabel di `RESULTS.md` bagian "Tahap 4: Speed Benchmark vs
Philox".

**Status: Tahap 5 SELESAI (2026-08-30).** File baru `tahap5_bench.c` +
`tahap5_benchmark.py` (fork struktur `tahap4_bench.c`/`tahap4_benchmark.py`,
tidak menyentuh keduanya in-place). Ketiga kandidat berperingkat di
`HANDOVER_TAHAP5.md` §4 diimplementasi dan diukur: **Rank 2** (rotate
eksplisit `_mm256_rolv_epi32` AVX-512VL, menggantikan pola `sllv+srlv+or`
auto-vectorizer) diadopsi ke file baru `winner_wired_addressable_v2.c`
(bit-identik terverifikasi 38 key, jadi otomatis mewarisi jaminan
statistik Tahap 3 tanpa perlu diulang). Rank 1 (`-mprefer-vector-width=512`,
flag saja) dan Rank 3 (`-funroll-loops`, flag saja) sama-sama menang nyata
tapi tidak diadopsi jadi utama (Rank 1 tidak menambah keuntungan di atas
Rank 2 karena `v1_rolv` sudah eksplisit 256-bit; Rank 3 lebih kecil
keuntungannya). Satu percobaan tambahan (strength-reduction manual/
akumulator) dites saat perencanaan dan **ditolak** -- bit-identik tapi
4.4-5.0x lebih lambat karena mematikan auto-vectorization compiler (lihat
`RESULTS.md` §Tahap 5 untuk detail lengkap + catatan penting soal noise
pengukuran reinit-sweep single-run yang sempat menyesatkan).

**Hasil**: break-even baru N\*~=26 (dari ~49), K\*~=57 (dari ~71,
median-of-3) -- desain addressable sekarang menang atas Philox produksi
pada kira-kira separuh ambang lifetime-alamat dibanding Tahap 4, tanpa
mengubah formula atau jaminan statistiknya.

**Keputusan (2026-08-30, setelah Tahap 5 selesai): `winner_wired_addressable.c`
(BUKAN `_v2.c`) tetap jadi baseline standar untuk eksperimen selanjutnya.**
Alasan eksplisit dari user: optimasi Tahap 5 (`_mm256_rolv_epi32`, intrinsic
AVX-512VL) spesifik arsitektur/device -- CPU x86 dengan AVX-512VL saja. Kalau
eksperimen berikutnya menyasar GPU, Android/ARM, atau arsitektur lain, jalur
optimasi yang relevan kemungkinan besar berbeda total (tidak ada AVX-512VL,
model vektorisasi beda, dst.), jadi memaksakan `_v2.c` sebagai basis baru
akan menanam asumsi non-portable ke eksperimen yang belum tentu jalan di
platform target. `winner_wired_addressable_v2.c` **tetap ada dan valid**
sebagai jalur cepat opsional khusus x86/AVX-512VL (formula identik, cuma
implementasi lebih cepat di kelas hardware itu) -- bukan dihapus, hanya
tidak dijadikan titik lanjut default. Kalau sesi mendatang butuh basis paling
cepat DAN sudah pasti target x86 dengan AVX-512VL, `_v2.c` masih boleh
dipakai secara eksplisit; default-nya tetap `winner_wired_addressable.c`.

## Terminologi (untuk paper baru, 2026-08-30)

Istilah dokumentasi/paper untuk mekanisme `f(key)` riset ini, dibangun di
percakapan lanjutan setelah Tahap 5 selesai — **bukan** rename kode. Nama
variabel/fungsi internal (`key`, `L`, `M`, `ra_init_state_addressable`,
dst.) tetap seperti sudah dibekukan di Tahap 1-2 di atas.

Latar: Philox `f(counter, key)` bersifat *stateless, point-addressing* —
tiap panggilan independen, bisa lompat ke posisi manapun O(1) tanpa histori.
RA-PRNG `f(key)` bersifat *stateful, coarse-addressing* — `key` tidak
memilih satu nilai, tapi memilih seluruh lintasan (trajectory) permutasi-
cycle dari `L`/`M` yang bisa dilanjutkan sekuensial dari titik itu.

Istilah yang dipilih (via pertanyaan pilihan ganda ke user):

- **Orbit Addressing** — nama mekanisme `f(key)` RA-PRNG secara keseluruhan,
  dari istilah dynamical systems: `key` memilih "orbit" — lintasan
  deterministik penuh dari state awal lewat permutation-cycle map. Presisi
  matematis, langsung nyambung ke formalisme `F = Reseed ∘ Mutate ∘ Permute`
  yang sudah ada di kedua paper (`research/csai2025_ra_prng/`,
  `research/iccs2026_ra_prng/`). Dikontraskan dengan **point addressing**
  untuk Philox.
- **Orbit Engine** — peran konseptual (bukan nama fungsi kode) dari
  mekanisme yang menghitung `f(key) → (L, M)` awal, yaitu apa yang
  diimplementasikan `ra_init_state_addressable()`.
- **Orbit** — objek yang dipilih `key`: seluruh lintasan deterministik state
  yang dilalui lewat pemanggilan `ra_permutation_cycle` berulang dari
  inisialisasi itu.
- `key` boleh disebut deskriptif "orbit key" di prosa paper.

Penamaan bagian-bagian lain RA-PRNG (di luar addressing) masih berlangsung
di sesi lanjutan — bagian ini akan diperbarui begitu ada keputusan final
berikutnya.

## Tahap 6 (2026-08-31)

Fast path tanpa `L[]` untuk kasus `rng <= 255` ("addressable penuh/agresif")
— ide yang sudah diantisipasi & ditunda di `HANDOVER_TAHAP5.md` section 4,
dikerjakan setelah user eksplisit meminta di sesi ini. **Selesai** —
validasi bit-identik exhaustif (9.945/9.945 kombinasi PASS) dan benchmark
menunjukkan `ra_core_singleblock` jauh lebih cepat dari baseline di semua
titik sweep `K in [1,255]` (36-92% lebih cepat tergantung `K`). Detail
lengkap: `HANDOVER_TAHAP6.md`, `RESULTS.md` bagian "## Tahap 6". Belum
dipromosikan ke `winner_wired_addressable.c`/`_v2.c`/`src/` — tetap
kandidat riset berscope sempit (`rng<=255` saja).

## Setelah selesai

Jalankan `/graphify --update` sebelum menutup tugas — wajib per `CLAUDE.md`
proyek ini untuk memasukkan file-file baru ke knowledge graph.

**Catatan (2026-08-31, Tahap 6)**: skill `graphify` tidak terdaftar di
sesi tempat Tahap 6 dikerjakan — langkah ini tidak bisa dijalankan otomatis
di sesi itu. Perlu di-flag ke user / dijalankan manual di sesi lain yang
skill-nya tersedia.
