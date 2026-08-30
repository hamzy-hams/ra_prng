# Handover: Riset Addressable State Init (gaya Philox) untuk winner_wired_v2

Ditulis untuk sesi Claude Code baru (atau user) yang mengambil ini nanti.
Ini adalah **spek untuk riset yang belum dimulai** — belum ada file
kode/harness/data apa pun di folder ini, hanya dokumen ini. Trigger untuk
lanjut: user bilang "lanjutkan riset addressable" dan akan menambahkan
kandidat formula `L[i]`/`M[i]` miliknya sendiri dari riset lama di luar repo.

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

## Lampiran: formula dari riset lama (belum diisi)

User punya riset terpisah **di luar repo ini** yang pernah mencoba bikin
init `L`/`M` bisa dikustomisasi (`seed`, `counter`, dan satu `multiplier`
untuk salah satu array — persis pola Kandidat 1-4 di atas, tapi versi
aslinya). Formula konkretnya belum ditemukan/ditempel di sini — user akan
mencarinya di direktori riset lama dan menambahkannya sendiri (atau
memintanya dicatat di sini) di sesi lanjutan. **Sesi berikutnya: kalau user
membawa formula ini, tempel di sini sebagai lampiran resmi sebelum mulai
Tahap 0**, supaya jadi kandidat tambahan yang disandingkan dengan Kandidat
1-4 draft asisten di atas — bukan menggantikannya begitu saja tanpa
dicatat dulu.

## Belum dikerjakan

Dokumen ini murni spesifikasi awal (ditulis 2026-08-30) — belum ada file
kode, harness, prototipe Python, maupun data pengukuran apa pun di folder
ini. Sesi berikutnya yang mengambil alih riset ini mulai dari Tahap 0
(prototipe Python murni, bukan C, bukan full pipeline) memakai kandidat di
atas SEBAGAI STARTING POINT — gabungkan dengan formula yang user bawa dari
riset lamanya sebelum mulai coding Tahap 0.

## Setelah selesai

Jalankan `/graphify --update` sebelum menutup tugas — wajib per `CLAUDE.md`
proyek ini untuk memasukkan file-file baru ke knowledge graph.
