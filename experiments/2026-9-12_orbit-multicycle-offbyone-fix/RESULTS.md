# RESULTS: Fix off-by-one di `ra_core_orbit` multi-cycle (produksi)

Status: **SELESAI, DITERAPKAN**. Eksekusi penuh dari `HANDOVER.md` (rencana
beku sesi 2026-09-12) — root cause dan rencana fix di sana diverifikasi
ulang cocok persis dengan kode aktual sebelum diterapkan.

## Root cause (ringkas)

`ra_permutation_cycle_orbit()` (`src/ra_core/c/ra_core.c`) memakai `*count`
untuk dua makna berbeda yang bertumbukan: (A) "word yang baru ditulis
adalah word ke-`rng` terakhir yang diminta" (`*count<=1`, `break` eksplisit)
vs (B) "cycle habis 255 iterasi penuh secara natural, masih ada word
tersisa untuk cycle berikutnya". Ketika `count == 256` persis di awal
sebuah cycle (terjadi iff `rng ≡ 1 (mod 255)` dan `rng > 255`), Kasus B
berakhir dengan `*count == 1` juga — persis sama dengan sinyal Kasus A —
sehingga `ra_core_orbit()`'s `if (count <= 1) return cons;` salah
menyimpulkan "selesai" dan tidak pernah menulis 1 word terakhir yang sudah
dijanjikan oleh `iteration = rng/255 + 1`. `ra_core_orbit_into()` (dipakai
`pip-ra-core`) terdampak lebih parah: slot terakhir buffer output (`out[rng-1]`)
sama sekali tidak ditulis (data lama/uninisialisasi bocor ke output).

Bug ditemukan sebagai Temuan #2 di
`experiments/2026-9-11_state-size-pareto-frontier/RESULTS.md` §9 dan
di-workaround di tooling saat itu (`tools/battery/practrand_gate.py`, +1
word margin untuk lane `continuous`) tanpa menyentuh source produksi.

## Fix yang diterapkan

Ganti sinyal terminasi dari "re-test nilai `count`" (ambigu) menjadi flag
eksplisit `bool` yang ditentukan tepat di titik asal ambiguitas — **tidak
mengubah aritmatika decrement/`count` sama sekali**:

```c
// src/ra_core/c/ra_core.c
static bool ra_permutation_cycle_orbit(uint32_t cons, size_t it,
                                        const uint32_t *M, uint32_t *L,
                                        uint64_t *count, FILE *raw_stream) {
    ...
    for (uint32_t i = 255; i > 0; --i) {
        ...
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

Tambahan kecil di luar rencana handover: `#include <stdbool.h>` ditambahkan
ke header includes `ra_core.c` — `bool`/`true`/`false` belum ter-include
transitif sebelumnya (handover sudah mengantisipasi kemungkinan ini di
catatan implementasi §3).

`src/ra_core/c/ra_core_engine.c` **tidak diedit** — `#include "ra_core.c"`
verbatim, signature baru otomatis konsisten; `ra_core_engine_fill_block()`
mengabaikan return value `bool` (selalu Kasus B by construction via
`count=UINT64_MAX`). Dikompilasi ulang standalone (`gcc -O2 -std=gnu17 -c
ra_core_engine.c`), 0 warning baru.

## Bukti verifikasi

1. **KAT self-test** (`./ra_core validate`): `0 checksum mismatches` untuk
   singleblock maupun orbit, byte-identik dengan sebelum fix (KAT hanya
   menguji `n` 1..255, di luar zona bug — sesuai prediksi §2 handover).

2. **Exhaustive word-count check** (`exhaustive_word_count_check.py`, rng
   1..3000):
   - **Sebelum fix**: 11 mismatch, persis di `rng ∈ {256, 511, 766, 1021,
     1276, 1531, 1786, 2041, 2296, 2551, 2806}` — cocok 100% dengan zona bug
     yang diprediksi (`rng % 255 == 1, rng > 255`).
   - **Sesudah fix**: 0 mismatch di seluruh rentang.

3. **Regression byte-identity**: untuk `rng ∈ {1, 2, 100, 255, 257, 300,
   512, 600, 1000}` (2 key berbeda), output binary sebelum vs sesudah fix
   **byte-identik persis** — fix hanya menambah word yang sebelumnya
   hilang, tidak mengubah word yang sudah benar.

4. **Parity check terhadap `ra_core_engine`** (`parity_check_engine.c`):
   `ra_core_orbit_into()` (setelah fix) dibandingkan word-demi-word dengan
   `ra_core_engine_next()` untuk `rng ∈ {256, 511, 766, 1021}` (zona bug)
   plus kontrol `{255, 300}` — **semua match**, membuktikan empiris bahwa
   keduanya sekarang menghasilkan stream yang identik seperti seharusnya
   (sesuai dugaan §2 handover berdasarkan konstruksi `count=UINT64_MAX`
   engine).
   - Catatan sampingan (bukan bug baru, ditemukan saat menulis harness):
     `ra_core_orbit_into()` mengharuskan buffer `out` punya minimal 1 byte
     ekstra setelah `rng` word (`fmemopen(out, rng*4+1, "wb")` — trailing
     NUL glibc). `pip-ra-core`'s ctypes binding (`ra_core/__init__.py:56`)
     mengalokasikan buffer persis `rng*4` byte tanpa slack. Ini kuirk
     pre-existing yang TIDAK disentuh handover ini (di luar scope), dicatat
     di sini murni supaya tidak hilang sebagai temuan sampingan.

5. **`tools/battery/practrand_gate.py`** (+1 word margin untuk lane
   `continuous`, `run_one()` baris ~154-174): margin ini **sekarang
   mubazir** (redundant) — pasca fix, `ra_core_orbit`/`--single` sudah
   selalu menulis word penuh sesuai `rng` yang diminta, jadi tidak ada lagi
   kebutuhan meminta 1 word ekstra untuk menghindari EOF prematur PractRand.
   **Kode ini TIDAK dihapus** dalam tugas ini (di luar scope eksplisit
   handover §4 langkah 5) — tetap aman dipertahankan karena hanya membuat
   `--single` meminta 1 word lebih banyak dari perlu, tidak salah secara
   fungsional. Sesi mendatang boleh menghapusnya sebagai pembersihan utang
   teknis kalau relevan.

6. **Konsumen lain**: grep repo untuk `ra_core_orbit`/`ra_core_orbit_into`
   menunjukkan hanya 3 konsumen langsung dari `src/ra_core/c/ra_core.c`
   produksi: CLI `ra_core.c`'s `main()` sendiri, `ra_core_engine.c`
   (`pip-ra-core`), dan `tools/battery/practrand_gate.py` (poin 5 di atas).
   Seluruh `experiments/*.c` lain yang menyebut `ra_core_orbit` adalah
   **salinan lokal beku** dari sesi-sesi riset sebelumnya (masing-masing
   `#include` file `ra_core.c`/`ra_core_param.c`/varian sendiri di folder
   eksperimennya, bukan file produksi) — di luar scope fix ini, sama
   seperti `ra_core_param.c` (studi 2026-9-11, sudah final) yang dicatat
   eksplisit di §8 handover sebagai "mewarisi bug yang sama, tidak perlu
   diedit ulang". Tidak ada regenerasi retroaktif dilakukan.

## Peringatan breaking change (dicatat eksplisit, sesuai §5 handover)

Fix ini **mengubah output** `ra_core_orbit()`/`ra_core_orbit_into()` untuk
setiap `rng` di zona `rng ≡ 1 (mod 255), rng > 255` (mis. 256, 511, 766,
...): sebelumnya `rng-1` word (dengan slot buffer `_into` terakhir berisi
data tak terdefinisi), sesudahnya `rng` word penuh dan benar, plus `cons`
akhir yang berbeda (sekarang benar-benar melewati 1 `ra_reseed` tambahan
yang sebelumnya terlewat). **Ini BUKAN regresi** — ini korupsi/kekurangan
data yang sudah ada sejak awal, sekarang diperbaiki. KAT **tidak
terpengaruh** (hanya menguji `n` 1..255, dikonfirmasi byte-identik di poin
1 di atas). Siapa pun yang sudah men-generate & MENYIMPAN stream dengan
`rng` di zona itu sebelum fix ini akan dapat hasil berbeda kalau
di-generate ulang.

## File yang disentuh

- **Diedit**: `src/ra_core/c/ra_core.c` (`ra_permutation_cycle_orbit`,
  `ra_core_orbit`, plus tambahan `#include <stdbool.h>`).
- **Tidak diedit, dikompilasi ulang untuk verifikasi**: `src/ra_core/c/ra_core_engine.c`.
- **Baru**: `exhaustive_word_count_check.py`, `parity_check_engine.c` (di
  folder eksperimen ini).
- **Dievaluasi, tidak diedit**: `tools/battery/practrand_gate.py`.
