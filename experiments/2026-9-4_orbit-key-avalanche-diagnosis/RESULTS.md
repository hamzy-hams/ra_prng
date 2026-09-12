# Diagnosis: kenapa `ra_core_orbit` K=1+hashed-key FAIL (dilution curve + bit-bias)

**Status: diagnosis SELESAI. Tidak ada fix yang didesain/diuji sesi ini —
laporan ini adalah dasar untuk keputusan arah fix berikutnya, bukan
implementasi.**

## 1. Konteks dan koreksi penting

`experiments/2026-9-4_orbit-init-cycle-combo-search/RESULTS.md` (32 kombinasi
init-formula × cycle-transform, dual-ordering) menyimpulkan **0/32 lolos** —
semua kombinasi FAIL pada urutan key hashed (SplitMix32) di 512MB-2GB, sinyatur
tes identik (`FPF-14+6/16`, eskalatif). Laporan itu sempat menunjuk
`ra_hash`/`ra_reseed` sebagai arah paling mungkin.

**Klaim itu salah, dikoreksi sesi ini**: membaca `ra_core_orbit`
(`../2026-9-1_family-productionization/ra_core.c` L254-270):

```c
uint32_t cons = key;
ra_init_state_orbit(L, M, key);
for (size_t it = 0; it < iteration; ++it) {
    ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
    if (count <= 1) return cons;          // <-- K=1: return DI SINI
    cons = ra_reseed(M, L);               //     ra_reseed TIDAK PERNAH sampai
}
```

Untuk `rng=1` (K=1), `count<=1` setelah satu putaran cycle → fungsi **return
sebelum `ra_reseed` dipanggil**. `ra_hash`/`ra_reseed` benar-benar dead code
di skenario K=1 yang FAIL. Root cause harus dicari di tempat lain.

Insight yang benar: K=1 = satu panggilan `ra_init_state_orbit(key)` + **satu
kali** `ra_permutation_cycle_orbit` (tidak ada reseed, tidak ada state yang
bertahan). Yang diuji Tier 1 sebenarnya bukan "kualitas stream dari satu
key", tapi **kualitas `orbit_round1` sebagai fungsi hash `key → 1 word`**,
dipanggil jutaan kali dengan key berbeda-beda.

## 2. Metodologi diagnosis

Reuse penuh binary kontrol yang sudah byte-exact-terverifikasi vs produksi
(`../2026-9-4_orbit-init-cycle-combo-search/candidates/init_linear_shared__w8_f10_i0`)
— **tidak ada kode C baru**, binary itu sudah punya `--stream`/`--stream-hashed`
untuk K berapa pun.

- **Part A — kurva dilusi** (`diag_dilution_curve.py`): PractRand pada urutan
  hashed, base_key `0x12345`, K ∈ {1, 2, 4, 8, 16, 32, 64}, `-tlmin 64MB
  -tlmax 1GB`, early-kill on hard FAIL. Tujuan: cari di K mana defect
  hilang, dan seberapa tajam transisinya.
- **Part B — lokalisasi bit-bias** (`bit_bias_analysis.py`): 50 juta word
  (200MB) K=1 hashed vs 50 juta word K=1 Weyl dari binary yang sama, hitung
  frekuensi per-bit (32 posisi) dengan z-score vs null p=0.5. Diverifikasi
  dulu di data sintetis (`os.urandom`, harus tidak ada bit ter-flag) sebelum
  dipercaya di data asli — PASS (max |z|=2.77 di self-check, murni noise).

## 3. Hasil Part A — kurva dilusi

| K | Verdict @1GB | Test yang muncul | R (raw) | p |
|---|---|---|---|---|
| 1 | **FAIL** | `FPF-14+6/16:all` | +18.7 | 5.1e-17 |
| 2 | VERY SUSPICIOUS (bukan FAIL keras) | `FPF-14+6/16:all` | +10.2 | 4.7e-9 |
| 4 | unusual | `FPF-14+6/16:all` | +5.1 | 2.9e-4 |
| 8 | unusual | `[Low8/32]FPF-14+6/16:all` | +5.0 | 3.8e-4 |
| 16 | bersih | — (0 anomali dari 194 tes) | — | — |
| 32 | bersih | — (0 anomali dari 194 tes) | — | — |
| 64 | bersih | — (0 anomali dari 194 tes) | — | — |

**Bukan cliff tajam, tapi peluruhan mulus** pada test **yang sama persis**
(`FPF-14+6/16`) di semua K yang masih menunjukkan sinyal: R turun kira-kira
1/K (18.7 → 10.2 → 5.1 → 5.0 → di bawah threshold). Ini bukti kuat bahwa
defect terkonsentrasi di **word pertama tiap panggilan segar** (satu bias
kecil, kira-kira tetap besarnya per word, yang ter-"encerkan" secara agregat
begitu ada word tambahan dari init yang sama). Catatan penting: K=16-64
"bersih" di skala 1GB tidak berarti bebas defect sepenuhnya — kalau
pola 1/K konsisten, sinyal residual mereka kemungkinan baru muncul di skala
jauh lebih besar (analog K=1 yang perlu ~512MB-1GB untuk terlihat) — **belum
diuji, di luar scope diagnosis murah ini.**

## 4. Hasil Part B — bit-bias marginal

Self-check (`os.urandom`, N=2 juta): max |z|=2.77, tidak ada bit ter-flag —
tool bekerja benar.

- **K=1 hashed** (N=50 juta): max |z| = 2.39, **tidak ada bit ter-flag**
  (ambang |z|≥4).
- **K=1 Weyl** (N=50 juta): max |z| = 1.91, **tidak ada bit ter-flag**.

**Tidak ada bias bit-tunggal** di kedua ordering — angka-angkanya nyaris
identik dengan noise floor self-check. Ini **menyingkirkan** hipotesis "satu
bit output rusak/bias". Konsisten dengan sinyatur tes yang FAIL
(`FPF-14+6/16` = *frequency profile*, tes windowed/tuple-frequency 14-bit,
bukan tes bit marginal) — defect-nya adalah pola order-tinggi (korelasi
antar-bit dalam window, bukan bias satu bit sendirian), yang secara desain
tidak akan tertangkap tes marginal seperti ini.

## 5. Sintesis dan pertanyaan terbuka

**Yang terkonfirmasi**:
1. `ra_hash`/`ra_reseed` bukan penyebab (dead code untuk K=1) — koreksi dari
   RESULTS.md sesi sebelumnya.
2. Defect terkonsentrasi di **word pertama** hasil `ra_permutation_cycle_orbit`
   segera setelah `ra_init_state_orbit` — dibuktikan lewat kurva dilusi yang
   meluruh mulus ~1/K pada test yang sama persis, bukan artefak acak per-K.
3. Bukan bias bit-tunggal — pola order-tinggi (window/tuple-frequency),
   bukan marginal.

**Yang BELUM terjawab (pertanyaan terbuka, penting untuk desain fix)**: kalau
defect murni "word pertama dari state segar secara inheren lemah", itu
seharusnya muncul SAMA PERSIS di urutan Weyl K=1 juga (semua word Weyl K=1
JUGA "word pertama" — 100% cold, identik strukturnya dengan hashed K=1) — tapi
Weyl K=1 sudah terbukti bersih sampai 8GB
(`../2026-9-4_orbit-kmin-battery/RESULTS.md`). Diagnosis ini **tidak
menjelaskan** kenapa urutan key (Weyl vs hashed) mempengaruhi apakah defect
"word pertama" ini terekspos atau tidak — kemungkinan: stride Weyl yang
sangat teratur (golden-ratio increment) berinteraksi dengan pola order-tinggi
tersebut sedemikian rupa hingga saling meniadakan dalam tes windowed
PractRand, sementara urutan hashed (nyaris acak) tidak. **Ini hipotesis, belum
diuji** — kalau mau dikonfirmasi, cara termurah adalah menjalankan urutan
Weyl K=1 di skala JAUH lebih besar (16-32GB) untuk cek apakah residual R-nya
juga perlahan naik seperti pola hashed di atas, atau benar-benar nol.

## 6. Rekomendasi arah fix (bukan keputusan, untuk didiskusikan user)

Karena defect terlokalisasi ke **satu titik spesifik** (output pertama
setelah init, sebelum ada "pemanasan" siklus) dan **bukan** bias bit
sederhana, ini bukan lagi kandidat untuk search kombinasi finalizer/init
lain seperti sesi sebelumnya (yang sudah terbukti 0/32, sesuai dugaan —
mengganti formula tanpa menambah ronde mixing sungguhan tidak akan
mengubah ini). Dua arah yang lebih masuk akal:

- **Tambah 1 ronde mixing khusus untuk word pertama** (fix ringan,
  tertarget): setelah `ra_init_state_orbit`, sebelum emit word pertama dari
  `ra_permutation_cycle_orbit`, jalankan satu putaran "pemanasan" ekstra
  (mis. ulang tap-8 + finalizer sekali tanpa emit) supaya word pertama
  punya kualitas mixing setara word ke-2+. Trade-off: overhead per
  panggilan (relevan untuk kasus K kecil yang sering dipanggil ulang).
- **Operasi finalizer baru (di luar a,b,c,d,o) khusus buat memperkuat ronde
  pertama** — lebih berat, cuma masuk akal kalau opsi di atas ternyata tidak
  cukup atau terlalu mahal performanya.

Keduanya BELUM didesain/diuji di sini — scope sesi ini murni diagnosis.
Pertanyaan terbuka §5 (kenapa Weyl "lolos" padahal sama-sama cold) sebaiknya
dijawab dulu sebelum comitting ke salah satu arah, karena kalau ternyata
Weyl JUGA punya residual defect yang sama (cuma belum cukup data untuk
terlihat), maka "fix ringan word-pertama" jadi jauh lebih urgent (produksi
saat ini berisiko diam-diam kalau dipakai skala sangat besar dengan urutan
key apa pun, bukan cuma hashed).

## 7. File terkait

- `diag_dilution_curve.py`, `dilution_logs/`, `dilution_summary.json`,
  `dilution_log.jsonl` — Part A lengkap.
- `bit_bias_analysis.py`, `bit_bias_output.txt` — Part B lengkap (data
  mentah `raw/*.bin` sudah dihapus setelah analisis — regenerable ~6 detik
  dari binary kontrol dengan perintah yang sama, lihat komentar di
  `bit_bias_analysis.py` cara pakainya).
