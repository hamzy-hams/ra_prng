# Graph Report - ra_prng  (2026-08-30)

## Corpus Check
- 18 files · ~511,987 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 1277 nodes · 1980 edges · 128 communities (104 shown, 24 thin omitted)
- Extraction: 96% EXTRACTED · 4% INFERRED · 0% AMBIGUOUS · INFERRED: 82 edges (avg confidence: 0.89)
- Token cost: 77,210 input · 0 output

## Community Hubs (Navigation)
- Candidate
- Wiring
- toy_prng.py
- pruned_winner 4-Operation Fast Variant
- STATUS: periodicity-heuristic-validation2
- next_state
- 2026-8-25_periodicity-heuristic-validation/cycle_measure.c
- Operand Position Search Experiment Results
- BuildPyWithCEngine
- ra_prng3 PRNG Generator
- other/avalanche_heatmap_original.py
- 2026-8-28_periodicity-heuristic-validation2/cycle_measure.c
- winner_wired_v2.c
- others/avalanche_heatmap_original.py
- RaPrng2
- ra_prng2 (Array-Based PRNG)
- Design decisions (read before touching `pruned_wired_toy_prng.py`)
- pruned_winner 4-Operation Fast Variant
- scc_test.py
- pruned_prng.c
- Cycle 3 Converged Avalanche Fraction (0.500751)
- Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)
- STATUS: periodicity-heuristic-validation2
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
- philox_amortized.c
- ra_prng2 API Reference
- original_m_inject Candidate (Fase M)
- original_m_overwrite Candidate (Fase M)
- original_m_permute Candidate (Fase M)
- avalanche_effect_analysis.py
- ACM (Association for Computing Machinery)
- pruned_winner 4-Operation Fast Variant
- winner_m_inject Candidate (Fase M)
- winner_m_overwrite Candidate (Fase M)
- winner_m_permute Candidate (Fase M)
- Fisher–Yates Shuffle
- ent (Entropy Testing Tool)
- xoshiro256_amortized.c
- scrambler.py
- PRNG Periodicity Analysis
- Design decisions (read before touching `pruned_wired_toy_prng.py`)
- source/chacha20.c
- pcg.c
- M-Mechanism Variant Generator
- source/philox.c
- source/xoshiro256.c
- test_ra_hash.py
- pcg32.c
- chacha20_amortized.c
- Auto-Stop Watchdog Script
- Avalanche Effect Analysis (ALL_OPS)
- heatmap.py
- State Transition Operator F
- ra_prng2.py
- c/ra_prng3.c
- src/chacha20.c
- src/philox.c
- Cycle 3 Avalanche Fraction (0.470588)
- Periodicity Heuristic Research Question
- Tap-Survivor Design Decisions
- Avalanche Effect Heatmap (Original Algorithm)
- Seed and Array Initialization (Algorithm 1)
- Graphify Knowledge Graph Rule
- Graphify Workflow Policy
- Cycle 1 vs Cycle 5 Avalanche Fraction Convergence
- Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)
- Greedy Operation-Pruning Search Paradigm
- dev_urandom.c
- Original Algorithm Avalanche Bit Distribution & Diffusion
- other_winners_perf.sh
- CI Build and Test Job
- ra_core (Main Generation Loop)
- Lemire's Fast Reduction
- ent Entropy Tool
- conftest.py
- /dev/urandom RNG Speed Benchmark Result
- PCG32 RNG Speed Benchmark Result
- Philox4x32 RNG Speed Benchmark Result
- cpython/__init__.py
- M-Mechanism Perf Script
- Phase 3 Tractability Probe Script
- M-Mechanism Toy Sweep Script
- Phase 3 Toy Sweep Script
- Project Root Changelog
- Op/Wiring Mapping Design
- Corrected Re-run Stderr Log
- Corrected Re-run Stdout Log
- Short Transient Tail Finding
- ra-prng
- ra_prng2 Package Changelog
- ra_prng3 Package Changelog
- Image Encrypting Documentation
- Toy-Scale Op/Wiring Port Decision
- n8w4rows4 Rerun Stderr Log
- n8w4rows4 Rerun Stdout Log
- Short Transient Tail Finding
- ra_prng3 Changelog
- Image Encrypting Sub-Project Docs
- ra_prng2 Changelog
- ra_prng3 Changelog
- Image Encrypting Sub-project

## God Nodes (most connected - your core abstractions)
1. `ra_prng2 (Array-Based PRNG)` - 30 edges
2. `State-Update Mechanism Research Results (Phase 1-3)` - 23 edges
3. `Params` - 22 edges
4. `Parallelization Research Handover` - 21 edges
5. `Candidate` - 20 edges
6. `Operand Position Search Experiment Results` - 19 edges
7. `Wiring` - 18 edges
8. `stream()` - 17 edges
9. `pruned_winner 4-Operation Fast Variant` - 14 edges
10. `next_state()` - 13 edges

## Surprising Connections (you probably didn't know these)
- `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` --semantically_similar_to--> `Candidate`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/avalanche_heatmap_pruned_winner_cycle1_vs_cycle2.png → experiments/2026-8-26_operation-pruning-research/pruned_prng.py
- `hamzy hams (GitHub maintainer)` --conceptually_related_to--> `Hamas A. Rahman`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf → README.md
- `winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039)` --conceptually_related_to--> `Avalanche Effect (Bit-Flip Sensitivity)`  [INFERRED]
  experiments/2026-8-27_operand-position-search/avalanche_heatmap_winner_v2.png → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf
- `Hamas A. Rahman` --references--> `ra_prng2 (Array-Based PRNG)`  [EXTRACTED]
  README.md → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf
- `pruned_winner 4-Operation Fast Variant` --semantically_similar_to--> `ra_prng2 32-bit Generator`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/RESULTS.md → README.md

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Q1-Q4 Parallelization Research Questions** — concept_stream_independence_testing, concept_seed_collision_probability, concept_throughput_scalability, concept_simd_vectorization_feasibility [INFERRED 0.85]
- **Harness Scripts Wrapping winner_wired_v2 CLI Without Modifying Source** — experiments_2026_8_29_parallelization_research_common, experiments_2026_8_29_parallelization_research_cross_correlation, experiments_2026_8_29_parallelization_research_interleave_practrand, experiments_2026_8_29_parallelization_research_collision_scan, experiments_2026_8_29_parallelization_research_wall_time_scaling, experiments_2026_8_29_parallelization_research_perf_scaling, experiments_2026_8_29_parallelization_research_simd_prototype [EXTRACTED 1.00]
- **Prior Research Folders That Tested Only Single-Stream Axes** — concept_periodicity_heuristic_validation_research, concept_operation_pruning_research, concept_operand_position_search_research, concept_state_update_mechanism_research, concept_periodicity_heuristic_validation2_research [EXTRACTED 1.00]
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_xoshiro256, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_philox4x32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]
- **ra_prng2 Statistical Validation Test Suite** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_practrand, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_bigcrush [EXTRACTED 0.90]
- **ra_prng2 State Transition Operator Composition (F = Mutate∘Permute∘Reseed)** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_transition_operator_f, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_mutate_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_permute_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_reseed_operator [EXTRACTED 0.95]
- **Bit-identical self-check chain linking pruned toy model to full-scale reference and winner C source** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_self_check_full_scale, experiments_2026_8_28_periodicity_heuristic_validation2_handover_pruned_wired_toy_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wired_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wiring_module, experiments_2026_8_28_periodicity_heuristic_validation2_handover_winner_wired_v2 [EXTRACTED 1.00]
- **2026-08-29 correction of (8,4,rows=4) lambda bound, documented across HANDOVER/RESULTS/STATUS** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_2026_08_29_correction, experiments_2026_8_28_periodicity_heuristic_validation2_handover_stale_lambda_lower_bound, experiments_2026_8_28_periodicity_heuristic_validation2_handover_corrected_lambda_value, experiments_2026_8_28_periodicity_heuristic_validation2_handover_checkpoint_file, experiments_2026_8_28_periodicity_heuristic_validation2_results_footnote_correction, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_summary, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_lower_bound_restated [EXTRACTED 1.00]
- **pruned_winner Cycle 1 vs Cycle 90 Avalanche Analysis** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_reseed_diffusion_result, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_avalanche_fraction [EXTRACTED 1.00]
- **winner_wired Avalanche Diffusion Convergence Across Cycles** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_cycle4_avalanche [EXTRACTED 1.00]
- **Winner Wired PRNG Avalanche Effect Analysis** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_avalanche_performance [EXTRACTED 1.00]
- **Avalanche Effect Cycle Comparison** — experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_avalanche_effect_analysis, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_1_avalanche, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_2_avalanche [EXTRACTED 1.00]
- **L Mechanism Spectrum Tested in Phase 1** — experiments_2026_8_28_state_update_mechanism_research_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_overwrite_mechanism [EXTRACTED 1.00]
- **M Mechanism Spectrum Tested in Phase 2** — experiments_2026_8_28_state_update_mechanism_research_xor_fold_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_overwrite_mechanism [EXTRACTED 1.00]
- **Original Algorithm Avalanche Cycle Evolution (Cycle 1 vs Cycle 3)** — experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_heatmap, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_avalanche_diffusion_comparison [EXTRACTED 1.00]
- **pruned_winner Avalanche Analysis and Metric Evaluation** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_figure, experiments_2026_8_26_operation_pruning_research_pruned_winner_avalanche_behavior, experiments_2026_8_26_operation_pruning_research_pruned_winner_candidate_configuration [EXTRACTED 1.00]
- **Watchdog process-matching bug, its fix, and the sweep changes it required** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_watchdog_pgrep_bug, experiments_2026_8_28_periodicity_heuristic_validation2_handover_auto_stop_power38, experiments_2026_8_28_periodicity_heuristic_validation2_handover_cmd_pattern_fix, experiments_2026_8_28_periodicity_heuristic_validation2_handover_run_sweep, experiments_2026_8_28_periodicity_heuristic_validation2_handover_manual_only_exclusion [EXTRACTED 1.00]
- **Array-Native Data Permutation and Fingerprinting Workflow** — research_iccs2026_ra_prng_avalanche_sensitivity, sub_projects_fingerprinting_tools_concept, benchmarks_comparisons_source_zepfold_cli [INFERRED 0.75]
- **ra_prng2 Original Benchmark Report Set** — benchmarks_results_ra_prng2_original_rnging_perf_stat_, benchmarks_results_ra_prng2_original_testu01, benchmarks_results_ra_prng2_original_dieharder_test, benchmarks_results_ra_prng2_original_shuffling_perf_stat [INFERRED 0.80]
- **ra_prng3 Benchmark Report Set** — benchmarks_results_ra_prng3_nist_sts_test, benchmarks_results_ra_prng3_dieharder_test, benchmarks_results_ra_prng3_ent_test, benchmarks_results_ra_prng3_perf_stat_rnging, benchmarks_results_ra_prng3_perf_stat_scrambling, benchmarks_results_ra_prng3_practrand_test [INFERRED 0.80]
- **ChaCha20 Speed + Entropy Benchmark Suite** — benchmarks_comparisons_rnging_speed_chacha20_result, benchmarks_comparisons_entropy_chacha20_result [INFERRED 0.85]
- **Pruned Winner Avalanche Progression Analysis (Cycle 1 vs Cycle 2)** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_heatmap, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle1_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle2_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_avalanche_strengthening [INFERRED 0.85]
- **Winner Wired Warmup Cycle Avalanche Progression** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_winner_wired_wiring, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle1_avalanche, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle3_avalanche [INFERRED 0.85]
- **Avalanche Effect Bit Diffusion Evaluation** — experiments_others_avalanche_effect_heatmap_figure, experiments_others_avalanche_effect_heatmap_avalanche_effect, experiments_others_avalanche_effect_heatmap_hamming_distance_analysis, experiments_others_avalanche_effect_heatmap_original_algorithm [INFERRED 0.85]
- **Four-Axis Validation Framework (periodicity/speed/avalanche/PractRand)** — experiments_2026_8_25_periodicity_heuristic_validation_toy_prng, experiments_2026_8_28_state_update_mechanism_research_instruction_count_speed_axis, experiments_2026_8_28_state_update_mechanism_research_avalanche_gate_min_bit, experiments_2026_8_28_state_update_mechanism_research_practrand_16gb_tier [INFERRED 0.85]
- **Periodicity Heuristic Empirical Validation Lifecycle** — research_iccs2026_ra_prng_periodicity_heuristic, experiments_2026_8_25_periodicity_heuristic_validation_toy_model_spec, experiments_2026_8_25_periodicity_heuristic_validation_scaling_verdict [INFERRED 0.85]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_ent [INFERRED 0.85]
- **Pruned Winner Cycle 1 vs Cycle 3 Avalanche Comparison** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_reseed_avalanche_diffusion [INFERRED 0.95]
- **Avalanche Proxy vs Full Empirical Quality Disconnect** — experiments_2026_8_26_operation_pruning_research_results_shift_repair_failure, experiments_2026_8_26_operation_pruning_research_status_gate_size_traps, experiments_2026_8_27_operand_position_search_handover_dead_seed_bits, experiments_2026_8_27_operand_position_search_handover_avalanche_heatmap_baseline [INFERRED]
- **Pruned Winner Multi-battery Statistical Validation** — experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_26_operation_pruning_research_practrand_pruned_winner_1tb_practrand_1tb_log, experiments_2026_8_26_operation_pruning_research_dieharder_pruned_winner_piped_piped_results, experiments_2026_8_26_operation_pruning_research_dieharder_gcd_fixed_gcd_test_log, experiments_2026_8_26_operation_pruning_research_dieharder_rgb_minimum_distance_fixed_rgb_min_dist_log [INFERRED]
- **ra_prng Algorithmic Evolution and Research Pipeline** — readme_ra_prng_architecture, readme_ra_prng2, experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_27_operand_position_search_handover_handover_spec [INFERRED]

## Communities (128 total, 24 thin omitted)

### Community 0 - "Candidate"
Cohesion: 0.06
Nodes (62): _log(), main(), phase_a_hash_access_check(), phase_b_greedy_ablation(), Driver for the operation-pruning greedy ablation search. Two phases: Phase A:…, analyze_cycle(), compute_avalanche_matrix(), Reseed-Driven Avalanche Strengthening Effect (+54 more)

### Community 1 - "Wiring"
Cohesion: 0.07
Nodes (57): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for the winning operand-position rewiring found by…, Mirrors winner_wired.c's ra_reseed()/ra_hash() exactly (identical to… (+49 more)

### Community 2 - "toy_prng.py"
Cohesion: 0.07
Nodes (46): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for toy_prng.py. Port of…, One outer iteration, no rehash - mirrors zepfold_capture() in…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), brent(), Brent's cycle-detection algorithm applied to toy_prng.next_state. Measures the… (+38 more)

### Community 3 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.07
Nodes (40): Four-Axis Validation: periodicity, PractRand, avalanche, speed, Identical L/M Init Across All Seeds; only cons differs, permute + xor_fold State-Update Mechanism (kept unchanged), pruned_winner Avalanche Fraction Metric (Cycle 1 vs Cycle 90), Avalanche Effect Heatmap: pruned_winner Cycle 1 vs Cycle 90, Multi-Cycle Reseed Avalanche Diffusion Improvement (0.418 to 0.468), winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039), Avalanche Effect Heatmap (winner_wired_v2) (+32 more)

### Community 4 - "STATUS: periodicity-heuristic-validation2"
Cohesion: 0.08
Nodes (36): capture_inner_outputs(), hamming(), Params, Scaled-down avalanche / bit-flip sanity check for pruned_wired_toy_prng.py.…, One outer iteration, no rehash -- mirrors next_state's inner loop exactly (tap-…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), brent() (+28 more)

### Community 5 - "next_state"
Cohesion: 0.07
Nodes (41): 2026-08-29 correction: (8,4,rows=4) watchdog bug + stale headline number, auto_stop_power38.py (watchdog script), avalanche_check.py, .cycle_measure_ckpt_n8_w4_rows4_seed0.bin (CkptHeader checkpoint), Fix: CMD_PATTERN uses exact argv instead of broad pgrep prefix, Corrected λ > 3.045×10^11 for (8,4,rows=4), cycle_measure.py / cycle_measure.c (Brent's algorithm), enumerate_n2w4.py (+33 more)

### Community 6 - "2026-8-25_periodicity-heuristic-validation/cycle_measure.c"
Cohesion: 0.10
Nodes (30): load_means(), main(), avalanche_gate_min_bit metric (per-bit floor 0.2, band [0.3,0.7]), check_no_repeat(), main(), Path, Full-scale (real 256x32-bit) practical periodicity sanity check, per…, build() (+22 more)

### Community 7 - "Operand Position Search Experiment Results"
Cohesion: 0.07
Nodes (33): baseline_local PractRand 16GB Test Log, v01 PractRand 16GB Test Log (FAIL), v02 PractRand 16GB Test Log (FAIL), v03 PractRand 16GB Test Log (Pass), v04 PractRand 16GB Test Log (Pass), v06 PractRand 16GB Test Log (Pass), v07 PractRand 16GB Test Log (Pass), v08 PractRand 16GB Test Log (Pass) (+25 more)

### Community 8 - "BuildPyWithCEngine"
Cohesion: 0.08
Nodes (32): Scrambling Speed Benchmark (perf stat, per-call vs amortized, 30 runs), ra_prng2 NIST STS Test Result, ra_prng2 PractRand Test Result (up to 128 GB, no anomalies), ra_prng Build and Test Workflow, Contributing Guide, ra_prng Implementation Variant Roles, Repository Structure Specification, Brent Cycle Measure Checkpoint/Resume Architecture (+24 more)

### Community 9 - "ra_prng3 PRNG Generator"
Cohesion: 0.09
Nodes (24): BigCrush (TestU01), Bit Shifting, Rationale: Branchless Design for CPU Pipeline Efficiency, ChaCha20, Claude E. Shannon, Daniel Lemire, ent Entropy Tool, Rationale: Golden-Ratio Constants Reduce Collisions, Dead States, Short Cycles (+16 more)

### Community 10 - "other/avalanche_heatmap_original.py"
Cohesion: 0.14
Nodes (21): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), Original Algorithm Avalanche Diffusion Comparison (Cycle 1 vs Cycle 3), Original Algorithm Cycle 1 Avalanche Fraction (0.497576), Original Algorithm Cycle 3 Avalanche Fraction (0.499740), Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3, main() (+13 more)

### Community 11 - "2026-8-28_periodicity-heuristic-validation2/cycle_measure.c"
Cohesion: 0.23
Nodes (20): brent_resumable(), Params, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+12 more)

### Community 12 - "winner_wired_v2.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 13 - "others/avalanche_heatmap_original.py"
Cohesion: 0.12
Nodes (19): Benchmark Source Comparisons Documentation, ZepFold Shuffling CLI Tool, Toy PRNG Avalanche Sanity Verification Gate, Brent Cycle Detection Methodology, Periodicity Validation Experiment Handover, Periodicity Heuristic Validation Results Report, Periodicity Heuristic Scalability & Non-Monotonicity Verdict, Initial Sweep Periodicity Measurement Summary (+11 more)

### Community 14 - "RaPrng2"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_reseed(), plot_comparison(), plot_single(), Path (+9 more)

### Community 15 - "ra_prng2 (Array-Based PRNG)"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_capture(), original_reseed(), plot_comparison(), plot_single() (+9 more)

### Community 16 - "Design decisions (read before touching `pruned_wired_toy_prng.py`)"
Cohesion: 0.18
Nodes (11): _find_lib_path(), _lib_handle(), _load_library(), Path, RaPrng2, ctypes binding for the ra_prng2 C engine. This is glue only - it marshals calls…, Thin wrapper around one RA_PRNG instance., CDLL (+3 more)

### Community 17 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.14
Nodes (15): Embarrassingly Parallel Architecture (per-instance state, no locks), Operand-Position Search Research (2026-8-27), Operation Pruning Research (2026-8-26), Periodicity Heuristic Validation 2 (2026-8-28), Periodicity Heuristic Validation Research (2026-8-25), Read-Only Source Constraint (never modify ra_prng2/ra_prng3/winner_wired_v2.c in place), State-Update Mechanism Research (2026-8-28), Multi-Stream Independence Testing (Q1) (+7 more)

### Community 18 - "scc_test.py"
Cohesion: 0.16
Nodes (14): Dieharder Test Results (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), PractRand Test Results (ra_prng3), Dieharder Test Suite, ent Entropy Test Tool (+6 more)

### Community 19 - "pruned_prng.c"
Cohesion: 0.19
Nodes (11): PractRand 128GB Single-Stream Clean Baseline (2026-8-27), Shared helpers for the parallelization-research harness. Never modifies…, main(), Q1 Method B: interleave K independent `--stream <seed> <n>` streams word-by-…, run_interleave_practrand(), Parallelization Research Results, main(), Q3(a): wall-clock speedup/efficiency when running N independent… (+3 more)

### Community 20 - "Cycle 3 Converged Avalanche Fraction (0.500751)"
Cohesion: 0.21
Nodes (12): build(), gen_c(), main(), Generate + compile the phase-2 (M-only) state-update-mechanism spectrum, per…, The M mechanism only affects the reseed fold (once per 255 steps) -- the first…, verify_first_cycle_matches_control(), ensure_ra_prng2_cli(), Streaming capture: caller reads/closes proc.stdout incrementally. Use for large… (+4 more)

### Community 21 - "Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)"
Cohesion: 0.17
Nodes (11): 1. Op/wiring mapping ported to toy scale, 2. Tap-survivor generalization: top-2-by-count, and why, 3. Tap shift-width collapse (caught during design, not present in either parent), 4. `ra_hash_gen` divergence from ground truth (found while reading source, not assumed), 5. Self-check (passed) -- MUST stay passing, CRITICAL BUG found and fixed this session -- read before writing more scripts, Design decisions (read before touching `pruned_wired_toy_prng.py`), HANDOVER.md (periodicity-heuristic-validation2) (+3 more)

### Community 22 - "STATUS: periodicity-heuristic-validation2"
Cohesion: 0.20
Nodes (7): _build_py, Distribution, BinaryDistribution, BuildPyWithCEngine, Imperative build customization pyproject.toml's static [tool.setuptools] table…, After the normal .py copy step, compiles the ra_prng2 C engine and drops…, Declares this distribution as containing compiled, platform-specific code…

### Community 23 - "pcg_amortized.c"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 24 - "source/ra_prng2.c"
Cohesion: 0.38
Nodes (10): build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32(), FILE, has() (+2 more)

### Community 25 - "source/ra_prng3.c"
Cohesion: 0.29
Nodes (11): Avalanche Metrics (overall=0.487783, min_bit=0.472304), Warmup Cycle Avalanche Effect and Bit Diffusion, Cycle 1 Initial State Avalanche Fraction (0.487783), Cycle 3 Converged Avalanche Fraction (0.500751), Winner Wired Avalanche Heatmap Cycle 1 vs Cycle 3, Winner Wired Wiring Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a), Cycle 4 Steady State Avalanche (fraction 0.499713), Avalanche Effect Heatmap Comparison: Cycle 1 vs Cycle 4 (+3 more)

### Community 26 - "scrambler.c"
Cohesion: 0.18
Nodes (10): 1. Summary Comparison: Unpruned Original vs `winner_wired_v2`, 2. Sebaran Siklus Utama (*Distinct $\lambda$ Distribution*), 3. Enumerasi Penuh & Uji Chi-Square vs Poisson(1), 4. Analisis Temuan Kunci (*Key Findings*), 5. Catatan Metodologi & Caveats (Sesuai `HANDOVER.md`), 6. Kesimpulan (*Verdict*), A. Skala $n=2, w=4$ ($|S| = 131.072$) — [`enumerate_n2w4_result.json`](./enumerate_n2w4_result.json), B. Skala $n=4, w=4$ ($|S| = 402.653.184$) — [`enumerate_n4w4_result.json`](./enumerate_n4w4_result.json) (+2 more)

### Community 27 - "pruned_winner.c"
Cohesion: 0.27
Nodes (10): ndarray, Bounded capture: run `binary --stream seed n`, return n uint32 values. Suitable…, stream_values(), analyze_group(), build_group(), main(), ndarray, Q1 Method A: Pearson cross-correlation between pairs of independent `--stream… (+2 more)

### Community 28 - "pruned_winner_refactored.c"
Cohesion: 0.33
Nodes (9): Compiler Auto-Vectorization (AVX-512) Beats Manual AVX2 Intrinsics, L[i]<->L[d] Data-Dependent Swap: SIMD Bottleneck, SIMD Vectorization Feasibility (Q4), main(), rot32_avx2(), rot32_scalar(), run_scalar(), run_simd() (+1 more)

### Community 29 - "baseline.c"
Cohesion: 0.33
Nodes (9): cycle_measure Binary Exit-42 Crash at n=8,w=4,rows=4, log2_fact(), main(), predicted(), Drives the pruned/wired periodicity-heuristic sweep. Same lambda_pred formula…, result_csv_name(), run_c(), run_python() (+1 more)

### Community 30 - "baseline_refactored.c"
Cohesion: 0.44
Nodes (8): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), pcg32_buffered_t

### Community 31 - "v01.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 32 - "v02.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 33 - "v03.c"
Cohesion: 0.33
Nodes (8): full_scale_sanity_check.py (intra-stream block-repeat check), Seed/Cycle Collision Probability (Q2), digest_collision_prob(), main(), Q2: probability of cross-stream collision/overlap between different seeds'…, Birthday-bound estimate for pure hash-digest collision probability across…, scan_blocksweep(), scan_prefix()

### Community 34 - "v04.c"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 35 - "v06.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 36 - "v07.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 37 - "v08.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 38 - "v09.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 39 - "v10.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 40 - "v11.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 41 - "v12.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 42 - "v13.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 43 - "winner_wired.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 44 - "ra_prng2_struct.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 45 - "c/ra_prng2.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 46 - "philox_amortized.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 47 - "ra_prng2 API Reference"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 48 - "original_m_inject Candidate (Fase M)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 49 - "original_m_overwrite Candidate (Fase M)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 50 - "original_m_permute Candidate (Fase M)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 51 - "avalanche_effect_analysis.py"
Cohesion: 0.33
Nodes (8): avalanche_gate_min_bit_binary(), capture_first_cycle(), main(), practrand_prefix_gate_binary(), Path, Avalanche + PractRand measurement for the phase-1 (L-only) state-update-…, `CYCLES` outer cycles' worth of `c` values -- see the CAPTURE_LEN comment above…, # NOTE: unlike pruned_prng.c's CLI (quality_gate.py's convention, where

### Community 52 - "ACM (Association for Computing Machinery)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 53 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 54 - "winner_m_inject Candidate (Fase M)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 55 - "winner_m_overwrite Candidate (Fase M)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 56 - "winner_m_permute Candidate (Fase M)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 57 - "Fisher–Yates Shuffle"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 58 - "ent (Entropy Testing Tool)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 59 - "xoshiro256_amortized.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 60 - "scrambler.py"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 61 - "PRNG Periodicity Analysis"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 62 - "Design decisions (read before touching `pruned_wired_toy_prng.py`)"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 63 - "source/chacha20.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 64 - "pcg.c"
Cohesion: 0.44
Nodes (8): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), ra_step(), rot32()

### Community 65 - "M-Mechanism Variant Generator"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 66 - "source/philox.c"
Cohesion: 0.46
Nodes (7): main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32(), philox_refill_buffer(), rand_bounded()

### Community 67 - "source/xoshiro256.c"
Cohesion: 0.43
Nodes (8): RNGing perf stat (ra_prng2 original), Shuffling perf stat (ra_prng2 original), RNGing perf stat (ra_prng3), Scrambling perf stat (ra_prng3), Linux perf stat Tool, Scrambler Tool, tokens.txt Input Corpus, scrambled.txt (scramble-design experiment output)

### Community 68 - "test_ra_hash.py"
Cohesion: 0.36
Nodes (8): CPython Binding Documentation, Orphaned CPython Shared Object Rationale, RaPrng2 ctypes Wrapper, ra_prng2 API Reference, ra_core PRNG Core Loop, ra_hash State Reduction Function, RA_PRNG C Struct API, rot32 Left Bitwise Rotation

### Community 69 - "pcg32.c"
Cohesion: 0.25
Nodes (8): Hamas A. Rahman, ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex, ra_prng2 and Beyond (Paper CA-162), Scopus, ra_prng GitHub Repository, hamzy hams (GitHub maintainer)

### Community 70 - "chacha20_amortized.c"
Cohesion: 0.25
Nodes (8): Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), R. Durstenfeld, Fisher–Yates Shuffle, Rationale: Non-Invertible Hash Reseed Trade-off (Reduces Period, Aids Diffusion), ra_hash (Reseed Hash Function), Reseed Operator, Internal State S

### Community 71 - "Auto-Stop Watchdog Script"
Cohesion: 0.29
Nodes (7): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result

### Community 72 - "Avalanche Effect Analysis (ALL_OPS)"
Cohesion: 0.52
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next()

### Community 73 - "heatmap.py"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 74 - "State Transition Operator F"
Cohesion: 0.29
Nodes (7): B. Harris, Donald E. Knuth, Makoto Matsumoto and Takuji Nishimura, Mersenne Twister (MT19937), PRNG Periodicity Analysis, Pierre L'Ecuyer, Random Mapping Theorem

### Community 75 - "ra_prng2.py"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 76 - "c/ra_prng3.c"
Cohesion: 0.67
Nodes (5): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded()

### Community 77 - "src/chacha20.c"
Cohesion: 0.60
Nodes (5): main(), philox4x32_10(), philox4x32_round(), philox_rand32(), rand_bounded()

### Community 78 - "src/philox.c"
Cohesion: 0.60
Nodes (5): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 79 - "Cycle 3 Avalanche Fraction (0.470588)"
Cohesion: 0.33
Nodes (3): Pure-Python unit tests for ra_prng2.ra_hash / ra_core. These do not require the…, Regression test for the historical bug: ra_hash must be a pure function writing…, test_ra_hash_does_not_mutate_input()

### Community 80 - "Periodicity Heuristic Research Question"
Cohesion: 0.70
Nodes (4): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom()

### Community 81 - "Tap-Survivor Design Decisions"
Cohesion: 0.70
Nodes (4): main(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 82 - "Avalanche Effect Heatmap (Original Algorithm)"
Cohesion: 0.60
Nodes (3): chacha20_block(), main(), rand_bounded()

### Community 83 - "Seed and Array Initialization (Algorithm 1)"
Cohesion: 0.60
Nodes (4): find_pid(), get_status(), main(), Monitors .cycle_measure_ckpt_n8_w4_rows4_seed0.bin and stops cycle_measure when…

### Community 84 - "Graphify Knowledge Graph Rule"
Cohesion: 0.50
Nodes (5): Avalanche Effect Analysis (ALL_OPS), Bit Difference Distribution Across 32 Flipped Bits, Cycle 1 Avalanche Fraction (0.497576), Cycle 2 Avalanche Fraction (0.502141), Avalanche Effect Heatmap (Cycle 1 vs Cycle 2)

### Community 85 - "Graphify Workflow Policy"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 86 - "Cycle 1 vs Cycle 5 Avalanche Fraction Convergence"
Cohesion: 0.40
Nodes (5): Array Index Shuffling, Bitwise Rotation (rotl32), Mutate Operator, Permute Operator, State Transition Operator F

### Community 87 - "Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 88 - "Greedy Operation-Pruning Search Paradigm"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 90 - "Original Algorithm Avalanche Bit Distribution & Diffusion"
Cohesion: 0.83
Nodes (3): main(), philox4x32_10(), philox4x32_round()

### Community 91 - "other_winners_perf.sh"
Cohesion: 0.83
Nodes (4): Cycle 1 Avalanche Fraction (0.418459), Cycle 3 Avalanche Fraction (0.470588), Avalanche Heatmap: Cycle 1 vs Cycle 3 (pruned_winner), Reseed Avalanche Diffusion in Pruned Winner

### Community 92 - "CI Build and Test Job"
Cohesion: 0.50
Nodes (4): Paper heuristic λ ≈ 0.7824·√|S|, Research Question: does pruned/rewired algorithm shift the heuristic's λ-vs-λ_pred fit, Central Research Question (RESULTS.md), Verdict 2: heuristic behaves as conservative lower bound at large-scale configs only

### Community 93 - "ra_core (Main Generation Loop)"
Cohesion: 0.50
Nodes (4): Tap shift-width collapse (e>=w yields identically-zero term), Tap-survivor generalization (top-2-by-count rule, G=8→(6,7)), Finding: non-vacuous tap pruning (G=4) gives closest fit to heuristic, Caveat: tap pruning is a no-op at G<=2 configs

### Community 94 - "Lemire's Fast Reduction"
Cohesion: 0.67
Nodes (4): Avalanche Effect, Avalanche Effect Heatmap (Original Algorithm), Hamming Distance Analysis, Original PRNG Algorithm

### Community 95 - "ent Entropy Tool"
Cohesion: 0.67
Nodes (3): Graphify Query & Navigation Protocol, Graphify Knowledge Graph Rule, Graphify Pipeline Workflow

### Community 96 - "conftest.py"
Cohesion: 0.67
Nodes (3): graphify-out/graph.json Knowledge Graph Artifact, graphify Skill, Graphify-First Research Workflow Policy

### Community 97 - "/dev/urandom RNG Speed Benchmark Result"
Cohesion: 1.00
Nodes (3): Cycle 1 vs Cycle 5 Avalanche Fraction Convergence, Avalanche Heatmap: Pruned Winner Cycle 1 vs Cycle 5, Pruned Winner Architecture (TAP6, TAP7, ROT_C, SHR13)

### Community 98 - "PCG32 RNG Speed Benchmark Result"
Cohesion: 1.00
Nodes (3): Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13), pruned_winner Cycle 1 Avalanche Sensitivity and Dead Zones, pruned_winner 4-Op Candidate Configuration (TAP6, TAP7, ROT_C, SHR13)

### Community 99 - "Philox4x32 RNG Speed Benchmark Result"
Cohesion: 1.00
Nodes (3): Greedy Operation-Pruning Search Paradigm, Operation-Pruning Research Handover Spec, Limits of Zero-Drift Micro-Optimizations on ra_core

## Ambiguous Edges - Review These
- `Hamas A. Rahman` → `hamzy hams (GitHub maintainer)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to

## Knowledge Gaps
- **147 isolated node(s):** `other_winners_practrand.sh script`, `perf_state_update.sh script`, `perf_state_update_m.sh script`, `toy_probe_phase3.sh script`, `toy_sweep_m.sh script` (+142 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **24 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Hamas A. Rahman` and `hamzy hams (GitHub maintainer)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `ra_prng2 (Array-Based PRNG)` connect `ra_prng3 PRNG Generator` to `pruned_winner 4-Operation Fast Variant`, `pcg32.c`, `chacha20_amortized.c`, `State Transition Operator F`, `scc_test.py`, `Cycle 1 vs Cycle 5 Avalanche Fraction Convergence`?**
  _High betweenness centrality (0.066) - this node is a cross-community bridge._
- **Why does `Avalanche Effect (Bit-Flip Sensitivity)` connect `pruned_winner 4-Operation Fast Variant` to `ra_prng3 PRNG Generator`?**
  _High betweenness centrality (0.063) - this node is a cross-community bridge._
- **Why does `Avalanche Effect Heatmap (winner_wired_v2)` connect `pruned_winner 4-Operation Fast Variant` to `Wiring`?**
  _High betweenness centrality (0.063) - this node is a cross-community bridge._
- **Are the 5 inferred relationships involving `Params` (e.g. with `capture_inner_outputs()` and `run_avalanche_check()`) actually correct?**
  _`Params` has 5 INFERRED edges - model-reasoned connections that need verification._
- **Are the 8 inferred relationships involving `Candidate` (e.g. with `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` and `avalanche_gate()`) actually correct?**
  _`Candidate` has 8 INFERRED edges - model-reasoned connections that need verification._
- **What connects `other_winners_practrand.sh script`, `perf_state_update.sh script`, `perf_state_update_m.sh script` to the rest of the system?**
  _147 weakly-connected nodes found - possible documentation gaps or missing edges._