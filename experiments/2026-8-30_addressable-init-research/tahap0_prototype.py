"""Tahap 0/1 (spec: HANDOVER.md) -- cheap, pure-Python mechanism test for
addressable state init. Never modifies winner_wired_v2.c/ra_prng2.c in place;
this is a faithful Python PORT of winner_wired_v2.c's core (rot32, ra_hash,
ra_permutation_cycle, ra_reseed, ra_core), with ra_init_state swapped out for
pluggable "address -> (L, M)" candidates. sanity_check() cross-validates the
port bit-for-bit against the real compiled binary before any candidate result
is trusted.

Naming (Tahap 1, 2026-08-30): the per-address parameter is called `key`
(Philox terminology -- it's the stream/address selector), not `seed`. The
free mixing constant in Kandidat 5's `L` formula is `L_MIX_CONST`, now frozen
to 0x9e3779b7 (Tahap 0 swept 4 values with no distinctness difference; this
is the canonical constant already used in the original non-addressable `L`
formula) -- it is NOT part of the key/counter address vocabulary, since it's
a fixed implementation constant, not an address dimension. `counter` is
reserved for a possible future second address dimension; Tahap 0/1 both test
Kandidat 5 with address = key only (confirmed by user).

Scale note: tiers here are deliberately much smaller than TIERS_Q2/TIERS_Q2_RANDOM
in ../2026-8-29_parallelization-research/common.py (that scale assumes a
compiled C binary; this is pure Python, ~1000x slower per word). Tahap 0's job
is only to cheaply prove/disprove the core hypothesis and flag obviously-bad
candidates before committing to a C implementation -- production-scale
collision/PractRand validation happens in Tahap 3 on the real C port, per
HANDOVER.md.
"""

from __future__ import annotations

import hashlib
import json
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / "2026-8-26_operation-pruning-research"))
from quality_gate import avalanche_stats  # noqa: E402

sys.path.insert(0, str(Path(__file__).parent.parent / "2026-8-29_parallelization-research"))
from common import WINNER_BIN, stream_values, random_seeds  # noqa: E402
from collision_scan import digest_collision_prob  # noqa: E402

HERE = Path(__file__).parent
MASK32 = 0xFFFFFFFF
MASK64 = 0xFFFFFFFFFFFFFFFF


# --- Faithful port of winner_wired_v2.c (lines 59-133) ----------------------

def rot32(n: int, r: int) -> int:
    r &= 31
    n &= MASK32
    return ((n << r) | (n >> (32 - r))) & MASK32


def ra_hash(N: list[int]) -> list[int]:
    out8 = [0] * 8
    for i in range(8):
        base = i * 32
        v = 0
        for j in range(32):
            v ^= N[base + j]
        out8[i] = v
    return out8


def ra_permutation_cycle(cons: int, it: int, M: list[int], L: list[int],
                          count: int, raw_out: list[int] | None) -> int:
    """Mutates L in place (swap-permute). M is read-only here (mutated
    separately by ra_reseed). Returns the updated `count`."""
    a = cons & MASK32
    b = it & MASK32
    d = 0
    for i in range(255, 0, -1):
        o = ((M[(i + 6) & 0xFF] << 6) ^ (M[(i + 7) & 0xFF] << 7)) & MASK32
        a = ((d ^ o) ^ ((cons + a) & MASK32)) & MASK32
        b = (((cons + a) & MASK32) ^ ((o + d) & MASK32)) & MASK32
        c = rot32((a >> 13) ^ a, b)
        if raw_out is not None:
            raw_out.append(c)
        d = c & 0xFF
        if count <= 1:
            return count
        count -= 1
        L[i], L[d] = L[d], L[i]
    return count


def ra_reseed(M: list[int], L: list[int]) -> int:
    for i in range(256):
        M[i] = (M[i] ^ L[i]) & MASK32
    tmp8 = ra_hash(M)
    new_cons = 0
    for e in range(8):
        new_cons ^= (tmp8[e] << e) & MASK32
    return new_cons & MASK32


def ra_core_from_state(key: int, rng: int, L: list[int], M: list[int],
                        raw_out: list[int] | None = None) -> int:
    """Like ra_core() in the C source, but takes an already-built (L, M)
    instead of calling ra_init_state -- this is the addressable-init hook.
    L and M are consumed (mutated) in place; pass fresh copies if reused.
    `key` seeds `cons` exactly like `seed` did in the original -- only the
    name changed (Tahap 1 naming decision), not the value or role."""
    if rng == 0:
        return key & MASK32
    count = rng
    iteration = rng // 255 + 1
    cons = key & MASK32
    for it in range(iteration):
        count = ra_permutation_cycle(cons, it, M, L, count, raw_out)
        if count <= 1:
            return cons
        cons = ra_reseed(M, L)
    return cons


# --- splitmix64 (standard constants, used by Kandidat 1/2) ------------------

def splitmix64_seed(x: int) -> int:
    return x & MASK64


def splitmix64_next(state: int) -> tuple[int, int]:
    state = (state + 0x9E3779B97F4A7C15) & MASK64
    z = state
    z = ((z ^ (z >> 30)) * 0xBF58476D1CE4E5B9) & MASK64
    z = ((z ^ (z >> 27)) * 0x94D049BB133111EB) & MASK64
    z = z ^ (z >> 31)
    return state, z


# --- Init candidates (address -> (L, M)) -------------------------------------
# All take (key, counter, **params) so the generic harness below can call
# every candidate the same way, even though some ignore some arguments.

def init_original(key: int, counter: int = 0, **_) -> tuple[list[int], list[int]]:
    """winner_wired_v2.c's ra_init_state(), untouched -- used only to
    cross-validate the port in sanity_check(), not an addressable candidate."""
    M = [(i * 0x06a0dd9b + 0x06a0dd9b) & MASK32 for i in range(256)]
    L = [(i * 0x9e3779b7 + 0x9e3779b7) & MASK32 for i in range(256)]
    return L, M


def init_kandidat1(key: int, counter: int = 0, **_) -> tuple[list[int], list[int]]:
    """HANDOVER.md Kandidat 1: only L keyed by (key, counter) via splitmix64;
    M kept byte-identical to the original (avalanche property untouched)."""
    k = splitmix64_seed(((key & MASK32) << 32) | (counter & MASK32))
    k, raw_mult = splitmix64_next(k)
    k, raw_off = splitmix64_next(k)
    L_mult = (raw_mult & MASK32) | 1
    L_off = (raw_off & MASK32) | 1
    L = [(i * L_mult + L_off) & MASK32 for i in range(256)]
    M = [(i * 0x06a0dd9b + 0x06a0dd9b) & MASK32 for i in range(256)]
    return L, M


def init_kandidat2(key: int, counter: int = 0, **_) -> tuple[list[int], list[int]]:
    """HANDOVER.md Kandidat 2: L and M independently keyed, domain-separated."""
    kL = splitmix64_seed(((key & MASK32) << 32) | (counter & MASK32))
    kM = splitmix64_seed(kL ^ 0x9e3779b97f4a7c15)
    kL, raw_Lmult = splitmix64_next(kL)
    kL, raw_Loff = splitmix64_next(kL)
    kM, raw_Mmult = splitmix64_next(kM)
    kM, raw_Moff = splitmix64_next(kM)
    L_mult = (raw_Lmult & MASK32) | 1
    L_off = (raw_Loff & MASK32) | 1
    M_mult = (raw_Mmult & MASK32) | 1
    M_off = (raw_Moff & MASK32) | 1
    L = [(i * L_mult + L_off) & MASK32 for i in range(256)]
    M = [(i * M_mult + M_off) & MASK32 for i in range(256)]
    return L, M


def init_kandidat3_control(key: int, counter: int = 0, **_) -> tuple[list[int], list[int]]:
    """HANDOVER.md Kandidat 3: negative control. L's MULTISET is unchanged
    (just reindexed by counter) -- this MUST fail the distinctness test; if it
    passes, the test methodology itself is too weak to trust for the other
    candidates. Ignores `key` on purpose (only `counter` is the address here)."""
    L = [0] * 256
    for i in range(256):
        j = (i + counter) & 0xFF
        L[j] = (i * 0x9e3779b7 + 0x9e3779b7) & MASK32
    M = [(i * 0x06a0dd9b + 0x06a0dd9b) & MASK32 for i in range(256)]
    return L, M


def init_kandidat4(key: int, counter: int = 0, **_) -> tuple[list[int], list[int]]:
    """HANDOVER.md Kandidat 4. M formula was left TBD in the spec ("ikut
    Kandidat 1 atau 2") -- resolved here as Kandidat 1's (M unchanged), a
    documented choice, not an unstated default."""
    product = (key * 0x2545F4914F6CDD1D) & MASK64
    L_mult = ((product >> 32) & MASK32) | 1
    L = [(i * L_mult + (counter & MASK32) * 0x9E3779B1 + 0x9e3779b7) & MASK32
         for i in range(256)]
    M = [(i * 0x06a0dd9b + 0x06a0dd9b) & MASK32 for i in range(256)]
    return L, M


# Tahap 1: frozen final value for Kandidat 5's mixing constant. Not part of
# the key/counter address vocabulary -- see module docstring.
L_MIX_CONST_DEFAULT = 0x9e3779b7


def init_kandidat5(key: int, counter: int = 0, L_MIX_CONST: int = L_MIX_CONST_DEFAULT,
                    **_) -> tuple[list[int], list[int]]:
    """User's formula from prior research (pasted into HANDOVER.md's Lampiran
    2026-08-30), FROZEN as the Tahap 1 final candidate. `counter` is accepted
    for interface uniformity but intentionally UNUSED -- address = key only,
    confirmed by user (see HANDOVER.md Lampiran/Tahap 1 for why, and what to
    change if that's wrong). `L_MIX_CONST` is frozen (not swept) as of Tahap 1."""
    L = [0] * 256
    M = [0] * 256
    for i in range(256):
        r = key ^ i  # rot32() masks this to r&31 internally, matching the C rot32
        l_val = (i * L_MIX_CONST + 0x9e3779b7 * key) & MASK32
        m_val = (i * 0x06a0dd9b + 0x06a0dd9b * key) & MASK32
        L[i] = rot32(l_val, r)
        M[i] = rot32(m_val, r)
    return L, M


CANDIDATES = {
    "kandidat1": init_kandidat1,
    "kandidat2": init_kandidat2,
    "kandidat3_control": init_kandidat3_control,
    "kandidat4": init_kandidat4,
    "kandidat5": init_kandidat5,
}

# Tahap 0 sweep of Kandidat 5's L_MIX_CONST (kept here for reference/rerun of
# the distinctness sweep only -- Tahap 1 froze the final value to
# L_MIX_CONST_DEFAULT, see init_kandidat5's docstring).
K5_MULTIPLIERS = [
    L_MIX_CONST_DEFAULT,
    (2**32 - L_MIX_CONST_DEFAULT) | 1,  # symmetry partner
    0x2545F4914F6CDD1D & MASK32 | 1,
    3,
]


# --- Sanity check: Python port vs. the real compiled binary ------------------

def sanity_check(keys: tuple[int, ...] = (1, 42, 12345), n: int = 2000) -> None:
    if not WINNER_BIN.exists():
        raise FileNotFoundError(
            f"{WINNER_BIN} not found -- compile winner_wired_v2 first "
            "(see ../2026-8-27_operand-position-search/HANDOVER.md)."
        )
    import numpy as np
    for key in keys:
        expected = stream_values(key, n)
        L, M = init_original(key=key, counter=0)
        raw: list[int] = []
        ra_core_from_state(key, n, L, M, raw_out=raw)
        actual = np.array(raw, dtype=np.uint32)
        if not np.array_equal(expected, actual):
            mismatch = int(np.argmax(expected != actual))
            raise AssertionError(
                f"Port mismatch at key={key}, first diff at index {mismatch}: "
                f"expected={expected[mismatch]:#x} actual={actual[mismatch]:#x}"
            )
    print(f"sanity_check: PASSED -- Python port bit-identical to {WINNER_BIN.name} "
          f"for keys {keys}, n={n}")


# --- Digest helpers (reusing collision_scan.py's birthday-bound formula) ----

def blake2b8(data: bytes) -> bytes:
    return hashlib.blake2b(data, digest_size=8).digest()


def pack_u32(arr: list[int]) -> bytes:
    return struct.pack(f"<{len(arr)}I", *arr)


def multiset_digest(L: list[int]) -> bytes:
    return blake2b8(pack_u32(sorted(L)))


def sequence_digest(L: list[int]) -> bytes:
    return blake2b8(pack_u32(L))


def scan_digests(items: list[tuple[str, bytes]]) -> dict:
    """items: (label, digest) pairs. A repeated digest under a DIFFERENT label
    is a candidate-level collision signal; digest_collision_prob bounds how
    much of that could be pure 64-bit hash noise (same idea as
    collision_scan.py's scan_prefix/scan_blocksweep, generalized to any label
    space instead of just seeds)."""
    seen: dict[bytes, str] = {}
    collisions = []
    for label, digest in items:
        owner = seen.get(digest)
        if owner is not None:
            if owner != label:
                collisions.append({"a": owner, "b": label, "hash": digest.hex()})
        else:
            seen[digest] = label
    return {
        "n_items": len(items),
        "collisions_found": len(collisions),
        "collisions": collisions[:20],
        "digest_collision_prob_estimate": digest_collision_prob(len(items)),
    }


# --- Direct "richness" check (non-degenerate values) -------------------------

def richness_stats(arr: list[int]) -> dict:
    return {
        "n_distinct": len(set(arr)),
        "min": min(arr),
        "max": max(arr),
        "mean": sum(arr) / len(arr),
    }


# --- Distinctness harness: sweep addresses, digest L (init state) and the
# resulting output stream, detect collisions -----------------------------------

def run_distinctness(name: str, init_fn, addresses: list[dict], n_stream_words: int,
                      default_key_for_cons: int = 1) -> dict:
    multiset_items, sequence_items, stream_items = [], [], []
    richness_L, richness_M = [], []
    for addr in addresses:
        L, M = init_fn(**addr)
        richness_L.append(richness_stats(L))
        richness_M.append(richness_stats(M))
        label = repr(sorted(addr.items()))
        multiset_items.append((label, multiset_digest(L)))
        sequence_items.append((label, sequence_digest(L)))
        cons_key = addr.get("key", default_key_for_cons)
        raw: list[int] = []
        ra_core_from_state(cons_key, n_stream_words, list(L), list(M), raw_out=raw)
        stream_items.append((label, blake2b8(pack_u32(raw))))
    return {
        "name": name,
        "n_addresses": len(addresses),
        "n_stream_words": n_stream_words,
        "multiset": scan_digests(multiset_items),
        "sequence": scan_digests(sequence_items),
        "min_n_distinct_L": min(r["n_distinct"] for r in richness_L),
        "min_n_distinct_M": min(r["n_distinct"] for r in richness_M),
        "stream": scan_digests(stream_items),
    }


def run_avalanche(name: str, init_fn, fixed_params: dict, base_key: int = 1) -> dict:
    """Reuses avalanche_stats() (quality_gate.py) unmodified -- generic
    capture_fn design, applied here to an addressable-init candidate's full
    generator instead of a wiring variant. Reports both the weakest
    (min_bit_fraction) and strongest (max_bit_fraction) single seed-bit --
    per_bit_fractions already has all 32 values, so max is free."""
    def capture_fn(key: int) -> list[int]:
        L, M = init_fn(key=key, **fixed_params)
        raw: list[int] = []
        ra_core_from_state(key, 255, L, M, raw_out=raw)  # exactly one cycle, no reseed
        return raw

    stats = avalanche_stats(capture_fn, base_seed=base_key)
    return {
        "name": name,
        **stats,
        "max_bit_fraction": max(stats["per_bit_fractions"]),
    }


# --- Kandidat 5-specific stress test: keys sharing low-5-bits ---------------

def run_kandidat5_low5bit_stress(L_MIX_CONST: int = L_MIX_CONST_DEFAULT,
                                  n_stream_words: int = 2000) -> dict:
    """HANDOVER.md Lampiran flags: rot32's internal r&=31 means L's rotation
    amount only depends on key's low 5 bits. Build keys that deliberately
    SHARE low-5-bits (but differ elsewhere) and check they still don't
    collide -- this is the specific case general random sampling could miss
    by chance."""
    base_low5 = 0x0A
    keys = [((k << 5) | base_low5) & MASK32 for k in range(1, 65)]  # 64 keys, same low 5 bits
    addresses = [{"key": k, "counter": 0, "L_MIX_CONST": L_MIX_CONST} for k in keys]
    return run_distinctness("kandidat5_low5bit_stress", init_kandidat5, addresses, n_stream_words)


# --- Tiers + main -------------------------------------------------------------

TIERS = {
    # Deliberately small (pure Python, see module docstring). "full" here is
    # still ~1-2 orders of magnitude below TIERS_Q2's C-binary scale.
    "smoke": {"m_addr": 50, "n_stream_words": 500},
    "full": {"m_addr": 2000, "n_stream_words": 5000},
}


def run_all_avalanche() -> dict:
    """Fast path (Tahap 1): just the avalanche check for all 5 candidates --
    milliseconds per candidate (32 seed-bit flips x 255-word capture), no
    expensive distinctness sweep. Used to (re)report min/max-bit without
    re-running the full-tier smoke/full distinctness scan."""
    results = {}
    for name, init_fn in CANDIDATES.items():
        fixed = {"counter": 0} if name != "kandidat5" else {"counter": 0, "L_MIX_CONST": L_MIX_CONST_DEFAULT}
        ava = run_avalanche(name, init_fn, fixed)
        results[name] = ava
        print(f"{name}: mean={ava['overall_mean_hamming_fraction']:.4f} "
              f"min_bit={ava['min_bit_fraction']:.4f} max_bit={ava['max_bit_fraction']:.4f}")
    out_path = HERE / "tahap0_avalanche.json"
    out_path.write_text(json.dumps(results, indent=2))
    print(f"\nWrote {out_path}")
    return results


def main() -> None:
    mode = sys.argv[1] if len(sys.argv) > 1 else "smoke"

    if mode == "avalanche":
        print("=== sanity_check ===")
        sanity_check()
        print("=== avalanche (all candidates, fast path) ===")
        run_all_avalanche()
        return

    tier = mode
    spec = TIERS[tier]
    m_addr, n_stream_words = spec["m_addr"], spec["n_stream_words"]

    print("=== sanity_check ===")
    sanity_check()

    results = {"tier": tier, "candidates": {}, "avalanche": {}}

    keys = random_seeds(m_addr, rng_seed=42)

    for name, init_fn in CANDIDATES.items():
        print(f"=== {name}: distinctness (tier={tier}) ===")
        if name == "kandidat3_control":
            addresses = [{"key": 1, "counter": c} for c in range(m_addr)]
            dist = run_distinctness(name, init_fn, addresses, n_stream_words)
        elif name == "kandidat5":
            addresses = [{"key": k, "counter": 0, "L_MIX_CONST": mult}
                         for mult in K5_MULTIPLIERS for k in keys]
            dist = run_distinctness(name, init_fn, addresses, n_stream_words)
        else:
            addresses = [{"key": k, "counter": 0} for k in keys]
            dist = run_distinctness(name, init_fn, addresses, n_stream_words)
        results["candidates"][name] = dist
        print(f"  multiset collisions={dist['multiset']['collisions_found']} "
              f"stream collisions={dist['stream']['collisions_found']} "
              f"min_n_distinct_L={dist['min_n_distinct_L']}/256")

        fixed = {"counter": 0} if name != "kandidat5" else {"counter": 0, "L_MIX_CONST": L_MIX_CONST_DEFAULT}
        ava = run_avalanche(name, init_fn, fixed)
        results["avalanche"][name] = ava
        print(f"  avalanche mean_fraction={ava['overall_mean_hamming_fraction']:.4f} "
              f"min_bit_fraction={ava['min_bit_fraction']:.4f} "
              f"max_bit_fraction={ava['max_bit_fraction']:.4f}")

    print("=== kandidat5_low5bit_stress ===")
    stress = run_kandidat5_low5bit_stress(n_stream_words=n_stream_words)
    results["kandidat5_low5bit_stress"] = stress
    print(f"  multiset collisions={stress['multiset']['collisions_found']} "
          f"stream collisions={stress['stream']['collisions_found']}")

    out_path = HERE / f"tahap0_results_{tier}.json"
    out_path.write_text(json.dumps(results, indent=2))
    print(f"\nWrote {out_path}")


if __name__ == "__main__":
    main()
