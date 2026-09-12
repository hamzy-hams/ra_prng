# Handover: diagnosis "resonansi konstanta" antara Weyl-stepping test harness dan konstanta internal `ra_init_state_orbit`, untuk defect K=1+hashed pada `ra_core_orbit`

**Status folder ini: BELUM DIKERJAKAN — dirancang siap-eksekusi, ditulis
supaya sesi lain (atau user) bisa langsung jalan tanpa perlu merancang
ulang dari nol.** Tidak ada kode/hasil di folder ini sama sekali sampai
tulisan ini.

**PENTING — baca §0 sebelum mulai implementasi apa pun.** Desain di bawah
adalah proposal siap-eksekusi, BUKAN keputusan final — ada checkpoint wajib
ke user sebelum beberapa langkah.

## 0. WAJIB: checkpoint ke user sebelum implementasi

1. **Nilai persis konstanta step alternatif** (`C_alt1`, dan `C_alt2` kalau
   dipakai) — diusulkan `C_alt1 = 0x2545F491` (odd, popcount=14, beda jauh
   dari `0x9e3779b7`) dan `C_alt2 = 0x85157AF5` (odd, popcount=17) sebagai
   cadangan cross-check. Sudah dicek odd + popcount saat menulis handover
   ini, tapi verifikasi ulang saat implementasi (pola sama seperti
   `GUARD_D` di `orbit-init-warmup-fix/`) — jangan hardcode blind tanpa
   re-cek, dan pastikan tidak beririsan dengan konstanta lain yang sudah
   ada di `ra_core.c` (`0x9e3779b7`, `0x06a0dd9b`, `0x85ebca6b`,
   `0xc2b2ae35`, `GUARD_L`, `GUARD_M`, `GUARD_D`).
2. **Skala PractRand** — diusulkan 2GB (staged 64MB→256MB→2GB), sama
   seperti sesi `orbit-init-warmup-fix`. Konfirmasi ke user dulu kalau mau
   skala beda (lebih murah atau lebih mahal).
3. **Kalau resonansi TERKONFIRMASI** (C_alt FAIL seperti hashed): arah fix
   berikutnya yang terbuka adalah mengubah konstanta internal
   `0x9e3779b7`/`0x06a0dd9b` di `ra_init_state_orbit` (atau cara key masuk
   ke `keyterm_l`/`keyterm_m`) — ini BUKAN keputusan otomatis untuk
   dieksekusi lanjut di sesi yang sama. Diagnostic ini murni untuk
   diagnosis, laporkan dulu hasilnya dan tanya user sebelum mulai
   implementasi fix apa pun, apalagi karena arah itu berarti akhirnya
   akan menyentuh `ra_core.c` (konvensi repo: `ra_core.c` tidak boleh
   disentuh tanpa konfirmasi eksplisit).
4. **Kalau resonansi TIDAK terkonfirmasi** (C_alt tetap CLEAN seperti
   Weyl asli): jangan langsung lanjut menebak hipotesis baru lain tanpa
   diskusi — laporkan dulu, ide lanjutan (lihat §4) baru didiskusikan
   dengan user.

Secara umum: perlakukan handover ini sebagai starting point untuk
didiskusikan ulang di fase plan sesi berikutnya, bukan spec yang tinggal
dieksekusi buta.

## 1. Kenapa diagnostic ini perlu (ringkasan, baca sumber untuk detail lengkap)

Defect `ra_core_orbit` K=1 + urutan key hashed (SplitMix32) FAIL PractRand
parah (`FPF-14+6/16`), sementara K=1 + urutan key **Weyl** (`base_key +
idx*0x9E3779B9`) tetap bersih sampai 8GB. Pertanyaan terbuka ini sudah
disebut di beberapa RESULTS.md berturut-turut dan belum pernah dijawab:
**kenapa dua urutan key yang sama-sama menghasilkan "word pertama setelah
init segar" (struktur cold-word yang identik) bisa punya hasil PractRand
yang sangat berbeda?**

Riwayat percobaan fix yang **SUDAH gagal** (jangan diulang):
1. **Zero-tap hypothesis** (`M[0]`/`L[0]`=0 di `i=0`, formula perkalian
   dengan nol) — patch diverifikasi benar menghilangkan nilai-nol itu,
   tapi hasil PractRand nyaris tidak berubah (R=+135.2 patched vs R=+132.3
   unpatched @8GB). Detail: `../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`.
2. **32-combo search** (8 varian init-formula × 4 varian cycle-transform,
   dual-ordering Weyl+hashed sampai 2GB) — **0/32 lolos**, semua FAIL di
   urutan hashed antara 512MB-2GB. Membuktikan defect BUKAN soal formula
   init/cycle yang mana yang dipakai — mengganti salah satu/kedua fungsi
   itu tidak mengubah hasil. Detail:
   `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`.
3. **Kandidat A `d0_hash`** (seed `d` dibuat key-dependent via
   `fmix32(GUARD_D^key)&0xFF` di `it==0`, badan loop cycle tidak disentuh)
   — lolos Tier 0 avalanche, tapi **FAIL Tier 1 K=1+hashed @512MB**,
   signature `FPF-14+6/16` identik dengan defect asli. K=255 tidak
   regresi. Detail: `../2026-9-4_orbit-init-warmup-fix/RESULTS.md`.
4. **Kandidat B `d0_realround`** (satu ronde asli `i=255` dihitung ulang
   di fase init sebagai warm-up) — **REJECT di Tier 0 avalanche sendiri**
   (mean_fraction 0.28 di bawah ambang 0.3), tidak sampai diuji PractRand.
   Detail sama: `../2026-9-4_orbit-init-warmup-fix/RESULTS.md`.

Kesimpulan dari 4 percobaan di atas: baik mengganti formula init/cycle,
maupun mengganti nilai seed `a`/`d` di ronde pertama (dengan cara murah
atau mahal), **tidak ada yang mengubah signature defect ini sama sekali**.
Ini sinyal kuat bahwa akar masalahnya bukan di titik-titik yang sudah
divariasikan itu.

**Observasi baru yang memicu handover ini** (belum pernah diperiksa di
sesi manapun): konstanta step Weyl yang dipakai di SEMUA test driver
`--stream` (`base_key + idx*0x9E3779B9u`, lihat
`../2026-9-1_family-productionization/multikey_stream_orbit.c` L28, dan
`combo_gen_orbit.py`/`combo_gen_warmup.py` MAIN_FN) hanya beda **2** di
digit hex terakhir dari konstanta internal yang dipakai
`ra_init_state_orbit` untuk `keyterm_l`/`l_val`
(`0x9e3779b7u`, `../2026-9-1_family-productionization/ra_core.c` L198+202):

```
Weyl-stepping constant (test harness, EXTERNAL): 0x9E3779B9
ra_init_state_orbit L-multiplier   (production, INTERNAL): 0x9e3779b7
                                                        beda: hanya 2
```

Ini kemungkinan bukan kebetulan biasa untuk dua "golden ratio constant"
32-bit yang independen (`0x9E3779B9` = konstanta rasio emas 32-bit yang
sangat umum dipakai — juga muncul sebagai `PHILOX_W0` di
`bench_release.c`/`bench_ra_core.c`, dan sebagai bagian dari konstanta
SplitMix32 `0x9E3779B97F4A7C15`). **Hipotesis**: kedekatan dua konstanta
ini membuat urutan key Weyl-stepped menghasilkan state `L[]`/`M[]` yang
"beresonansi" secara tidak wajar (korelasi yang seharusnya tidak ada),
membuat defect tersembunyi khusus untuk urutan key jenis ini — BUKAN
karena urutan key terstruktur/aritmetik itu sendiri aman. Kalau benar,
"Weyl bersih sampai 8GB" adalah **false-negative akibat kebetulan
konstanta**, bukan bukti keamanan asli.

Diagnostic ini **tidak menyentuh `ra_core.c` sama sekali** — hanya
mengubah cara test driver membangkitkan urutan key (konstanta step-nya),
jadi jauh lebih murah dan lebih rendah risiko dibanding 4 percobaan
sebelumnya, dan langsung menjawab pertanyaan terbuka yang paling sering
disebut.

## 2. Scope

Target: HANYA test driver baru (file C baru di folder ini), tidak ada
perubahan ke `ra_core.c` kanonik
(`../2026-9-1_family-productionization/ra_core.c`) — `ra_init_state_orbit`,
`ra_permutation_cycle_orbit`, `ra_core_orbit` di-copy verbatim tanpa
modifikasi sama sekali (pola sama seperti driver-driver sebelumnya, mis.
`multikey_stream_orbit.c`). Yang berubah HANYA formula pembangkit key di
`main()`: konstanta step yang tadinya hardcode `0x9E3779B9u` (Weyl) atau
`splitmix32` (hashed) diganti jadi bisa dipilih dari command line.

K yang diuji: **K=1 saja** untuk diagnostic awal ini (K=1 adalah kasus
yang FAIL parah dan paling murah untuk dites cepat; K=255 tidak perlu
diulang di tahap diagnosis ini — beda dari sesi fix candidate yang wajib
cek regresi K=255).

## 3. File yang harus dibuat

`multikey_stream_orbit_altconst.c` — copy PERSIS preamble (`rot32`/
`fmix32`/`GUARD_L`/`GUARD_M`/`ra_hash`/`ra_reseed`), `ra_init_state_orbit`,
`ra_permutation_cycle_orbit`, `ra_core_orbit` dari
`../2026-9-1_family-productionization/ra_core.c` L144-270 (byte-for-byte,
tidak dimodifikasi — `ra_permutation_cycle_orbit` itu `static` jadi tidak
bisa extern-link, harus verbatim copy, konvensi sama seperti semua driver
sebelumnya).

`main()` custom, CLI surface:
```
<bin> --single <key> <rng>                         # sanity check, sama seperti biasa
<bin> --stream <base_key> <n> <K>                   # kontrol: Weyl asli, step=0x9E3779B9 (hardcode, untuk байт-exact baseline)
<bin> --stream-hashed <base_key> <n> <K>            # kontrol: hashed (SplitMix32), untuk baseline pembanding
<bin> --stream-altconst <base_key> <n> <K> <const_hex>   # BARU: step=<const_hex> bebas dipilih
```

`--stream-altconst` mengimplementasikan pola YANG SAMA PERSIS dengan
`--stream` (`key = base_key + (uint32_t)(block_idx * <const_hex>)`), cuma
konstantanya parameter, bukan hardcode `0x9E3779B9u`.

Wajib: verifikasi byte-exact `--single 1 1` = `397095967` dulu sebelum
menjalankan mode apa pun (disiplin standar repo ini, lihat §5).

## 4. Konstanta step alternatif yang diusulkan

- **`C_alt1 = 0x2545F491`** — odd (bit terakhir 1, syarat konstanta
  multiplicative-step yang baik), popcount=14, berbeda jauh secara bitwise
  dari `0x9e3779b7`/`0x9E3779B9` (XOR keduanya = `0xbb728d26`, tidak ada
  pola dekat).
- **`C_alt2 = 0x85157AF5`** (cadangan cross-check kalau C_alt1 ambigu) —
  odd, popcount=17.
- Kontrol pembanding yang SUDAH ada, tidak perlu di-generate ulang dari
  nol, cukup jadi acuan angka saat interpretasi:
  - Weyl asli (`0x9E3779B9`): CLEAN sampai 8GB
    (`../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`).
  - Hashed (SplitMix32): FAIL mulai skala kecil, R=+18.7 @1GB
    (`../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md`), makin parah
    di skala besar (R=+132.3 @8GB, ADDENDUM di atas).

## 5. Pipeline validasi (urutan wajib)

1. **Byte-exact `--single 1 1`** = `397095967`. Gagal → STOP, perbaiki
   codegen dulu, jangan lanjut ke PractRand (disiplin `feedback_no_reseed_on_fail`
   + pelajaran berulang dari sesi-sesi sebelumnya: verifikasi kode SEBELUM
   percaya statistik).
2. **PractRand, K=1, base_key=`0x12345`** (konsisten dengan sesi-sesi
   sebelumnya), staged **64MB→256MB→2GB**, early-kill on hard FAIL, TIDAK
   retry seed lain kalau FAIL (final):
   - `--stream-altconst 0x12345 <n> 1 0x2545F491`
   - `--stream-altconst 0x12345 <n> 1 0x85157AF5` (kalau hasil pertama
     ambigu atau untuk cross-check langsung)

## 6. Interpretasi hasil (laporkan apa adanya, JANGAN lanjut fix otomatis)

- **Kalau `C_alt1`/`C_alt2` FAIL** (mirip hashed, signature `FPF-14+6/16`
  muncul di skala serupa) → **resonansi terkonfirmasi**. "Weyl bersih"
  adalah artefak kebetulan-konstanta, BUKAN properti aman asli defect ini.
  Arah fix baru yang terbuka (BUKAN untuk dieksekusi otomatis, lihat §0.3):
  ubah konstanta internal `0x9e3779b7`/`0x06a0dd9b` di
  `ra_init_state_orbit` (atau cara key masuk ke `keyterm_l`/`keyterm_m`)
  supaya tidak beresonansi dengan stepping-constant umum manapun — target
  axis BARU (`ra_init_state_orbit`), beda dari 4 percobaan sebelumnya yang
  semuanya menyasar `ra_permutation_cycle_orbit`/nilai seed `a`/`d`.
- **Kalau `C_alt1`/`C_alt2` tetap CLEAN** (seperti Weyl asli sampai skala
  yang diuji) → hipotesis resonansi gugur. Defect memang spesifik ke
  struktur statistik urutan key yang benar-benar acak/hashed (bukan soal
  kedekatan konstanta tertentu). Ide lanjutan yang bisa didiskusikan
  dengan user (BUKAN keputusan final, lihat §0.4): cross-check dengan
  urutan key terstruktur lain yang lebih "acak-terlihat" tapi tetap
  deterministik (mis. LCG dengan konstanta berbeda, atau increment=1
  linear sederhana) untuk memetakan persis batas "terstruktur vs acak"
  yang memicu defect ini.

## 7. Peringatan teknis yang wajib diwarisi (jangan mengulang bug lama)

- **No re-seed retry on FAIL** (`feedback_no_reseed_on_fail`, memory repo
  ini): FAIL di satu base_key/konstanta pada kandidat manapun sudah final
  — jangan retry base_key lain untuk "menjelaskan" kegagalan itu.
- **Verifikasi kode SEBELUM percaya statistik**: byte-exact `--single 1 1`
  (§5 langkah 1) WAJIB lolos sebelum PractRand dijalankan — pelajaran
  berulang dari beberapa sesi lalu.
- **`ra_core.c` TIDAK boleh disentuh** di diagnostic ini sama sekali —
  kalau nanti hasilnya mengarah ke perubahan `ra_init_state_orbit`, itu
  jadi sesi/handover terpisah dengan konfirmasi eksplisit user dulu.

## 8. Referensi cepat

- Diagnosis yang memicu pertanyaan terbuka Weyl-vs-hashed:
  `../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md` §5.
- Defect asli (skala besar, dua base_key independen):
  `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`.
- Percobaan fix yang sudah gagal (jangan diulang):
  `../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`,
  `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`,
  `../2026-9-4_orbit-init-warmup-fix/RESULTS.md`.
- Fungsi target (dibaca, TIDAK diubah):
  `../2026-9-1_family-productionization/ra_core.c`
  (`ra_init_state_orbit` L197-207, `ra_permutation_cycle_orbit` L217-252,
  `ra_core_orbit` L254-270).
- Pola driver yang di-reuse:
  `../2026-9-1_family-productionization/multikey_stream_orbit.c`.
- Status/tempat mencatat hasil setelah selesai:
  `../2026-9-1_family-productionization/PRODUCTION_READINESS_HANDOVER.md`
  §2b (update lagi setelah ada hasil, jangan tambah section baru), memory
  `project_orbit_key_avalanche_diagnosis` dan
  `project_ra_prng_family_productionization`. Jalankan `/graphify --update`
  setelah selesai (wajib per `CLAUDE.md` repo ini).
