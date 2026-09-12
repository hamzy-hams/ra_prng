"""HANDOVER.md Tahap E (4.4): safe-K floor scan khusus N=16 (dipicu oleh
Tier1 K=1+weyl FAIL, lihat tier1_out/orbit_N16_LOWONLY). Generalisasi
metodologi ../2026-9-2_singleblock-k-threshold-characterization/ (fail-fast,
base_key tunggal 111222, weyl ordering -- itu yang menemukan K=1+weyl FAIL
di N=16, K=1+hashed CLEAN) ke N=16: K max cuma 15, jadi sapu SEMUA K in
[2,14] (K=1 sudah FAIL dari Tier1, K=15 sudah CLEAN ke 2GB dari Tier1),
bukan sampling seperti studi asli N=256.

Reuse practrand_gate.Lane/run_one langsung (bukan port ulang) -- satu
"stage" per K, tlmin=256M tlmax=2G (kedalaman sama dengan Tier1 supaya
banding-able), base_key=111222 tetap (konvensi studi asli demi komparasi),
fail-fast bawaan run_one (early-kill on hard FAIL) sudah cukup.

Reseed mode tidak relevan untuk scan ini: K<=N-1 berarti ra_reseed tidak
pernah terpanggil di satu block manapun (lihat catatan
project_singleblock_kmin_defect / KAT identity ORBIT==SINGLEBLOCK untuk
rng<=N-1) -- jadi orbit_N16_LOWONLY dipakai sebagai representasi baik utk
orbit MAUPUN singleblock di rentang K ini.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent.parent / "tools/battery"))
import practrand_gate  # noqa: E402

HERE = Path(__file__).parent
BINARY = HERE / "bin/battery_orbit_N16_LOWONLY"
PRACTRAND_BIN = practrand_gate.DEFAULT_PRACTRAND_BIN
LOGS_DIR = HERE / "floor_scan_n16"
BASE_KEY = 111222  # konvensi studi asli, demi komparabilitas
TLMIN, TLMAX = "256M", "2G"


def main():
    LOGS_DIR.mkdir(exist_ok=True)
    results = []
    for k in range(2, 15):  # K=1 sudah FAIL (Tier1), K=15 sudah CLEAN (Tier1)
        lane = practrand_gate.Lane(f"K{k}", k, ["weyl"])
        r = practrand_gate.run_one(
            f"K{k}", BINARY, lane, "weyl", BASE_KEY,
            PRACTRAND_BIN, TLMIN, TLMAX, "scan", LOGS_DIR,
        )
        results.append((k, r))

    print("\n=== RINGKASAN FLOOR SCAN N=16 (weyl, base_key=111222, up to 2G) ===")
    print("K=1  : FAIL (dari Tier1, escalating BCFN, lihat tier1_out/orbit_N16_LOWONLY)")
    for k, r in results:
        status = "CLEAN" if r["clean_to_target"] else "FAIL"
        print(f"K={k:<3}: {status:<5} @ {r['final_length']}")
    print("K=15 : CLEAN (dari Tier1, 2GB dual-base-key)")


if __name__ == "__main__":
    main()
