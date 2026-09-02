# RESULTS: pre-reseed experiment untuk defect K=1 `ra_core_singleblock`

## Status: EKSPERIMEN GAGAL — pre-mix M[] sebelum cycle TIDAK menutup defect K=1

## Konteks

`ra_permutation_cycle_singleblock` (`../2026-9-1_family-productionization/ra_core.c`,
baris 214-236) selalu memulai loop internalnya dari `i=255`. Untuk `rng=1`
(K=1), loop hanya jalan **satu iterasi** sebelum `break` (karena
`*count<=1`). PractRand gagal katastrofik pada pola multikey K=1
(`BRank` R sampai ribuan di 1GB) — ditemukan di
`../2026-9-1_production-candidate-battery/RESULTS.md`, dikonfirmasi lagi
sebagai kontrol sanity di sini (`practrand_baseline_k1_control_2g.log`,
`BCFN(2+0,...) R=+1033`, `BRank(12):1K(2) R=+5666`, dst — identik magnitude
dengan temuan sebelumnya).

## Eksperimen yang diuji (permintaan user, 2026-09-02)

"Balik urutan algoritma — reseed dulu sebelum cycle dimulai, supaya cons
dan M entropinya sudah naik duluan di awal." Diimplementasikan di
`diag_prereseed.c` sebagai `ra_core_singleblock_prereseed`: setelah
`ra_init_state_singleblock(M, key)`, jalankan pre-mix pass
`ra_prereseed_singleblock(M)` — hash `M[]` (256 kata) jadi 8 kata via
`ra_hash` (reuse fungsi yang sama persis dipakai `ra_reseed`), fold balik
ke `M[]` (XOR tiap `tmp8[e]` ke blok 32-kata sumbernya), turunkan `cons`
baru dari `tmp8` (formula sama seperti `ra_reseed`'s `new_cons`). `cons`
hasil pre-mix ini (bukan `key` mentah) yang jadi input awal ke
`ra_permutation_cycle_singleblock` — cycle-nya sendiri TIDAK diubah. Tidak
memunculkan kembali `L[]` (tetap self-hash `M[]` saja, mempertahankan
properti no-L Tahap 6).

## Hasil

| Run | Target | Hasil |
|---|---|---|
| `practrand_baseline_k1_control_2g.log` (kontrol sanity) | 1-2GB | FAIL katastrofik — `BCFN(2+0,13-1,T) R=+1033`, `BRank(12):3K(1) R=+12273` |
| `practrand_prereseed_k1_1g.log` (kandidat) | 1GB | **FAIL katastrofik, MAGNITUDE HAMPIR IDENTIK** — `BCFN(2+0,13-1,T) R=+1028`, seluruh baris FPF-14+6/16 FAIL sama persis polanya dengan baseline |

Tidak dilanjutkan ke skala lebih besar (8-32GB) — sudah gagal jelas di 1GB,
sesuai instruksi "kalau masih FAIL katastrofik di awal, tidak perlu lanjut
ke skala lebih besar."

Edge-case check (`--single`, dilakukan sebelum PractRand): `key=0` dan
`key=0xFFFFFFFF` menghasilkan output non-degenerate di KEDUA varian
(baseline maupun prereseed) — ini karena fix guard-XOR key=0
(`../2026-9-1_keyzero-guard-fix/`) ada di `ra_init_state_singleblock` yang
dipakai bersama oleh keduanya, independen dari eksperimen ini. Bukan
bagian dari yang diuji di sini, dicatat sebagai konfirmasi saja.

Speed overhead (`--bench`, untuk referensi meski tidak relevan lagi karena
eksperimen gagal): K=1 36.0→47.5 ns/word (+32%), K=255 0.572→0.551 ns/word
(dalam noise, pre-mix teramortisasi).

## Kenapa gagal — analisis akar masalah

Pre-mix mengubah **input** (`cons`, dan isi `M[]`) yang dipakai iterasi
pertama cycle, tapi TIDAK mengubah **bentuk transformasinya**. Untuk K=1,
satu-satunya iterasi yang jalan (selalu `i=255`, baca `M[5]`/`M[6]` via
`M[(uint8_t)(i+6)]`/`M[(uint8_t)(i+7)]`) menghitung:

```
o = (M[5]<<6) ^ (M[6]<<7)
a = (d ^ o) ^ (cons + a)      // d=0, a=cons di awal → a = o ^ (2*cons)
b = (cons + a) ^ (o + d)      // = (cons + a) ^ o
c = rot32((a>>13) ^ a, b)
```

Ini tetap fungsi SATU PUTARAN dari `(cons, M[5], M[6])` — cuma satu kali
rotate-dan-combine. Seberapa pun bagus `cons`/`M[5]`/`M[6]` dicampur DI
LUAR transformasi ini (pre-mix menyeluruh 256 kata sekalipun), transformasi
satu-putaran ITU SENDIRI tidak cukup mendifusikan bit — dan PractRand's
BCFN test justru mendeteksi persis pola "struktur berdekatan-bit yang bisa
diprediksi" ini, yang ada di BENTUK transformasinya, bukan di seberapa
random inputnya. **Pre-mix seed tidak bisa menutup kelemahan yang letaknya
di jumlah putaran mixing** — itu butuh transformasi diaplikasikan LEBIH
dari satu kali sebelum output pertama, bukan input yang lebih "kaya
entropi" untuk satu kali aplikasi yang sama.

Ini konsisten dengan temuan sebelumnya di
`../2026-9-1_multikey-remix-search/HANDOVER.md`: opsi "warm-up cycle
(≥128 dari 255 putaran) sebelum output pertama" TERBUKTI menutup BCFN
defect yang lama, tapi ditolak karena overhead 5.5-13x di K kecil. Temuan
di sini menguatkan alasan itu — jumlah putaran memang yang menentukan,
bukan kualitas seed, jadi warm-up-cycle (atau varian yang lebih murah
darinya) kemungkinan satu-satunya kelas solusi yang bekerja, bukan
pendekatan pre-mix-seed-murah yang dicoba di sini.

## Rekomendasi

**Jangan terapkan** `ra_core_singleblock_prereseed` — tidak menutup
defect, cuma menambah overhead 32% di K=1 tanpa manfaat. Tidak ada
perubahan ke `ra_core.c` (sesuai instruksi, produksi tetap seperti semula
plus fix key=0 dari sesi sebelumnya).

Sesuai instruksi: TIDAK melanjutkan ke variasi lain atau karakterisasi
K-threshold tanpa konfirmasi user — itu langkah terpisah yang perlu
diminta eksplisit.
