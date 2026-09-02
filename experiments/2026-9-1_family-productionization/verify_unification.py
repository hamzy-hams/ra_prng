#!/usr/bin/env python3
"""verify_unification.py -- Tahap 1 family-productionization.

Membuktikan ra_core.c's ra_core_orbit bit-identical terhadap
winner_wired_addressable.c's ra_core (yang lama), untuk membuktikan
unifikasi ini murni rename/merge, bukan perubahan formula. Tidak mengedit
winner_wired_addressable.c sama sekali (read-only precedent repo ini) --
cuma compile & jalankan --stream-nya, lalu bandingkan byte mentah dengan
ra_core.c --stream orbit.

CATATAN SCOPE (lihat RESULTS.md): winner_wired_v2.c's mode fixed-init
(non-addressable) SENGAJA TIDAK diuji di sini -- mode itu tidak dibawa ke
ra_core.c sama sekali (keputusan 2-mode: orbit + singleblock saja), jadi
tidak ada baseline lama untuk dibandingkan.
"""
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO_ROOT = HERE.parent.parent
OLD_SRC = REPO_ROOT / "experiments" / "2026-8-30_addressable-init-research" / "winner_wired_addressable.c"
OLD_BIN = HERE / "winner_wired_addressable_ref"
NEW_BIN = HERE / "ra_core"

CFLAGS = ["-O3", "-march=native", "-std=gnu17", "-include", "stdalign.h"]

KEYS = [0, 0xFFFFFFFF, 0xDEADBEEF, 0x12345678, 1, 2, 3, 42]
NS = [1, 2, 254, 255, 256, 257, 1000, 300000]  # covers <255, ==255, multi-cycle reseed


def compile_old():
    if not OLD_SRC.exists():
        sys.exit(f"ERROR: {OLD_SRC} not found")
    cmd = ["gcc", *CFLAGS, str(OLD_SRC), "-o", str(OLD_BIN), "-Wall", "-Wextra"]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit(f"ERROR compiling old reference:\n{r.stderr}")


def compile_new():
    if not NEW_BIN.exists():
        cmd = ["gcc", *CFLAGS, str(HERE / "ra_core.c"), "-o", str(NEW_BIN), "-Wall", "-Wextra"]
        r = subprocess.run(cmd, capture_output=True, text=True)
        if r.returncode != 0:
            sys.exit(f"ERROR compiling ra_core.c:\n{r.stderr}")


def run_stream_old(key, n):
    r = subprocess.run([str(OLD_BIN), "--stream", str(key), str(n)],
                        capture_output=True)
    return r.stdout


def run_stream_new(key, n):
    r = subprocess.run([str(NEW_BIN), "--stream", "orbit", str(key), str(n)],
                        capture_output=True)
    return r.stdout


def main():
    compile_old()
    compile_new()

    total = 0
    mismatches = 0
    for key in KEYS:
        for n in NS:
            total += 1
            old_out = run_stream_old(key, n)
            new_out = run_stream_new(key, n)
            if old_out != new_out:
                mismatches += 1
                print(f"MISMATCH key={key} n={n} "
                      f"(old {len(old_out)} bytes, new {len(new_out)} bytes)")

    print(f"verify_unification: {total} combinations checked "
          f"({len(KEYS)} keys x {len(NS)} lengths), {mismatches} mismatches")
    if mismatches == 0:
        print("verify_unification: PASS -- ra_core_orbit is bit-identical to "
              "winner_wired_addressable.c's ra_core for all tested (key, n).")
        return 0
    else:
        print("verify_unification: FAIL -- do not trust ra_core_orbit; "
              "unification introduced a formula change.")
        return 1


if __name__ == "__main__":
    sys.exit(main())
