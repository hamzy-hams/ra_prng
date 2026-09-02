# Workflow riset repo ini

- Untuk setiap tugas riset/eksplorasi/validasi di repo ini (termasuk membaca
  atau memvalidasi laporan di `experiments/`), **selalu invoke skill
  `graphify` terlebih dahulu** sebelum melakukan pencarian manual
  (grep/find/baca file satu per satu). Jika `graphify-out/graph.json` sudah
  ada, pakai `/graphify query "..."` langsung; jika belum ada, bangun dulu
  dengan `/graphify`.
- Setelah riset/analisis selesai, **selalu jalankan `/graphify --update`**
  supaya file baru/berubah yang baru dibaca ikut masuk ke graph, sebelum
  menutup tugas.

# VPS untuk proses riset yang lama (PractRand skala besar, dll.)

Ada VPS yang bisa dipakai untuk run yang makan waktu berjam-jam (mis. uji
PractRand 1TB) supaya tidak tergantung laptop nyala terus. **Detail
akses (IP/user/key) sengaja TIDAK ditulis di sini** karena file ini publik
di GitHub -- lihat `VPS_ACCESS.md` di root repo (gitignored, lokal saja) atau
tanya user langsung kalau file itu tidak ada. Kalau sedang mengerjakan tugas
yang melibatkan run panjang (PractRand skala besar/interleaved dsb.), cek
`VPS_ACCESS.md` dulu untuk lihat apakah ada proses yang masih jalan di sana
sebelum mulai run baru.
