# tools/battery/

Satu tool generik untuk memvalidasi kandidat edit ke `ra_core_orbit` /
`ra_core_singleblock`, tanpa perlu menulis ulang `main()`/argv parsing dan
script `tier0_avalanche_*.py` + `tier1_gate_*.py` tiap kali ada eksperimen
baru (lihat pola lama di `experiments/2026-9-{4,5}_orbit-*/`).

Cakupan: Tier 0 (avalanche gate) + Tier 1 (PractRand dual-ordering, K=1,
K=255, dan continuous). Dieharder battery / collision-scan **tidak**
digeneralisasi di sini — tetap pakai script lama
(`experiments/2026-9-4_orbit-kmin-battery/run_dieharder_battery.py`,
`collision_scan_orbit_k1.py`) kalau perlu lapisan tambahan setelah lolos
Tier 1.

## Kontrak yang harus dipenuhi kandidat

**2026-09-06: kontrak berubah breaking** — semua signature dan mode CLI di
bawah sekarang punya parameter kedua `addr_hi` (32-bit, independen dari
`key`), mengikuti widening `ra_core_orbit`/`ra_core_singleblock` (Kandidat D,
lihat `experiments/2026-9-6_orbit-address-widen-64bit-promotion/RESULTS.md`).
Ini aman untuk kandidat historis yang sudah beku di `experiments/` (mereka
tidak pernah di-rebuild ulang lewat template ini) — hanya kandidat BARU yang
kena kontrak baru.

1. Fungsi inti kandidat harus bersignature persis:
   ```c
   uint32_t your_core_fn(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream);
   ```
   sama seperti `ra_core_orbit`/`ra_core_singleblock` di
   `src/ra_core/c/ra_core.c` (atau salinan riset di
   `experiments/2026-9-1_family-productionization/ra_core.c:254,360`) —
   tulis `rng` word 32-bit ke `raw_stream` via `fwrite`. **`addr_hi` harus
   divariasikan independen dari `key`, bukan dibiarkan konstan** — `addr_hi`
   konstan collapse balik ke domain 32-bit-key-saja dan tidak mengaktifkan
   fix birthday-paradox yang menutup gap K=1+hashed-key (lihat
   `experiments/2026-9-5_orbit-address-widen-64bit/RESULTS.md`).

2. Binary kandidat harus mengimplementasikan CLI berikut (didapat gratis
   dari `driver_main_template.c`, jangan tulis ulang manual):
   - `--single <key> <addr_hi> <rng>` — **continuous**: satu pasangan
     (key, addr_hi), tanpa reinit periodik, sepanjang `rng` word. Ini yang
     dimaksud "K=N continuous". Hanya valid untuk core tanpa batas rng
     (orbit). `ra_core_singleblock` hard-abort di atas rng=255 — jangan
     pakai mode ini untuk singleblock.
   - `--stream <base_key> <base_addr_hi> <n> <K>` — multistream Weyl-ordered,
     reinit tiap `K` word (`K` di [1,255]), `key = base_key +
     block_idx*0x9E3779B9`, `addr_hi = base_addr_hi + block_idx*0x85EBCA6B`
     — delta independen dari delta key, jadi keduanya tidak pernah bergerak
     lock-step.
   - `--stream-hashed <base_key> <base_addr_hi> <n> <K>` — sama, tapi `key`
     DAN `addr_hi` masing-masing dari chain `splitmix32` independen sendiri
     (bukan langkah Weyl linear). **Ini axis yang selama ini membongkar
     defect K=1** (lihat
     `experiments/2026-9-4_orbit-kmin-battery/ADDENDUM_HASHED_KEY_GAP.md`)
     — jangan pernah cuma uji `--stream` biasa dan menyimpulkan "bersih".

## Cara pakai untuk kandidat baru

```c
// experiments/2026-9-X_nama-eksperimen/candidates/my_fix.c
#include <stdio.h>
#include <stdint.h>
// ... salin/edit ra_init_state_orbit, ra_permutation_cycle_orbit, dst,
//     dari ra_core.c sesuai perubahan yang mau dicoba ...

static uint32_t my_core_fn(uint32_t key, uint32_t addr_hi, size_t rng, FILE *raw_stream) {
    // ... panggil fungsi-fungsi di atas, addr_hi divariasikan independen ...
}

#define CORE_FN my_core_fn
#include "../../../tools/battery/driver_main_template.c"
```

Compile:
```sh
gcc -O3 -march=native -std=gnu17 -include stdalign.h my_fix.c -o my_fix
```

Jalankan satu perintah:
```sh
python ../../tools/battery/run_battery.py --binary ./my_fix --core orbit \
    --k 1,255 --continuous
```

Untuk iterasi cepat saat masih coba-coba (stage size lebih kecil, bukan
untuk keputusan final):
```sh
python ../../tools/battery/run_battery.py --binary ./my_fix --core orbit --quick
```

Untuk menguji kandidat singleblock (continuous otomatis di-skip dengan
warning, karena `rng<=255` adalah batas keras):
```sh
python ../../tools/battery/run_battery.py --binary ./my_fix_singleblock \
    --core singleblock --k 1,255
```

## Mode dual-core: edit sekali, tes dua core sekaligus (direkomendasikan)

`ra_permutation_cycle_orbit` dan `ra_permutation_cycle_singleblock` di
`ra_core.c` **identik byte-demi-byte** kecuali satu hal: orbit melakukan
swap `L[i]<->L[d]` setelah tiap word, singleblock melewatinya. `L[]`
write-only dalam satu block (cuma dibaca `ra_reseed()` di block berikutnya),
jadi swap itu TIDAK memengaruhi output block yang sama — inilah kenapa
`ORBIT_KAT_CHECKSUMS == SINGLEBLOCK_KAT_CHECKSUMS` di `ra_core.c` untuk
semua `rng` di [1,255].

Konsekuensinya: kamu cukup tulis **satu** fungsi init + **satu** fungsi
transform, parameter-kan lewat `L` yang nullable (`NULL` = singleblock,
non-NULL = orbit), dan `driver_main_template.c` menyediakan wrapper
loop-vs-single-shot + reseed-vs-no-reseed-nya secara otomatis — tidak ada
lagi risiko lupa mengedit salah satu dari dua fungsi terpisah.

```c
// candidates/my_fix.c
#include <stdio.h>
#include <stdint.h>

static void my_init(uint32_t *L /* NULL = singleblock */, uint32_t *M, uint32_t key, uint32_t addr_hi) {
    // ... hitung M[i] untuk semua i (pakai key DAN addr_hi); kalau L != NULL, hitung juga L[i] ...
}

static void my_transform(uint32_t cons, size_t it, const uint32_t *M,
                          uint32_t *L /* NULL = singleblock */,
                          uint64_t *count, FILE *raw_stream) {
    // ... a/b/c/d/o computation, fwrite(&c, ...) ...
    // if (L) { swap L[i]/L[d] -- orbit only }
}

#define CORE_INIT my_init
#define CORE_TRANSFORM my_transform
#include "../../../tools/battery/driver_main_template.c"
```

Baseline siap-edit (salinan byte-identik dari `ra_core.c`, sudah
diverifikasi KAT-parity) ada di
`experiments/2026-9-5_orbit-singleblock-shared-baseline/candidates/baseline.c`
— mulai dari situ.

Jalankan satu perintah untuk kedua core sekaligus:
```sh
python ../../../tools/battery/run_battery.py --binary ./my_fix --dual-core \
    --k 1,255 --continuous --quick
```
Output-nya satu tabel `(core, lane) -> CLEAN/FAIL`. Satu core FAIL **tidak**
menghentikan pengujian core yang lain — tujuannya melihat semua kombinasi
yang gagal dalam satu run, bukan berhenti di kegagalan pertama.
`--continuous` otomatis di-skip untuk singleblock (hard cap `rng<=255`),
seperti mode single-core biasa.

`rot32`/`ra_hash`/`ra_reseed` disediakan kanonik oleh template (verbatim
dari `ra_core.c`) — kamu tidak perlu mendefinisikannya sendiri kecuali
memang sengaja mau eksperimen di situ juga (override lewat
`#define ROT32_FN`/`RA_HASH_FN`/`RA_RESEED_FN` sebelum `#include`).

## Menguji core produksi (ra_core.c) tanpa mengubahnya

```sh
cd experiments/2026-9-1_family-productionization  # atau src/ra_core/c/ untuk salinan promosi resmi
gcc -O3 -march=native -std=gnu17 -include stdalign.h -Dmain=ra_core_unused_main \
    -c ra_core.c -o ra_core_nomain.o
cat > /tmp/verify_orbit.c <<'EOF'
extern uint32_t ra_core_orbit(uint32_t key, uint32_t addr_hi, unsigned long rng, void *raw_stream);
#define CORE_FN ra_core_orbit
#include "../../tools/battery/driver_main_template.c"
EOF
gcc -O3 -march=native -std=gnu17 -include stdalign.h /tmp/verify_orbit.c \
    ra_core_nomain.o -o /tmp/verify_orbit
python ../../tools/battery/run_battery.py --binary /tmp/verify_orbit --core orbit \
    --k 1,255 --continuous
```

**Sebelum mempercayai hasil statistik apa pun**, verifikasi dulu bahwa
binary lewat `driver_main_template.c` menghasilkan output byte-identik
dengan `ra_core.c` yang tidak diubah (mis. bandingkan beberapa word dari
`--single`/`--stream` terhadap `./ra_core validate`'s KAT checksum, atau
`./ra_core --stream <core> <key> <n>` langsung) —
lihat `feedback_verify_external_baselines` di memory: baseline eksternal
(di sini, driver template) harus diverifikasi terhadap sumber otoritatif
(`ra_core.c`) sebelum dipercaya, bukan cuma konsisten dengan dirinya
sendiri.

## Yang TIDAK dicakup tool ini

- Dieharder battery, collision-scan — masih script terpisah lama.
- Continuous mode untuk singleblock — tidak ada secara matematis (hard
  cap rng<=255).
- Verifikasi bahwa kandidat C anda benar secara aljabar/logika — tool ini
  cuma menjalankan statistik pada apa pun output binary anda; kesalahan
  logika di luar cakupan avalanche+PractRand tidak otomatis tertangkap.
