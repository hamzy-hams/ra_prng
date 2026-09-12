# ERRATA

Catatan resmi untuk bug produksi dan kesalahan metodologi riset yang
pernah dipublikasikan (di repo ini, dokumen internal, atau draft
publik/paper) dan kemudian dikoreksi. Tujuannya supaya siapa pun yang
menemukan versi lama angka/klaim ini (commit lama, draft paper, cache
lokal) tahu ke mana harus melihat koreksinya, dan supaya reviewer paper
menemukan kejujuran ini di sini dulu, bukan menemukannya sendiri.

Setiap entri: tanggal ditemukan, apa yang salah, apa yang benar, sumber
detail lengkap.

---

## 2026-09-12 — Off-by-one di `ra_core_orbit()` multi-cycle

**Yang salah**: `ra_core_orbit()` menulis 1 word lebih sedikit dari yang
diminta setiap kali `rng ≡ 1 (mod 255)` dan `rng > 255` (mis. rng = 256,
511, 766, 1021, ...). `ra_core_orbit_into()` (dipakai binding
`pip-ra-core`) lebih parah: slot terakhir buffer output tidak ditulis
sama sekali (data lama/uninisialisasi bocor ke output). Penyebab: variabel
`count` dipakai untuk dua makna berbeda ("ini word terakhir yang diminta
user" vs "cycle 255-iterasi ini habis secara natural") yang bertumbukan
tepat saat `count == 256` di awal sebuah cycle.

**Dampak**: KAT (`./ra_core validate`, hanya menguji `n` 1..255) TIDAK
terpengaruh — bug ini murni di jalur multi-cycle. Setiap output lama yang
pernah dihasilkan dengan `rng` pada pola di atas berpotensi punya 1 word
hilang/rusak di akhir stream.

**Yang benar (fix)**: `ra_permutation_cycle_orbit` diubah dari `void`
menjadi `bool`, membedakan eksplisit "berhenti karena diminta user" (return
`true`) vs "cycle habis natural, lanjut ke reseed" (return `false`) —
tidak lagi mengandalkan overload nilai `count`.

**Detail lengkap**: `experiments/2026-9-12_orbit-multicycle-offbyone-fix/RESULTS.md`.

---

## 2026-09-12 — Kesimpulan awal investigasi "orbit vs singleblock" salah (self-correction)

**Yang salah**: investigasi awal hari ini menyimpulkan gap performa ~17x
antara `ra_core_singleblock` (K=255) dan `ra_core_orbit` yang dilaporkan
`bench_release.c` adalah **artefak benchmark** — dugaannya, CPU
out-of-order bisa meng-overlap banyak panggilan independen berturut-turut
(instruction-level parallelism lintas-panggilan) jauh lebih baik untuk
`singleblock` daripada `orbit`, dan gap "sebenarnya" (diukur dengan
microbenchmark buatan sendiri yang mengklaim "adil") cuma ~15%.

**Kenapa salah**: microbenchmark "adil" itu sendiri (`experiments/2026-9-12_orbit-multicycle-offbyone-fix/microbench_orbit_vs_singleblock.c`)
punya bug instrumentasi — menulis ke `volatile` accumulator anti-dead-code-
elimination **255x per panggilan** (di dalam loop 255-iterasi), padahal
harness resmi `bench_release.c` hanya menulis **1x per panggilan** (sekali
per batch K-word, lewat return value). Overhead paksa-tulis-memori 255x
lebih sering itu nyaris tidak berpengaruh ke `orbit` (yang memang sudah
banyak akses memori lewat swap `L[i]<->L[d]`), tapi mendominasi total biaya
`singleblock` (yang aslinya nyaris murni komputasi register) — sehingga gap
keduanya "menyusut" secara artifisial di alat ukur itu sendiri, bukan
karena gap aslinya memang kecil.

**Yang benar (setelah dikoreksi dan diverifikasi ulang)**: gap ~17-21x
antara `singleblock` dan `orbit` **genuine**, dikonfirmasi dengan
microbenchmark yang link langsung ke `src/ra_core/c/ra_core.c` produksi,
meniru persis teknik dan frekuensi anti-DCE harness resmi, PLUS satu
pengukuran `orbit --stream` genuine (1 panggilan langsung, N=20 juta word,
tanpa reinit sama sekali) yang menunjukkan hasil yang secara esensial
sama dengan mode reinit-K-nya (beda <10%) — dikonfirmasi juga dengan
`perf stat` (hardware performance counter). Kesimpulannya: `singleblock`
memang jauh lebih murah per-word secara intrinsik (tidak ada swap `L[]`,
tidak pernah lewat jalur reseed) — ini perbedaan biaya algoritma yang
nyata, bukan artefak cara mengukur. Angka `bench_release.c` yang ada
sekarang **valid dan boleh dikutip apa adanya** di paper.

**Detail lengkap (termasuk arsip kesalahan aslinya, dipertahankan untuk
jejak audit)**: `experiments/2026-9-12_orbit-multicycle-offbyone-fix/BENCHMARK_METHODOLOGY_FINDINGS.md`.

**Catatan terkait (bukan kesalahan, tapi metodologi berdekatan)**: proyek
ini secara independen juga menemukan dan mengoreksi masalah fairness
codegen yang berbeda pada tanggal yang sama — Philox4x32-10/Xoshiro256\*\*
sempat dibandingkan sebagai fungsi `static` (inlined penuh oleh compiler)
melawan `ra_core_orbit`/`ra_core_singleblock` yang `extern`-linked (tidak
inlined). Lihat `README.md` (bagian "Update 2026-09-12") untuk detail —
dampaknya paling besar di `singleblock` K=255, dan angka di README sudah
direvisi untuk itu.
