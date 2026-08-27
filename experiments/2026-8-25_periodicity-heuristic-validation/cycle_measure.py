"""Brent's cycle-detection algorithm applied to toy_prng.next_state.

Measures the real (lambda, mu) - cycle length and tail/pre-period length -
of the deterministic toy generator's trajectory from a given seed, so it
can be compared against the paper's 0.7824*sqrt(|S|) heuristic prediction.
Brent's algorithm is used over Floyd's because it needs fewer state
comparisons/advances (per the handover's step 3 suggestion).
"""

from __future__ import annotations

from toy_prng import Params, init_state, next_state, state_key


def brent(p: Params, seed: int) -> tuple[int, int]:
    """Standard Brent's cycle detection. Returns (lam, mu):
      lam = cycle length (period)
      mu  = tail length before entering the cycle
    """
    x0 = init_state(seed, p)

    # Phase 1: find lambda (a power-of-two bound on the cycle length).
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

    # Phase 2: find mu, the position of the first repetition, by advancing
    # two pointers lam apart until they collide.
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
