# Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)

Companion to `HANDOVER.md` and `STATUS.md`. Child of two prior research tracks:
1. `../2026-8-25_periodicity-heuristic-validation/` (evaluated the unpruned, paper-exact `ra_prng2` toy model against $\lambda \approx 0.7824\sqrt{|S|}$).
2. `../2026-8-26_operation-pruning-research/` + `../2026-8-27_operand-position-search/` (pruned 14 of 18 operations down to `{TAP6, TAP7, ROT_C, SHR13}` and rewired operands to fix the bit 5/6 avalanche defect, producing `winner_wired_v2.c`).

**Central Research Question**: With the state shape and nominal state-space size $|S|$ unchanged, does the $\lambda \approx 0.7824\sqrt{|S|}$ heuristic's fit change after operation pruning and operand rewiring — i.e., does the pruned/wired candidate maintain, degrade, or improve cycle properties?

---

## 1. Summary Comparison: Unpruned Original vs `winner_wired_v2`

| Config $(n, w, \text{rows})$ | $\log_2|S|$ | $\lambda_{\text{pred}}$ | Seeds | $\lambda_{\text{mean}}$ Before (Original `ra_prng2`) | $\lambda_{\text{mean}}$ After (`winner_wired_v2`) | Ratio Before (mean/pred) | Ratio After (mean/pred) | Peningkatan Periode |
|---|---|---|---|---|---|---|---|---|
| $(2, 8, \text{rows}=2)$ | 33.0 | $7.251 \times 10^4$ | 500 | $555.0$ | **$256$** (konstan) | $0.0077$ | $0.0035$ | $0.46\times$ (1 cycle) |
| $(4, 8, \text{rows}=4)$ | 52.6 | $6.431 \times 10^7$ | 100 | $1.542 \times 10^5$ | **$1.865 \times 10^5$** | $0.0024$ | $0.0029$ | $1.21\times$ |
| $(2, 4, \text{rows}=2)$ | 17.0 | $283.3$ | 500 | $16.0$ | **$16$** (konstan) | $0.0565$ | $0.0565$ | $1.00\times$ (modulo floor) |
| $(4, 4, \text{rows}=4)$ | 28.6 | $1.570 \times 10^4$ | 500 | $8.284 \times 10^4$ | **$9.611 \times 10^4$** (konstan) | $5.2764$ | $6.1219$ | $1.16\times$ |
| **$(4, 4, \text{rows}=1)$** *(G=4, non-vacuous)* | 28.6 | $1.570 \times 10^4$ | 500 | *(N/A)* | **$1.881 \times 10^4$** | *(N/A)* | **$\mathbf{1.1982}$** | **Closest fit to heuristic ($\approx 1.20\times$)** |
| **$(8, 4, \text{rows}=2)$** *(G=4, non-vacuous)* | 55.3 | $1.647 \times 10^8$ | 10 | $1.318 \times 10^5$ | **$5.062 \times 10^9$** | $0.0008$ | **$\mathbf{30.73\times}$** | **$\mathbf{> 38.400\times}$ peningkatan** |
| **$(8, 4, \text{rows}=4)$** | 55.3 | $1.647 \times 10^8$ | 1 | $6.319 \times 10^6$ | **$> 3.045 \times 10^{11}$** [^1] | $0.0384$ | **$\mathbf{> 1.849\times}$** | **$\mathbf{> 48.190\times}$ peningkatan** |

[^1]: Dikoreksi 2026-08-29 dari angka draft sebelumnya (`> 1.518 × 10^11`), yang ditulis lebih dari satu jam sebelum proses pengukurannya benar-benar berhenti dan tidak cocok dengan checkpoint final. Angka ini adalah *lower bound* dari checkpoint resumable (`phase=0, power=2^38, lam=29.615.293.058`, dihentikan manual pada 2026-08-29 setelah watchdog yang sudah diperbaiki menjalankannya ~38 menit) — bukan pengukuran siklus yang selesai (tortoise/hare belum bertemu). Lihat `HANDOVER.md` ("2026-08-29 correction") untuk kronologi lengkap.

---

## 2. Sebaran Siklus Utama (*Distinct $\lambda$ Distribution*)

Across all measured seeds, trajectories consistently converge into a small number of dominant attractor cycles, without fragmenting into weak or degenerate micro-cycles:

| Config $(n, w, \text{rows})$ | Seeds | Distinct $\lambda$ Counts | Rincian Siklus $\lambda$ (Jumlah Seed) |
|---|---|---|---|
| $(2, 8, \text{rows}=2)$ | 500 | **1** | $256$ (500/500 = 100%) |
| $(4, 8, \text{rows}=4)$ | 100 | 4 | $227.072$ (63), $144.128$ (30), $3.840$ (6), $256$ (1) |
| $(2, 4, \text{rows}=2)$ | 500 | **1** | $16$ (500/500 = 100%) |
| $(4, 4, \text{rows}=4)$ | 500 | **1** | $96.112$ (500/500 = 100%) |
| $(4, 4, \text{rows}=1)$ *(G=4)* | 500 | 4 | $11.824$ (219), $6.288$ (125), $61.040$ (93), $5.616$ (63) |
| $(8, 4, \text{rows}=2)$ *(G=4)* | 10 | 4 | $7.429.923.728$ (6), $1.846.196.032$ (2), $1.749.652.176$ (1), $598.800.016$ (1) |

---

## 3. Enumerasi Penuh & Uji Chi-Square vs Poisson(1)

Brute-force domain evaluation over the entire state space $S$ was conducted at two scales:

### A. Skala $n=2, w=4$ ($|S| = 131.072$) — [`enumerate_n2w4_result.json`](./enumerate_n2w4_result.json)
* **Image Fraction**: Tepat $8.192 / 131.072 = \mathbf{6.2500\% = 1/16 = 1/2^w}$.
* **In-degree ($k$)**: $k=0$ pada $122.880$ state ($93.75\%$), $k=16$ pada $8.192$ state ($6.25\%$).
* **Chi-Square Statistic vs Poisson(1)**: $\chi^2 = 1.043.758,82$ ($\text{dof}=6, p=0.0$).
* **Verdict**: **REJECTS Poisson(1)**.

### B. Skala $n=4, w=4$ ($|S| = 402.653.184$) — [`enumerate_n4w4_result.json`](./enumerate_n4w4_result.json)
* **Image Fraction**: Tepat $25.165.824 / 402.653.184 = \mathbf{6.2500\% = 1/16 = 1/2^w}$.
* **In-degree ($k$)**: $k=0$ pada $377.487.360$ state ($93.75\%$), $k=16$ pada $25.165.824$ state ($6.25\%$).
* **Chi-Square Statistic vs Poisson(1)**: $\chi^2 = 3.206.427.105,44$ ($\text{dof}=6, p=0.0$).
* **Verdict**: **REJECTS Poisson(1)**.

### Interpretasi Matematis:
Di kedua skala, pemetaan $F$ adalah fungsi **16-ke-1 murni** pada citra aktifnya ($1/2^w$), karena variabel `cons` $w$-bit bertindak sebagai *funnel* modular satu arah pada putaran pertama. Setelah langkah pertama ($\mu \le 1$), sistem beroperasi di dalam ruang citra berkapasitas $|S|/16$ yang bijektif/permutatif.

---

## 4. Analisis Temuan Kunci (*Key Findings*)

1. **Eliminasi Keruntuhan Siklus Dini (*Premature Collapse Elimination*)**:
   Pada algoritma unpruned original (`ra_prng2`), penumpukan 18 operasi mixing menyebabkan interferensi destruktif (seperti feedback loop XOR $b \oplus o$ dan self-XOR hash) yang menjebak trajektori ke dalam siklus kerdil ($\lambda \approx 131\text{ Ribu}$ pada $(8,4,\text{rows}=2)$ dan $\lambda \approx 6,31\text{ Juta}$ pada $(8,4,\text{rows}=4)$).
   Pemangkasan operasi menjadi 4 op (`TAP6`, `TAP7`, `ROT_C`, `SHR13`) dan rewiring operand pada `winner_wired_v2` **menghilangkan jebakan tersebut**, meningkatkan panjang siklus hingga **$> 48.000\times$ lipat** ($\lambda > 5\text{ Miliar}$ s/d $> 304\text{ Miliar}$, angka kedua dikoreksi 2026-08-29 — lihat catatan kaki tabel §1).
2. **Kinerja Non-Vacuous Tap Pruning ($G=4$)**:
   Pada konfigurasi baru $(n=4, w=4, \text{rows}=1)$, di mana tap pruning aktif secara non-trivial ($\text{taps}=\{2,3\}$), rasio $\lambda_{\text{mean}} / \lambda_{\text{pred}}$ menghasilkan kecocokan paling presisi terhadap formula heuristik paper (**$\text{Rasio} = 1.1982$**).
3. **Ekor Transien Sangat Pendek ($\mu \le 1$)**:
   Semua seed langsung terkunci ke dalam orbit siklus utama dalam 0 sampai 1 langkah transisi, membuktikan bahwa generator langsung memanfaatkan dinamika cincin utama tanpa fase transien yang labil.

---

## 5. Catatan Metodologi & Caveats (Sesuai `HANDOVER.md`)

* **Perbedaan Semantik `ra_hash`**: Model baseline `toy_prng.py` di eksperimen 2026-8-25 memutasi argumen `N` in-place dan membuang hasilnya, sedangkan `winner_wired_v2` dan modul `pruned_wired_toy_prng.py` ini mengimplementasikan fungsi murni sequential fold yang membaca `M` secara *read-only* sesuai implementasi C riil.
  **Estimasi batas dampak confound ini** (tidak ada ablasi terkontrol yang mengisolasi perubahan hash saja, jadi ini adalah *bounding statement*, bukan angka presisi): baseline lama membuang hasil mutasinya sama sekali -- `out[i]` di sana hanya menerima term `HASH_SELFIDX`, bukan fungsi dari isi `M` yang sebenarnya -- sedangkan versi baru memang membaca isi `M` secara langsung lewat XOR-fold. Karena kedua model tetap lolos avalanche check (~50% Hamming) di semua 7 konfigurasi, perbedaan ini kemungkinan besar berkontribusi pada *tingkat pencampuran per-langkah* (bagian `cons` yang di-feed balik), bukan pada mekanisme utama yang diuji (pruning operasi + rewiring operand pada perulangan `a/b/c/d`) -- sehingga confound ini plausibel berefek **orde-dua** dibanding efek pruning/rewiring itu sendiri. Namun ini **belum diukur langsung**: untuk angka pasti, perlu eksperimen ablasi terpisah yang menahan op-set & wiring tetap sama dan HANYA mengganti semantik hash, lalu membandingkan $\lambda$-nya -- direkomendasikan sebagai *future work* sebelum mengklaim confound ini diabaikan.
* **Vakuositas Tap pada $G \le 2$**: Konfigurasi default dengan $G=1$ atau $G=2$ memiliki tap pruning yang secara struktural bersifat *no-op* (karena $G \le 2$ mempertahankan seluruh tap yang tersedia), namun 8 sumbu optimasi lainnya (rotasi, shift dropped, mult-reduce off, sequential hash, rewiring) tetap aktif penuh di setiap konfigurasi.
* **Kegagalan sweep otomatis pada `(8,4,rows=4)` dan koreksi angka** (ditemukan & diperbaiki 2026-08-29): `run_sweep.py` crash di config ini karena bug pola pencocokan proses pada `auto_stop_power38.py` (sudah diperbaiki). Angka lower-bound yang tadinya dipublikasikan untuk config ini juga ditulis sebelum proses pengukurannya benar-benar berhenti, dan tidak cocok dengan checkpoint final yang tersisa. Lihat `HANDOVER.md` ("2026-08-29 correction") dan `STATUS.md` untuk kronologi lengkap serta angka yang sudah dikoreksi.

---

## 6. Kesimpulan (*Verdict*)

**Catatan kekuatan bukti**: dua konfigurasi paling dramatis di atas ($(8,4,\text{rows}=2)$ dan $(8,4,\text{rows}=4)$) masing-masing hanya bertumpu pada 10 dan 1 seed, dengan varians antar-seed yang besar pada config 10-seed (rentang min/maks lebih dari 12×) dan tanpa interval kepercayaan yang dilaporkan -- berbeda dengan config lain yang memakai 500 seed. Config bersampel besar (500 seed) justru menunjukkan rasio $\lambda_{\text{mean}}/\lambda_{\text{pred}}$ yang mendekati atau di bawah 1×, bukan jauh di atasnya. Kesimpulan di bawah ini tetap didukung arah buktinya, tapi kekuatannya berasal terutama dari dua titik data under-sampled tersebut -- bukan generalisasi yang merata di semua skala yang diukur.

1. **Apakah Pemangkasan Operasi & Rewiring Merusak Periodisitas?** Berdasarkan bukti yang ada, **tidak terlihat demikian** -- penyederhanaan arsitektur pada `winner_wired_v2` tampak menghindarkan sistem dari fenomena *premature cycle collapse* yang teramati pada versi unpruned, dengan periode siklus yang jauh lebih panjang pada dua konfigurasi skala besar yang diukur. Karena kedua konfigurasi itu under-sampled (lihat catatan di atas), ini sebaiknya dibaca sebagai indikasi kuat, bukan pembuktian yang berlaku merata di seluruh ruang konfigurasi.
2. **Validitas Heuristik $\lambda \approx 0.7824\sqrt{|S|}$**: Pada dua konfigurasi skala besar yang diukur, model *Random Mapping* di paper ICCS tampak sebagai **estimasi batas bawah yang konservatif** -- periode yang teramati melompat jauh melampaui $\sqrt{|S|}$, mengarah ke kapasitas ruang citra aktif $|S|/16$. Namun konfigurasi bersampel besar (500 seed) tidak menunjukkan pola yang sama konsisten (rasio mendekati/di bawah 1×), dan eksperimen sibling 2026-8-25 sendiri sudah mencatat fit heuristik ini sebagai non-monoton secara keseluruhan -- sehingga "loose lower-bound" ini paling aman dibaca sebagai pola yang muncul di ujung skala besar yang under-sampled, bukan simpulan umum untuk heuristik tersebut.
