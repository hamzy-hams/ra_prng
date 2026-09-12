# HANDOVER: Fix off-by-one di `ra_core_orbit` multi-cycle (produksi)

Status: **BELUM DIMULAI** — rencana siap-eksekusi hasil sesi diagnosis
2026-09-12, dilanjutkan di sesi lain (kemungkinan sesi ini sendiri atau
sesi berikutnya). Jangan diedit lagi setelah eksekusi dimulai (konvensi
repo: `HANDOVER.md` = rencana beku, `RESULTS.md` = hasil final).

## 1. Latar belakang

Bug ini ditemukan sebagai **Temuan #2** di
`experiments/2026-9-11_state-size-pareto-frontier/RESULTS.md` §9 (dan versi
`ra_core_param.c` studi itu, karena core loop-nya ditranslasi mekanis dari
`ra_core.c` produksi tanpa mengubah logika ini). Saat itu di-workaround di
tooling (`practrand_gate.py`, margin +1 word untuk lane `continuous`) tanpa
menyentuh `ra_core.c` — eksplisit di luar scope riset itu. Sesi ini
menindaklanjuti dengan rencana fix untuk source produksi.

**Bug**: `ra_core_orbit()` (`src/ra_core/c/ra_core.c`, mode multi-cycle,
yaitu ketika `rng > 255`) menulis **1 word lebih sedikit** dari yang
diminta setiap kali `rng ≡ 1 (mod 255)` — mis. `rng = 256, 511, 766, 1021,
...`. Ini murni bug integer-accounting di logika terminasi loop, BUKAN bug
di formula/algoritma RNG-nya (rolling-o, rotasi, fmix32, dll. semua tidak
tersentuh).

## 2. Root cause (diverifikasi lewat trace manual + baca kode, sesi ini)

Kode saat ini (`ra_core.c:242-296`):

```c
static void ra_permutation_cycle_orbit(uint32_t cons, size_t it,
                                        const uint32_t *M, uint32_t *L,
                                        uint64_t *count, FILE *raw_stream) {
    ...
    for (uint32_t i = 255; i > 0; --i) {
        ...
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) break;      // <-- (A) "word ini yang terakhir diminta"
        --(*count);
        ...                          // swap L[i]<->L[d], update o
    }
}

uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    ...
    uint64_t count = rng;
    uint64_t iteration = rng / 255 + 1;
    for (size_t it = 0; it < iteration; ++it) {
        ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
        if (count <= 1) return cons; // <-- (B) "anggap semua selesai"
        cons = ra_reseed(M, L);
    }
    ...
}
```

`count` dipakai untuk dua makna berbeda yang bertumbukan di titik (B):

- **Kasus A — cycle berhenti lewat `break` eksplisit** (baris ditandai (A)):
  word yang baru ditulis memang word ke-`rng` yang terakhir diminta.
  `count` berhenti di nilai 1 tanpa sempat di-decrement lagi. Ini memang
  sinyal "selesai" yang valid.
- **Kasus B — cycle habis 255 iterasi penuh secara alami** (bukan lewat
  `break`, `for`-loop keluar sendiri karena `i` mencapai 0): kalau nilai
  `count` di awal cycle itu kebetulan **persis 256**, maka setelah 255 kali
  `--(*count)` nilainya JUGA berakhir di 1 — tapi maknanya beda total:
  cycle ini baru saja menghabiskan 255 word (satu cycle penuh), masih ada
  **1 word lagi** yang seharusnya ditulis di cycle berikutnya (butuh
  `ra_reseed` dulu). Cek `if (count <= 1) return cons;` di titik (B) tidak
  bisa membedakan Kasus A dari Kasus B — keduanya sama-sama memicu `return`
  dini, sehingga 1 word terakhir yang sudah "dijanjikan" oleh
  `iteration = rng/255 + 1` tidak pernah ditulis.

**Kapan tepatnya count == 256 di awal sebuah cycle?** Selama cycle demi
cycle mengonsumsi tepat 255 unit `count` (kasus B berulang), maka bug ini
kena persis ketika sisa `count` menyusut sampai bernilai 256 di suatu
titik — yang terjadi jika dan hanya jika `rng ≡ 256 (mod 255) ≡ 1 (mod
255)` **dan** `rng > 255` (kalau `rng <= 255`, satu-satunya cycle yang
jalan selalu berakhir lewat Kasus A yang benar, termasuk kasus tepat
`rng == 255` — sudah diverifikasi manual, tidak kena bug).

**Dampak terukur (ditelusuri manual, 2 contoh)**:
- `rng = 256`: cycle pertama mengonsumsi 255 word (Kasus B, `count`
  256→1), fungsi `return` di iterasi `it=0` — **255 word ditulis, 1
  kurang**.
- `rng = 511`: cycle pertama & kedua masing-masing Kasus B (`count`
  511→256→1), `return` di `it=1` setelah 2×255=510 word — **510 word
  ditulis, 1 kurang**. (Bug ini tidak berakumulasi >1 word kurang — begitu
  misfire terjadi, fungsi langsung `return`, jadi selalu persis 1 word
  kurang, bukan lebih.)

**Konfirmasi independen — `ra_core_engine.c` (layer `pip-ra-core`)**:
`ra_core_engine_fill_block()` (baris 87-96) memanggil
`ra_permutation_cycle_orbit` yang sama tapi men-seed `count = UINT64_MAX`
supaya loop SELALU keluar lewat Kasus B (jalur natural, tidak pernah
`break`) dan SELALU memanggil `ra_reseed` setelahnya — komentarnya sendiri
bilang "the early-exit path only exists for ra_core_orbit's fixed
total-rng case". Artinya versi *streaming* (tanpa batas `rng` tetap) tidak
pernah kena bug ini by construction, dan untuk `rng` yang sama persis di
zona `rng ≡ 1 (mod 255), rng > 255`, engine ini akan menghasilkan 1 word
LEBIH BANYAK daripada `ra_core_orbit(rng, ...)` — bukti independen bahwa
outputnya seharusnya `rng` word penuh, bukan `rng-1`.

**KAT (`SINGLEBLOCK_KAT_CHECKSUMS`/`ORBIT_KAT_CHECKSUMS`) TIDAK terpengaruh**:
`checksum_key_core()` (baris 497-509) hanya menguji `n` dari 1 s.d. 255
(`for (size_t n = 1; n <= 255; ++n)`) — seluruhnya di bawah ambang bug
(`rng > 255` disyaratkan). Jadi **tidak perlu regenerasi KAT** untuk fix
ini — golden checksum yang ada tetap valid sebagai gerbang regresi.

**Konsumen yang langsung terdampak**:
- `ra_core_orbit()` sendiri dipanggil lewat CLI `--stream`/`--multistream`
  (`ra_core.c` `main()`) dengan `rng` sembarang dari argumen — siapa pun
  yang minta `rng` di zona bug dapat 1 word lebih sedikit dari yang diminta,
  diam-diam (tidak ada error/warning).
- `ra_core_orbit_into()` (`ra_core_engine.c:51-55`, dipakai `pip-ra-core`)
  — buffer `out` dialokasikan sebesar `rng` word oleh caller, tapi hanya
  `rng-1` slot pertama yang benar-benar ditulis untuk `rng` di zona bug;
  slot terakhir (`out[rng-1]`) TIDAK ditulis (nilai lama/uninisialisasi
  bocor ke output) — ini korupsi data nyata untuk Python binding, bukan
  cuma kekurangan jumlah word.

## 3. Rencana fix (disarankan, BELUM diterapkan)

**Pendekatan**: ganti sinyal terminasi dari "re-test nilai `count`" (ambigu)
menjadi **flag eksplisit** yang ditentukan tepat di titik asal ambiguitas
(dalam `ra_permutation_cycle_orbit`, di baris `if (*count <= 1) break;`),
lalu diteruskan ke caller. Tidak mengubah aritmatika decrement/`count`
sama sekali — murni menambah sinyal yang sudah hilang.

```c
// return true  = cycle berhenti lewat Kasus A (word terakhir yang diminta
//                sudah ditulis, caller harus berhenti SEKARANG).
// return false = cycle habis 255 iterasi penuh secara alami (Kasus B,
//                caller harus reseed lalu lanjut cycle berikutnya, MASIH
//                ada word tersisa meski `*count` kebetulan == 1 juga).
static bool ra_permutation_cycle_orbit(uint32_t cons, size_t it,
                                        const uint32_t *M, uint32_t *L,
                                        uint64_t *count, FILE *raw_stream) {
    ...
    for (uint32_t i = 255; i > 0; --i) {
        ...
        if (raw_stream) fwrite(&c, sizeof(uint32_t), 1, raw_stream);
        d = c & 0xFFu;
        if (*count <= 1) return true;   // Kasus A, eksplisit
        --(*count);
        ...
    }
    return false;                       // Kasus B, loop habis natural
}

uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    ...
    for (size_t it = 0; it < iteration; ++it) {
        bool done = ra_permutation_cycle_orbit(cons, it, M, L, &count, raw_stream);
        if (done) return cons;
        cons = ra_reseed(M, L);
    }
    ...
}
```

**Catatan implementasi**:
- Signature berubah dari `void` ke `bool` (butuh `#include <stdbool.h>` —
  cek dulu apakah sudah ter-include transitif; kalau belum, tambahkan).
- `iteration = rng / 255 + 1` (baris `ra_core.c:284`) **TIDAK perlu
  diubah** — sudah diverifikasi manual (lihat §4) bahwa formula ini selalu
  menyediakan tepat 1 cycle "cadangan" yang cukup untuk kasus `rng ≡ 1 (mod
  255)`, baik di kode lama (di mana cadangan itu TIDAK PERNAH terpakai
  karena bug) maupun di kode baru (di mana cadangan itu sekarang benar-benar
  terpakai untuk menulis word terakhir yang hilang).
- `ra_core_engine.c` men-`#include "ra_core.c"` verbatim dan memanggil
  `ra_permutation_cycle_orbit` langsung (baris 90) — signature yang berubah
  (`void`→`bool`) otomatis konsisten di sana karena source-nya sama; return
  value `bool` itu **diabaikan** di context itu (engine sengaja selalu
  Kasus B by construction via `count=UINT64_MAX`), jadi tidak perlu
  perubahan kode di `ra_core_engine.c` sendiri — cukup pastikan compile
  ulang tidak keluar warning `-Wunused-result` (fungsi `static`, biasanya
  aman, tapi cek saat build).

**TIDAK direkomendasikan** (alternatif yang ditimbang, ditolak):
- Mengubah kondisi jadi `count == 0` dan menyesuaikan titik decrement —
  secara teknis bisa berhasil, tapi mengubah 2 titik sekaligus
  (kondisi break DAN kapan decrement terjadi) sehingga lebih berisiko
  memunculkan bug baru di kasus batas lain (`rng` kecil, `rng` tepat
  kelipatan 255, dll.) yang sudah terverifikasi benar sekarang. Pendekatan
  flag di atas mengubah paling sedikit (0 baris aritmatika), jadi paling
  aman untuk diverifikasi.

## 4. Rencana verifikasi (WAJIB sebelum promosi, urutan)

1. **KAT self-test tetap PASS** (`./ra_core validate`) — gerbang keras
   pertama, harus 0 mismatch persis seperti sebelum fix (dikonfirmasi §2
   bahwa KAT tidak menyentuh zona bug, jadi harus byte-identik).
2. **Exhaustive word-count check** (script baru, disarankan Python,
   panggil binary `--stream <key> <addr_hi> <rng>` dan hitung actual word
   ter-fwrite lewat panjang stdout dalam byte / 4): untuk **setiap** `rng`
   di rentang `[1, 3000]` (mencakup beberapa periode 255: 255, 256, 510,
   511, 765, 766, ..., plus semua nilai kecil non-boundary sebagai kontrol
   negatif), verifikasi `actual_word_count == rng` persis. Sebelum fix,
   script ini harus mereproduksi bug (word count `rng-1` tepat di
   `rng ∈ {256, 511, 766, 1021, 1276, ...}`); sesudah fix, semua `rng` di
   rentang itu harus pas.
3. **Regression check nilai word yang SUDAH benar tidak berubah**: untuk
   `rng` di LUAR zona bug (terutama `rng <= 255` yang notabene dicek KAT,
   dan beberapa `rng` besar non-boundary seperti 300, 600, 1000), simpan
   output binary sebelum & sesudah fix, harus byte-identik persis (fix ini
   seharusnya HANYA menambah word yang sebelumnya hilang, tidak mengubah
   satu byte pun dari word-word yang sudah ditulis).
4. **Parity check terhadap `ra_core_engine`**: untuk beberapa `rng` di
   zona bug (mis. 256, 511), bandingkan `ra_core_orbit(key, addr_hi, rng,
   ...)` (sesudah fix) word-demi-word dengan hasil
   `ra_core_engine_next(key, addr_hi, rng word diminta dari engine baru)`
   — harus identik (§2 sudah menduga keduanya seharusnya sama; ini
   pembuktian empiris).
5. **Regenerasi/verifikasi tooling downstream**: cek apakah
   `tools/battery/practrand_gate.py` workaround (+1 word margin untuk lane
   `continuous`, ditambahkan sesi 2026-09-11) masih diperlukan atau kini
   jadi mubazir/berlebihan setelah fix — kalau berlebihan, catat
   (TIDAK harus dihapus dalam tugas ini kalau di luar scope, tapi WAJIB
   dicatat di `RESULTS.md` supaya tidak jadi utang teknis tak terdokumentasi).
6. **Cek pemakai lain `ra_core_orbit`/`ra_core_orbit_into`**: grep seluruh
   repo (`experiments/`, `tools/`, binding `pip-ra-core` kalau ada) untuk
   pemanggil dengan `rng` tetap besar (>255) yang mungkin kena bug ini
   sebelum fix — dicatat sebagai "data lama yang mungkin di-generate
   dengan bug ini" tanpa perlu regenerasi retroaktif kecuali user minta.

## 5. Peringatan penting — ini PERUBAHAN PERILAKU (breaking change kecil)

Fix ini **mengubah output** `ra_core_orbit()`/`ra_core_orbit_into()` untuk
setiap `rng` di zona `rng ≡ 1 (mod 255), rng > 255` — sebelumnya
menghasilkan `rng-1` word (dengan word terakhir buffer `_into` berisi data
lama/tidak terdefinisi), sesudahnya menghasilkan `rng` word penuh dan
benar. Ini BUKAN regresi — ini korupsi/kekurangan yang sudah ada dari
awal, sekarang diperbaiki — tapi WAJIB dicatat eksplisit di changelog/
`RESULTS.md` karena:
- Siapa pun yang sudah men-generate & MENYIMPAN stream dengan `rng` di
  zona itu (mis. data riset lama, seed reproduksibilitas) akan dapat
  hasil BERBEDA (lebih panjang 1 word, dan `cons`/checkpoint akhir juga
  berbeda karena sekarang benar-benar melewati 1 reseed tambahan yang
  sebelumnya terlewat) kalau di-generate ulang setelah fix.
- KAT tidak terpengaruh (§2), jadi tidak ada regresi di situ — tapi ini
  perlu ditulis eksplisit di pesan commit/`RESULTS.md` supaya tidak
  dikira "kenapa checksum lain berubah" oleh sesi mendatang.

## 6. Verifikasi end-to-end (urutan eksekusi)

1. Terapkan fix (§3) ke `src/ra_core/c/ra_core.c` — **HANYA** dua fungsi
   yang disebutkan, tidak menyentuh apa pun di formula/algoritma.
2. Compile ulang (`gcc -O3 -march=native -std=gnu17 -include stdalign.h
   ra_core.c -o ra_core -Wall -Wextra`), 0 warning baru.
3. Jalankan §4 langkah 1-4 (KAT, exhaustive word-count, regression,
   parity engine) — semua harus PASS sebelum lanjut.
4. Compile ulang `ra_core_engine.c` (dipakai `pip-ra-core`, kalau
   binding-nya di-build di sesi ini) — pastikan tidak ada warning baru dari
   perubahan signature `bool`.
5. Tulis `RESULTS.md` pendek: root cause, fix, bukti verifikasi §4, catatan
   breaking-change §5, dan status tooling workaround
   (`practrand_gate.py`) — apakah masih perlu dipertahankan atau sudah
   mubazir.
6. `/graphify --update` sebelum menutup tugas (CLAUDE.md repo).

## 7. File kritis

- **Diedit**: `src/ra_core/c/ra_core.c` (`ra_permutation_cycle_orbit`
  baris ~242-277, `ra_core_orbit` baris ~279-296).
- **Tidak diedit tapi harus tetap compile bersih**: `src/ra_core/c/
  ra_core_engine.c` (`#include "ra_core.c"` verbatim, signature call-nya
  otomatis ikut berubah).
- **Referensi/tidak wajib diubah**: `tools/battery/practrand_gate.py`
  (workaround +1 word — evaluasi ulang di §4 langkah 5, JANGAN dihapus
  diam-diam tanpa mencatat alasannya).
- **Baru**: script exhaustive word-count check (§4 langkah 2) — taruh di
  folder eksperimen ini (`experiments/2026-9-12_orbit-multicycle-offbyone-fix/`).

## 8. Catatan untuk sesi lanjutan

- Bug ini SPESIFIK untuk `ra_core_orbit` mode multi-cycle (`rng > 255`).
  `ra_core_singleblock` tidak pernah punya lebih dari 1 cycle (hard-capped
  `rng <= 255`, `ra_core.c:407-411`), jadi tidak mungkin kena pola bug yang
  sama — TIDAK perlu fix serupa di sana.
- `ra_core_param.c` (studi state-size 2026-09-11) mewarisi bug yang sama
  (translasi mekanis `255`→`N-1`) — riset itu SUDAH SELESAI dan
  `RESULTS.md`-nya sudah final; TIDAK perlu diedit ulang untuk fix ini
  kecuali user secara eksplisit minta (di luar scope handover ini, murni
  catatan supaya tidak lupa kalau relevan nanti).
- Famili lain (`ra_prng2`/`ra_prng3`, `src/ra_prng2/`, `src/ra_prng3/`)
  punya pola `count <= 1` serupa di beberapa titik (ditemukan lewat grep
  sesi ini, BELUM dianalisis) — TIDAK termasuk scope handover ini, cuma
  dicatat sebagai kemungkinan area yang perlu dicek terpisah kalau ada
  waktu/permintaan user.
