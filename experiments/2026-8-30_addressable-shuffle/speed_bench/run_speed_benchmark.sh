#!/bin/bash
# perf stat (elapsed time, 30 repeats) for every shuffle-CLI binary in this
# folder, driven by the same tokens.txt (1,000,000 tokens, same file as
# ../../../benchmarks/comparisons/source/tokens.txt) used by the original
# ../../../benchmarks/comparisons/scrambling_speed/benchmark_perf.txt.
# ra_prng2 (old ZepFold shuffle, no addressable init) is re-run here in the
# same session as scrambler_addressable (new, addressable-init) so that
# comparison is directly controlled -- the other binaries (xoshiro/pcg/
# chacha20/philox) are included for the same wider context as the original
# benchmark, but ran on a possibly different machine/session historically.
set -euo pipefail
cd "$(dirname "$0")"

OUT=speed_benchmark_perf.log
: > "$OUT"

run() {
    local name="$1"; shift
    echo "=== $name ===" | tee -a "$OUT"
    perf stat -r 30 -- "$@" >> "$OUT" 2>&1
    echo "" >> "$OUT"
}

run xoshiro256            ./xoshiro256            -i tokens.txt -o out_xoshiro256.txt
run xoshiro256_amortized  ./xoshiro256_amortized  -i tokens.txt -o out_xoshiro256_amortized.txt
run pcg                   ./pcg                   tokens.txt    -o out_pcg.txt
run pcg_amortized         ./pcg_amortized         tokens.txt    -o out_pcg_amortized.txt
run philox                ./philox                -i tokens.txt -o out_philox.txt
run philox_amortized      ./philox_amortized      -i tokens.txt -o out_philox_amortized.txt
run chacha20               ./chacha20              -i tokens.txt -o out_chacha20.txt
run chacha20_amortized    ./chacha20_amortized    -i tokens.txt -o out_chacha20_amortized.txt
run ra_prng2               ./ra_prng2              --input tokens.txt --output out_ra_prng2.txt
run scrambler_addressable  ./scrambler_addressable --input tokens.txt --output out_scrambler_addressable.txt --key 1
run scrambler_wired_addressable ./scrambler_wired_addressable --input tokens.txt --output out_scrambler_wired_addressable.txt --key 1
run stream_driven_shuffle  ./stream_driven_shuffle --input tokens.txt --output out_stream_driven_shuffle.txt --key 1

echo "Done. See $OUT"
