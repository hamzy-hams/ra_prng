"""Brent's cycle-detection algorithm applied to pruned_wired_toy_prng.next_state.

Identical algorithm to ../2026-8-25_periodicity-heuristic-validation/
cycle_measure.py -- brent() is fully algorithm-agnostic (only calls
Params/init_state/next_state/state_key), so only the import target changes.
"""

from __future__ import annotations

from pruned_wired_toy_prng import Params, init_state, next_state, state_key


def brent(p: Params, seed: int) -> tuple[int, int]:
    """Standard Brent's cycle detection. Returns (lam, mu):
      lam = cycle length (period)
      mu  = tail length before entering the cycle
    """
    x0 = init_state(seed, p)

    power = lam = 1
    tortoise = x0
    hare = next_state(x0, p)
    while state_key(tortoise) != state_key(hare):
        if power == lam:
            tortoise = hare
            power *= 2
            lam = 0
        hare = next_state(hare, p)
        lam += 1

    tortoise = hare = x0
    for _ in range(lam):
        hare = next_state(hare, p)

    mu = 0
    while state_key(tortoise) != state_key(hare):
        tortoise = next_state(tortoise, p)
        hare = next_state(hare, p)
        mu += 1

    return lam, mu


if __name__ == "__main__":
    import time

    p = Params(n=4, w=8)
    for seed in range(5):
        start = time.time()
        lam, mu = brent(p, seed)
        print(f"seed={seed}: lambda={lam} mu={mu}  ({time.time()-start:.3f}s)")
