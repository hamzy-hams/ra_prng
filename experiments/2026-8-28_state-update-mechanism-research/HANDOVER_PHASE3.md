# Handover: 2 poin lanjutan riset mekanisme update state L/M

> **Status: SELESAI.** Kedua poin lanjutan di dokumen ini sudah
> dieksekusi dan dijawab — lihat section **"Fase 3: resolusi arah
> `inject` dan keputusan kombinasi L×M"** di akhir `RESULTS.md` untuk
> tabel data, kesimpulan, dan keputusan lengkapnya. Ringkasan: arah
> `inject` berkorelasi kuat dengan `w` (bukan `n`) — memendek di `w=4`,
> memanjang (makin kuat) di `w≥8`, termasuk skala produksi `w=32`; opsi
> (b) di Poin #2 terpenuhi tapi sudah dijawab oleh data Fase 1 yang ada,
> tidak perlu eksekusi baru.

Dokumen ini adalah handover berdiri sendiri untuk melanjutkan riset
`experiments/2026-8-28_state-update-mechanism-research/` — bisa dibaca
tanpa perlu membuka riwayat percakapan sebelumnya. Untuk spek riset asli
(target algoritma, aturan non-hybrid, metodologi 4-axis) lihat
`HANDOVER.md` di folder yang sama. Untuk hasil lengkap fase 1 dan fase 2
(semua tabel, angka, dan analisis) lihat `RESULTS.md` di folder yang
sama — dokumen ini hanya meringkas kesimpulan yang relevan sebagai
konteks, tidak menduplikasi tabelnya.

## Status: apa yang sudah selesai

- **Fase 1 (mekanisme L)** — 3 mekanisme diuji (`permute`=swap
  sekarang, `inject`, `overwrite`) × 2 target (`original`, `winner`) = 6
  binary. Kesimpulan: **swap (`permute`) dipertahankan**. `overwrite`
  konsisten merusak periodisitas di toy-model (~400× lebih pendek di
  `n=8,w=4`) meski ~4-10% lebih cepat — trade-off jelas, bukan win-win.
  `inject` **tidak konsisten arahnya** (lihat detail di bawah) — inilah
  poin lanjutan #1.
- **Fase 2 (mekanisme M/fold)** — 3 alternatif diuji (`m_permute`,
  `m_inject`, `m_overwrite`) vs kontrol `xor_fold`. Kesimpulan: **XOR-fold
  dipertahankan**, jauh lebih superior secara tegas — semua alternatif
  memperpendek periodisitas toy-model 2,2× sampai 47.014× dan
  `winner_m_inject` gagal PractRand katastrofik (p=5,3×10⁻³⁵¹) padahal
  `original_m_inject` (mekanisme sama, target beda) lolos bersih. Tidak
  ada alasan mengganti XOR-fold.
- Semua kandidat sudah tervalidasi bit-identik (varian `permute`/kontrol
  vs binary asli), paritas Python (`toy_prng.py`) vs C
  (`cycle_measure.c`) dicek bit-for-bit di `n=2,w=4` sebelum dipakai
  untuk sweep besar, dan `/graphify --update` sudah dijalankan untuk
  memasukkan semua file fase 1+2 ke knowledge graph.

## Poin lanjutan #1 — Investigasi mekanisme `inject` (L) lebih dalam

**Temuan yang perlu dijelaskan**: `inject` tidak punya arah konsisten di
grid toy-model `n∈{2,4,8}, w∈{4,8}` (lihat `RESULTS.md` bagian "1.
Periodisitas" fase L):

| config | λ permute | λ inject | arah |
|---|---|---|---|
| n=2, w=8 | 560 | 1526 | inject **lebih panjang** |
| n=4, w=4 | 82,807 | 4,448 | inject **jauh lebih pendek** |
| n=4, w=8 | 154,000 | 847,000 | inject **lebih panjang** |
| n=8, w=4 | 6,318,640 | 553,000 | inject **jauh lebih pendek** |

Tidak ada pola jelas (bukan soal ukuran `n` atau `w` semata) — perlu data
lebih banyak sebelum bisa disimpulkan `inject` "aman" atau "berbahaya".

**Rencana konkret untuk sesi lanjutan**:
1. Perluas grid toy-model: tambah config baru (mis. `n=16,w=4` jika
   masih tractable, atau variasikan `rows` yang selama ini fixed
   mengikuti config) untuk lebih banyak titik data.
2. Naikkan jumlah seed di config yang saat ini rendah (`n=8,w=4`:
   50 seed, `n=4,w=8`: 100 seed) — bandingkan dengan config lain yang
   pakai 300 seed, untuk pastikan arah yang diamati bukan noise sampel
   kecil.
3. Analisis tambahan yang belum dilakukan: cek apakah arah `inject`
   (lebih panjang vs lebih pendek dari `permute`) berkorelasi dengan
   paritas `n`/`w`, atau dengan bagaimana index `e`/`d` diturunkan dari
   `c` di setiap config — ini bisa menjelaskan kenapa arahnya berbalik
   antar config alih-alih random.

**Tooling yang sudah ada, tinggal dipakai ulang** (tidak perlu
regenerasi apa pun):
- `../2026-8-25_periodicity-heuristic-validation/toy_prng.py` dan
  `cycle_measure.c` — sudah punya parameter `mechanism` (`0`=permute,
  `1`=inject, `2`=overwrite), tinggal jalankan dengan config baru.
- `toy_sweep.sh` di folder ini — template bash sweep, tinggal duplikasi
  dan ganti `CONFIGS`/`MECHS` untuk grid baru. **Perhatian**: gunakan
  script bash (bukan loop zsh inline) karena zsh tidak word-split
  variabel multi-kata secara default — ini pernah jadi bug di sesi
  sebelumnya.
- Binary skala-penuh `original_inject`/`winner_inject` (`.c` dan
  binary-nya) sudah ada di folder ini dan sudah tervalidasi — tidak
  perlu dibuat ulang kalau hanya perlu data toy-model tambahan.

## Poin lanjutan #2 — Keputusan Fase 3 (kombinasi L×M)

**Konteks keputusan**: Karena Fase 2 sudah menunjukkan XOR-fold jelas
superior (beda orde magnitudo, bukan cuma "lebih baik"), full
cross-product L×M (3 mekanisme L × 4 mekanisme M = 12 kombinasi)
kemungkinan besar **tidak perlu** dijalankan — kombinasi yang realistis
untuk diuji hanya L={permute, inject} × M=xor_fold, yang notabene **sudah
persis sama dengan Fase 1** yang sudah selesai (Fase 1 sudah mengunci
M=xor_fold sebagai default saat menguji varian L).

**Dua opsi yang perlu diputuskan** (jangan langsung eksekusi salah satu
tanpa memutuskan — ini butuh keputusan eksplisit di sesi lanjutan):

- **(a) Tutup riset tanpa Fase 3 terpisah.** Alasan: pertanyaan "L×M
  kombinasi mana yang terbaik" secara efektif sudah terjawab oleh Fase
  1 (L, dengan M=xor_fold) + Fase 2 (M, dengan L=permute) + investigasi
  tambahan `inject` di poin #1 di atas. Kesimpulan akhir riset:
  **pertahankan swap L (`permute`) + XOR-fold M** sebagai kombinasi
  optimal yang sudah divalidasi di kedua sisi secara terpisah.
- **(b) Jalankan minimal 1 kombinasi baru sebelum menutup.** Kalau poin
  #1 di atas menemukan bahwa `inject` konsisten menang di kondisi
  tertentu (mis. `n`/`w` besar), maka masuk akal untuk mengonfirmasi
  kombinasi tsb (`inject` L × `xor_fold` M) di skala penuh — sanity
  check 300 juta nilai, speed, avalanche, dan PractRand 16GB — untuk
  melengkapi baris yang belum ada datanya sebelum menutup riset.

**Aturan pengambilan keputusan**: opsi ini bergantung langsung pada hasil
poin #1. Kalau `inject` terbukti tidak konsisten/berbahaya secara robust
setelah grid diperluas → pilih (a). Kalau ternyata ada
config/kondisi di mana `inject` konsisten menang → pilih (b) untuk
konfirmasi sebelum menutup.

## Constraint yang tetap berlaku (diwarisi dari `HANDOVER.md`)

- **Read-only** terhadap `src/ra_prng2/*`, `src/ra_prng3/*`, dan
  `winner_wired_v2.c` — semua kandidat baru tetap harus jadi file mirror
  baru, tidak boleh edit in-place.
- PractRand: minimum **16GB** untuk kandidat baru mana pun,
  **128GB+** hanya untuk finalis yang lolos axis lain.
- Avalanche: pakai `avalanche_gate_min_bit` (band [0.3,0.7], floor
  per-bit 0.2) — **bukan** `avalanche_gate`/`quality_gate` yang cuma cek
  rata-rata skalar (ini yang pernah meloloskan defect bit di eksperimen
  `2026-8-27`).
- Speed: instruction count (`perf stat -e instructions,cycles`) sebagai
  axis utama, bukan cycle count (noisy di bawah load sistem). Jangan
  jalankan `perf stat` bersamaan job CPU-heavy lain, terutama PractRand.
- Capture avalanche harus **≥2 siklus outer** (bukan 1) — L/M mekanisme
  hanya memengaruhi output setelah reseed pertama; capture 1 siklus
  mathematically tidak bisa mendeteksi perbedaan mekanisme sama sekali
  (pelajaran dari sesi Fase 1/2).

## Setelah selesai

Setelah kedua poin di atas dieksekusi (baik hasilnya opsi (a) atau (b)
di poin #2), **jalankan `/graphify --update`** sebelum menutup tugas —
ini wajib per `CLAUDE.md` proyek ini untuk memasukkan file-file baru ke
knowledge graph.
