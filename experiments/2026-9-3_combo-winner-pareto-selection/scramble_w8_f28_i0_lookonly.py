import subprocess, struct, sys

BIN = "/home/zep/Documents/research/ra_prng/experiments/2026-9-2_singleblock-cycle-combo-search/candidates/w8_f28_i0"
GOLDEN = 0x9E3779B9


def pull_n(key, n):
    if n == 0:
        return []
    out = subprocess.run([BIN, "--single", str(key & 0xFFFFFFFF), str(n)],
                          stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True).stdout
    assert len(out) == n * 4, f"expected {n*4} bytes got {len(out)}"
    return list(struct.unpack(f"<{n}I", out))


def bulk_chain(base_key, count):
    """key = base_key + i*GOLDEN for i=0..count-1, one word each -- matches
    the candidate binary's own --stream K=1 chaining exactly, done in ONE
    subprocess call instead of `count` separate ones."""
    out = subprocess.run([BIN, "--stream", str(base_key), str(count), "1"],
                          stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, check=True).stdout
    assert len(out) == count * 4
    return list(struct.unpack(f"<{count}I", out))


def fisher_yates_k255(n, key_r):
    words = pull_n(key_r, n - 1) if n > 1 else []
    arr = list(range(n))
    for step, i in enumerate(range(n - 1, 0, -1)):
        w = words[step]
        idx = (w * (i + 1)) >> 32
        arr[i], arr[idx] = arr[idx], arr[i]
    return arr


def gen(mode, reps, n, base_key, out_path):
    with open(out_path, "w") as f:
        if mode == "k255":
            for r in range(reps):
                key_r = (base_key + r * GOLDEN) & 0xFFFFFFFF
                arr = fisher_yates_k255(n, key_r)
                f.write(" ".join(map(str, arr)))
                if r + 1 < reps:
                    f.write(" ")
        else:
            # k1: step_key = base_key + (r+step)*GOLDEN exactly as the real
            # scrambler_ra_core_singleblock.c derives it (key_r = base_key +
            # r*GOLDEN, then += step*GOLDEN per step) -- precompute the whole
            # (r+step) index range in one bulk call instead of one subprocess
            # per swap-step (would be ~5M calls otherwise).
            max_idx = (reps - 1) + (n - 2) if n > 1 else reps - 1
            chain = bulk_chain(base_key, max_idx + 1)
            for r in range(reps):
                arr = list(range(n))
                for step, i in enumerate(range(n - 1, 0, -1)):
                    w = chain[r + step]
                    idx = (w * (i + 1)) >> 32
                    arr[i], arr[idx] = arr[idx], arr[i]
                f.write(" ".join(map(str, arr)))
                if r + 1 < reps:
                    f.write(" ")


if __name__ == "__main__":
    mode, reps, n, base_key, out_path = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), sys.argv[5]
    gen(mode, reps, n, base_key, out_path)
    print(f"wrote {out_path}")
