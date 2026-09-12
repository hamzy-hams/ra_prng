# Hasil validasi `ra_core` dengan workload Monte Carlo nyata

Status: **Workload A dan Workload B SELESAI**. Lihat `HANDOVER.md` untuk
desain lengkap keduanya (dikunci sebelum implementasi).

---

# Bagian A — Monte Carlo π estimation (niche `singleblock`)

## Setup

Sesuai `HANDOVER.md`: 1.000.000 trial independen, tiap trial di-reseed
dengan key baru (`key = trial_index * 2654435761u + 1u`), generate TEPAT
255 word (K=255, sweet-spot `singleblock`), pakai 254 word pertama (127
pasang koordinat) untuk estimasi Monte Carlo π via unit-circle hit test.
Dibandingkan: `ra_core_singleblock`, `xoshiro256**`, `PCG32` — ketiganya
dengan pola reinit-per-trial yang identik.

**Fidelitas kode**: `sb_init_state`/`sb_cycle_buf` di
`workload_a_singleblock_pi.c` adalah salinan byte-for-byte dari
`ra_init_state_singleblock`/`ra_permutation_cycle_singleblock` di
`src/ra_core/c/ra_core.c` (termasuk `always_inline`, memakai `rot32`/
`fmix32`/`GUARD_M`/`GUARD_M2` langsung dari `ra_core.c` yang di-`#include`,
bukan didefinisikan ulang, supaya tidak mungkin ada drift konstanta) —
buffer-output menggantikan `fwrite`. **Diverifikasi lewat `run_parity_check()`**
sebelum run utama: output `sb_cycle_buf`/`sb_init_state` dibandingkan
byte-demi-byte terhadap `ra_core_singleblock` ASLI (dipanggil lewat
`fmemopen`) untuk 6 key berbeda — **PASS**, 0 mismatch.

**Anti-DCE**: dikonfirmasi lewat `objdump` bahwa `ra_core_singleblock_into`
di binary akhir berisi kode AVX2 (`ymm`) genuine untuk hash M[256] —
BUKAN dead-code-eliminated (lihat catatan metodologi di bawah, ini
relevan karena sesi ini berulang kali menemukan jebakan DCE serupa).

## Hasil

| Generator | π estimasi | \|error\| | 4×SE | Status | Wall-clock (min-of-5, 1.000.000 trial × 255 word) |
|---|---:|---:|---:|---|---:|
| `singleblock` | 3.141684 | 0.000091 | 0.000583 | **PASS** | 0.93 s |
| `xoshiro256**` | 3.141851 | 0.000259 | 0.000583 | **PASS** | 0.49 s |
| `pcg32` | 3.141399 | 0.000194 | 0.000583 | **PASS** | 0.51 s |

True π = 3.141593. Total titik per generator = 127.000.000. Kriteria
penerimaan (dikunci di `HANDOVER.md` sebelum run): `|error| < 4×SE`.

**(b) Konvergensi statistik — SEMUA PASS.** Ketiga generator menghasilkan
estimasi π yang benar dalam batas galat Monte Carlo yang diharapkan. Tidak
ada bukti bias atau korupsi statistik pada `ra_core_singleblock` untuk
workload nyata ini.

**(a) Wall-clock — TEMUAN PENTING, TIDAK TERDUGA.** `singleblock` justru
**~1.9x LEBIH LAMBAT** dari `xoshiro256**`/`PCG32` di workload nyata ini
(0.93s vs 0.49s/0.51s), meskipun raw generate-per-word `singleblock`
diketahui jauh lebih murah secara isolasi (`bench_release.c`: 0.457 ns/word
vs xoshiro 1.04 ns/word — singleblock harusnya ~2x LEBIH CEPAT, bukan
lebih lambat). Penjelasan: `bench_release.c` mengukur biaya "reinit +
generate" per K-word SUDAH teramortisasi, tapi workload nyata ini
menambahkan overhead yang tidak ada di microbenchmark murni — kemungkinan
kombinasi dari (i) biaya `M[256]` full re-init per trial yang genuinely
besar (256 kali multiply+rotate+xor, walau sudah AVX2-vectorized) versus
`xoshiro256**`'s init yang cuma 4 langkah `splitmix64` dan `PCG32` yang
hampir tanpa biaya init sama sekali, dan (ii) kemungkinan konteks compiler
optimization yang berbeda ketika fungsi dipanggil lewat pola
"pointer-ke-fungsi generik dengan 3 target berbeda" (dipakai di sini untuk
mencegah dead-code-elimination, konsisten dengan metodologi resmi
`bench_release.c`) dibanding konteks compile `bench_release.c`'s sendiri
yang mungkin memungkinkan optimisasi lebih agresif untuk kasus
single-target. **Ini BELUM sepenuhnya direkonsiliasi** — dicoba beberapa
verifikasi ad-hoc (lihat "Catatan metodologi" di bawah) yang mengonfirmasi
angka ini GENUINE (bukan DCE), tapi akar penyebab persis KENAPA gap-nya
sebesar ini (~8x lebih mahal per trial daripada prediksi dari angka
`bench_release.c`) belum ditelusuri tuntas — di luar scope untuk
menyelesaikan Workload A ini, dicatat sebagai open question untuk
penulisan paper.

## Implikasi untuk paper

Ini justru temuan yang BERNILAI untuk validasi "workload nyata", bukan
cuma konfirmasi angka lama: **raw per-word generate speed tidak otomatis
diterjemahkan jadi keunggulan di workload aplikasi nyata** kalau pola
pemakaiannya reinit-berat (banyak trial pendek, tiap trial re-init dari
nol) — biaya re-init `M[256]` `singleblock` genuinely lebih mahal daripada
re-init state kecil `xoshiro`/`PCG32`, sampai-sampai membalik keunggulan
kecepatan yang tampak di microbenchmark murni. **Rekomendasi**: paper
JANGAN mengklaim "singleblock lebih cepat untuk workload reinit-heavy"
tanpa kualifikasi — untuk workload SEPERTI Monte Carlo π estimation
K=255-per-trial ini, `xoshiro256**`/`PCG32` justru menang di wall-clock
end-to-end, meski `singleblock` tetap benar secara statistik. Kalau paper
tetap ingin mengklaim keunggulan `singleblock`, klaim itu harus dibatasi
eksplisit ke "biaya generate murni, state sudah ter-init" (skenario di
mana K jauh lebih besar dari overhead re-init, atau re-init diamortisasi
lewat pemakaian berulang tanpa reinit) — BUKAN ke skenario reinit-per-batch
kecil seperti Workload A ini.

## Catatan metodologi (jejak audit sesi ini)

Selama membangun harness ini, sempat muncul kekhawatiran BARU soal DCE:
beberapa uji ad-hoc cepat di `/tmp` (dihapus, tidak disimpan) yang memanggil
`ra_core_singleblock`/salinan `sb_` lewat **pointer-ke-fungsi GLOBAL TETAP**
(diinisialisasi sekali, tidak pernah berubah target) menunjukkan angka
mustahil cepat (~0.33 ns/word) — ternyata itu JUGA korban DCE (GCC bisa
membuktikan pointer itu SELALU menunjuk fungsi yang sama dalam translation
unit yang sama, lalu tetap bisa membuktikan return value invariant,
persis pola bug yang sudah didokumentasikan di
`BENCHMARK_METHODOLOGY_FINDINGS.md`). File `workload_a_singleblock_pi.c`
yang sebenarnya TIDAK memakai pola itu — ia memanggil lewat
**parameter fungsi generik** (`bench_and_report(name, fn, N)`, dipanggil 3x
dengan 3 target fungsi berbeda), pola yang SAMA dengan `bench_real_reinit4`
resmi dan `corrected_fair_bench.c` sesi sebelumnya — dikonfirmasi genuine
lewat `objdump` (kode AVX2 asli, bukan hasil eliminasi). Pelajaran yang
terus berulang sesi ini: **setiap teknik anti-DCE HARUS diverifikasi
lewat disassembly, tidak cukup "terlihat masuk akal"** — global-fixed-
pointer TERNYATA tidak cukup untuk mencegah DCE, berbeda dengan yang
diasumsikan sebelumnya di sesi ini.

---

# Bagian B — Asian option Monte Carlo (niche `orbit`/Orbit Addressing)

## Setup

Sesuai `HANDOVER.md`: 1.000 path independen, tiap path 10.000 timestep GBM
(`S0=100, K=100, r=5%, sigma=20%, T=1`), payoff Asian call
`exp(-rT)*max(mean(S_t)-K, 0)`, dihitung dari SATU panggilan generator
per path (steady-state, TANPA reinit di tengah path) — persis regime
desain asli Orbit Addressing ("sedikit alamat, masing-masing jalan
panjang"). Tiap path butuh tepat 10.000 word (Box-Muller pasangan uniform
→ pasangan normal, jadi rasio word:timestep = 1:1 persis).

**Dibandingkan**: `ra_core_orbit` (fungsi PRODUKSI ASLI, di-`#include`
langsung — bukan salinan tangan seperti Workload A, karena kebenaran
payoff di sini penting dan jalur multi-cycle/reseed `orbit` lebih
kompleks untuk disalin ulang dengan aman) vs `philox4x32-10` vs
`xoshiro256**` (keduanya salinan byte-for-byte dari `bench_release.c`,
provenance sama dengan Workload A). Ketiganya dibaca lewat `fmemopen`
yang SAMA (1x buka/tutup per path, bukan per word) supaya tidak ada yang
diuntungkan/dirugikan I/O secara tidak adil.

**Skema alamat**: `orbit` pakai pola yang SUDAH divalidasi
(`docs/PITCH.md` §5): `key_i = key_base + i*0x9e3779b7` (Weyl chaining
antar-key), `addr_hi` tetap. `philox`/`xoshiro` pakai
`key_i = i*2654435761u+1u` (pola sintetis standar yang sama dipakai
`bench_release.c`/Workload A) — **tidak ada implementasi jump-ahead
xoshiro di repo ini** (dicek lewat grep sebelum menulis kode, sesuai
`HANDOVER.md`'s instruksi eksplisit "jangan asumsikan ada"), jadi
reseed-per-key-lalu-generate-panjang adalah analog steady-state yang adil
untuk xoshiro — secara struktural sama dengan cara `orbit` sendiri
menurunkan stream dari key (bukan jump-ahead juga).

## Hasil

| Generator | mean payoff | SE | wall-clock (min-of-5, 1000 path × 10.000 step) |
|---|---:|---:|---:|
| `orbit` | 5.596228 | 2.417×10⁻¹ | 0.248 s |
| `philox4x32-10` | 6.052145 | 2.591×10⁻¹ | 0.292 s |
| `xoshiro256**` | 5.645977 | 2.427×10⁻¹ | 0.237 s |

**(b) Konsistensi lintas-generator — PASS untuk ketiga pasang.** Kriteria
dikunci sebelum run: `|diff mean| < 3×sqrt(SE_i²+SE_j²)`.

| Pasangan | \|diff\| | 3×SE gabungan | Status |
|---|---:|---:|---|
| orbit vs philox4x32-10 | 0.4559 | 1.0630 | PASS |
| orbit vs xoshiro256** | 0.0497 | 1.0277 | PASS |
| philox4x32-10 vs xoshiro256** | 0.4062 | 1.0652 | PASS |

Tidak ada bukti bias pada estimasi payoff `orbit` — konsisten dengan dua
generator pembanding dalam batas galat Monte Carlo yang diharapkan.

**(a) Wall-clock** — untuk workload steady-state (satu panggilan panjang
per alamat, tanpa reinit), `orbit` (0,248s) SEBANDING dengan
`philox4x32-10` (0,292s, `orbit` malah sedikit lebih cepat) dan hanya
sedikit lebih lambat dari `xoshiro256**` (0,237s). **Ini mengonfirmasi
ulang** kesimpulan `BENCHMARK_METHODOLOGY_FINDINGS.md`/`docs/PITCH.md` §4:
di regime steady-state yang memang didesain untuk `orbit`, generator ini
kompetitif — gap besar (~17-21x) yang ada di `bench_release.c` HANYA
muncul pada pola reinit-K pendek, bukan di sini. Ini konsisten dan
memperkuat (bukan bertentangan dengan) angka resmi yang sudah ada.

**(c) Independensi cross-stream — dijalankan untuk KETIGA generator**
(bukan cuma `orbit`) sebagai kontrol diri terhadap metodologi uji korelasi
itu sendiri. 50 pasang stream acak dari 1000, ambang null-hypothesis
`1,96/sqrt(10000) ≈ 0,0196`:

| Generator | max\|r\| (n=50) | Pelanggaran (n=50) | Pelanggaran (n=500, robustness check) |
|---|---:|---:|---:|
| `orbit` | 0.0228 | 2/50 (4%) | 22/500 (4,4%) |
| `philox4x32-10` | 0.0338 | 8/50 (16%) | 41/500 (8,2%) |
| `xoshiro256**` | 0.0309 | 3/50 (6%) | 24/500 (4,8%) |

`orbit` dan `xoshiro256**` berada persis di sekitar tingkat kebetulan yang
diharapkan (~5% pada CI 95%) — **PASS**, tidak ada sinyal korelasi
sistematis antar-stream Orbit Addressing pada skala 1000-alamat ini,
mengisi gap yang diakui eksplisit di `docs/PITCH.md` §8.

`philox4x32-10` justru menunjukkan tingkat pelanggaran yang **secara
statistik signifikan di atas kebetulan** (16% pada n=50, ~3,6σ; 8,2% pada
n=500, ~3,3σ — dicek ulang dengan sampel 10x lebih besar justru untuk
memastikan ini bukan noise sampel kecil, dan pola tetap konsisten).
**Interpretasi hati-hati**: ini KEMUNGKINAN BESAR bukan cacat pada
algoritma Philox itu sendiri, melainkan artefak skema pengalamatan yang
saya pakai di sini — semua 1000 stream philox dimulai dari `ctr=[0,0,0,0]`
yang SAMA, dibedakan hanya oleh `key[0]` yang berurutan
(`i*2654435761+1`); interaksi antara skedul multiplikasi Philox dan
counter awal yang identik lintas-stream berpotensi meninggalkan korelasi
residual halus di word-word awal tiap stream yang tidak muncul kalau
counter juga divariasikan atau kalau memakai lebih banyak putaran
"warm-up" sebelum pengambilan sampel. **Ini WAJIB dilaporkan eksplisit di
paper** (sesuai kriteria yang dikunci di `HANDOVER.md`), tapi TIDAK
mengubah kesimpulan (b) — dan uji lanjutan yang lebih kuat (PractRand pada
output interleaved dari sample stream ini, disebut opsional di
`HANDOVER.md`) belum dijalankan, dicatat sebagai open item kalau paper
ingin klaim lebih kuat soal ini.

## Implikasi untuk paper

- Klaim "Orbit Addressing aman untuk cross-stream independence pada skala
  ~1000 alamat paralel" sekarang punya bukti empiris langsung (bukan cuma
  desain-target), mengisi gap `docs/PITCH.md` §8.
- `orbit` kompetitif (bahkan sedikit lebih cepat dari Philox) di regime
  aslinya (steady-state) — memperkuat, bukan mengoreksi, klaim yang sudah
  ada.
- Temuan philox-correlation di atas HARUS masuk sebagai catatan metodologi
  di paper (bukan disembunyikan) — kemungkinan artefak skema pengalamatan
  `ctr=0`-seragam, bukan klaim "Philox rusak"; perlu investigasi lanjut
  (PractRand interleaved) kalau ingin klaim definitif.

## File

- `workload_a_singleblock_pi.c` — harness Bagian A (generator + MC +
  parity check + statistik).
- `workload_b_orbit_asian.c` — harness Bagian B (GBM + Asian payoff +
  independence test).
- `HANDOVER.md` — desain terkunci sebelum implementasi (Workload A & B).
