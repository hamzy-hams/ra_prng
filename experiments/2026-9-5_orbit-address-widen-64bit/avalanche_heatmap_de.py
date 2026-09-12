"""experiments/2026-9-5_orbit-address-widen-64bit -- heatmap avalanche
32x32 (bit input x bit output) untuk kandidat D & E, mode orbit K=1
(mode paling kritis -- satu word output langsung dari init segar).

Beda dari avalanche_3way_addr_hi.py: skrip lama cuma menyimpan rata-rata
per-bit-input yang SUDAH diagregasi ke satu angka (mean Hamming fraction
ke SELURUH output word) -- cukup untuk gate PASS/FAIL, tidak cukup untuk
divisualisasikan sebagai heatmap bit-vs-bit. Di sini per (bit_input,
bit_output) pair disimpan terpisah, dirata-ratakan lewat banyak trial
(base_key/base_addr_hi acak) supaya heatmap K=1 (1 word/trial) tetap
halus statistiknya.
"""

from __future__ import annotations

import json
import random
import struct
import subprocess
from pathlib import Path

HERE = Path(__file__).parent
CANDIDATES_DIR = HERE / "candidates"

CANDIDATES = ["candidate_d", "candidate_e"]
DIRECTIONS = ["key_only", "addr_hi_only", "combined"]
N_TRIALS = 200
N_WORDS = 1  # orbit K=1


def capture_word(binary: Path, key: int, addr_hi: int) -> int:
    out = subprocess.run(
        [str(binary), "--stream", "orbit",
         str(key & 0xFFFFFFFF), str(addr_hi & 0xFFFFFFFF), str(N_WORDS)],
        stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True,
    ).stdout
    assert len(out) == 4
    return struct.unpack("<I", out)[0]


def build_heatmap(binary: Path, direction: str, rng: random.Random) -> list[list[float]]:
    counts = [[0] * 32 for _ in range(32)]
    for _ in range(N_TRIALS):
        base_key = rng.getrandbits(32)
        base_addr = rng.getrandbits(32)
        baseline = capture_word(binary, base_key, base_addr)
        for bit in range(32):
            mask = 1 << bit
            if direction == "key_only":
                key2, addr2 = base_key ^ mask, base_addr
            elif direction == "addr_hi_only":
                key2, addr2 = base_key, base_addr ^ mask
            elif direction == "combined":
                key2, addr2 = base_key ^ mask, base_addr ^ mask
            else:
                raise ValueError(direction)
            mutated = capture_word(binary, key2, addr2)
            diff = baseline ^ mutated
            for out_bit in range(32):
                if diff & (1 << out_bit):
                    counts[bit][out_bit] += 1
    return [[c / N_TRIALS for c in row] for row in counts]


def main():
    rng = random.Random(20260906)
    result = {}
    for cand_name in CANDIDATES:
        binary = CANDIDATES_DIR / cand_name
        assert binary.exists(), f"{binary} not found"
        result[cand_name] = {}
        for direction in DIRECTIONS:
            print(f"=== {cand_name} | orbit K=1 | arah={direction} ===", flush=True)
            heatmap = build_heatmap(binary, direction, rng)
            result[cand_name][direction] = heatmap
            flat = [v for row in heatmap for v in row]
            mean = sum(flat) / len(flat)
            mn = min(flat)
            mx = max(flat)
            print(f"  mean={mean:.4f} min={mn:.4f} max={mx:.4f}")

    out_path = HERE / "avalanche_heatmap_de.json"
    out_path.write_text(json.dumps(result, indent=1))
    print(f"\nSaved: {out_path}")


if __name__ == "__main__":
    main()
