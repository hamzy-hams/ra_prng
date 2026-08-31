# Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`

Ditulis untuk sesi Claude Code baru yang melanjutkan topik ini. **Status:
Tahap 6 SELESAI (2026-08-31)** -- implementasi, validasi bit-identik
exhaustif, dan benchmark semuanya tuntas dan terdokumentasi. Ringkasan
lengkap ada di `RESULTS.md` bagian "## Tahap 6". File ini fokus ke konteks
alur/keputusan, bukan mengulang angka (baca `RESULTS.md` untuk angka).

## 1. Trigger dan motivasi

User (2026-08-31) mengamati bahwa array `L[256]` di generator addressable
kemungkinan bisa dihapus untuk kasus pemakaian tertentu: "addressable
penuh/agresif" -- satu key/alamat = satu init = ambil sampai 255 kata lalu
buang/re-init ke alamat baru, tanpa reseed/lanjutan.

Ini **persis** ide yang sudah diantisipasi `HANDOVER_TAHAP5.md` section 4
dan sengaja ditunda:

> Tidak mungkin tanpa mengubah formula... Catat sebagai kemungkinan
> "Tahap 6" terpisah... hanya kalau user eksplisit minta.

User sekarang memintanya secara eksplisit, jadi dikerjakan sebagai
**Tahap 6**, melanjutkan folder `experiments/2026-8-30_addressable-init-research/`
yang sudah ada (bukan folder tanggal baru) -- sesuai konvensi folder ini
sendiri.

## 2. Analisis (dikonfirmasi sebelum menulis kode apa pun)

Dua Explore agent + pembacaan source langsung memverifikasi:

1. `L[]` ditulis oleh `ra_init_state_addressable` dan di-swap
   (`L[i]<->L[d]`) di `ra_permutation_cycle`, tapi isinya **tidak pernah
   dibaca** oleh apa pun yang memengaruhi `a`/`b`/`c`/`d`/`o`. Satu-satunya
   pembaca fungsional `L[]` adalah `ra_reseed`'s `M[i] ^= L[i]`.
2. `ra_reseed` **terbukti tidak pernah dipanggil** kalau `rng <= 255`
   (`iteration = rng/255 + 1 == 1`, loop `ra_core` return sebelum iterasi
   kedua -- dan karenanya `ra_reseed` -- tercapai). Identik di
   `winner_wired_addressable.c`, `tahap4_bench.c`, `tahap5_bench.c`.
3. `d = c & 0xFFu` **tetap wajib dipertahankan** meski swap dihapus --
   dipakai lagi di iterasi berikutnya untuk `a`/`b`, independen dari swap.
   Hanya perannya sebagai index swap `L[]` yang hilang, bukan `d` itu
   sendiri. (User sempat menduga `d` juga bisa dipotong -- analisis kode
   menunjukkan ini tidak akurat; `d` tetap bagian aritmetika inti.)
4. Tidak ada jalur baca `L[]` tersembunyi lain di ~30 file lineage yang
   dicek (grep menyeluruh: `src/ra_prng2/*`, semua `2026-8-27_operand-position-search/v*.c`,
   `2026-8-28_state-update-mechanism-research/*`, semua `2026-8-30_*`).
5. `scrambler_addressable.c`/`scrambler_wired_addressable.c` (tool shuffle
   terpisah) punya `L[]` internal yang SAMA (copy byte-for-byte generator
   ini), tapi input shuffle nyata biasanya `> 255` token sehingga jalur
   reseed-nya rutin terpakai -- **tidak termasuk scope Tahap 6 ini**, dan
   tidak boleh dialihkan ke `ra_core_singleblock`.

## 3. File & perannya

| File | Peran |
|---|---|
| `tahap6_bench.c` | Harness baru. Copy byte-for-byte `rot32`/`ra_hash`/`ra_permutation_cycle`/`ra_reseed`/`ra_init_state_addressable_baseline`/`ra_core_baseline` (kontrol, tak diubah) + Philox4x32-10 (comparator, sama seperti Tahap 4/5) + fungsi baru `ra_init_state_full`/`ra_permutation_cycle_full`/`ra_core_singleblock` (fast path, scope `rng<=255` saja, hard-abort di luar itu). Mode CLI: `verify` (KAT Philox), `validate` (gate bit-identik exhaustif -- **wajib PASS sebelum percaya angka apa pun**), `--stream <core> <key> <n>`, `init-cost <core> <repeats>`, `throughput <core> <n1>...`, `reinit-sweep <core> <target_words> <k1>...`. Cores: `baseline`, `singleblock`. |
| `tahap6_benchmark.py` | Orkestrasi Python -- jalankan `verify` lalu `validate` dulu (refuse lanjut kalau salah satu gagal), lalu `init-cost`/`throughput`/`reinit-sweep` untuk kedua core, tulis `tahap6_results_*.json`, cetak perbandingan langsung baseline-vs-singleblock di K yang sama. |
| `tahap6_results_init-cost_{baseline,singleblock}.json`, `tahap6_results_throughput_{baseline,singleblock}.json`, `tahap6_results_reinit-sweep_{baseline,singleblock}.json` | Data mentah hasil run terakhir. |

**Tidak diubah (read-only precedent, diperluas dari daftar Tahap 5)**:
`winner_wired_addressable.c`, `winner_wired_addressable_v2.c`,
`tahap4_bench.c`, `tahap4_benchmark.py`, `tahap5_bench.c`,
`tahap5_benchmark.py`, plus daftar lama (`src/ra_prng2/*`,
`src/ra_prng3/*`, `winner_wired*` di `2026-8-27_operand-position-search/`,
3 file Philox buggy, `2026-8-29_parallelization-research/*.py`+
`simd_prototype.c`).

## 4. Batas scope keras (WAJIB dibaca sebelum memakai `ra_core_singleblock`)

- **Hanya valid untuk `rng` di `[1,255]`**. `ra_core_singleblock` hard-abort
  (`fprintf(stderr,...); abort();`) kalau dipanggil dengan `rng>255` --
  bukan sekadar "belum diuji", tapi memang **secara struktural tidak bisa**
  reseed sama sekali (tidak ada jalur kode untuk itu).
- Validasi bit-identik dan warisan validasi statistik (128GB PractRand,
  0 collision, 0 cross-correlation) **hanya berlaku untuk rentang itu**.
  Jangan pernah klaim validasi itu berlaku untuk `rng>255` -- variant ini
  memang tidak bisa jalan di situ.
- **Bukan drop-in replacement** untuk `ra_core`/`ra_core_baseline` secara
  umum -- fast path khusus, bukan pengganti default.
- Tidak menyentuh `scrambler_addressable.c`/shuffle tools sama sekali.

## 5. Hasil kunci (angka lengkap di `RESULTS.md` "## Tahap 6")

Gate korektnes: **9.945/9.945 kombinasi (39 key x 255 panjang) bit-identik,
0 mismatch** -- plus spot-check `--stream`+`cmp` terhadap
`winner_wired_addressable` asli, 0 mismatch.

Benchmark (reinit-sweep, kasus paling relevan untuk "addressable agresif"):
`singleblock` lebih cepat dari `baseline` di **semua** titik sweep
`K in [1,255]` yang diuji, dengan margin membesar seiring `K` (36% lebih
cepat di K=1, 92% lebih cepat di K=192) -- karena penghematan terjadi di
DUA tempat (init O(256) DAN tiap iterasi hot-loop), bukan cuma biaya init
sekali yang teramortisasi. Titik impas ke Philox (`K*`) turun dari ~60.1
(baseline) ke ~12.3 (singleblock).

## 6. Status & langkah selanjutnya

**Tahap 6 selesai sebagai kandidat riset** -- belum dipromosikan ke
`winner_wired_addressable.c`/`_v2.c`, belum dipindah ke `src/`. Ide lanjutan
yang secara eksplisit BUKAN bagian Tahap 6 ini (dicatat, bukan dikerjakan):

1. Optimasi/perubahan term `d = c & 0xFFu` itu sendiri (mis. tidak perlu
   dibatasi ke 1 byte lagi karena bukan index array lagi) -- ini PERUBAHAN
   OUTPUT algoritmik (formula baru), butuh Tahap 0 (avalanche/periodicity)
   + Tahap 3 (128GB PractRand dkk) penuh diulang dari nol sebelum bisa
   dipercaya, TIDAK bisa mewarisi validasi Tahap 6 ini.
2. Varian yang mendukung `rng>255` dengan tetap menghemat biaya blok
   pertama (no-`L` untuk blok awal, baru munculkan `L`/reseed kalau
   lanjutan benar-benar diminta) -- desain berbeda, belum dieksplorasi.
3. Perbandingan multi-RNG lebih luas (xoshiro256\*\*/pcg32/chacha20, bukan
   cuma Philox) -- topik terpisah yang dibahas di sesi ini juga tapi
   sengaja tidak digabung ke Tahap 6 (tetap apples-to-apples dengan
   metodologi Philox-only Tahap 4/5).

## 7. Setelah mengerjakan follow-up di atas

Jalankan `/graphify --update` sebelum menutup sesi (wajib per `CLAUDE.md`
proyek ini). **Catatan penting**: di sesi tempat Tahap 6 ini dikerjakan,
skill `graphify` **tidak terdaftar** di daftar skill yang tersedia --
langkah ini tidak bisa dijalankan otomatis di sesi itu dan perlu di-flag
ke user / dijalankan manual di sesi lain yang skill-nya tersedia.
