# Handover: uji 2 kandidat fix tertarget (seed `d0` di fase init) untuk defect K=1+hashed pada `ra_core_orbit`

**Status folder ini: BELUM DIKERJAKAN — dirancang siap-eksekusi, ditulis
supaya sesi lain (atau user) bisa langsung jalan tanpa perlu merancang
ulang dari nol.** Tidak ada kode/hasil di folder ini sama sekali sampai
tulisan ini.

**PENTING — baca §0 sebelum mulai implementasi apa pun.** Desain di bawah
adalah proposal siap-eksekusi, BUKAN keputusan final — ada checkpoint wajib
ke user sebelum beberapa langkah.

## 0. WAJIB: checkpoint ke user sebelum implementasi

Instruksi eksplisit dari sesi yang menulis handover ini: **lebih sering
bertanya ke user sebelum implementasi, khususnya di fase plan** — jangan
asumsikan salah satu pilihan default dan langsung jalan. Titik-titik
keputusan yang WAJIB dikonfirmasi dulu:

1. **Nilai konstanta `GUARD_D`** (Kandidat A, §3) — perlu dipilih & diverifikasi
   balanced-popcount saat implementasi; tanya user apakah ada preferensi cara
   memilihnya (mis. pola yang sama seperti `GUARD_L`/`GUARD_M` dulu dipilih)
   sebelum hardcode nilai apa pun.
2. **Skala Tier 1** (§6 langkah 4) — handover ini menyarankan 2GB (comparable
   dengan sesi combo-search sebelumnya), tapi kalau ingin confidence setingkat
   `orbit-kmin-battery`'s bar asli (8GB), itu jauh lebih mahal waktu —
   konfirmasi ke user skala mana yang diinginkan SEBELUM menjalankan Tier 1,
   jangan pilih sendiri.
3. **Kalau Kandidat A dan B dua-duanya CLEAN-DUAL** — tanya user mana yang mau
   dilanjutkan ke Promotion Tier (lebih murah/A vs lebih dekat bukti kurva
   dilusi/B), jangan otomatis pilih salah satu atau lanjutkan keduanya tanpa
   konfirmasi.
4. **Kalau keduanya FAIL** — JANGAN langsung lanjut ke "cari operasi finalizer
   baru" (opsi berat) tanpa diskusi; laporkan dulu dengan data lengkap,
   biarkan user putuskan arah berikutnya (termasuk opsi menjawab dulu
   pertanyaan terbuka Weyl-vs-hashed di skala besar, §8).
5. **Perubahan signature `ra_permutation_cycle_orbit_combo`** (§4) — ini
   desain yang diusulkan sesi lalu, BUKAN sudah difinalkan; kalau saat
   implementasi ternyata ada cara lebih bersih/sederhana, diskusikan dulu
   sebelum menyimpang dari desain di sini.

Secara umum: perlakukan handover ini sebagai starting point untuk
didiskusikan ulang di fase plan sesi berikutnya, bukan spec yang tinggal
dieksekusi buta.

## 1. Kenapa search ini perlu (ringkasan, baca sumber untuk detail lengkap)

- `../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md`: defect K=1+hashed-key
  `ra_core_orbit` terlokalisasi ke **word pertama** tiap panggilan segar —
  kurva dilusi menunjukkan peluruhan mulus ~1/K pada tes `FPF-14+6/16` yang
  sama persis: K=1 FAIL (R=+18.7) → K=2 VERY SUSPICIOUS (R=+10.2) → K=4/K=8
  "unusual" (R≈+5) → K=16+ bersih di skala 1GB. Bit-bias marginal test
  (50 juta word, self-check lolos di data sintetis) **tidak menemukan bias
  bit-tunggal** — defect-nya pola order-tinggi (window/tuple-frequency),
  konsisten dengan sinyatur `FPF-14+6/16`.
- `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`: 32 kombinasi
  init-formula × cycle-transform (dual-ordering) sudah diuji duluan — **0/32
  lolos**. Membuktikan defect BUKAN soal "formula init yang mana" atau
  "formula cycle yang mana" — mengganti salah satu/kedua fungsi itu tidak
  mengubah hasil. Jangan ulangi search kombinasi serupa.
- Diskusi lanjutan (sesi yang menulis handover ini): user bertanya variabel
  apa yang berpindah dari ronde 1 ke ronde 2 di `ra_permutation_cycle_orbit`
  (`../2026-9-1_family-productionization/ra_core.c` L217-252), dengan ide:
  kalau bisa diidentifikasi, bikin "pemanasan" khusus untuk variabel itu di
  fase init, supaya loop cycle internal SAMA SEKALI tidak disentuh.

**Analisis (dikonfirmasi baca kode persis produksi)**: hanya `a` dan `d`
yang benar-benar loop-carried antar ronde (`b`,`c` dihitung ulang tiap
ronde dari nol — baris 230-232 — jadi nilai awalnya tidak pernah dipakai;
`o` murni fungsi `i` + `M[]` statis, tidak bergantung ronde sebelumnya).
`a` mulai dari `cons`=`key` — **sudah key-dependent sejak ronde pertama**.
`d` mulai dari **`0`, konstan untuk SEMUA key** — satu-satunya titik
non-key-dependent di ronde pertama, tepat di formula `a = (d^o)^(cons+a)`
dan `b = (cons+a)^(o+d)` (baris 229-230). Ini kandidat mekanisme yang
**belum pernah diuji** di sesi mana pun sebelumnya.

## 2. Scope

Target: `ra_core.c` kanonik
(`../2026-9-1_family-productionization/ra_core.c`), fungsi
`ra_permutation_cycle_orbit` (baris 217-252) — **HANYA signature/nilai
masuk `a`/`d` yang diusulkan berubah, badan loop (baris 228-251) TIDAK
disentuh sama sekali** (lihat §4). `ra_init_state_orbit` dan
cycle-transform (`w8_f10_i0`, kontrol/produksi) **TIDAK divariasikan lagi**
— sudah terbukti bukan akar masalah lewat 32-combo search di §1.

Perubahan hanya berlaku saat `it==0` (key segar, belum pernah reseed) —
untuk `it>=1` (setelah `ra_reseed`, hanya relevan di `rng>255`), `d` tetap
`0` seperti sekarang, supaya perilaku long-continuous-stream yang sudah
tervalidasi ("128GB clean") tidak berubah sama sekali.

**Catatan penting**: karena `rng<=255` (termasuk K=255) SELALU `it==0`
dalam satu panggilan (`ra_reseed` dead code untuk rentang ini — dikonfirmasi
di sesi diagnosis §1), kedua kandidat di bawah otomatis berlaku juga untuk
K=255, bukan cuma K=1. Jadi **regresi K=255 (yang sudah bersih) WAJIB
dicek ulang**, bukan diasumsikan aman.

## 3. Kandidat A — `d0_hash` (sintetis, murah)

```c
#define GUARD_D 0x????????u   // lihat §0.1 -- konstanta balanced-popcount baru,
                               // BEDA dari GUARD_L/GUARD_M, diverifikasi popcount
                               // seimbang saat implementasi (pola sama seperti
                               // GUARD_L=0x38916df4/GUARD_M=0x6c26fc92)
uint32_t d0 = fmix32(GUARD_D ^ key) & 0xFFu;
uint32_t a0 = cons; // TIDAK diubah, sudah key-dependent
```

Guard-XOR (bukan `fmix32(key)` polos) mengikuti pola fix weak-key `key=0`
yang sudah ada di codebase (`fmix32(0)=0` adalah known pitfall — lihat
riwayat `GUARD_L`/`GUARD_M` di `ra_init_state_orbit`). ~1 `fmix32` call,
jauh lebih murah dari ronde cycle penuh (8-tap `o` + 2 formula + finalizer).

## 4. Kandidat B — `d0_realround` (satu ronde asli i=255, dibuang, dihitung di fase init)

Duplikasi PERSIS formula ronde pertama (bukan formula baru — pakai `o` yang
sama seperti yang akan dipakai loop asli di `i=255`), dengan `a=key,d=0`
sebagai starting point (yaitu, kondisi hari ini), hasilnya jadi seed baru:

```c
// dihitung sekali di init/pre-cycle phase, SEBELUM ra_permutation_cycle_orbit()
// dipanggil -- pakai o255 yang identik dengan yang dihitung ulang di awal
// loop asli (M[] belum berubah)
uint32_t o255 = /* 8-tap sama persis seperti ra_core.c L222-225 */;
uint32_t a_warm = (0u ^ o255) ^ (key + key);
uint32_t b_warm = (key + a_warm) ^ (o255 + 0u);
uint32_t c_warm = rot32((a_warm >> 13) ^ a_warm, b_warm);
c_warm ^= c_warm >> 17u;              // finalizer w8_f10_i0, sama seperti produksi
uint32_t d0 = c_warm & 0xFFu;
uint32_t a0 = a_warm;                  // bukan lagi `key` mentah -- sudah "satu ronde lebih maju"
```

Catatan desain: `i=255` dipakai LAGI di ronde nyata pertama (bukan lanjut ke
`i=254`) — beda dari K=2 alami (yang maju ke `i=254`, jendela `o` berbeda).
Ini sengaja: tujuannya "satu ronde dibuang tanpa mengganggu progresi index
loop asli", bukan mereplikasi K=2 persis. Kalau kandidat B gagal tapi kurva
dilusi K=2 (§1) jelas berhasil, itu sinyal progresi index `o` (bukan cuma
nilai `a`/`d`) yang penting — dicatat sebagai temuan, bukan dianggap gagal
total.

## 5. Perubahan kode (minimal, hanya signature — lihat §0.5)

```c
// SEBELUM (produksi, tidak berubah untuk kontrol):
static void ra_permutation_cycle_orbit(uint32_t cons, size_t it, ...) {
    uint32_t a = cons, b = (uint32_t)it, c = 0, d = 0;
    ...
}

// SESUDAH (kandidat A/B, badan loop L228-251 IDENTIK):
static void ra_permutation_cycle_orbit_combo(uint32_t cons, size_t it,
                                              uint32_t a0, uint32_t d0,   // BARU
                                              const uint32_t *M, uint32_t *L,
                                              uint64_t *count, FILE *raw_stream) {
    uint32_t a = (it == 0) ? a0 : cons;
    uint32_t b = (uint32_t)it, c = 0;
    uint32_t d = (it == 0) ? d0 : 0u;
    ... // loop L228-251 SAMA PERSIS, tidak ada baris yang diubah
}
```

`a0`/`d0` dihitung oleh helper terpisah (`ra_init_warmup_orbit(key, &a0, &d0)`,
dua varian A/B) dipanggil dari `ra_core_orbit_combo` tepat setelah
`ra_init_state_orbit_combo`, sebelum loop `for(it...)` — murni bagian fase
init, konsisten dengan permintaan user (§0).

## 6. Struktur kode yang harus dibuat

File baru di `experiments/2026-9-4_orbit-init-warmup-fix/`. Reuse PREAMBLE
(`rot32`/`fmix32`/`GUARD_L`/`GUARD_M`/`ra_hash`/`ra_reseed`/`splitmix32`) dan
pola CLI (`--single`/`--stream`/`--stream-hashed`) dari
`../2026-9-4_orbit-init-cycle-combo-search/combo_gen_orbit.py` — init-formula
dan cycle-transform tidak divariasikan lagi (§2), jadi codegen jauh lebih
sederhana: **3 binary total** — `control` (baseline produksi persis, `d0=0`
selalu, buat regresi byte-exact), `d0_hash` (Kandidat A), `d0_realround`
(Kandidat B).

- `combo_prng_warmup.py` — oracle Python (extend
  `../2026-9-4_orbit-init-cycle-combo-search/combo_prng_orbit.py`'s pola,
  tambah 2 varian seed).
- `combo_gen_warmup.py` — codegen C, `write_and_compile`, `sanity_check`
  (pola sama persis).
- `tier0_avalanche_warmup.py`, `tier1_gate_warmup.py` — port langsung dari
  `../2026-9-4_orbit-init-cycle-combo-search/tier0_avalanche_orbit.py` /
  `tier1_gate_orbit.py`, ganti daftar kombinasi jadi 3 kandidat.

## 7. Pipeline validasi (urutan wajib, sama disiplin seperti sesi-sesi sebelumnya)

1. **Kontrol byte-exact vs produksi**: `control` harus `397095967` untuk
   `key=1,rng=1` (nilai ground-truth yang sama dipakai sejak awal).
2. **Oracle vs codegen** untuk Kandidat A dan B (`--single key rng` vs oracle
   Python, byte-for-byte) — SEBELUM PractRand, disiplin yang sama seperti
   selalu (lihat §9).
3. **Tier 0 avalanche** (K=1, band `LOW=0.3,HIGH=0.7,MIN_BIT_FLOOR=0.2`)
   untuk A dan B.
4. **Tier 1 dual-ordering, K=1 DAN K=255** (bukan cuma K=1 — lihat §2 catatan
   regresi K=255), base_key `0x12345`, Weyl+hashed. **Skala: konfirmasi ke
   user dulu, §0.2** — sampai 2GB (comparable dengan Tier 1 combo-search
   sebelumnya) adalah baseline yang disarankan, tapi 8GB (setingkat bar asli
   `orbit-kmin-battery`) memberi confidence lebih kuat kalau user mau
   menunggu lebih lama. Staged screen (64MB→256MB) lalu confirm, early-kill
   on FAIL, pola persis `tier1_gate_orbit.py`.
5. **Regresi long-stream** (opsional, BUKAN blocking): `it>=1` tidak berubah
   secara desain, risikonya rendah, tapi kalau ingin extra confidence,
   jalankan 1 stream `rng` besar (mis. 1M+ words) satu key untuk pastikan
   tidak ada crash/perilaku aneh di percabangan `it==0` vs `it>=1`.

## 8. Kriteria hasil

- Kandidat CLEAN-DUAL (K=1 dan K=255, Weyl dan hashed, sampai skala yang
  disepakati §0.2) → kandidat layak untuk didiskusikan promosi ke Promotion
  Tier (VPS, 8-16GB) — **keputusan terpisah, bukan otomatis, lihat §0.3**.
- Kandidat A gagal tapi B berhasil (atau sebaliknya) → informatif:
  menunjukkan apakah cukup "nilai `d0` yang key-dependent" (A) atau perlu
  "progresi state riil satu ronde" (B).
- Keduanya gagal → **lihat §0.4** — jangan langsung lanjut ke opsi berat.
  Pertanyaan terbuka soal Weyl-vs-hashed (kenapa Weyl K=1 bersih padahal
  sama strukturnya — belum terjawab di sesi diagnosis, lihat
  `../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md` §5) makin penting
  dijawab dulu sebelum lanjut ke arah lain (operasi finalizer baru).
- `ra_core.c` TIDAK boleh disentuh tanpa konfirmasi eksplisit user (konvensi
  repo) — walau kandidat lolos semua tahap di sini, promosi ke `ra_core.c`
  kanonik tetap keputusan terpisah.

## 9. Peringatan teknis yang wajib diwarisi (jangan mengulang bug lama)

- **No re-seed retry on FAIL** (`feedback_no_reseed_on_fail`, memory repo
  ini): FAIL di satu seed/base_key pada kandidat manapun sudah final —
  jangan retry seed lain untuk "menjelaskan" kegagalan itu.
- **Verifikasi kode SEBELUM percaya statistik**: byte-exact kontrol vs
  produksi DAN byte-exact codegen vs oracle Python (§7 langkah 1-2) WAJIB
  lolos sebelum PractRand dijalankan — pelajaran berulang dari beberapa
  sesi lalu (`../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`,
  `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`).
- **`fmemopen` glibc bug**: kalau ada harness yang pakai `fmemopen`, buffer
  WAJIB `data_size+1` byte, bukan pas (lihat riwayat bug ini di
  `../2026-9-1_production-candidate-battery/ADDENDUM_POST_FIX_STATUS.md`).
  Tidak dipakai di pipeline validasi §7 ini (pakai `subprocess.run(...).stdout`
  langsung), tapi catat kalau menambah harness baru.

## 10. Referensi cepat

- Diagnosis yang memicu handover ini:
  `../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md`.
- Search kombinasi yang sudah membuktikan bukan soal formula init/cycle:
  `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md` (infra reusable:
  `recipes_orbit.py`, `combo_prng_orbit.py`, `combo_gen_orbit.py`,
  `tier0_avalanche_orbit.py`, `tier1_gate_orbit.py`).
- Fungsi target: `../2026-9-1_family-productionization/ra_core.c`
  (`ra_permutation_cycle_orbit` L217-252, `ra_core_orbit` L254-270,
  `ra_init_state_orbit` L197-207 — tidak divariasikan).
- Status/tempat mencatat hasil setelah selesai:
  `../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md`
  §2b (update lagi setelah ada hasil, jangan tambah section baru), memory
  `project_singleblock_kmin_defect` dan `project_orbit_key_avalanche_diagnosis`.
  Jalankan `/graphify --update` setelah selesai (wajib per `CLAUDE.md` repo
  ini).
