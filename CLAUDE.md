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
