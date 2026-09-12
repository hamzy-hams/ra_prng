# Production Readiness Handover — `ra_core_orbit` / `ra_core_singleblock`

**Tujuan dokumen ini**: satu tempat terpusat untuk memantau semua yang harus
selesai sebelum `experiments/2026-9-1_family-productionization/ra_core.c`
(dua entry point publik: `ra_core_orbit`, `ra_core_singleblock`) bisa disebut
siap produksi. **Dimaksudkan untuk terus di-update tiap sesi** sampai battery
gate lolos — bukan laporan sekali-jalan seperti `HANDOVER.md`/`RESULTS_TAHAP2.md`
di folder ini (yang tetap riwayat Tahap 1/2, jangan diubah).

Scope pelacakan (protokol benchmark/status standar, lihat memory
`feedback_orbit_singleblock_benchmark_standard`): `singleblock` K=1..K=255
(seluruh rentang valid), `orbit --multistream` (reinit tiap K kata), dan
`orbit --stream` (kontinu, tanpa reinit paksa).

Ditulis 2026-09-03, setelah sesi investigasi kecepatan (kenapa lebih lambat
dari versi lama, perbandingan vs Philox/Xoshiro) dan setelah commit `d2f1675`
(combo-winner Pareto-selection, hari yang sama).

---

## 1. Status matrix ringkas

| mode | PractRand | dieharder | cross-corr/collision-scan | weak-key (key=0) | status |
|---|---|---|---|---|---|
| `ra_core_orbit` | 128GB clean (stream-panjang-kontinu) + 16GB clean K=1/K=255 multistream **tapi Weyl-key-ordering only** — K=1+hashed-key FAIL parah, lihat §2b UPDATE | 0 FAILED (Weyl ordering) | PASS (Weyl ordering) | fixed (GUARD_L/GUARD_M) | **BLOCKER BARU (2026-09-04, sesi lebih lambat) — K=1 + urutan key hashed FAIL, lihat §2b UPDATE. K=255 & K=1+Weyl tetap aman.** |
| `ra_core_singleblock` | **FIXED 2026-09-03 — `w8_f10_i0` dipromosikan ke `ra_core.c`, battery gate Step 0-8 CLOSED** | 0 FAILED (K=1/K=96 standalone, K=1/K=255 battery gate) | PASS (K=1/K=255, battery gate Step 4) | fixed (GUARD_M) | **TIDAK ADA BLOCKER TERSISA — production-candidate-battery gate PASS, lihat `../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`** |

---

## 2. Blocker utama: singleblock K-small structural defect

`ra_core_singleblock` (formula saat ini di `ra_core.c`, dengan GUARD_M +
fmix32 + keyterm-multiply-combine) punya defect PractRand BCFN-style pada K
kecil. Status resmi di `RESULTS_TAHAP2.md`: **"DEFECT NYATA DITEMUKAN, root
cause terdiagnosis, fix BELUM diimplementasikan ke `ra_core.c`."**
Konsekuensinya, battery gate produksi
(`experiments/2026-9-1_production-candidate-battery/RESULTS.md`) berstatus:
**"key=0 defect FIXED dan Step 0-3 RECONFIRMED clean, tapi defect TERPISAH
yang lebih parah (pola K=1) sekarang memblokir progres lebih lanjut — gate
STILL NOT PASSED."** Dikonfirmasi ulang di
`experiments/2026-9-2_singleblock-o-width-fix/RESULTS.md:173`: gate **tetap
PAUSED** sampai ada kandidat fix K-kecil yang terbukti bersih di seluruh
rentang K relevan pada skala 16GB+.

### Safe-K floor formula SAAT INI (belum di-fix)

Dari `experiments/2026-9-2_singleblock-k-threshold-characterization/RESULTS.md`:

> "Ambang minimum aman: K ≈ 80-96 (batas bawah pasti tidak aman di ≤72, batas
> atas pasti aman di ≥96, K=80 kemungkinan besar sudah aman tapi satu noise
> ringan @32GB belum sepenuhnya menghilangkan keraguan). **Rekomendasi
> konservatif: K≥96.**"

Crossover kecepatan singleblock vs orbit (di mana keduanya sama cepat, ~2.2
ns/word): **K≈20-21**. Caveat: ambang K=80 hanya diuji **satu** `base_key`
dan **satu** seed — belum di-reproduksi lintas seed.

### 2 kandidat fix co-frontier (commit `d2f1675`, hari ini)

Dari `experiments/2026-9-3_combo-winner-pareto-selection/RESULTS.md`. Dari 11
kandidat cycle-op combo-search yang tadinya "fully clean" di K∈{1,2,4,8,16,32,
64,96}@16GB, **8/11 gagal** uji normalitas avalanche multi-seed (kemungkinan
artefak diskretisasi test, belum tentu defect asli) — tersisa 2 kandidat
non-dominated:

| kandidat | finalizer | K=1 (ns/word) | continuous-stream (MB/s) | avalanche worst-case margin |
|---|---|---:|---:|---:|
| `w8_f10_i0` (tercepat) | `c ^= c >> 17;` | 36.7 | 134.1 | 0.4810 |
| `w8_f28_i0` (margin terbaik) | `MUL(0xc2b2ae35)->XORSHIFT(13)` | 37.4 | 128.0 | 0.4827 |

(threshold avalanche margin: 0.2 — keduanya jauh di atas ambang)

**DIPROMOSIKAN 2026-09-03**: user memilih `w8_f10_i0` (tercepat) secara
eksplisit ("tidak perlu, langsung saja aku pilih yang tercepat w8_f10_i0"),
tanpa menunggu speed/avalanche test tambahan untuk 11 varian inject baru
(sudah cukup dari dieharder+inject-crossing di atas). Diterapkan ke
`ra_permutation_cycle_singleblock` di `ra_core.c`: `o` diperlebar dari 2-tap
jadi 8-tap penuh (`M[i+0..7]`), ditambah finalizer `c ^= c >> 17u;` persis
formula `w8_f10_i0`. `ra_init_state_singleblock`, `ra_core_singleblock`,
dan seluruh jalur `ra_core_orbit` TIDAK disentuh. Diverifikasi: compile
bersih (`gcc -O3 -march=native -std=gnu17 -include stdalign.h ra_core.c -o
ra_core -Wall -Wextra`), cross-check 70 vektor (10 key x 7 panjang) vs
binary `w8_f10_i0` tervalidasi (`../2026-9-2_singleblock-cycle-combo-search/
candidates/w8_f10_i0`) — 0 mismatch.

**Efek samping penting**: `./ra_core validate` yang tadinya membuktikan
`ra_core_singleblock` bit-identik dengan `ra_core_orbit` (bukti L[] dead-code,
Tahap 1) SEKARANG SENGAJA TIDAK LAGI BERLAKU — fix ini justru membuat kedua
core berbeda formula per-round di K kecil. `run_validate_singleblock()` di
`ra_core.c` diganti jadi known-answer-test (checksum tetap terhadap formula
`w8_f10_i0` yang sudah tervalidasi), bukan lagi perbandingan vs orbit —
lihat komentar di `ra_core.c` dekat `SINGLEBLOCK_KAT_CHECKSUMS`.

### Battery gate Step 4-8 dibuka lagi dan CLOSED (2026-09-03)

Setelah promosi, `../2026-9-1_production-candidate-battery/` (yang berhenti
di Step 3 karena defect K-kecil) dilanjutkan penuh Step 4-8 — detail lengkap
di `ADDENDUM_POST_FIX_STATUS.md` folder itu. Ringkas: collision-scan K=1
PASS (0/50.000 collision, sesuai ekspektasi ~0,29 di ruang 32-bit),
dieharder K=1/K=255 PASS (0 FAILED), PractRand 16GB K=1/K=255 di VPS PASS
(bersih total, "no anomalies" di setiap checkpoint).

**Bug ditemukan+diperbaiki di tengah jalan (bukan defect RNG)**:
`scrambler_ra_core_singleblock.c` (Step 7) awalnya melaporkan K=1
katastropik (rotasi trivial, runs-test z=-3528) — ternyata bug `fmemopen()`
glibc: kalau buffer PAS ukuran tulisan, NUL-terminator menimpa byte
terakhir (byte teratas word ter-nol-kan tiap panggilan). Diperbaiki
(buffer +1 byte) di file itu DAN di `checksum_key()` `ra_core.c` (bug sama,
tapi tautologis-aman karena golden checksum & verifikasi sama-sama pakai
fungsi yang sama — checksum sudah di-regenerate). Semua `fmemopen` lain di
repo membandingkan 2 buffer yang sama-sama kena bug ini secara identik,
jadi hasil PASS historis lain TIDAK perlu diragukan.

Setelah fix: K=255 bersih total, K=1 masih ada bias kecil-tapi-nyata di
runs-test (z=+9,25 vs K=255's -0,35; per-repetisi mean z=0,242 vs 0,155,
beda ~2,8 sigma) — kemungkinan sebagian artefak rumus runs-test untuk data
permutasi (bukan i.i.d. kontinu), tapi K=1 punya lebih banyak dari itu.
**User memutuskan: PASS dengan catatan** — bias ini jauh lebih kecil dari
defect pra-fix, tidak terlihat di PractRand 16GB, dicatat sebagai limitasi
diketahui untuk use-case ekstraksi-bit-rendah (mis. Fisher-Yates) di K=1,
BUKAN alasan membatalkan promosi `w8_f10_i0`.

**Verdict gate: PASS keseluruhan. Tidak ada blocker tersisa.**

### Validasi dieharder + inject-crossing (2026-09-03, `../2026-9-3_dieharder-inject-crossing/RESULTS.md`)

Action item 1 & 2 (lihat §3) sekarang **CLEAR**:

1. **Dieharder**: `w8_f10_i0` dan `w8_f28_i0`, K=1 dan K=96, 27 test
   Good-reliability — **0 FAILED** di keempat run (25-26/27 PASSED, 0-2
   WEAK per run), sebanding dengan baseline produksi `winner_wired_v2`
   (26/27, 2 WEAK) dan `winner_wired_addressable` (25/27, 1 WEAK).
2. **Inject-crossing**: 12 varian extra-inject (`i1`..`i6` untuk kedua
   base) diuji Promotion Tier penuh (16GB, K∈{2,4,8,16,32,64,96}, K=1
   di-skip krn provably identik dgn `i0`) — **11/12 FULLY CLEAN**. Satu
   pengecualian: `w8_f10_i3` FAIL di K=4 (defect BCFN nyata, eskalatif,
   bukan artefak) — K lain untuk kandidat yang sama tetap bersih, jadi ini
   kombinasi (kandidat,K) sempit, bukan pola yang menggeneralisasi. Kandidat
   Pareto-selection asli (`i0`/inject off, keduanya) **tidak terpengaruh**
   temuan ini — tetap sebersih sebelumnya.

Kesimpulan: kalau inject tetap OFF (`i0`, seperti kandidat Pareto-selection
asli), tidak ada temuan baru yang menghalangi promosi. Kalau extra-inject
mau diaktifkan di produksi nanti, `i3` pada `w8_f10` harus dihindari atau
divalidasi ulang independen.

---

## 2b. Orbit K-small multistream defect (ditemukan & diperbaiki 2026-09-04)

Klaim "128GB clean" untuk `ra_core_orbit` di §1 di atas hanya mewarisi
validasi pola **stream-panjang-kontinu** (`winner_wired_addressable.c`,
satu key, jutaan kata berturut-turut) — tidak pernah diuji ulang di pola
**multistream** (banyak panggilan pendek, key baru tiap panggilan, K
kecil) pasca-fix singleblock 2026-09-03.

Investigasi dipicu pertanyaan user: "apa yang terjadi kalau ada orang
mengambil orbit K=1 sebagai input?" — dan menemukan celah nyata:
`ra_permutation_cycle_orbit` masih pakai formula `o` 2-tap sempit tanpa
finalizer, byte-identik dengan formula singleblock versi PRA-fix yang
terbukti gagal PractRand (BCFN-style) di bawah K=96. Karena
`ra_init_state_orbit`'s M[] byte-identik dengan singleblock dan L[] tak
berpengaruh sebelum reseed, `ra_core_orbit(key, rng<=255, ...)`
mereplikasi persis defek itu di pola multistream.

Fix: port formula `w8_f10_i0` yang sama (8-tap `o` + XORSHIFT(17)
finalizer) ke `ra_permutation_cycle_orbit`, plus optimasi rolling-register
untuk `o` (identitas aljabar bit-exact, diterapkan ke KEDUA core, ~30-34%
speedup di K=255/stream, tidak ada regresi K=1). Detail derivasi:
`../2026-9-4_orbit-fix-and-wideo-rolling-optimization/HANDOVER.md`.

Statistik re-validasi (battery yang tadinya di-defer): dieharder K=1/K=255
0 FAILED, PractRand 16GB K=1/K=255 no anomalies, avalanche + collision-scan
PASS — `../2026-9-4_orbit-kmin-battery/RESULTS.md`. **PASS**, dipromosikan
ke `ra_core.c` kanonik 2026-09-04 (`ra_permutation_cycle_orbit` +
`ORBIT_KAT_CHECKSUMS` baru, `./ra_core validate` 0 mismatch kedua core).

`ra_reseed`/`ra_hash` orbit (defek BCFN pra-reseed yang berbeda, lihat §5
research backlog di `2026-9-4_orbit-fix-and-wideo-rolling-optimization/HANDOVER.md`
§6) TIDAK termasuk fix ini — masih terbuka, track terpisah.

> **UPDATE 2026-09-04 (sesi lebih lambat) — GATE DIBUKA SEBAGIAN LAGI:**
> PASS di atas hanya valid untuk urutan key **Weyl-stepped** (satu-satunya
> yang diuji battery `orbit-kmin-battery`). Ditemukan saat membangun
> kontrol sanity untuk `../2026-9-4_orbit-reseed-postboundary-bcfn/`: K=1
> dengan urutan key **hashed (SplitMix32)** FAIL PractRand parah dan
> reproducible (eskalasi BCFN-style, `FPF-14+6/16` R=+132.3 @8GB, lebih
> parah dari defect asli R=+40.7 @4GB; direproduksi dengan 2 base_key
> independen; dikonfirmasi bukan bug harness — dua binary independen,
> beda object file, menghasilkan stream byte-identik dan sama-sama FAIL).
> K=255 (kedua urutan key) dan K=1+Weyl tetap bersih (dikonfirmasi ulang,
> 8GB). Detail lengkap:
> `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`. Root-cause
> + fix belum dikerjakan (diputuskan user: dokumentasikan dulu, root-cause
> penuh jadi sesi terpisah). **Sampai diperbaiki, jangan anggap
> `ra_core_orbit` aman untuk pola multistream K=1 dengan urutan key yang
> tidak terstruktur/tidak diketahui — hanya Weyl-stepped yang tervalidasi.**

> **UPDATE 2026-09-04 (sesi lebih lambat lagi) — hipotesis fix pertama
> DITOLAK:** Analisis statis menemukan `ra_init_state_orbit`'s formula
> `l_val=(i*const)*keyterm` / `m_val=(i*const)*keyterm` selalu nol persis
> di `i=0` (perkalian dengan nol) → `L[0]`/`M[0]` selalu nol untuk key
> apa pun, dan `ra_permutation_cycle_orbit`'s wide-`o` membaca
> `M[(uint8_t)(255+1)]=M[0]` sebagai salah satu dari 8 tap pada siklus
> pertama tiap panggilan — cacat struktural nyata (diverifikasi via probe
> C + replika Python). **Diuji sebagai kandidat fix** (geser index jadi
> `(i+1)`, `experiments/2026-9-4_orbit-init-zerotap-fix/`, patch
> diverifikasi benar-benar menghilangkan nilai-nol itu) — **hasil
> PractRand nyaris tidak berubah** (R=+135.2 patched vs R=+132.3
> unpatched @8GB, K=1+hashed, dua base_key independen, pola eskalasi
> `FPF-14+6/16` identik). **Tap-nol BUKAN (satu-satunya) mekanisme
> penyebab gap ini** — root cause asli masih terbuka. Detail lengkap +
> hipotesis yang belum diuji:
> `../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`.
>
> **Arah selanjutnya (disepakati dengan user)**: bukan lanjut
> tebak-satu-hipotesis-per-sesi, tapi search tertarget yang meng-cross
> axis init-formula (baru, belum pernah dieksplorasi) dengan axis
> cycle-transform (yang sudah terbukti solid di combo-search 2026-9-2,
> tapi **combo-search itu sendiri ternyata cuma pernah diuji dengan
> urutan key Weyl** — celah metodologi yang sama persis yang membuat gap
> ini lolos gate lama). Desain lengkap siap-eksekusi (handover-only,
> belum ada kode): `../2026-9-4_orbit-init-cycle-combo-search/HANDOVER.md`.

> **UPDATE 2026-09-04 (sesi lebih lambat lagi) — search init×cycle
> DIJALANKAN, Stage 1: 0 survivor.** 8 varian init-formula × 4 varian
> cycle-transform (32 kombinasi total, termasuk kandidat paling agresif
> `hashed_per_index` yang mengganti total mekanisme index-to-state) diuji
> dual-ordering (Weyl + hashed) sampai 2GB lokal. Tier 0 avalanche: 32/32
> lolos. Tier 1 Stage A (256MB): 32/32 lolos dual-ordering. Tier 1 Stage B
> (2GB): **0/32 lolos** — seluruh 32 kombinasi tanpa kecuali FAIL pada
> urutan key hashed antara 512MB-2GB (kontrol produksi FAIL @1GB, kandidat
> terbaik `hashed_per_index__w8_f10_i0` bertahan sampai tepat FAIL @2GB).
> Stage C (base_key kedua) tidak dijalankan karena tidak ada survivor
> Stage B. **Kesimpulan: root cause gap ini kemungkinan besar BUKAN di
> `ra_init_state_orbit` atau `ra_permutation_cycle_orbit`** (kedua fungsi
> itulah yang divariasikan agresif di search ini, tak satupun menutup
> gap). Stage 2 (perluas axis cycle, §5 HANDOVER.md) didesain sebagai
> fallback tapi **belum dijalankan** — mengingat pola 100% FAIL di seluruh
> Stage 1, Stage 2 dinilai kemungkinan besar tidak akan mengubah hasil.
> Detail lengkap: `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`.
> `ra_core.c` TIDAK disentuh.
>
> **RALAT (2026-09-04, sesi lanjutan)**: paragraf di atas sempat menunjuk
> `ra_hash`/`ra_reseed` sebagai arah paling mungkin — **salah**, dikonfirmasi
> baca `ra_core_orbit` (`ra_core.c` L254-270): untuk K=1, `count<=1` setelah
> satu putaran cycle → fungsi return SEBELUM `ra_reseed` dipanggil,
> benar-benar dead code di skenario FAIL ini. Analisis yang benar: K=1 =
> satu panggilan `ra_init_state_orbit` + satu `ra_permutation_cycle_orbit`
> (tanpa reseed) — yang diuji sebenarnya adalah kualitas `orbit_round1`
> sebagai fungsi hash `key → 1 word`, bukan kualitas reseed. Diagnosis
> murah (kurva dilusi K + lokalisasi bit-bias, BUKAN search kombinasi lagi)
> dirancang dan dijalankan di
> `../2026-9-4_orbit-key-avalanche-diagnosis/` (lihat RESULTS.md di sana
> untuk hasil dan arah fix yang direkomendasikan).
>
> **UPDATE 2026-09-04 (diagnosis selesai) — defect terlokalisasi ke "word
> pertama setelah init segar", bukan bias bit-tunggal:** kurva dilusi
> (`--stream-hashed`, base_key `0x12345`, K∈{1,2,4,8,16,32,64}, `-tlmax
> 1GB`) menunjukkan peluruhan MULUS pada test `FPF-14+6/16` yang SAMA
> persis — R turun ~1/K: K=1 FAIL (R=+18.7), K=2 VERY SUSPICIOUS (R=+10.2),
> K=4 unusual (R=+5.1), K=8 unusual (R=+5.0), K=16/32/64 bersih di 1GB
> (tapi belum tentu bersih di skala lebih besar — pola 1/K belum diuji
> lanjut). Tes bit-bias marginal (50 juta word, K=1 hashed vs Weyl,
> self-check lolos di data sintetis) **tidak menemukan bias bit-tunggal**
> di kedua ordering — konsisten dengan `FPF-14+6/16` sebagai tes
> windowed/tuple-frequency, bukan bit marginal. **Pertanyaan terbuka yang
> BELUM terjawab**: kenapa urutan Weyl K=1 (100% word "cold" juga, sama
> strukturnya) tetap bersih sampai 8GB padahal secara struktur identik
> dengan hashed K=1 yang FAIL — belum diuji apakah Weyl K=1 skala jauh
> lebih besar (16-32GB) juga punya residual R yang perlahan naik. Detail
> lengkap + rekomendasi arah fix (belum didesain/diuji):
> `../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md`.

> **UPDATE 2026-09-04 (sesi lanjutan) — 2 kandidat fix tertarget (seed
> `d0`/`a0` di fase init) DIUJI, KEDUANYA GAGAL:** berdasarkan diagnosis di
> atas (defect di word pertama, `a`/`d` adalah satu-satunya variabel
> loop-carried antar ronde, `d` mulai dari `0` konstan untuk semua key),
> diuji 2 kandidat yang HANYA mengubah nilai seed `a`/`d` di `it==0` (loop
> body `ra_permutation_cycle_orbit` verbatim tidak disentuh):
> - **Kandidat A (`d0_hash`)**: `d0=fmix32(GUARD_D^key)&0xFF` (GUARD_D
>   dipilih baru, popcount=16, beda dari GUARD_L/GUARD_M) — lolos Tier 0
>   avalanche (K=1), tapi **FAIL Tier 1 K=1+hashed @512MB** dengan
>   signature identik (`FPF-14+6/16`) dengan defect asli. K=255 (kedua
>   ordering, dua base_key, 2GB) tetap CLEAN-DUAL — tidak ada regresi K=255.
> - **Kandidat B (`d0_realround`)**: satu ronde asli `i=255` dihitung ulang
>   di fase init (dibuang) sebagai warm-up — **REJECT di Tier 0 avalanche
>   sendiri** (mean_fraction=0.28 di bawah ambang 0.3), tidak sampai diuji
>   PractRand.
>
> **Kesimpulan: "nilai `d0` yang key-dependent saja" (A) tidak cukup, dan
> "progresi state riil satu ronde tapi mengulang index `i=255`" (B) malah
> merusak avalanche.** Pertanyaan terbuka Weyl-vs-hashed K=1 (kenapa Weyl
> tetap bersih) masih belum terjawab dan makin relevan sebelum mencoba arah
> fix lain. Sesuai kesepakatan sebelumnya, sesi ini TIDAK lanjut otomatis ke
> opsi berat ("cari operasi finalizer baru") — laporan lengkap + data mentah
> di `../2026-9-4_orbit-init-warmup-fix/RESULTS.md`. `ra_core.c` TIDAK
> disentuh.

> **UPDATE 2026-09-04 (sesi lanjutan) — hipotesis "resonansi-konstanta"
> DIGUGURKAN.** Observasi pemicu: konstanta Weyl test-harness (`0x9E3779B9`)
> hanya beda 2 di digit hex terakhir dari konstanta internal
> `ra_init_state_orbit` (`0x9e3779b7u`). Hipotesis: kedekatan ini membuat
> "Weyl bersih" jadi resonansi kebetulan, bukan properti aman asli. Diuji
> dengan driver baru (`--stream-altconst`, HANYA mengganti konstanta step
> di test driver, `ra_core.c` tidak disentuh) memakai `C_alt1=0x2545F491`
> (bitwise jauh dari `0x9e3779b7`/`0x9E3779B9`), K=1, base_key=`0x12345`,
> staged 64MB→256MB→2GB: **CLEAN sampai 2GB, 0 anomali di setiap
> checkpoint** — sama bersihnya dengan Weyl asli. `C_alt2` tidak perlu
> dijalankan (hasil tidak ambigu). **Kesimpulan: "Weyl bersih" BUKAN
> artefak kebetulan-konstanta** — defect K=1+hashed tetap spesifik ke
> struktur statistik urutan key acak/hashed itu sendiri, bukan soal
> kedekatan nilai konstanta step tertentu. Pertanyaan terbuka Weyl-vs-hashed
> masih belum terjawab akar sebabnya (satu hipotesis penjelas gugur, bukan
> ditemukan penyebab sebenarnya) — arah lanjutan (cross-check urutan key
> terstruktur lain, mis. LCG/increment=1) didiskusikan dulu dengan user
> sebelum dijalankan. Detail lengkap:
> `../2026-9-4_orbit-weyl-constant-resonance-diagnosis/RESULTS.md`.
> `ra_core.c` TIDAK disentuh.

> **UPDATE 2026-09-04 (sesi lanjutan) — kandidat fix `fmix32(o)` DIUJI,
> GAGAL.** Axis baru (dikonfirmasi belum pernah dicoba di manapun di repo
> lewat baca kode + grep menyeluruh): terapkan `fmix32()` pada variabel `o`
> (8-tap XOR-shift dari `M[]`) SEBELUM masuk ke update `a`/`b` —
> berbeda dari axis finalizer-pada-`c` yang sudah 32× dicoba dan gagal
> semua (`orbit-init-cycle-combo-search`). 2 kandidat diuji: `init_only`
> (mix hanya `o` awal) dan `every_round` (mix tiap `o` dihitung ulang di
> dalam loop). Byte-exact PASS (setelah perbaikan bug implementasi: rolling-
> register produksi untuk update `o` ternyata TIDAK valid lagi begitu `o`
> di-`fmix32`, harus diganti recompute naive 8-tap tiap ronde). Tier 0
> avalanche: 2/2 PASS. **Tier 1 PractRand: KEDUA kandidat FAIL K=1+hashed
> @1GB** (`FPF-14+6/16:all` R=+15.7, p=3.3e-14 — signature sama dengan
> defect asli), hasil identik byte-for-byte antar kedua kandidat (karena
> K=1 cuma pernah mengeksekusi SATU ronde, jadi beda A/B yang menyentuh
> ronde ke-2+ tidak pernah teruji di K=1). K=255 tetap CLEAN-DUAL (dua
> base_key, kedua urutan key) — tidak ada regresi. **Insight**: defect K=1
> 100% ditentukan oleh kombinasi SATU nilai `o` awal dengan `a`/`d` lewat
> `a=(d^o)^(cons+a)` — mixing `o` sendiri (di titik manapun) tidak cukup;
> kemungkinan titik gabungan XOR+ADD itu sendiri yang jadi akar masalah,
> bukan nilai `o`/`c` secara terpisah. Detail lengkap:
> `../2026-9-4_orbit-o-fmix-fix/RESULTS.md`. `ra_core.c` TIDAK disentuh.

> **UPDATE 2026-09-04 (sesi lanjutan) — kandidat fix MUL-combine
> `a=(d*o)^(cons+a)` DIUJI, REJECT di Tier 0 (tidak sampai PractRand).**
> Axis baru: ganti combine `(d^o)` di update `a` jadi `(d*o)` (satu baris
> saja, `o` TETAP tidak dimixing — beda dari eksperimen `fmix32(o)` di
> atas), memakai teknik multiply-combine yang pernah berhasil menutup
> defect afin-dalam-key berbeda di `multikey_remix_search` (2026-09-01).
> Byte-exact vs oracle Python: **PASS** (1 kata, 300 kata lintas reseed,
> 2 juta kata — semua cocok). **Tier 0 avalanche: REJECT**
> (`mean_fraction=0.12` vs minimum 0.3, `min_bit_fraction=0.03` vs minimum
> 0.2) — jauh di bawah ambang, tidak lanjut ke PractRand sama sekali
> (disiplin pipeline sama seperti kandidat `d0_realround` sebelumnya).
> **Akar sebab (bukan bug)**: ronde pertama tiap panggilan selalu mulai
> dengan `d=0`; combine MUL membuat `d*o=0` di ronde itu, jadi kontribusi
> `o` (satu-satunya sumber keragaman dari `M[]`/key) hilang total dari `a`
> di ronde 1 — justru ronde yang sudah terdiagnosis sebagai lokasi defect.
> Efeknya melemahkan avalanche secara umum, bukan spesifik menutup celah
> K=1+hashed. **Ringkasan kumulatif axis yang sudah dicoba** (finalizer-`c`
> 32-combo, init×cycle 8×4 combo, `d0_hash`/`d0_realround`,
> weyl-constant-resonance, `fmix32(o)` init/every-round, MUL-combine ini)
> semuanya gagal menutup defect K=1+hashed — titik kombinasi
> `a=(d^o)^(cons+a)`/`b=(cons+a)^(o+d)` tetap kandidat kuat sebagai lokasi
> defect, tapi penggantian XOR→MUL sederhana di titik itu bukan solusinya.
> Detail lengkap: `../2026-9-4_orbit-ad-mul-combine-fix/RESULTS.md`.
> `ra_core.c` TIDAK disentuh.

> **UPDATE 2026-09-05 — diagnosis posisi: hipotesis "collision `a=cons`
> round-1" REFUTED sebagai penjelasan tunggal.** User menyadari pola
> struktural: `a` diinisiasi `=cons` di `ra_permutation_cycle_orbit`,
> jadi round pertama tiap siklus reseed (`i=255`, `d=0`) menghasilkan
> `a = (0^o)^(cons+cons) = o^(2*cons)` -- self-collision, bukan kombinasi
> dua sumber independen. Diuji lewat driver diagnosis-only
> (`position_split_orbit.c`, byte-exact verified vs produksi + rekonstruksi
> interleave verified) yang men-split output K=2/hashed berdasarkan posisi
> round: `round1-only` (`i=255`, kena collision) vs `rest-only` (`i=254`,
> "round2", TIDAK pernah kena collision itu). Staged 64MB→1GB, base_key
> `0x12345`. **Hasil: KEDUA substream FAIL @1GB dengan magnitude nyaris
> sama** (`FPF-14+6/16:all` round1 R=+18.7 vs round2 R=+17.0) — bukan
> pola yang diprediksi (round1 FAIL, round2 CLEAN). **Kesimpulan:
> collision `a=cons` round-1 adalah properti aljabar nyata tapi BUKAN
> satu-satunya sumber defect** — round2 (secara aljabar tidak pernah kena
> collision itu) sama parahnya, artinya defect lebih terkait kedekatan
> umum ke batas init segar (radius >=2 posisi pertama), bukan satu titik
> kombinasi tunggal. Memperhalus (bukan menggantikan) diagnosis lama
> "kata pertama setelah init segar, dilusi ~1/K"
> ([[project_orbit_key_avalanche_diagnosis]]). Stage 2 (K=255 asli,
> lebih mahal) BELUM dijalankan — menunggu diskusi arah lanjutan dengan
> user (mis. peta per-posisi lebih halus di K=4/8). Detail lengkap:
> `../2026-9-5_orbit-round1-collision-position-isolation/RESULTS.md`.
> `ra_core.c` TIDAK disentuh, tidak ada kandidat fix baru ditulis.

> **UPDATE 2026-09-05 — kandidat fix `d0_raw_cons` DIUJI, GAGAL (axis
> ke-8).** User mengusulkan varian sempit dari ide "buat `a,b,c,d,o` awal
> tidak nol": verifikasi kode langsung membuktikan `b`/`c` inisial dead
> code (tidak pernah dibaca sebelum ditimpa di round-1), `a`/`o` sudah
> generik tidak nol -- satu-satunya lever hidup adalah `d`. Ide "d nonzero
> dari key/cons" sudah dicoba sebagai `d0_hash` (`fmix32(GUARD_D^key)&0xFF`)
> dan gagal; kandidat baru menguji versi TANPA hash: `d0 = cons & 0xFF`
> (`a0=key` tidak berubah), scope identik `d0_hash` (`it==0` saja).
> Byte-exact PASS (1/300/2M kata). Tier 0 avalanche: **PASS**
> (mean=0.4932, min_bit=0.3125). **Tier 1 PractRand: FAIL K=1+hashed
> @512MB** (`FPF-14+6/16:all` R=+9.4, p=2.7e-8, "very suspicious") --
> signature sama dengan defect asli dan dengan `d0_hash`. K=255
> CLEAN-DUAL (dua base_key, dua ordering, 2GB) — tidak ada regresi.
> **Kesimpulan**: menghilangkan langkah `fmix32` (d nonzero via hash vs
> raw cons langsung) tidak mengubah hasil secara kualitatif — memperkuat
> temuan diagnosis posisi di atas bahwa "`d` nol di round-1" bukan
> mekanisme akar tunggal. Total 8 axis independen sudah dicoba untuk
> defect K=1+hashed, semua gagal. Detail lengkap:
> `../2026-9-5_orbit-d0-raw-cons-fix/RESULTS.md`. `ra_core.c` TIDAK
> disentuh.

> **UPDATE 2026-09-05 — HANDOVER (belum dieksekusi) ditulis untuk arah
> redesign arsitektur, bukan patch formula lagi.** Setelah 8 axis di atas
> semua gagal, diskusi dengan user menyimpulkan: masalahnya bukan di
> titik seed/init/nilai tunggal, tapi di ARSITEKTUR loop
> `ra_permutation_cycle_orbit` sendiri (diffusi terakumulasi bertahap
> per-posisi-round). Arah baru yang disiapkan: generator terpisah
> ("Weyl-add state advance 32-bit murni + finalizer 32×32→32-multiply-only,
> gaya `fmix32`/`triple32`") yang MENGHILANGKAN loop sekuensial dan tabel
> `L[]`/`M[]` sama sekali — bukan modifikasi atas `ra_permutation_cycle_orbit`
> yang ada. Handover lengkap siap-eksekusi (checkpoint wajib, 2 kandidat
> finalizer, pipeline validasi, kriteria hasil):
> `../2026-9-5_orbit-splitmix-finalizer-redesign/HANDOVER.md`. **BELUM
> ADA KODE DITULIS/DIUJI** — ini dokumen rencana untuk sesi eksekusi
> berikutnya, `ra_core.c` TIDAK disentuh.

> **UPDATE 2026-09-05 (sesi eksekusi) — redesign Weyl-add+finalizer
> DIEKSEKUSI, KEDUA kandidat GAGAL Tier 1.** Checkpoint user: kandidat
> pengganti (bukan variant paralel selamanya), key→state mapping opsi (a)
> "SplitMix-literal", skala Tier 1 8GB. Kandidat A `weyl_fmix32` (finalizer
> 3-multiply, sudah ada di `ra_core.c`) dan Kandidat B `weyl_triple32`
> (finalizer 4-multiply, hash-prospector, konstanta diverifikasi ulang
> terhadap README resmi sebelum dipakai) — byte-exact PASS dan Tier 0
> avalanche PASS keduanya, tapi **Tier 1 PractRand 8GB dual-ordering: 0/2
> pasang (kandidat,K) lolos bahkan sampai Stage B (belum sampai Stage C
> reproducibility) untuk KEDUA kandidat.** Pola kegagalan identik antar
> kandidat (mengganti finalizer 3-mul→4-mul TIDAK mengubah hasil sama
> sekali): K=1+Weyl-ordering FAIL @1GB (`FPF-14+6/16`, R=-18.9/A,
> R=-12.1/B) — regresi BARU, K=1+Weyl selalu bersih di arsitektur lama;
> K=1+hashed-ordering justru CLEAN @8GB kedua kandidat (defect asli
> seolah "pindah axis", bukan hilang); K=255 FAIL kedua ordering @512MB-1GB
> (`DC6-9x1Bytes-1`, signature byte-correlation BARU, tidak pernah muncul
> di 8 axis sebelumnya) — regresi BARU, K=255 selalu bersih sebelumnya.
> **Kesimpulan**: karena kedua finalizer (beda keluarga, beda kekuatan
> avalanche terukur) gagal identik, masalahnya BUKAN kekuatan finalizer —
> kemungkinan besar struktur linear murni `state_j=state0+j*WEYL32` (offset
> aditif sama untuk tiap key) itu sendiri tidak cukup didekorelasi oleh
> bijeksi tunggal apa pun di lebar 32-bit (persis risiko birthday-bound
> yang diperingatkan HANDOVER §0.2). Per HANDOVER §7 "keduanya gagal":
> dilaporkan lengkap, TIDAK lompat ke desain lebih berat — menunggu arahan
> user. Benchmark 5-angka tidak dijalankan (scope-nya hanya kandidat yang
> lolos Tier 1). Detail lengkap + log PractRand:
> `../2026-9-5_orbit-splitmix-finalizer-redesign/RESULTS.md`. `ra_core.c`
> TIDAK disentuh — status `ra_core_orbit` produksi TIDAK BERUBAH oleh sesi
> ini (K=255 & K=1+Weyl tetap aman, K=1+hashed tetap belum diperbaiki).

---

## 3. Action item yang belum dikerjakan (blocking promosi)

Dibawa verbatim dari `experiments/2026-9-3_combo-winner-pareto-selection/RESULTS.md`
§Rekomendasi (item 1 dan 2 — item 3 di dokumen asal cuma caveat metodologi,
bukan action item):

1. ~~**Uji dieharder** (bagian dari battery gate yang PAUSED) untuk
   `w8_f10_i0` dan `w8_f28_i0`~~ — **DONE 2026-09-03**, 0 FAILED, lihat di
   atas.
2. ~~**Crossing dengan 7 varian extra-inject** (slot 3 DSL combo-search)
   untuk kedua kandidat~~ — **DONE 2026-09-03**, 11/12 FULLY CLEAN, lihat
   di atas.

Setelah 1 & 2 bersih:
3. ~~**Keputusan user**: kandidat mana yang dipromosikan ke `ra_core.c`~~ —
   **DONE 2026-09-03**, `w8_f10_i0` dipilih user dan diterapkan, lihat §2.
4. ~~Re-verifikasi battery gate~~ — **DONE 2026-09-03**, Step 4-8
   (`../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`):
   collision-scan K=1 PASS, dieharder K=1/K=255 PASS (0 FAILED), PractRand
   16GB K=1/K=255 PASS (bersih total di VPS), shuffle-implementation PASS
   dengan catatan (lihat §2). **Gate ini sekarang CLOSED — tidak ada action
   item tersisa.**

5. ~~**BELUM DIKERJAKAN** — orbit K=1+hashed defect~~ — **DONE 2026-09-06**,
   diperbaiki lewat pendekatan berbeda dari redesign arsitektur di atas
   (root cause asli ternyata birthday-paradox key-repetition 32-bit, bukan
   soal arsitektur loop): widening `addr_hi` (Kandidat D,
   `../2026-9-5_orbit-address-widen-64bit/RESULTS.md`), dipromosikan ke
   `ra_core.c` di `../2026-9-6_orbit-address-widen-64bit-promotion/`. Lihat
   UPDATE 2026-09-06 di §8 untuk detail lengkap + validasi.

---

## 4. Selesai/tertutup sesi ini (2026-09-03) — jangan ditelusuri ulang

- **Kekhawatiran "guard/fmix32 bikin ~100x lebih lambat" terbukti salah.**
  Diukur via `diag_init_candidates.c` (kandidat baru `singleblock_oldera`,
  replay byte-for-byte formula lama `winner_wired_addressable.c` di harness
  yang sama): overhead loop init terisolasi cuma **+0.7%**, K=255 full-call
  cuma **+13.9%** — jauh dari skala "berlipat/100x" yang dicurigai.
- **Klaim tabel lama 30303.0 MB/s** (`experiments/2026-8-31_prng-family-benchmark/RESULTS.md`)
  **tidak reproducible**, bahkan saat formula lama itu sendiri direplay
  persis di mesin ini (~7019 MB/s, beda 4.3x dari klaim). Indikasi kuat:
  artefak metodologi benchmark lama (kemungkinan OLS-ekstrapolasi "asymptotic
  ns/word" yang tidak valid untuk core yang hard-capped di K≤255 by design),
  bukan regresi algoritma nyata dari fix guard/fmix32.
- **Perbandingan vs Philox4x32-10** (multiplier `0xD2511F53U` terkoreksi,
  KAT-verified) **dan Xoshiro256\*\*** (KAT-verified, seeding SplitMix64):
  di K=255, `singleblock` menang **4.2x** atas Philox, **1.5x** atas
  Xoshiro; `orbit` menang atas Philox tapi kalah atas Xoshiro. Di K=1
  RA-PRNG kalah dari keduanya — ini trade-off desain (init O(256) per
  key/address vs seeding O(1) Philox/Xoshiro), **bukan anomali**.
- Kode diagnostik: `experiments/2026-9-3_init-loop-optimization/diag_init_candidates.c`
  (cores: `orbit_baseline`, `orbit_accum`, `singleblock_baseline`,
  `singleblock_accum`, `singleblock_oldera`, `philox`, `xoshiro256`),
  hasil lengkap di `RESULTS.md` folder yang sama.

---

## 5. Item terbuka lain, prioritas lebih rendah — TIDAK memblokir gate korektnes

Init-loop speed optimization (`experiments/2026-9-3_init-loop-optimization/`,
lihat memory `project_init_loop_speed_optimization`) — ini soal kecepatan,
bukan bagian battery gate korektnes:

- Kandidat `accum` (ring-arithmetic strength-reduction pada init loop):
  menang ~20%/14% di K=1 (singleblock/orbit), tapi **singleblock meregresi
  +37.3% di K=255**. Root cause dipersempit ke backend-bound execution
  stalls (bukan cache miss/store-forwarding/frontend) via `perf stat`
  topdown, tapi mekanisme port-level belum terkonfirmasi (butuh profiling
  VTune-style, belum dicoba). **Belum diterapkan ke `ra_core.c`.**
- 4 keputusan terbuka dikembalikan ke user (lihat memory di atas): (1) K=1
  gain worth K=255 regression?, (2) orbit's modest-but-consistent win lebih
  disukai?, (3) apply sekarang vs tunda sampai root-cause tuntas?, (4)
  lanjut Priority 2 (formula baru, output boleh berubah)?

---

## 6. Eksplisit di luar scope gate ini (track terpisah)

Quote dari `HANDOVER.md:134` folder ini sendiri — item-item ini **sengaja
tidak masuk** scope productionization/gate ini, boleh dikerjakan kapan saja
secara independen:

> "Tidak mengerjakan item-item di `project_research_backlog_2026_09`
> (dieharder mandiri di luar Tahap 2 ini, mekanisme `inject` di w=32,
> `docs/ERRATA.md`, SIMD cross-stream L-swap) — itu track riset terpisah,
> independen dari productionization ini."

Status masing-masing: **semua belum dimulai**. `docs/ERRATA.md` bahkan belum
punya file sama sekali di repo ini (baru rencana).

---

## 7. Referensi cepat

- `ra_core.c` — target akhir, saat ini TIDAK boleh disentuh tanpa konfirmasi
  eksplisit user.
- `../2026-9-1_production-candidate-battery/RESULTS.md` — status battery
  gate (PAUSED).
- `../2026-9-2_singleblock-k-threshold-characterization/RESULTS.md` — safe-K
  floor formula saat ini (K≥80-96).
- `../2026-9-2_singleblock-cycle-combo-search/RESULTS.md` — 11 kandidat
  cycle-op combo-search awal (sebelum Pareto-selection).
- `../2026-9-3_combo-winner-pareto-selection/HANDOVER.md` + `RESULTS.md` — 2
  kandidat co-frontier + 2 action item §3 di atas.
- `../2026-9-3_dieharder-inject-crossing/RESULTS.md` — hasil dieharder +
  inject-crossing (action item 1&2 §3, DONE 2026-09-03).
- `../2026-9-3_init-loop-optimization/RESULTS.md` + `diag_init_candidates.c`
  — investigasi kecepatan sesi ini (§4-5 di atas).
- `../2026-8-31_prng-family-benchmark/RESULTS.md` — tabel lama yang
  diklarifikasi di §4.
- `../2026-8-30_addressable-init-research/winner_wired_addressable.c` —
  formula lama (pre-fix) yang direplay sebagai `singleblock_oldera`.

---

## 7b. Benchmark rilis resmi (2026-09-04, `bench_release.c`)

Angka kecepatan di §4 (perbandingan vs Philox/Xoshiro) sudah basi sejak
promosi rolling-`o` + variable-length M[] init (§2b) — diukur ulang resmi
untuk README, hasil lengkap + metodologi di `RESULTS.md`'s "Release
benchmark" section (folder ini). Ringkas: singleblock K=255 sekarang 6.1x
lebih cepat dari Philox (dulu 4.2x), 2.27x dari Xoshiro (dulu 1.5x); K=1
singleblock kini malah lebih cepat dari Philox (1.19x, dulu kalah) berkat
variable-length init, meski masih kalah dari Xoshiro di K=1. orbit K=255/
stream mendekati impas dengan Philox (bukan lagi jelas menang seperti
klaim §4 yang kurang terkontrol), tetap kalah dari Xoshiro. **Caveat
penting yang belum pernah ditulis eksplisit sebelumnya**: seluruh
keunggulan ini murni throughput CPU single-core dalam regime K yang
didesain — belum ada implementasi GPU/SIMD, dan validasi statistik yang
ada mencakup kualitas output single-key, bukan independensi cross-stream
skala besar untuk pola addressable/multikey. Jangan menyiratkan lebih dari
itu di dokumentasi/README.

## 8. Ringkasan satu-baris untuk sesi berikutnya

**GATE PRODUKSI CLOSED (2026-09-03, diperluas 2026-09-04): `w8_f10_i0`
dipromosikan ke `ra_core.c` untuk KEDUA core** (§2 — singleblock,
2026-09-03; §2b — orbit + rolling-`o` kedua core, 2026-09-04) **DAN semua
battery relevan PASS**: production-candidate-battery Step 0-8 (singleblock,
`../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`) +
orbit-kmin-battery (orbit multistream K=1/K=255,
`../2026-9-4_orbit-kmin-battery/RESULTS.md`). `./ra_core validate` sekarang
KAT-checksum untuk kedua core (bukan lagi vs-orbit cross-check). Satu bug
harness (`fmemopen` glibc, bukan defect RNG) ditemukan+diperbaiki di jalan
Step 7 (2026-09-03); hasil akhirnya PASS-dengan-catatan (bias kecil di K=1
runs-test, diterima user, tidak menghalangi apapun). **Tidak ada blocker
korektnes/statistik tersisa** untuk `ra_core.c` — kecepatan juga bukan
masalah, desain sudah kompetitif vs Philox/Xoshiro di regime pemakaian
aslinya (K besar). Item yang MASIH terbuka (bukan blocker gate ini, lihat
§2b/§5/§6 dan `2026-9-4_orbit-fix-and-wideo-rolling-optimization/HANDOVER.md`
§6): `ra_reseed`/`ra_hash` orbit BCFN pra-reseed, eksperimen end-diffusion
singleblock, dan — di luar `ra_core.c` sepenuhnya — keputusan promosi
`ra_core.c` kanonik ini sendiri ke `src/` resmi (belum pernah di-scope).**

**UPDATE 2026-09-04 (sesi ke-3 hari ini)**: satu blocker BARU ditemukan
dan MASIH TERBUKA — `ra_core_orbit` K=1 + urutan key hashed (SplitMix32)
FAIL PractRand parah (§2b UPDATE). Satu hipotesis fix (tap-nol di init)
sudah diuji dan **ditolak** (§2b UPDATE kedua) — root cause asli belum
ketemu. Rencana search tertarget (init-formula × cycle-transform, dual
key-ordering wajib) sudah dirancang siap-eksekusi tapi **belum
diimplementasikan sama sekali** —
lihat `../2026-9-4_orbit-init-cycle-combo-search/HANDOVER.md` untuk mulai
sesi berikutnya. **Jangan anggap gate produksi orbit K=1 tertutup penuh**
sampai ini selesai — status akurat saat ini: singleblock CLOSED, orbit
CLOSED untuk Weyl-key-ordering saja, TERBUKA untuk hashed-key-ordering
K=1.

---

**UPDATE 2026-09-06 — blocker K=1+hashed DITUTUP (root cause: birthday
paradox, bukan arsitektur), DAN item `ra_reseed`/`ra_hash` (§2b baris 178,
§5, §6, dan paragraf di atas) juga DITUTUP. Kedua item terakhir yang
tersisa dari gate produksi `ra_core.c` sekarang resolved.**

1. **K=1+hashed** (§2b UPDATE seri panjang 2026-09-04/05, 8 axis patch
   formula gagal semua, lalu handover redesign arsitektur §5): akhirnya
   diperbaiki BUKAN lewat redesign arsitektur atau patch formula lagi,
   tapi lewat diagnosis ulang total di
   `../2026-9-5_orbit-k1-exhaustive-function-analysis/` yang menemukan
   root cause sebenarnya: **birthday-paradox key-repetition** di domain
   `key` 32-bit (bukan cacat formula `ra_permutation_cycle_orbit`/
   `ra_init_state_orbit`/`ra_reseed` sama sekali). Fix: widening address
   efektif jadi 64-bit lewat parameter kedua `addr_hi`, independen dari
   `key` (Kandidat D dari 5 kandidat,
   `../2026-9-5_orbit-address-widen-64bit/RESULTS.md`), dipromosikan ke
   `ra_core.c` kanonik di
   `../2026-9-6_orbit-address-widen-64bit-promotion/` (signature
   `ra_core_orbit`/`ra_core_singleblock` sekarang 4-arg, `addr_hi=0`
   backward-compatible sebagai default). Battery penuh (PractRand 16GB,
   dieharder, FPF collision-scan, dual key-ordering) PASS untuk Kandidat D
   sebelum promosi.

2. **`ra_reseed`/`ra_hash` orbit BCFN pra-reseed** (§2b baris 178, item
   terbuka sejak 2026-09-04): diuji di
   `../2026-9-4_orbit-reseed-postboundary-bcfn/RESULTS.md` (2026-09-06,
   dijalankan di VPS karena K=1 di sini ~257x lebih mahal dari K=1 biasa —
   tiap kata post-reseed yang disimpan butuh generate 257 kata penuh).
   Sempat BLOCKED gara-gara kontrol sanity gagal (tapi itu gejala item 1
   di atas, bukan `ra_reseed` sendiri — dan driver test lama ternyata
   masih hardcode `addr_hi=0u`, jadi tidak mengaktifkan fix item 1 sampai
   diperbaiki). Setelah driver diperbaiki (addr_hi divariasikan independen
   dari key) dan kontrol sanity bersih: **PASS** — 4 run utama (K∈{1,255}
   × urutan-key∈{weyl,hashed}) semua bersih sampai 8GB, tiga blip
   "unusual" terisolasi (tidak eskalasi, pola noise yang sudah dikenal),
   tidak ada FAIL.

**Konsekuensi untuk §7/§8 di bawah**: kedua item "MASIH terbuka" yang
disebut di §8 (`ra_reseed`/`ra_hash` orbit BCFN pra-reseed, dan status
K=1+hashed) **SEKARANG RESOLVED** — tidak lagi jadi item terbuka. Item
`src/` promotion (disebut sama di §8) juga sudah **DIMULAI DAN SELESAI
untuk scope C-only** 2026-09-06: `src/ra_core/c/ra_core.c` sekarang ada
(copy dari `ra_core.c` kanonik di folder ini, `make build-c` +
`./build/bin/ra_core validate` PASS), `tools/battery/driver_main_template.c`
diupdate untuk kontrak 4-arg (`addr_hi`), README diupdate. Python binding
untuk `ra_core` TETAP belum ada — itu satu-satunya bagian dari roadmap
`src/` promotion yang masih belum dikerjakan (scope C-only disepakati
eksplisit dengan user).

**Status akhir gate produksi `ra_core.c` per 2026-09-06: TIDAK ADA
BLOCKER KOREKTNES/STATISTIK TERSISA** untuk kedua core, K berapa pun di
[1,255], kedua urutan key (Weyl dan hashed), termasuk pola continuous
stream lintas-boundary-reseed. Item yang masih terbuka HANYA yang memang
sudah dari awal di luar scope gate ini (§5 init-loop speed optimization
"accum" — soal kecepatan bukan korektnes, belum diterapkan; §6 dieharder
mandiri/inject-mekanisme/ERRATA.md/SIMD L-swap — track riset terpisah;
Python binding untuk `src/ra_core/`).
