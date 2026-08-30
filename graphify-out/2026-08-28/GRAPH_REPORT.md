# Graph Report - ra_prng  (2026-08-28)

## Corpus Check
- 115 files · ~485,490 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 979 nodes · 1519 edges · 110 communities (87 shown, 23 thin omitted)
- Extraction: 94% EXTRACTED · 6% INFERRED · 0% AMBIGUOUS · INFERRED: 85 edges (avg confidence: 0.89)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `3a2167fc`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- Wiring
- toy_prng.py
- Candidate
- pruned_winner 4-Operation Fast Variant
- Operand Position Search Experiment Results
- other/avalanche_heatmap_original.py
- ra_prng3 PRNG Generator
- 2026-8-25_periodicity-heuristic-validation/cycle_measure.c
- 2026-8-28_periodicity-heuristic-validation2/cycle_measure.c
- winner_wired_v2.c
- ICCS 2026 Research Paper: Array-Native Randomness
- avalanche_effect_analysis.py
- others/avalanche_heatmap_original.py
- ra_prng2 (Array-Based PRNG)
- RaPrng2
- BuildPyWithCEngine
- scc_test.py
- pruned_prng.c
- pcg_amortized.c
- source/ra_prng2.c
- source/ra_prng3.c
- scrambler.c
- pruned_winner.c
- pruned_winner_refactored.c
- baseline.c
- baseline_refactored.c
- v01.c
- v02.c
- v03.c
- v04.c
- v06.c
- v07.c
- v08.c
- v09.c
- v10.c
- v11.c
- v12.c
- v13.c
- winner_wired.c
- ra_prng2_struct.c
- c/ra_prng2.c
- ra_prng2_thread.c
- philox_amortized.c
- ra_prng2 API Reference
- Fisher–Yates Shuffle
- ent (Entropy Testing Tool)
- xoshiro256_amortized.c
- scrambler.py
- PRNG Periodicity Analysis
- source/chacha20.c
- pcg.c
- source/philox.c
- source/xoshiro256.c
- ACM (Association for Computing Machinery)
- test_ra_hash.py
- pcg32.c
- src/xoshiro256.c
- chacha20_amortized.c
- Cycle 3 Converged Avalanche Fraction (0.500751)
- Avalanche Effect Analysis (ALL_OPS)
- heatmap.py
- State Transition Operator F
- ra_prng2.py
- c/ra_prng3.c
- src/chacha20.c
- src/philox.c
- Cycle 3 Avalanche Fraction (0.470588)
- Avalanche Metrics (overall=0.487783, min_bit=0.472304)
- Cycle 4 Steady State Avalanche (fraction 0.499713)
- Avalanche Effect Heatmap (Original Algorithm)
- Seed and Array Initialization (Algorithm 1)
- Graphify Knowledge Graph Rule
- Cycle 1 vs Cycle 5 Avalanche Fraction Convergence
- Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)
- Greedy Operation-Pruning Search Paradigm
- Original Algorithm Avalanche Bit Distribution & Diffusion
- other_winners_perf.sh
- other_winners_practrand.sh
- CI Build and Test Job
- NIST Statistical Test Suite (STS)
- ra_core (Main Generation Loop)
- Shannon Entropy
- Lemire's Fast Reduction
- Dieharder Test Suite
- ent Entropy Tool
- conftest.py
- /dev/urandom RNG Speed Benchmark Result
- PCG32 RNG Speed Benchmark Result
- Philox4x32 RNG Speed Benchmark Result
- xoshiro256** RNG Speed Benchmark Result
- Project Root Changelog
- rows=2 Configuration Log
- ra-prng
- ra_prng2 Package Changelog
- ra_prng3 Package Changelog
- Image Encrypting Documentation
- next_state
- Params
- 2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py
- Design decisions (read before touching `pruned_wired_toy_prng.py`)
- Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)
- 2026-8-25_periodicity-heuristic-validation/avalanche_check.py
- 2026-8-28_periodicity-heuristic-validation2/enumerate_n4w4.py
- 2026-8-25_periodicity-heuristic-validation/run_sweep.py
- 2026-8-28_periodicity-heuristic-validation2/avalanche_check.py
- 2026-8-28_periodicity-heuristic-validation2/run_sweep.py
- STATUS: periodicity-heuristic-validation2

## God Nodes (most connected - your core abstractions)
1. `Params` - 33 edges
2. `ra_prng2 (Array-Based PRNG)` - 30 edges
3. `Candidate` - 20 edges
4. `Operand Position Search Experiment Results` - 19 edges
5. `Wiring` - 18 edges
6. `stream()` - 17 edges
7. `next_state()` - 14 edges
8. `pruned_winner 4-Operation Fast Variant` - 14 edges
9. `init_state()` - 12 edges
10. `next_state()` - 11 edges

## Surprising Connections (you probably didn't know these)
- `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` --semantically_similar_to--> `Candidate`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/avalanche_heatmap_pruned_winner_cycle1_vs_cycle2.png → experiments/2026-8-26_operation-pruning-research/pruned_prng.py
- `winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039)` --conceptually_related_to--> `Avalanche Effect (Bit-Flip Sensitivity)`  [INFERRED]
  experiments/2026-8-27_operand-position-search/avalanche_heatmap_winner_v2.png → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf
- `pruned_winner 4-Operation Fast Variant` --semantically_similar_to--> `ra_prng2 32-bit Generator`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/RESULTS.md → README.md
- `Data Fingerprinting Application Primitive` --cites--> `ICCS 2026 Research Paper: Array-Native Randomness`  [EXTRACTED]
  sub-projects/fingerprinting_tools/README.md → research/iccs2026_ra_prng/iccs_ra_prng.pdf
- `Lemire Fast Modulo Reduction Integration` --conceptually_related_to--> `ZepFold Shuffling CLI Tool`  [INFERRED]
  research/iccs2026_ra_prng/iccs_ra_prng.pdf → benchmarks/comparisons/source/README.md

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_xoshiro256, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_philox4x32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]
- **ra_prng2 Statistical Validation Test Suite** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_nist_sts, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_dieharder, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_practrand, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_bigcrush [EXTRACTED 0.90]
- **ra_prng2 State Transition Operator Composition (F = Mutate∘Permute∘Reseed)** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_transition_operator_f, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_mutate_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_permute_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_reseed_operator [EXTRACTED 0.95]
- **pruned_winner Cycle 1 vs Cycle 90 Avalanche Analysis** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_reseed_diffusion_result, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_avalanche_fraction [EXTRACTED 1.00]
- **winner_wired Avalanche Diffusion Convergence Across Cycles** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_cycle1_avalanche, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_cycle4_avalanche [EXTRACTED 1.00]
- **Winner Wired PRNG Avalanche Effect Analysis** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_avalanche_performance [EXTRACTED 1.00]
- **Avalanche Effect Cycle Comparison** — experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_avalanche_effect_analysis, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_1_avalanche, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_2_avalanche [EXTRACTED 1.00]
- **Original Algorithm Avalanche Cycle Evolution (Cycle 1 vs Cycle 3)** — experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_heatmap, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_avalanche_diffusion_comparison [EXTRACTED 1.00]
- **pruned_winner Avalanche Analysis and Metric Evaluation** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_figure, experiments_2026_8_26_operation_pruning_research_pruned_winner_avalanche_behavior, experiments_2026_8_26_operation_pruning_research_pruned_winner_candidate_configuration [EXTRACTED 1.00]
- **Array-Native Data Permutation and Fingerprinting Workflow** — research_iccs2026_ra_prng_avalanche_sensitivity, sub_projects_fingerprinting_tools_concept, benchmarks_comparisons_source_zepfold_cli [INFERRED 0.75]
- **ra_prng2 Original Benchmark Report Set** — benchmarks_results_ra_prng2_original_rnging_perf_stat_, benchmarks_results_ra_prng2_original_testu01, benchmarks_results_ra_prng2_original_dieharder_test, benchmarks_results_ra_prng2_original_shuffling_perf_stat [INFERRED 0.80]
- **ra_prng3 Benchmark Report Set** — benchmarks_results_ra_prng3_nist_sts_test, benchmarks_results_ra_prng3_dieharder_test, benchmarks_results_ra_prng3_ent_test, benchmarks_results_ra_prng3_perf_stat_rnging, benchmarks_results_ra_prng3_perf_stat_scrambling, benchmarks_results_ra_prng3_practrand_test [INFERRED 0.80]
- **ChaCha20 Speed + Entropy Benchmark Suite** — benchmarks_comparisons_rnging_speed_chacha20_result, benchmarks_comparisons_entropy_chacha20_result [INFERRED 0.85]
- **Pruned Winner Avalanche Progression Analysis (Cycle 1 vs Cycle 2)** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_heatmap, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle1_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle2_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_avalanche_strengthening [INFERRED 0.85]
- **Winner Wired Warmup Cycle Avalanche Progression** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_winner_wired_wiring, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle1_avalanche, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle3_avalanche [INFERRED 0.85]
- **Avalanche Effect Bit Diffusion Evaluation** — experiments_others_avalanche_effect_heatmap_figure, experiments_others_avalanche_effect_heatmap_avalanche_effect, experiments_others_avalanche_effect_heatmap_hamming_distance_analysis, experiments_others_avalanche_effect_heatmap_original_algorithm [INFERRED 0.85]
- **Periodicity Heuristic Empirical Validation Lifecycle** — research_iccs2026_ra_prng_periodicity_heuristic, experiments_2026_8_25_periodicity_heuristic_validation_toy_model_spec, experiments_2026_8_25_periodicity_heuristic_validation_scaling_verdict [INFERRED 0.85]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_practrand, concept_ent [INFERRED 0.85]
- **Pruned Winner Cycle 1 vs Cycle 3 Avalanche Comparison** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_reseed_avalanche_diffusion [INFERRED 0.95]
- **Avalanche Proxy vs Full Empirical Quality Disconnect** — experiments_2026_8_26_operation_pruning_research_results_shift_repair_failure, experiments_2026_8_26_operation_pruning_research_status_gate_size_traps, experiments_2026_8_27_operand_position_search_handover_dead_seed_bits, experiments_2026_8_27_operand_position_search_handover_avalanche_heatmap_baseline [INFERRED]
- **Pruned Winner Multi-battery Statistical Validation** — experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_26_operation_pruning_research_practrand_pruned_winner_1tb_practrand_1tb_log, experiments_2026_8_26_operation_pruning_research_dieharder_pruned_winner_piped_piped_results, experiments_2026_8_26_operation_pruning_research_dieharder_gcd_fixed_gcd_test_log, experiments_2026_8_26_operation_pruning_research_dieharder_rgb_minimum_distance_fixed_rgb_min_dist_log [INFERRED]
- **ra_prng Algorithmic Evolution and Research Pipeline** — readme_ra_prng_architecture, readme_ra_prng2, experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_27_operand_position_search_handover_handover_spec [INFERRED]

## Communities (110 total, 23 thin omitted)

### Community 0 - "Wiring"
Cohesion: 0.07
Nodes (57): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for the winning operand-position rewiring found by…, Mirrors winner_wired.c's ra_reseed()/ra_hash() exactly (identical to… (+49 more)

### Community 1 - "toy_prng.py"
Cohesion: 0.26
Nodes (13): brent(), Brent's cycle-detection algorithm applied to toy_prng.next_state. Measures the…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, enumerate_state_space(), main(), Full state-space enumeration for n=2,w=4 (|S|=131072, default rows=2). Directly…, init_state(), next_state() (+5 more)

### Community 2 - "Candidate"
Cohesion: 0.06
Nodes (62): _log(), main(), phase_a_hash_access_check(), phase_b_greedy_ablation(), Driver for the operation-pruning greedy ablation search. Two phases: Phase A:…, analyze_cycle(), compute_avalanche_matrix(), Reseed-Driven Avalanche Strengthening Effect (+54 more)

### Community 3 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.08
Nodes (33): Scrambling Speed Benchmark (perf stat, per-call vs amortized, 30 runs), ra_prng2 NIST STS Test Result, ra_prng2 PractRand Test Result (up to 128 GB, no anomalies), ra_prng Build and Test Workflow, Contributing Guide, ra_prng Implementation Variant Roles, Repository Structure Specification, Brent Cycle Measure Checkpoint/Resume Architecture (+25 more)

### Community 4 - "Operand Position Search Experiment Results"
Cohesion: 0.07
Nodes (33): baseline_local PractRand 16GB Test Log, v01 PractRand 16GB Test Log (FAIL), v02 PractRand 16GB Test Log (FAIL), v03 PractRand 16GB Test Log (Pass), v04 PractRand 16GB Test Log (Pass), v06 PractRand 16GB Test Log (Pass), v07 PractRand 16GB Test Log (Pass), v08 PractRand 16GB Test Log (Pass) (+25 more)

### Community 5 - "other/avalanche_heatmap_original.py"
Cohesion: 0.14
Nodes (21): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), Original Algorithm Avalanche Diffusion Comparison (Cycle 1 vs Cycle 3), Original Algorithm Cycle 1 Avalanche Fraction (0.497576), Original Algorithm Cycle 3 Avalanche Fraction (0.499740), Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3, main() (+13 more)

### Community 6 - "ra_prng3 PRNG Generator"
Cohesion: 0.14
Nodes (21): Dieharder Test Results (ra_prng2 original), RNGing perf stat (ra_prng2 original), Shuffling perf stat (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), RNGing perf stat (ra_prng3) (+13 more)

### Community 7 - "2026-8-25_periodicity-heuristic-validation/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 8 - "2026-8-28_periodicity-heuristic-validation2/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 9 - "winner_wired_v2.c"
Cohesion: 0.13
Nodes (20): pruned_winner Avalanche Fraction Metric (Cycle 1 vs Cycle 90), Avalanche Effect Heatmap: pruned_winner Cycle 1 vs Cycle 90, Multi-Cycle Reseed Avalanche Diffusion Improvement (0.418 to 0.468), winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039), Avalanche Effect Heatmap (winner_wired_v2), winner_wired_v2 Wiring Parameters (a_xor=d, c_shift=a, rotc_amount=b, rotc_xor=a), FILE, main() (+12 more)

### Community 10 - "ICCS 2026 Research Paper: Array-Native Randomness"
Cohesion: 0.12
Nodes (19): Benchmark Source Comparisons Documentation, ZepFold Shuffling CLI Tool, Toy PRNG Avalanche Sanity Verification Gate, Brent Cycle Detection Methodology, Periodicity Validation Experiment Handover, Periodicity Heuristic Validation Results Report, Periodicity Heuristic Scalability & Non-Monotonicity Verdict, Initial Sweep Periodicity Measurement Summary (+11 more)

### Community 11 - "avalanche_effect_analysis.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_reseed(), plot_comparison(), plot_single(), Path (+9 more)

### Community 12 - "others/avalanche_heatmap_original.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_capture(), original_reseed(), plot_comparison(), plot_single() (+9 more)

### Community 13 - "ra_prng2 (Array-Based PRNG)"
Cohesion: 0.12
Nodes (17): ACM (Association for Computing Machinery), BigCrush (TestU01), Bit Shifting, ChaCha20, CSAI 2025 (9th Int'l Conf. on Computer Science and AI), ra_prng GitHub Repository, Hamas A. Rahman, hamzy hams (GitHub maintainer) (+9 more)

### Community 14 - "RaPrng2"
Cohesion: 0.18
Nodes (11): _find_lib_path(), _lib_handle(), _load_library(), Path, RaPrng2, ctypes binding for the ra_prng2 C engine. This is glue only - it marshals calls…, Thin wrapper around one RA_PRNG instance., CDLL (+3 more)

### Community 15 - "BuildPyWithCEngine"
Cohesion: 0.20
Nodes (7): _build_py, Distribution, BinaryDistribution, BuildPyWithCEngine, Imperative build customization pyproject.toml's static [tool.setuptools] table…, After the normal .py copy step, compiles the ra_prng2 C engine and drops…, Declares this distribution as containing compiled, platform-specific code…

### Community 16 - "scc_test.py"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 17 - "pruned_prng.c"
Cohesion: 0.38
Nodes (10): build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32(), FILE, has() (+2 more)

### Community 18 - "pcg_amortized.c"
Cohesion: 0.44
Nodes (8): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), pcg32_buffered_t

### Community 19 - "source/ra_prng2.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 20 - "source/ra_prng3.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 21 - "scrambler.c"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 22 - "pruned_winner.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 23 - "pruned_winner_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 24 - "baseline.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 25 - "baseline_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 26 - "v01.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 27 - "v02.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 28 - "v03.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 29 - "v04.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 30 - "v06.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 31 - "v07.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 32 - "v08.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 33 - "v09.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 34 - "v10.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 35 - "v11.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 36 - "v12.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 37 - "v13.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 38 - "winner_wired.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 39 - "ra_prng2_struct.c"
Cohesion: 0.44
Nodes (8): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), ra_step(), rot32()

### Community 40 - "c/ra_prng2.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 41 - "ra_prng2_thread.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 42 - "philox_amortized.c"
Cohesion: 0.46
Nodes (7): main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32(), philox_refill_buffer(), rand_bounded()

### Community 43 - "ra_prng2 API Reference"
Cohesion: 0.36
Nodes (8): CPython Binding Documentation, Orphaned CPython Shared Object Rationale, RaPrng2 ctypes Wrapper, ra_prng2 API Reference, ra_core PRNG Core Loop, ra_hash State Reduction Function, RA_PRNG C Struct API, rot32 Left Bitwise Rotation

### Community 44 - "Fisher–Yates Shuffle"
Cohesion: 0.25
Nodes (8): Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), R. Durstenfeld, Fisher–Yates Shuffle, Rationale: Non-Invertible Hash Reseed Trade-off (Reduces Period, Aids Diffusion), ra_hash (Reseed Hash Function), Reseed Operator, Internal State S

### Community 45 - "ent (Entropy Testing Tool)"
Cohesion: 0.29
Nodes (7): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result

### Community 46 - "xoshiro256_amortized.c"
Cohesion: 0.52
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next()

### Community 47 - "scrambler.py"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 48 - "PRNG Periodicity Analysis"
Cohesion: 0.29
Nodes (7): B. Harris, Donald E. Knuth, Makoto Matsumoto and Takuji Nishimura, Mersenne Twister (MT19937), PRNG Periodicity Analysis, Pierre L'Ecuyer, Random Mapping Theorem

### Community 49 - "source/chacha20.c"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 50 - "pcg.c"
Cohesion: 0.67
Nodes (5): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded()

### Community 51 - "source/philox.c"
Cohesion: 0.60
Nodes (5): main(), philox4x32_10(), philox4x32_round(), philox_rand32(), rand_bounded()

### Community 52 - "source/xoshiro256.c"
Cohesion: 0.60
Nodes (5): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 53 - "ACM (Association for Computing Machinery)"
Cohesion: 0.33
Nodes (6): ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex, Hamas A. Rahman, ra_prng2 and Beyond (Paper CA-162), Scopus

### Community 54 - "test_ra_hash.py"
Cohesion: 0.33
Nodes (3): Pure-Python unit tests for ra_prng2.ra_hash / ra_core. These do not require the…, Regression test for the historical bug: ra_hash must be a pure function writing…, test_ra_hash_does_not_mutate_input()

### Community 55 - "pcg32.c"
Cohesion: 0.70
Nodes (4): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom()

### Community 56 - "src/xoshiro256.c"
Cohesion: 0.70
Nodes (4): main(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 57 - "chacha20_amortized.c"
Cohesion: 0.60
Nodes (3): chacha20_block(), main(), rand_bounded()

### Community 58 - "Cycle 3 Converged Avalanche Fraction (0.500751)"
Cohesion: 0.60
Nodes (5): Warmup Cycle Avalanche Effect and Bit Diffusion, Cycle 1 Initial State Avalanche Fraction (0.487783), Cycle 3 Converged Avalanche Fraction (0.500751), Winner Wired Avalanche Heatmap Cycle 1 vs Cycle 3, Winner Wired Wiring Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a)

### Community 59 - "Avalanche Effect Analysis (ALL_OPS)"
Cohesion: 0.50
Nodes (5): Avalanche Effect Analysis (ALL_OPS), Bit Difference Distribution Across 32 Flipped Bits, Cycle 1 Avalanche Fraction (0.497576), Cycle 2 Avalanche Fraction (0.502141), Avalanche Effect Heatmap (Cycle 1 vs Cycle 2)

### Community 60 - "heatmap.py"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 61 - "State Transition Operator F"
Cohesion: 0.40
Nodes (5): Array Index Shuffling, Bitwise Rotation (rotl32), Mutate Operator, Permute Operator, State Transition Operator F

### Community 62 - "ra_prng2.py"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 63 - "c/ra_prng3.c"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 65 - "src/philox.c"
Cohesion: 0.83
Nodes (3): main(), philox4x32_10(), philox4x32_round()

### Community 66 - "Cycle 3 Avalanche Fraction (0.470588)"
Cohesion: 0.83
Nodes (4): Cycle 1 Avalanche Fraction (0.418459), Cycle 3 Avalanche Fraction (0.470588), Avalanche Heatmap: Cycle 1 vs Cycle 3 (pruned_winner), Reseed Avalanche Diffusion in Pruned Winner

### Community 67 - "Avalanche Metrics (overall=0.487783, min_bit=0.472304)"
Cohesion: 0.83
Nodes (4): Avalanche Metrics (overall=0.487783, min_bit=0.472304), Early Output Index (0-5) Diffusion Transient, Avalanche Effect Heatmap: winner_wired, winner_wired Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a)

### Community 68 - "Cycle 4 Steady State Avalanche (fraction 0.499713)"
Cohesion: 0.83
Nodes (4): Cycle 1 Initial State Avalanche (fraction 0.487783), Cycle 4 Steady State Avalanche (fraction 0.499713), Avalanche Effect Heatmap Comparison: Cycle 1 vs Cycle 4, winner_wired Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a)

### Community 69 - "Avalanche Effect Heatmap (Original Algorithm)"
Cohesion: 0.67
Nodes (4): Avalanche Effect, Avalanche Effect Heatmap (Original Algorithm), Hamming Distance Analysis, Original PRNG Algorithm

### Community 70 - "Seed and Array Initialization (Algorithm 1)"
Cohesion: 0.50
Nodes (4): Rationale: Golden-Ratio Constants Reduce Collisions, Dead States, Short Cycles, PCG32, Seed and Array Initialization (Algorithm 1), xoshiro256**

### Community 71 - "Graphify Knowledge Graph Rule"
Cohesion: 0.67
Nodes (3): Graphify Query & Navigation Protocol, Graphify Knowledge Graph Rule, Graphify Pipeline Workflow

### Community 72 - "Cycle 1 vs Cycle 5 Avalanche Fraction Convergence"
Cohesion: 1.00
Nodes (3): Cycle 1 vs Cycle 5 Avalanche Fraction Convergence, Avalanche Heatmap: Pruned Winner Cycle 1 vs Cycle 5, Pruned Winner Architecture (TAP6, TAP7, ROT_C, SHR13)

### Community 73 - "Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)"
Cohesion: 1.00
Nodes (3): Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13), pruned_winner Cycle 1 Avalanche Sensitivity and Dead Zones, pruned_winner 4-Op Candidate Configuration (TAP6, TAP7, ROT_C, SHR13)

### Community 74 - "Greedy Operation-Pruning Search Paradigm"
Cohesion: 1.00
Nodes (3): Greedy Operation-Pruning Search Paradigm, Operation-Pruning Research Handover Spec, Limits of Zero-Drift Micro-Optimizations on ra_core

### Community 99 - "next_state"
Cohesion: 0.18
Nodes (14): Brent's cycle-detection algorithm applied to pruned_wired_toy_prng.next_state.…, enumerate_state_space(), main(), Full state-space enumeration for pruned/wired PRNG at n=2,w=4 (|S|=131072,…, next_state(), State, ra_hash_gen_sequential(), Toy-scale (n, w, rows)-parameterized port of winner_wired_v2.c's permutation… (+6 more)

### Community 100 - "Params"
Cohesion: 0.15
Nodes (10): group_size(), Params, Rescale the four fixed 32-bit plain-shift amounts to width w. s_w = round(s * w…, Top-w-bits truncation of a 32-bit golden-ratio-derived constant. Trap fix:…, G(n, rows): generalization of ra_hash's fixed 8/32 structural constants (256 =…, rescale_shifts(), truncate_const(), brent() (+2 more)

### Community 101 - "2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py"
Cohesion: 0.23
Nodes (12): decode_batch(), main(), pack_key(), Full state-space enumeration for n=4,w=4 (default rows=4 -> G=1), |S| = 4! *…, Cross-check vec_next_state against the scalar reference on random states drawn…, Pack (L, M, cons, it) arrays into a single uint64 key array, w bits per field,…, Mixed-radix decompose a flat index array into (lperm, m_0..m_{n-1}, it)…, Vectorized rotw: rotate low w bits of x left by r bits (mod w). x, r are numpy… (+4 more)

### Community 102 - "Design decisions (read before touching `pruned_wired_toy_prng.py`)"
Cohesion: 0.17
Nodes (11): 1. Op/wiring mapping ported to toy scale, 2. Tap-survivor generalization: top-2-by-count, and why, 3. Tap shift-width collapse (caught during design, not present in either parent), 4. `ra_hash_gen` divergence from ground truth (found while reading source, not assumed), 5. Self-check (passed) -- MUST stay passing, CRITICAL BUG found and fixed this session -- read before writing more scripts, Design decisions (read before touching `pruned_wired_toy_prng.py`), HANDOVER: periodicity-heuristic-validation2 (+3 more)

### Community 103 - "Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)"
Cohesion: 0.18
Nodes (10): 1. Summary Comparison: Unpruned Original vs `winner_wired_v2`, 2. Sebaran Siklus Utama (*Distinct $\lambda$ Distribution*), 3. Enumerasi Penuh & Uji Chi-Square vs Poisson(1), 4. Analisis Temuan Kunci (*Key Findings*), 5. Catatan Metodologi & Caveats (Sesuai `HANDOVER.md`), 6. Kesimpulan (*Verdict*), A. Skala $n=2, w=4$ ($|S| = 131.072$) — [`enumerate_n2w4_result.json`](./enumerate_n2w4_result.json), B. Skala $n=4, w=4$ ($|S| = 402.653.184$) — [`enumerate_n4w4_result.json`](./enumerate_n4w4_result.json) (+2 more)

### Community 104 - "2026-8-25_periodicity-heuristic-validation/avalanche_check.py"
Cohesion: 0.31
Nodes (8): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for toy_prng.py. Port of…, One outer iteration, no rehash - mirrors zepfold_capture() in…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), Rotate the low w bits of x left by r bits (mod w)., rotw()

### Community 105 - "2026-8-28_periodicity-heuristic-validation2/enumerate_n4w4.py"
Cohesion: 0.39
Nodes (8): decode_batch(), main(), pack_key(), Full state-space enumeration for pruned/wired PRNG at n=4,w=4 (default rows=4…, Cross-check vec_next_state against the scalar reference on random states drawn…, rotw_vec(), validate(), vec_next_state()

### Community 106 - "2026-8-25_periodicity-heuristic-validation/run_sweep.py"
Cohesion: 0.48
Nodes (6): log2_fact(), main(), predicted(), Drives the full periodicity-heuristic-validation sweep. For each measurable (n,…, run_c(), run_python()

### Community 107 - "2026-8-28_periodicity-heuristic-validation2/avalanche_check.py"
Cohesion: 0.38
Nodes (6): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for pruned_wired_toy_prng.py.…, One outer iteration, no rehash -- mirrors next_state's inner loop exactly (tap-…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check()

### Community 108 - "2026-8-28_periodicity-heuristic-validation2/run_sweep.py"
Cohesion: 0.48
Nodes (6): log2_fact(), main(), predicted(), Drives the pruned/wired periodicity-heuristic sweep. Same lambda_pred formula…, result_csv_name(), run_c()

## Ambiguous Edges - Review These
- `Hamas A. Rahman` → `hamzy hams (GitHub maintainer)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to

## Knowledge Gaps
- **121 isolated node(s):** `other_winners_perf.sh script`, `other_winners_practrand.sh script`, `ra-prng`, `What this is`, `Non-goals` (+116 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **23 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Hamas A. Rahman` and `hamzy hams (GitHub maintainer)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `Avalanche Effect (Bit-Flip Sensitivity)` connect `winner_wired_v2.c` to `ra_prng2 (Array-Based PRNG)`?**
  _High betweenness centrality (0.034) - this node is a cross-community bridge._
- **Why does `ra_prng2 (Array-Based PRNG)` connect `ra_prng2 (Array-Based PRNG)` to `Seed and Array Initialization (Algorithm 1)`, `winner_wired_v2.c`, `Fisher–Yates Shuffle`, `NIST Statistical Test Suite (STS)`, `ra_core (Main Generation Loop)`, `PRNG Periodicity Analysis`, `Lemire's Fast Reduction`, `Dieharder Test Suite`, `ent Entropy Tool`, `Shannon Entropy`, `State Transition Operator F`?**
  _High betweenness centrality (0.033) - this node is a cross-community bridge._
- **Why does `Avalanche Effect Heatmap: pruned_winner Cycle 1 vs Cycle 90` connect `winner_wired_v2.c` to `Candidate`?**
  _High betweenness centrality (0.028) - this node is a cross-community bridge._
- **Are the 7 inferred relationships involving `Params` (e.g. with `capture_inner_outputs()` and `run_avalanche_check()`) actually correct?**
  _`Params` has 7 INFERRED edges - model-reasoned connections that need verification._
- **Are the 8 inferred relationships involving `Candidate` (e.g. with `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` and `avalanche_gate()`) actually correct?**
  _`Candidate` has 8 INFERRED edges - model-reasoned connections that need verification._
- **Are the 9 inferred relationships involving `Wiring` (e.g. with `_capture_first_cycle()` and `tier0_avalanche()`) actually correct?**
  _`Wiring` has 9 INFERRED edges - model-reasoned connections that need verification._