# Graph Report - ra_prng  (2026-08-25)

## Corpus Check
- 59 files · ~140,882 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 276 nodes · 375 edges · 32 communities (30 shown, 2 thin omitted)
- Extraction: 96% EXTRACTED · 4% INFERRED · 0% AMBIGUOUS · INFERRED: 15 edges (avg confidence: 0.79)
- Token cost: 0 input · 190,068 output

## Community Hubs (Navigation)
- Paper Core Concepts & Citations
- Entropy & Dieharder Benchmarks
- ra_prng Statistical Test Results
- Statistical Analysis Utilities
- PCG32 (Amortized) Implementation
- ra_prng2 Shuffle CLI (ZepFold variant)
- ra_prng3 Shuffle CLI (ZepFold variant)
- Scrambler CLI (C)
- Philox4x32 (Amortized) Implementation
- ra_prng2 Struct API (RA_PRNG)
- xoshiro256** (Amortized) Implementation
- Scrambler CLI (Python)
- PRNG Periodicity & Prior Work Citations
- ChaCha20 Implementation
- PCG32 Implementation
- Philox4x32 Implementation
- xoshiro256** Implementation
- CSAI2025 Publication Metadata
- Avalanche Effect & Hamming Distance Citations
- PCG32 Implementation (variant)
- xoshiro256** Implementation (variant)
- ChaCha20 (Amortized) Implementation
- Heatmap Visualization Script
- ra_prng2 Canonical Generator (C)
- ra_prng2 OpenMP Generator
- ra_prng2 Reference Implementation (Python)
- ra_prng3 Canonical Generator (C)
- ChaCha20 Implementation (variant)
- Philox4x32 Implementation (variant)
- Avalanche Effect Analysis Script

## God Nodes (most connected - your core abstractions)
1. `ra_prng2 (Array-Based PRNG)` - 30 edges
2. `ra_prng2 (32-bit stable version)` - 11 edges
3. `ra_prng (Array-Based PRNG Architecture)` - 10 edges
4. `ra_prng_next()` - 6 edges
5. `comparisons/ (benchmark & evaluation component)` - 6 edges
6. `Dieharder Test Suite` - 6 edges
7. `Scrambling Speed Benchmark (perf stat, per-call vs amortized, 30 runs)` - 6 edges
8. `ra_prng3 PRNG Generator` - 6 edges
9. `main()` - 5 edges
10. `main()` - 5 edges

## Surprising Connections (you probably didn't know these)
- `ra_prng2 PractRand Test Result (up to 128 GB, no anomalies)` --references--> `ra_prng2 (32-bit stable version)`  [INFERRED]
  benchmarks/results/ra_prng2_original/PractRand.txt → README.md
- `ChaCha20 RNG Speed Benchmark Result` --references--> `ChaCha20`  [EXTRACTED]
  benchmarks/comparisons/RNGing_speed/chacha20.txt → README.md
- `Philox4x32 RNG Speed Benchmark Result` --references--> `Philox4x32`  [EXTRACTED]
  benchmarks/comparisons/RNGing_speed/philox.txt → README.md
- `scrambled.txt (scramble-design experiment output)` --shares_data_with--> `Shuffling perf stat (ra_prng2 original)`  [INFERRED]
  experiments/2025-10-5_scramble-design/scrambled.txt → benchmarks/results/ra_prng2_original/shuffling_perf_stat.txt
- `scrambled.txt (scramble-design experiment output)` --shares_data_with--> `Scrambling perf stat (ra_prng3)`  [INFERRED]
  experiments/2025-10-5_scramble-design/scrambled.txt → benchmarks/results/ra_prng3/perf_stat_scrambling.txt

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **ChaCha20 Speed + Entropy Benchmark Suite** — readme_chacha20, benchmarks_comparisons_rnging_speed_chacha20_result, benchmarks_comparisons_entropy_chacha20_result [INFERRED 0.85]
- **ra_prng2 Statistical Validation (NIST STS + PractRand)** — readme_ra_prng2, benchmarks_results_ra_prng2_original_nist_sts_test_result, benchmarks_results_ra_prng2_original_practrand_result [INFERRED 0.85]
- **RNG Comparison Baseline Algorithms** — readme_chacha20, readme_dev_urandom, readme_pcg32, readme_philox4x32, readme_xoshiro256 [EXTRACTED 1.00]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_practrand, concept_ent [INFERRED 0.85]
- **ra_prng2 Original Benchmark Report Set** — benchmarks_results_ra_prng2_original_rnging_perf_stat_, benchmarks_results_ra_prng2_original_testu01, benchmarks_results_ra_prng2_original_dieharder_test, benchmarks_results_ra_prng2_original_shuffling_perf_stat [INFERRED 0.80]
- **ra_prng3 Benchmark Report Set** — benchmarks_results_ra_prng3_nist_sts_test, benchmarks_results_ra_prng3_dieharder_test, benchmarks_results_ra_prng3_ent_test, benchmarks_results_ra_prng3_perf_stat_rnging, benchmarks_results_ra_prng3_perf_stat_scrambling, benchmarks_results_ra_prng3_practrand_test [INFERRED 0.80]
- **ra_prng2 Statistical Validation Test Suite** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_nist_sts, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_dieharder, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_practrand, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_bigcrush [EXTRACTED 0.90]
- **ra_prng2 State Transition Operator Composition (F = Mutate∘Permute∘Reseed)** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_transition_operator_f, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_mutate_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_permute_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_reseed_operator [EXTRACTED 0.95]
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_xoshiro256, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_philox4x32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]

## Communities (32 total, 2 thin omitted)

### Community 0 - "Paper Core Concepts & Citations"
Cohesion: 0.05
Nodes (46): ACM (Association for Computing Machinery), Andrew Rukhin et al., Array Index Shuffling, Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), BigCrush (TestU01), Bit Shifting, Bitwise Rotation (rotl32) (+38 more)

### Community 1 - "Entropy & Dieharder Benchmarks"
Cohesion: 0.09
Nodes (35): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result, /dev/urandom RNG Speed Benchmark Result (+27 more)

### Community 2 - "ra_prng Statistical Test Results"
Cohesion: 0.14
Nodes (21): Dieharder Test Results (ra_prng2 original), RNGing perf stat (ra_prng2 original), Shuffling perf stat (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), RNGing perf stat (ra_prng3) (+13 more)

### Community 3 - "Statistical Analysis Utilities"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 4 - "PCG32 (Amortized) Implementation"
Cohesion: 0.44
Nodes (8): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), pcg32_buffered_t

### Community 5 - "ra_prng2 Shuffle CLI (ZepFold variant)"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 6 - "ra_prng3 Shuffle CLI (ZepFold variant)"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 7 - "Scrambler CLI (C)"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 8 - "Philox4x32 (Amortized) Implementation"
Cohesion: 0.46
Nodes (7): main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32(), philox_refill_buffer(), rand_bounded()

### Community 9 - "ra_prng2 Struct API (RA_PRNG)"
Cohesion: 0.50
Nodes (7): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), rot32()

### Community 10 - "xoshiro256** (Amortized) Implementation"
Cohesion: 0.52
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next()

### Community 11 - "Scrambler CLI (Python)"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 12 - "PRNG Periodicity & Prior Work Citations"
Cohesion: 0.29
Nodes (7): B. Harris, Donald E. Knuth, Makoto Matsumoto and Takuji Nishimura, Mersenne Twister (MT19937), PRNG Periodicity Analysis, Pierre L'Ecuyer, Random Mapping Theorem

### Community 13 - "ChaCha20 Implementation"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 14 - "PCG32 Implementation"
Cohesion: 0.67
Nodes (5): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded()

### Community 15 - "Philox4x32 Implementation"
Cohesion: 0.60
Nodes (5): main(), philox4x32_10(), philox4x32_round(), philox_rand32(), rand_bounded()

### Community 16 - "xoshiro256** Implementation"
Cohesion: 0.60
Nodes (5): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 17 - "CSAI2025 Publication Metadata"
Cohesion: 0.33
Nodes (6): ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex, Hamas A. Rahman, ra_prng2 and Beyond (Paper CA-162), Scopus

### Community 18 - "Avalanche Effect & Hamming Distance Citations"
Cohesion: 0.33
Nodes (6): Avalanche Effect (Bit-Flip Sensitivity), Joan Daemen and Vincent Rijmen, Douglas R. Stinson, Hamming Distance, Richard W. Hamming, Alan F. Webster and Stafford E. Tavares

### Community 19 - "PCG32 Implementation (variant)"
Cohesion: 0.70
Nodes (4): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom()

### Community 20 - "xoshiro256** Implementation (variant)"
Cohesion: 0.70
Nodes (4): main(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 21 - "ChaCha20 (Amortized) Implementation"
Cohesion: 0.60
Nodes (3): chacha20_block(), main(), rand_bounded()

### Community 22 - "Heatmap Visualization Script"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 23 - "ra_prng2 Canonical Generator (C)"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 24 - "ra_prng2 OpenMP Generator"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 25 - "ra_prng2 Reference Implementation (Python)"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 26 - "ra_prng3 Canonical Generator (C)"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 28 - "Philox4x32 Implementation (variant)"
Cohesion: 0.83
Nodes (3): main(), philox4x32_10(), philox4x32_round()

## Ambiguous Edges - Review These
- `Hamas A. Rahman` → `hamzy hams (GitHub maintainer)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to

## Knowledge Gaps
- **42 isolated node(s):** `Array Index Shuffling`, `Bitwise Rotation`, `Bit Shifting`, `XORing`, `Nonlinear Indexing` (+37 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **2 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Hamas A. Rahman` and `hamzy hams (GitHub maintainer)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `ra_prng2 (Array-Based PRNG)` connect `Paper Core Concepts & Citations` to `Avalanche Effect & Hamming Distance Citations`, `PRNG Periodicity & Prior Work Citations`?**
  _High betweenness centrality (0.041) - this node is a cross-community bridge._
- **Why does `PRNG Periodicity Analysis` connect `PRNG Periodicity & Prior Work Citations` to `Paper Core Concepts & Citations`?**
  _High betweenness centrality (0.009) - this node is a cross-community bridge._
- **Why does `Avalanche Effect (Bit-Flip Sensitivity)` connect `Avalanche Effect & Hamming Distance Citations` to `Paper Core Concepts & Citations`?**
  _High betweenness centrality (0.007) - this node is a cross-community bridge._
- **Are the 2 inferred relationships involving `ra_prng2 (32-bit stable version)` (e.g. with `ra_prng2 PractRand Test Result (up to 128 GB, no anomalies)` and `ra_prng3 (experimental 64-bit version)`) actually correct?**
  _`ra_prng2 (32-bit stable version)` has 2 INFERRED edges - model-reasoned connections that need verification._
- **What connects `Array Index Shuffling`, `Bitwise Rotation`, `Bit Shifting` to the rest of the system?**
  _42 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Paper Core Concepts & Citations` be split into smaller, more focused modules?**
  _Cohesion score 0.04927536231884058 - nodes in this community are weakly interconnected._