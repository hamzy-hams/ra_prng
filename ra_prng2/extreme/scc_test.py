#!/usr/bin/env python3

"""
randomness_tests_with_scc.py

Mengukur beberapa metrik kualitas keacakan pada data integer dalam 'scrambled.txt':
1. Shannon Entropy
2. Chi-Square Goodness-of-Fit (terhadap distribusi seragam)
3. Runs Test (Wald–Wolfowitz) untuk urutan naik/turun
4. Serial Correlation Coefficient (korelasi linier antar elemen berurutan)

Format file input ('scrambled.txt'):
    - Baris tunggal (atau beberapa baris) berisi angka-angka dipisah spasi, misalnya:
        487 2 91238 113 42 ...

Cara menjalankan:
    python3 randomness_tests_with_scc.py
"""

import math
from collections import Counter

def read_tokens(filename):
    """
    Membaca file teks yang berisi token (integer) dipisah spasi/newline,
    lalu mengembalikan list of ints.
    """
    try:
        with open(filename, 'r') as f:
            content = f.read().strip()
            if not content:
                return []
            tokens = list(map(int, content.split()))
            return tokens
    except FileNotFoundError:
        print(f"Error: File '{filename}' tidak ditemukan.")
        return []
    except ValueError:
        print(f"Error: Isi file '{filename}' mengandung token yang bukan integer.")
        return []

def entropy(data):
    """
    Menghitung Shannon Entropy (bit) dari distribusi nilai dalam 'data'.
    """
    n = len(data)
    if n == 0:
        return 0.0
    freq = Counter(data)
    ent = 0.0
    for count in freq.values():
        p = count / n
        ent -= p * math.log2(p)
    return ent

def chi_square_uniform(data):
    """
    Menghitung Chi-Square Statistic untuk menguji apakah frekuensi tiap nilai
    dalam 'data' cocok dengan distribusi seragam.
    """
    n = len(data)
    if n == 0:
        return 0.0
    distinct_values = len(set(data))
    expected = n / distinct_values
    freq = Counter(data)
    chi2 = sum((count - expected) ** 2 / expected for count in freq.values())
    return chi2

def runs_test(data):
    """
    Menghitung Runs Test (Wald–Wolfowitz) untuk urutan naik/turun berturut-turut.
    Mengembalikan tuple (z_statistic, actual_runs, expected_runs).
    """
    n = len(data)
    if n < 2:
        return 0.0, 0, 0.0

    # Hitung jumlah run; run naik/turun berganti ketika tanda (>) berubah
    runs = 1
    for i in range(1, n):
        # Jika perbandingan (data[i] > data[i-1]) berubah dibanding perbandingan sebelumnya,
        # itu menandakan awal run baru.
        prev_cmp = (data[i-1] > data[i-2]) if i >= 2 else None
        curr_cmp = (data[i] > data[i-1])
        if i == 1 or curr_cmp != prev_cmp:
            runs += 1

    # Formula ekspektasi dan variansi untuk runs test pada data numerik berurut:
    expected_runs = (2 * n - 1) / 3
    variance_runs = (16 * n - 29) / 90
    if variance_runs <= 0:
        z = 0.0
    else:
        z = (runs - expected_runs) / math.sqrt(variance_runs)
    return z, runs, expected_runs

def serial_correlation(data):
    """
    Menghitung Serial Correlation Coefficient (korelasi linier) antara data[i] dan data[i+1].
    Formula:
        r = [ Σ_{i=0..n-2} (x_i * x_{i+1}) - (n-1)*mean^2 ] 
            / [ Σ_{i=0..n-1} (x_i^2) - n*mean^2 ]
    """
    n = len(data)
    if n < 2:
        return 0.0

    mean = sum(data) / n
    sum_xi_xip1 = sum(data[i] * data[i+1] for i in range(n - 1))
    sum_xi_squared = sum(x ** 2 for x in data)

    numerator = sum_xi_xip1 - (n - 1) * (mean ** 2)
    denominator = sum_xi_squared - n * (mean ** 2)

    # Hindari pembagian nol
    if denominator == 0:
        return 0.0

    r = numerator / denominator
    return r

if __name__ == "__main__":
    filename = "scrambled.txt"
    data = read_tokens(filename)

    if not data:
        print("Tidak ada data untuk diuji (file kosong atau tidak valid).")
        exit(0)

    # 1. Hitung Entropy
    H = entropy(data)
    print(f"Entropy (bit): {H:.6f}")

    # 2. Hitung Chi-Square Statistic
    chi2 = chi_square_uniform(data)
    print(f"Chi-Square Statistic: {chi2:.6f}")

    # 3. Runs Test (Wald–Wolfowitz)
    z_stat, actual_runs, expected_runs = runs_test(data)
    print(f"Runs Test: z = {z_stat:.4f}, actual_runs = {actual_runs}, expected_runs ≈ {expected_runs:.2f}")

    # 4. Serial Correlation Coefficient
    r = serial_correlation(data)
    print(f"Serial Correlation Coefficient: {r:.6f}")
