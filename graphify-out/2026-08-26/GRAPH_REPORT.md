# Graph Report - ra_prng  (2026-08-26)

## Corpus Check
- 80 files · ~152,112 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 391 nodes · 555 edges · 48 communities (39 shown, 9 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 27 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- Avalanche and Bit-Flip Testing
- Entropy and Statistical Benchmark Comparisons
- Bindings and Project Documentation
- ra_prng Benchmark Results
- Periodicity and Cycle Measurement
- Cryptographic and PRNG Architecture
- Serial Correlation and Uniformity Testing
- Python ctypes Binding Wrapper
- PCG32 Amortized Benchmark
- ra_prng2 CLI Implementation
- ra_prng3 CLI Implementation
- C Scrambler Tool
- Philox Amortized Benchmark
- ra_prng2 Struct Implementation
- State Array and Reseed Hash Mechanism
- xoshiro256 Amortized Benchmark
- Python Scrambler Tool
- PRNG Periodicity Literature and Theory
- ChaCha20 Source Implementation
- PCG Source Implementation
- Philox Source Implementation
- xoshiro256 Source Implementation
- CSAI 2025 Paper and Academic Citations
- Avalanche Effect and Cryptographic Metrics
- PCG32 Implementation
- xoshiro256 Implementation
- ChaCha20 Amortized Benchmark
- Heatmap and Diffusion Visualizer
- State Transition and Mutation Operators
- ra_prng2 Core Implementation
- ra_prng2 Threaded Implementation
- ra_prng2 Python Implementation
- ra_prng3 Core Implementation
- ChaCha20 Generator
- Philox Generator
- Periodicity Experiment Logs
- Initialization and Golden Ratio Constants
- Graphify Rules and Workflows
- Avalanche Effect Analysis Tool
- NIST Statistical Test Suite
- Branchless CPU Core Loop
- Shannon Entropy Foundations
- Lemire Fast Reduction
- Dieharder Test Suite
- ent Entropy Tool
- ra-prng Root Module

## God Nodes (most connected - your core abstractions)
1. `ra_prng2 (Array-Based PRNG)` - 30 edges
2. `Params` - 16 edges
3. `ra_prng2 (32-bit stable version)` - 11 edges
4. `ra_prng (Array-Based PRNG Architecture)` - 10 edges
5. `brent_resumable()` - 9 edges
6. `Repository Structure Specification` - 8 edges
7. `brent()` - 7 edges
8. `init_state()` - 7 edges
9. `next_state()` - 7 edges
10. `ICCS 2026 Research Paper: Array-Native Randomness` - 7 edges

## Surprising Connections (you probably didn't know these)
- `Non-Invertible State Transition Operator F = Reseed o Mutate o Permute` --semantically_similar_to--> `ra_prng2 C Struct API (ra_prng_init/next/advance)`  [INFERRED] [semantically similar]
  research/iccs2026_ra_prng/iccs_ra_prng.pdf → docs/api/ra_prng2.md
- `Lemire Fast Modulo Reduction Integration` --conceptually_related_to--> `ZepFold Shuffling CLI Tool`  [INFERRED]
  research/iccs2026_ra_prng/iccs_ra_prng.pdf → benchmarks/comparisons/source/README.md
- `Periodicity Heuristic Scalability & Non-Monotonicity Verdict` --rationale_for--> `Random Mapping Periodicity Heuristic (lambda ≈ 0.7824 * sqrt(|S|))`  [INFERRED]
  experiments/2026-8-25_periodicity-heuristic-validation/RESULTS.md → research/iccs2026_ra_prng/iccs_ra_prng.pdf
- `Toy PRNG Avalanche Sanity Verification Gate` --conceptually_related_to--> `Multi-Seed Perturbation Avalanche Analysis`  [INFERRED]
  experiments/2026-8-25_periodicity-heuristic-validation/RESULTS.md → research/iccs2026_ra_prng/iccs_ra_prng.pdf
- `Data Fingerprinting Application Primitive` --cites--> `ICCS 2026 Research Paper: Array-Native Randomness`  [EXTRACTED]
  sub-projects/fingerprinting_tools/README.md → research/iccs2026_ra_prng/iccs_ra_prng.pdf

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **ra_prng2 Statistical Validation (NIST STS + PractRand)** — readme_ra_prng2, benchmarks_results_ra_prng2_original_nist_sts_test_result, benchmarks_results_ra_prng2_original_practrand_result [INFERRED 0.85]
- **RNG Comparison Baseline Algorithms** — readme_chacha20, readme_dev_urandom, readme_pcg32, readme_philox4x32, readme_xoshiro256 [EXTRACTED 1.00]
- **ChaCha20 Speed + Entropy Benchmark Suite** — readme_chacha20, benchmarks_comparisons_rnging_speed_chacha20_result, benchmarks_comparisons_entropy_chacha20_result [INFERRED 0.85]
- **ra_prng2 Original Benchmark Report Set** — benchmarks_results_ra_prng2_original_rnging_perf_stat_, benchmarks_results_ra_prng2_original_testu01, benchmarks_results_ra_prng2_original_dieharder_test, benchmarks_results_ra_prng2_original_shuffling_perf_stat [INFERRED 0.80]
- **ra_prng3 Benchmark Report Set** — benchmarks_results_ra_prng3_nist_sts_test, benchmarks_results_ra_prng3_dieharder_test, benchmarks_results_ra_prng3_ent_test, benchmarks_results_ra_prng3_perf_stat_rnging, benchmarks_results_ra_prng3_perf_stat_scrambling, benchmarks_results_ra_prng3_practrand_test [INFERRED 0.80]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_practrand, concept_ent [INFERRED 0.85]
- **ra_prng2 Statistical Validation Test Suite** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_nist_sts, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_dieharder, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_practrand, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_bigcrush [EXTRACTED 0.90]
- **ra_prng2 State Transition Operator Composition (F = Mutate∘Permute∘Reseed)** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_transition_operator_f, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_mutate_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_permute_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_reseed_operator [EXTRACTED 0.95]
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_xoshiro256, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_philox4x32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]
- **Periodicity Heuristic Empirical Validation Lifecycle** — research_iccs2026_ra_prng_periodicity_heuristic, experiments_2026_8_25_periodicity_heuristic_validation_toy_model_spec, experiments_2026_8_25_periodicity_heuristic_validation_scaling_verdict, experiments_2026_8_25_periodicity_heuristic_validation_hash_depth_hypothesis [INFERRED 0.85]
- **ra_prng2 Architecture Implementation & Bindings Ecosystem** — research_iccs2026_ra_prng_array_native_architecture, docs_structure_ra_prng2_variants, docs_api_ra_prng2_c_api, bindings_cpython_ctypes_wrapper [EXTRACTED 1.00]
- **Array-Native Data Permutation and Fingerprinting Workflow** — research_iccs2026_ra_prng_avalanche_sensitivity, sub_projects_fingerprinting_tools_concept, benchmarks_comparisons_source_zepfold_cli [INFERRED 0.75]

## Communities (48 total, 9 thin omitted)

### Community 0 - "Avalanche and Bit-Flip Testing"
Cohesion: 0.09
Nodes (31): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for toy_prng.py. Port of…, One outer iteration, no rehash - mirrors zepfold_capture() in…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), brent(), Brent's cycle-detection algorithm applied to toy_prng.next_state. Measures the… (+23 more)

### Community 1 - "Entropy and Statistical Benchmark Comparisons"
Cohesion: 0.09
Nodes (35): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result, /dev/urandom RNG Speed Benchmark Result (+27 more)

### Community 2 - "Bindings and Project Documentation"
Cohesion: 0.07
Nodes (35): Benchmark Source Comparisons Documentation, ZepFold Shuffling CLI Tool, RaPrng2 ctypes Wrapper, CPython Binding Documentation, Project Root Changelog, Build and Test Targets, Contributor Guidelines, ra_prng2 C Struct API (ra_prng_init/next/advance) (+27 more)

### Community 3 - "ra_prng Benchmark Results"
Cohesion: 0.14
Nodes (21): Dieharder Test Results (ra_prng2 original), RNGing perf stat (ra_prng2 original), Shuffling perf stat (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), RNGing perf stat (ra_prng3) (+13 more)

### Community 4 - "Periodicity and Cycle Measurement"
Cohesion: 0.21
Nodes (19): brent_resumable(), State, init_state(), load_checkpoint(), main(), next_state(), params_init(), ra_hash_gen() (+11 more)

### Community 5 - "Cryptographic and PRNG Architecture"
Cohesion: 0.12
Nodes (17): ACM (Association for Computing Machinery), BigCrush (TestU01), Bit Shifting, ChaCha20, CSAI 2025 (9th Int'l Conf. on Computer Science and AI), ra_prng GitHub Repository, Hamas A. Rahman, hamzy hams (GitHub maintainer) (+9 more)

### Community 6 - "Serial Correlation and Uniformity Testing"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 7 - "Python ctypes Binding Wrapper"
Cohesion: 0.31
Nodes (6): _lib_handle(), _load_library(), RaPrng2, ctypes binding for the ra_prng2 C engine. This is glue only - it marshals calls…, Thin wrapper around one RA_PRNG instance., CDLL

### Community 8 - "PCG32 Amortized Benchmark"
Cohesion: 0.44
Nodes (8): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), pcg32_buffered_t

### Community 9 - "ra_prng2 CLI Implementation"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 10 - "ra_prng3 CLI Implementation"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 11 - "C Scrambler Tool"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 12 - "Philox Amortized Benchmark"
Cohesion: 0.46
Nodes (7): main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32(), philox_refill_buffer(), rand_bounded()

### Community 13 - "ra_prng2 Struct Implementation"
Cohesion: 0.50
Nodes (7): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), rot32()

### Community 14 - "State Array and Reseed Hash Mechanism"
Cohesion: 0.25
Nodes (8): Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), R. Durstenfeld, Fisher–Yates Shuffle, Rationale: Non-Invertible Hash Reseed Trade-off (Reduces Period, Aids Diffusion), ra_hash (Reseed Hash Function), Reseed Operator, Internal State S

### Community 15 - "xoshiro256 Amortized Benchmark"
Cohesion: 0.52
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next()

### Community 16 - "Python Scrambler Tool"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 17 - "PRNG Periodicity Literature and Theory"
Cohesion: 0.29
Nodes (7): B. Harris, Donald E. Knuth, Makoto Matsumoto and Takuji Nishimura, Mersenne Twister (MT19937), PRNG Periodicity Analysis, Pierre L'Ecuyer, Random Mapping Theorem

### Community 18 - "ChaCha20 Source Implementation"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 19 - "PCG Source Implementation"
Cohesion: 0.67
Nodes (5): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded()

### Community 20 - "Philox Source Implementation"
Cohesion: 0.60
Nodes (5): main(), philox4x32_10(), philox4x32_round(), philox_rand32(), rand_bounded()

### Community 21 - "xoshiro256 Source Implementation"
Cohesion: 0.60
Nodes (5): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 22 - "CSAI 2025 Paper and Academic Citations"
Cohesion: 0.33
Nodes (6): ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex, Hamas A. Rahman, ra_prng2 and Beyond (Paper CA-162), Scopus

### Community 23 - "Avalanche Effect and Cryptographic Metrics"
Cohesion: 0.33
Nodes (6): Avalanche Effect (Bit-Flip Sensitivity), Joan Daemen and Vincent Rijmen, Douglas R. Stinson, Hamming Distance, Richard W. Hamming, Alan F. Webster and Stafford E. Tavares

### Community 24 - "PCG32 Implementation"
Cohesion: 0.70
Nodes (4): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom()

### Community 25 - "xoshiro256 Implementation"
Cohesion: 0.70
Nodes (4): main(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 26 - "ChaCha20 Amortized Benchmark"
Cohesion: 0.60
Nodes (3): chacha20_block(), main(), rand_bounded()

### Community 27 - "Heatmap and Diffusion Visualizer"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 28 - "State Transition and Mutation Operators"
Cohesion: 0.40
Nodes (5): Array Index Shuffling, Bitwise Rotation (rotl32), Mutate Operator, Permute Operator, State Transition Operator F

### Community 29 - "ra_prng2 Core Implementation"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 30 - "ra_prng2 Threaded Implementation"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 31 - "ra_prng2 Python Implementation"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 32 - "ra_prng3 Core Implementation"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 34 - "Philox Generator"
Cohesion: 0.83
Nodes (3): main(), philox4x32_10(), philox4x32_round()

### Community 35 - "Periodicity Experiment Logs"
Cohesion: 0.50
Nodes (4): Cycle Measurement Checkpoint and Resume Mechanism, Hash Reduction Depth (rows) Impact Hypothesis, rows=2 Configuration Log, Periodicity Experiment Execution Status

### Community 36 - "Initialization and Golden Ratio Constants"
Cohesion: 0.50
Nodes (4): Rationale: Golden-Ratio Constants Reduce Collisions, Dead States, Short Cycles, PCG32, Seed and Array Initialization (Algorithm 1), xoshiro256**

### Community 37 - "Graphify Rules and Workflows"
Cohesion: 0.67
Nodes (3): Graphify Query & Navigation Protocol, Graphify Knowledge Graph Rule, Graphify Pipeline Workflow

## Ambiguous Edges - Review These
- `Hamas A. Rahman` → `hamzy hams (GitHub maintainer)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to

## Knowledge Gaps
- **57 isolated node(s):** `ra-prng`, `Array Index Shuffling`, `Bitwise Rotation`, `Bit Shifting`, `XORing` (+52 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **9 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Hamas A. Rahman` and `hamzy hams (GitHub maintainer)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `ra_prng2 (Array-Based PRNG)` connect `Cryptographic and PRNG Architecture` to `Initialization and Golden Ratio Constants`, `NIST Statistical Test Suite`, `Branchless CPU Core Loop`, `Shannon Entropy Foundations`, `Lemire Fast Reduction`, `Dieharder Test Suite`, `ent Entropy Tool`, `State Array and Reseed Hash Mechanism`, `PRNG Periodicity Literature and Theory`, `Avalanche Effect and Cryptographic Metrics`, `State Transition and Mutation Operators`?**
  _High betweenness centrality (0.021) - this node is a cross-community bridge._
- **Why does `PRNG Periodicity Analysis` connect `PRNG Periodicity Literature and Theory` to `Cryptographic and PRNG Architecture`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Are the 3 inferred relationships involving `Params` (e.g. with `capture_inner_outputs()` and `run_avalanche_check()`) actually correct?**
  _`Params` has 3 INFERRED edges - model-reasoned connections that need verification._
- **Are the 2 inferred relationships involving `ra_prng2 (32-bit stable version)` (e.g. with `ra_prng2 PractRand Test Result (up to 128 GB, no anomalies)` and `ra_prng3 (experimental 64-bit version)`) actually correct?**
  _`ra_prng2 (32-bit stable version)` has 2 INFERRED edges - model-reasoned connections that need verification._
- **What connects `ra-prng`, `Array Index Shuffling`, `Bitwise Rotation` to the rest of the system?**
  _57 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Avalanche and Bit-Flip Testing` be split into smaller, more focused modules?**
  _Cohesion score 0.09146341463414634 - nodes in this community are weakly interconnected._