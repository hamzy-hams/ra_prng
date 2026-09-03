"""Generate the 6 extra-inject variants (i1-i6) for each of the two
Pareto-selection co-frontier fix candidates, w8_f10 (finalizer_idx=10) and
w8_f28 (finalizer_idx=28), per PRODUCTION_READINESS_HANDOVER.md action item 2
("Crossing dengan 7 varian extra-inject ... untuk kedua kandidat").

i0 (off) is already built and PractRand-tested in
../2026-9-2_singleblock-cycle-combo-search/candidates/. This script only
builds i1..i6 (12 new binaries total), reusing combo_gen.write_and_compile
verbatim (same codegen, same gcc flags, same byte-identical-to-production
guarantee) -- just redirected to write into ./candidates/ instead of the
2026-9-2 folder, so that folder stays a historical record.

recipes.py's all_promotion_candidates(width, finalizer_idx) is exactly the
generator meant for this crossing step -- see its docstring: "7 inject
variants for a single (width, finalizer_idx) Tier-1 survivor."
"""

from __future__ import annotations

import sys
from pathlib import Path

COMBO_SEARCH_DIR = Path(__file__).parent.parent / "2026-9-2_singleblock-cycle-combo-search"
sys.path.insert(0, str(COMBO_SEARCH_DIR))

import combo_gen  # noqa: E402
from recipes import all_promotion_candidates, candidate_id  # noqa: E402

HERE = Path(__file__).parent
CAND_DIR = HERE / "candidates"
CAND_DIR.mkdir(exist_ok=True)

# Redirect combo_gen's output dir to this experiment's own candidates/.
combo_gen.CAND_DIR = CAND_DIR

FINALIZER_IDX = {"w8_f10": 10, "w8_f28": 28}
WIDTH = 8


def main():
    combo_gen.verify_control_matches_production()
    built = []
    for name, fidx in FINALIZER_IDX.items():
        for width, finalizer_idx, inject_idx in all_promotion_candidates(WIDTH, fidx):
            if inject_idx == 0:
                continue  # i0 already built+tested in 2026-9-2 folder
            bin_path = combo_gen.write_and_compile(width, finalizer_idx, inject_idx)
            combo_gen.sanity_check(bin_path, width, finalizer_idx, inject_idx)
            cid = candidate_id(width, finalizer_idx, inject_idx)
            built.append(cid)
            print(f"  built+sanity-OK: {cid}")
    print(f"\nBuilt and sanity-checked {len(built)} inject-crossing candidates in {CAND_DIR}")
    assert len(built) == 12, f"expected 12, got {len(built)}"


if __name__ == "__main__":
    main()
