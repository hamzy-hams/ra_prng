# ra\_prng2 and Beyond: A Novel Lightweight PRNG Architecture for High-Performance Random Data Generation

`ra_prng` adalah arsitektur baru untuk generator angka acak deterministik yang mengandalkan teknik *array index shuffling*, *bitwise rotation*, *bit shifting*, XORing, dan *nonlinear indexing*. Algoritma ini secara eksplisit didesain untuk efisiensi tinggi dan entropi besar, dengan struktur internal berbasis array yang memungkinkan periodisitas panjang tanpa mengorbankan performa.

Berbeda dari PRNG tradisional yang memisahkan pengacakan dan entropi, `ra_prng` menjadikan proses pengacakan array sebagai inti desain PRNG itu sendiri. Hal ini menjadikannya sangat cocok untuk aplikasi seperti *data shuffling* pada pelatihan model AI/ML, inisialisasi bobot neural network, simulasi deterministik, dan eksperimen statistik.

## Komponen Proyek

Repositori ini terdiri dari beberapa komponen utama:

* `ra_prng2/` — Versi 32-bit yang menjadi inti paper utama, stabil dan deterministik.
* `ra_prng3/` — Versi eksperimental 64-bit dengan state internal lebih besar, kecepatan lebih tinggi, dan periodisitas teoritis mencapai $2^{16352}$.
* `comparisons/` — Data benchmark dan evaluasi perbandingan dengan algoritma lain seperti xoshiro256\*\*, PCG32, Philox4x32, ChaCha20, dan dev/urandom.

## Kompilasi dan Penggunaan

Untuk kompilasi:

```bash
# Versi standar
cd ra_prng2/src
gcc -O3 -march=native ra_prng2.c -o ra_prng2

# Versi dengan dukungan OpenMP
gcc -O3 -march=native -fopenmp ra_prng_thread.c -o ra_prng_thread
```

Output default berupa bilangan acak atau hasil dari proses folding tergantung implementasi.

## Benchmarking dan Evaluasi

Semua hasil benchmark tersedia dalam folder `benchmark/` di setiap subfolder algoritma. Untuk fair comparison, pengujian dilakukan di environment yang sama menggunakan:

* CPU: Intel Core i3-1115G4
* OS: Arch Linux 6.8.7
* Compiler: GCC 13.2.0

Beberapa hasil ringkasan:

* `ra_prng2` mengungguli ChaCha20 dan dev/urandom dalam throughput
* `ra_prng3` bahkan melampaui Philox4x32 dalam kecepatan RNGing
* Evaluasi lengkap dalam `comparisons/benchmark_speed.md`

## Hasil Tes Statistik

* Lolos semua tes Dieharder dan NIST STS
* Tidak ada kegagalan fatal pada subtes
* Beberapa flag WEAK muncul, namun ini wajar dan juga ditemukan pada algoritma populer lain (Xoshiro, PCG, ChaCha20)

## Roadmap

* Peningkatan efisiensi dan kecepatan PRNG lebih lanjut
* Penyesuaian model agar fleksibel untuk LLM dataset shuffling
* Optimalisasi SIMD dan distribusi lintas platform
* Paper kedua akan diterbitkan untuk `ra_prng3`

## Lisensi

Proyek ini dilisensikan di bawah **Apache License Version 2.0** — [http://www.apache.org/licenses/](http://www.apache.org/licenses/)

## Sitasi

Kami sedang menyiapkan format sitasi resmi. Untuk sementara, silakan tautkan ke repositori GitHub ini dan sertakan nama proyek dan penulis utama.

## Kontribusi

Saat ini, kontribusi eksternal belum dibuka secara resmi. Namun feedback dan diskusi teknis sangat diterima.

---

> Untuk pertanyaan, saran, atau kolaborasi, silakan hubungi melalui halaman issue atau pull request setelah proyek dibuka untuk kontribusi publik.
