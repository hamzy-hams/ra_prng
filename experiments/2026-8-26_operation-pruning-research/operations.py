"""Catalog of ablatable operations for the ra_prng2 operation-pruning search.

Granularity is deliberately coarse (flags, not individual bits) so the
discrete on/off search space stays small enough for exhaustive-per-round
greedy ablation (see HANDOVER.md / RESULTS.md for why this granularity, not
finer or coarser).

Each flag name maps to the exact expression it toggles in
`src/ra_prng2/c/ra_prng2.c` (post-c4fc35c) / `src/ra_prng2/python/ra_prng2.py`
`ra_permutation_cycle` / `ra_hash`. Semantics of "off" are spelled out here,
not left implicit -- the periodicity-heuristic experiment
(experiments/2026-8-25_periodicity-heuristic-validation/RESULTS.md) found two
silent-collapse traps when simplifying this same algorithm family, so every
"off" fallback here is deliberately a *value substitution*, never a bare
zero/no-op that could silently degenerate the state.
"""

from __future__ import annotations

# --- o-loop taps (8-wide shift-XOR building `o` from M) ---
# o ^= (M[(i+e) & 0xFF] << e), for e in 0..7. Off = that single XOR term is
# skipped entirely (not replaced by anything -- the loop just does one less
# iteration for that e).
TAPS = tuple(f"TAP{e}" for e in range(8))

# --- rot32 calls (4 total in the a/b/o/c update chain) ---
# Off = pass the pre-rotation value straight through unrotated (identity in
# place of rot32(x, r)). Never "rotate by 0" implicitly -- always an explicit
# substitution of the un-rotated operand.
ROT_A = "ROT_A"   # a = rot32(b ^ o, d) ^ (cons + a)   -> off: a = (b ^ o) ^ (cons + a)
ROT_B = "ROT_B"   # b = rot32(cons + a, i) ^ (o + d)   -> off: b = (cons + a) ^ (o + d)
ROT_O = "ROT_O"   # o' = rot32(a ^ o, i) << 9 ^ ...    -> off: o' = (a ^ o) << 9 ^ ...
ROT_C = "ROT_C"   # c = rot32((o+c<<14)^(b>>13)^a, b)  -> off: c = (o+c<<14)^(b>>13)^a
ROTS = (ROT_A, ROT_B, ROT_O, ROT_C)

# --- explicit shift terms in the o/c updates ---
# Each of these names one XOR *operand* of a 2-3 term chain, not a bare shift
# amount. Off = that whole operand is dropped from the XOR chain (the
# remaining operand(s) alone become the new value) -- turning off SHL9 does
# NOT mean "shift by 0", it means "this term isn't part of the mix at all".
SHL9 = "SHL9"     # o' = (rot32(a^o,i) << 9) ^ (b >> 18)  -> off: o' = (b >> 18)
SHR18 = "SHR18"   # (same expression)                     -> off: o' = (rot32(a^o,i) << 9)
SHL14 = "SHL14"   # c = rot32(((o+c)<<14) ^ (b>>13) ^ a, b) -> off: drop (o+c)<<14 term
SHR13 = "SHR13"   # (same expression)                       -> off: drop (b>>13) term
SHIFTS = (SHL9, SHR18, SHL14, SHR13)

# --- Lemire-style multiply reduction for the swap index d ---
# d = (c * (i+1)) >> 32  -> off: d = c & 0xFF (still a valid L[] index in
# [0,255], just a single AND instead of a 64-bit multiply + shift -- the
# cheapest possible substitute that preserves the "d must be a valid index"
# invariant, per HANDOVER.md's "cheapen" framing).
MULT_REDUCE = "MULT_REDUCE"

# --- ra_hash self-index term ---
# out[i] ^= N[N[i] & 0xFF]  -> off: term dropped entirely.
HASH_SELFIDX = "HASH_SELFIDX"

# All boolean (on/off) flags subject to greedy ablation. HASH_DEPTH is
# intentionally NOT in this set -- see below.
ALL_FLAGS = frozenset(TAPS + ROTS + SHIFTS + (MULT_REDUCE, HASH_SELFIDX))

# Canonical ordering, used both as pruned_prng.c's op-bitmask bit order (bit
# i <-> FLAG_ORDER[i]) and as ablation_search.py's default greedy-scan order.
# Must stay in sync with pruned_prng.c's `enum` block if either changes.
FLAG_ORDER = list(TAPS) + list(ROTS) + list(SHIFTS) + [MULT_REDUCE, HASH_SELFIDX]
assert set(FLAG_ORDER) == ALL_FLAGS


def ops_to_bitmask(ops: frozenset[str]) -> int:
    """Encode an op set as pruned_prng.c's bitmask (bit i set = FLAG_ORDER[i] active)."""
    mask = 0
    for i, name in enumerate(FLAG_ORDER):
        if name in ops:
            mask |= (1 << i)
    return mask

# --- ra_hash reduction depth: FIXED, not an ablation knob ---
# for j in range(depth): out[i] ^= N[j*8 + i]
#
# CORRECTION (user, after seeing the first search run): depth is NOT to be
# reduced. An earlier version of this catalog treated depth as a swept
# parameter ({32,16,8,4}) on the same "shrink it and see" logic as the
# periodicity-heuristic experiment's `rows` sweep -- but that experiment's
# finding was a *warning*, not a green light: shrinking this same reduction's
# depth changed measured cycle length by 47.93x at fixed state-space size
# (n=8,w=4, rows 2->4). A first pruning-search run confirmed the danger
# directly: at depth=4, greedy ablation converged to a 3-of-18-flag candidate
# that passed a (too-weak) 1MB PractRand gate cleanly, then failed
# catastrophically (multiple hard FAILs) at 16MB -- i.e. reducing depth
# doesn't just risk periodicity in theory, it actively let a broken candidate
# hide from the quality gate in practice. Depth stays at 32, always.
HASH_DEPTH_FIXED = 32

# --- ra_hash reduction ACCESS PATTERN: the actual pruning target for hash ---
# Depth (term count) is fixed, but *which* N[] elements get combined, and in
# what order, is still open for cheapening -- e.g. a cache-friendlier
# indexing scheme could be faster without touching the 32-term mixing depth
# that periodicity depends on. Two variants, both XOR exactly 32 elements of
# N into each out[i] (same depth, same total XOR count):
#
#   "strided"    (original): out[i] ^= N[j*8 + i] for j in 0..31.
#                Each output word i pulls from N at stride 8, i.e. reads
#                scattered across the whole 256-element array.
#   "sequential" (candidate): out[i] ^= N[i*32 + j] for j in 0..31.
#                Each output word i pulls a single contiguous 32-element
#                block of N -- sequential memory access, no striding.
#
# Tested separately from the flat ALL_FLAGS ablation loop, same reasoning as
# before: this is a structural change to the hash's mixing topology, not a
# simple op removal, and deserves its own isolated quality-gate comparison
# rather than being silently mixed into flag-removal attribution.
HASH_ACCESS_STRIDED = "strided"
HASH_ACCESS_SEQUENTIAL = "sequential"
HASH_ACCESS_CHOICES = (HASH_ACCESS_STRIDED, HASH_ACCESS_SEQUENTIAL)
HASH_ACCESS_DEFAULT = HASH_ACCESS_STRIDED


def all_ops_baseline() -> frozenset[str]:
    """The full, unpruned operation set -- must reproduce ra_prng2 exactly."""
    return frozenset(ALL_FLAGS)


def describe(ops: frozenset[str], hash_access: str = HASH_ACCESS_DEFAULT) -> str:
    """Human-readable summary of a candidate for logging in STATUS.md/RESULTS.md."""
    removed = sorted(ALL_FLAGS - ops)
    access_note = f", HASH_ACCESS={hash_access}" if hash_access != HASH_ACCESS_DEFAULT else ""
    if not removed:
        return f"[baseline: all {len(ALL_FLAGS)} ops active, HASH_DEPTH={HASH_DEPTH_FIXED} (fixed){access_note}]"
    return (f"[{len(ops)}/{len(ALL_FLAGS)} ops active, removed: {', '.join(removed)}, "
            f"HASH_DEPTH={HASH_DEPTH_FIXED} (fixed){access_note}]")
