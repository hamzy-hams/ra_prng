# Hasil: diagnosis resonansi-konstanta Weyl-stepping vs `ra_init_state_orbit`

**Status: SELESAI (§0-§6 HANDOVER.md dijalankan penuh).** Hipotesis
resonansi-konstanta **GUGUR** — `C_alt1` (bitwise jauh dari `0x9e3779b7`)
tetap CLEAN sampai 2GB, sama seperti Weyl asli (`0x9E3779B9`). Hasil tidak
ambigu sehingga `C_alt2` **tidak dijalankan** (sesuai §0.1 handover: cadangan
hanya untuk kasus ambigu). Sesuai HANDOVER.md §0.4, laporan ini berhenti di
sini — TIDAK lanjut otomatis ke ide berikutnya (LCG/increment=1 di §4).
`ra_core.c` kanonik TIDAK disentuh sama sekali oleh sesi ini.

## 1. Konteks

Pertanyaan terbuka dari beberapa sesi sebelumnya: `ra_core_orbit` K=1 dengan
key ordering **hashed** (SplitMix32) FAIL PractRand parah (`FPF-14+6/16`,
R=+18.7 @1GB, R=+132.3 @8GB), sementara K=1 dengan ordering **Weyl**
(`base_key + idx*0x9E3779B9`) tetap CLEAN sampai 8GB — padahal keduanya
menghasilkan struktur "cold-word pertama setelah init segar" yang identik.
Observasi pemicu sesi ini: konstanta step Weyl test-harness (`0x9E3779B9`)
hanya beda 2 di digit hex terakhir dari konstanta internal
`ra_init_state_orbit` untuk `keyterm_l`/`l_val` (`0x9e3779b7u`). Hipotesis:
kedekatan ini membuat "Weyl bersih" jadi resonansi kebetulan, bukan properti
aman asli.

## 2. File yang dibuat

`multikey_stream_orbit_altconst.c` — copy verbatim byte-for-byte `rot32`,
`fmix32`, `GUARD_L`/`GUARD_M`, `ra_hash`, `ra_reseed`, `ra_init_state_orbit`,
`ra_permutation_cycle_orbit`, `ra_core_orbit` dari
`../2026-9-1_family-productionization/ra_core.c` L144-270 (tidak dimodifikasi
sama sekali). `main()` custom dengan 4 mode CLI (`--single`, `--stream`,
`--stream-hashed`, `--stream-altconst`); `--stream-altconst` mengambil
konstanta step dari argv, bukan hardcode `0x9E3779B9u`.

`run_altconst_practrand.py` — staged PractRand runner (port dari
`../2026-9-4_orbit-init-warmup-fix/tier1_gate_warmup.py`), single-ordering
(hanya `--stream-altconst`), early-kill on hard FAIL, tanpa retry seed lain.

## 3. Konstanta yang diuji

Cross-check tidak bentrok (dibaca dari `ra_core.c`): konstanta existing di
file itu adalah `0x9e3779b7` (popcount=21), `0x06a0dd9b` (popcount=15),
`0x85ebca6b` (fmix32, popcount=18), `0xc2b2ae35` (fmix32, popcount=16),
`GUARD_L=0x38916df4` (popcount=16), `GUARD_M=0x6c26fc92` (popcount=16).

- **`C_alt1 = 0x2545F491`** (odd, popcount=14) — diuji. Tidak bentrok
  dengan daftar di atas.
- `C_alt2 = 0x85157AF5` (odd, popcount=17) — **tidak diuji**, hasil `C_alt1`
  tidak ambigu (lihat §5).

## 4. Validasi §5 langkah 1: byte-exact (WAJIB lolos sebelum PractRand)

- `--single 1 1` = `397095967` — **PASS**, cocok persis dengan spec.
- Sanity tambahan (di luar pipeline wajib, untuk memverifikasi jalur kode
  `--stream-altconst` sebelum dipercaya): `--stream-altconst 0x12345 1000 1
  0x9E3779B9` menghasilkan output byte-identik (`sha256sum` sama) dengan
  `--stream 0x12345 1000 1` — membuktikan `--stream-altconst` adalah
  generalisasi murni dari `--stream`, bukan implementasi berbeda yang
  kebetulan cocok di kasus lain. `--stream-altconst` dengan `C_alt1`
  menghasilkan output berbeda (`sha256sum` beda), membuktikan konstanta
  benar-benar dipakai.

## 5. Tier 1: PractRand staged, K=1, base_key=`0x12345`, `C_alt1=0x2545F491`

| Stage | Range | Hasil | Waktu |
|---|---|---|---|
| A (screen) | 64MB→256MB | **CLEAN** (0 anomali di setiap checkpoint) | 8s |
| B (confirm) | 256MB→2GB | **CLEAN** (0 anomali di setiap checkpoint, termasuk di 2GB: "no anomalies in 205 test result(s)") | 48s |

Log lengkap: `altconst_0x2545F491_K1_0x00012345_A.log`,
`altconst_0x2545F491_K1_0x00012345_B.log`. Ringkasan mesin:
`altconst_0x2545F491_result.json`.

Hasil ini **bukan sekadar "tidak FAIL"** — tidak ada satupun soft-anomaly
("unusual"/"suspicious") di seluruh checkpoint sampai 2GB, sama bersihnya
dengan baseline Weyl asli (`../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`).
Tidak ada ambiguitas yang memerlukan cross-check `C_alt2`.

## 6. Verdict

**Hipotesis resonansi-konstanta GUGUR.** `C_alt1` — bitwise jauh dari
`0x9e3779b7`/`0x9E3779B9` (XOR = `0xbb728d26`, tidak ada pola dekat), dan
sama sekali bukan turunan "golden ratio constant" — tetap CLEAN sampai 2GB
dengan pola stepping yang identik. Ini membuktikan:

- "Weyl bersih sampai 8GB" **bukan** artefak kebetulan dari kedekatan
  `0x9E3779B9` vs `0x9e3779b7` — properti aman itu genuine untuk urutan
  key berbasis stepping aritmetik linear apa pun (setidaknya untuk 2
  konstanta step yang sudah dicoba: `0x9E3779B9` asli dan `C_alt1`).
- Defect K=1+hashed tetap spesifik ke **struktur statistik urutan key**
  (hashed/acak-terlihat via SplitMix32) — bukan soal konstanta step
  tertentu yang "beresonansi" dengan internal `ra_init_state_orbit`.

Sesuai HANDOVER.md §0.4/§6 cabang kedua ("resonansi TIDAK terkonfirmasi"):
laporan berhenti di sini. Ide lanjutan yang terbuka untuk didiskusikan
dengan user (BUKAN keputusan final) — memetakan batas persis
"terstruktur vs acak" yang memicu defect ini via cross-check urutan key
lain (mis. LCG dengan konstanta berbeda, atau increment=1 linear
sederhana) — **belum dijalankan**, menunggu diskusi user.

## 7. Yang TIDAK dilakukan sesi ini (di luar scope, per persetujuan awal)

- TIDAK menjalankan `C_alt2` (tidak diperlukan, hasil `C_alt1` tidak ambigu).
- TIDAK mengubah `ra_core.c` kanonik sama sekali.
- TIDAK melanjutkan ke ide LCG/increment=1 (§4 HANDOVER.md) tanpa diskusi
  user — pertanyaan terbuka "Weyl-vs-hashed" masih belum terjawab akar
  sebabnya (hanya satu hipotesis penjelas yang sudah digugurkan sesi ini).

## 8. Referensi

- Spec sesi ini: `HANDOVER.md`.
- Diagnosis pemicu pertanyaan terbuka: `../2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md` §5.
- Defect asli (skala besar): `../2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`.
- Percobaan fix yang sudah gagal (konteks, tidak diulang di sini):
  `../2026-9-4_orbit-init-zerotap-fix/RESULTS.md`,
  `../2026-9-4_orbit-init-cycle-combo-search/RESULTS.md`,
  `../2026-9-4_orbit-init-warmup-fix/RESULTS.md`.
- File kode: `multikey_stream_orbit_altconst.c`, `run_altconst_practrand.py`.
- Artefak: `altconst_0x2545F491_K1_0x00012345_A.log`,
  `altconst_0x2545F491_K1_0x00012345_B.log`,
  `altconst_0x2545F491_result.json`.
