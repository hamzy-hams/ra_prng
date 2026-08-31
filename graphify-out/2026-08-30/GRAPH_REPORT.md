# Graph Report - ra_prng  (2026-08-30)

## Corpus Check
- 223 files · ~11,749,847 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 1615 nodes · 2524 edges · 195 communities (141 shown, 54 thin omitted)
- Extraction: 96% EXTRACTED · 4% INFERRED · 0% AMBIGUOUS · INFERRED: 109 edges (avg confidence: 0.88)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `885e9344`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- Wiring
- winner_wired_v2.c
- State-Update Mechanism Research Results (Phase 1-3)
- others/avalanche_heatmap_original.py
- Completed Tasks
- tahap0_prototype.py
- Operand Position Search Experiment Results
- pruned_winner 4-Operation Fast Variant
- ra_prng2 (Array-Based PRNG)
- other/avalanche_heatmap_original.py
- 2026-8-30_addressable-shuffle/scc_test.py
- 2026-8-25_periodicity-heuristic-validation/cycle_measure.c
- 2026-8-28_periodicity-heuristic-validation2/cycle_measure.c
- tahap5_bench.c
- ICCS 2026 Research Paper: Array-Native Randomness
- tahap4_bench.c
- avalanche_effect_analysis.py
- Params
- quality_gate.py
- Candidate
- collision_scan.py
- Parallelization Research Handover
- toy_prng.py
- avalanche_heatmap_pruned_winner.py
- next_state
- common.py
- stream_driven_shuffle.c
- interleave_practrand.py
- Handover: Tahap 5 — Optimasi Biaya Init Addressable
- ablation_search.py
- io_only_isolation_test.c
- 2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py
- Fisher–Yates Shuffle
- Avalanche Effect (Bit-Flip Sensitivity)
- Design decisions (read before touching `pruned_wired_toy_prng.py`)
- RESULTS.md — Parallelization Research Results
- BuildPyWithCEngine
- 2025-10-5_scramble-design/scc_test.py
- pruned_prng.c
- Cycle 1 Initial State Avalanche Fraction (0.487783)
- Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)
- cross_correlation.py
- tahap5_benchmark.py
- SPEED_BENCHMARK.md
- pcg_amortized.c
- 2026-8-28_periodicity-heuristic-validation2/run_sweep.py
- c/ra_prng2.c
- tahap4_benchmark.py
- scrambler_addressable.c
- source/ra_prng2.c
- source/ra_prng3.c
- ra_prng3 PRNG Generator
- scrambler.c
- 2026-8-25_periodicity-heuristic-validation/avalanche_check.py
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
- 2026-8-28_periodicity-heuristic-validation2/enumerate_n4w4.py
- measure_state_update.py
- original_inject.c
- original_m_inject.c
- original_m_overwrite.c
- original_m_permute.c
- original_overwrite.c
- original_permute.c
- winner_inject.c
- winner_m_inject.c
- winner_m_overwrite.c
- winner_m_permute.c
- winner_overwrite.c
- winner_permute.c
- Addressable-Init Data Shuffling — RESULTS
- ra_prng2_struct.c
- ra_prng2_thread.c
- philox_amortized.c
- xoshiro256_amortized.c
- Dieharder Test Suite
- ra_prng2 API Reference
- simd_prototype.c
- Hamas A. Rahman
- ent (Entropy Testing Tool)
- pcg.c
- source/xoshiro256.c
- scrambler.py
- 2026-8-25_periodicity-heuristic-validation/run_sweep.py
- 2026-8-28_periodicity-heuristic-validation2/avalanche_check.py
- Speed Benchmark: `scrambler_addressable` vs Algoritma Lain
- PRNG Periodicity Analysis
- source/chacha20.c
- source/philox.c
- Shuffling perf stat (ra_prng2 original)
- pcg32.c
- src/xoshiro256.c
- chacha20_amortized.c
- Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2)
- auto_stop_power38.py
- JAX
- Orbit Addressing
- Avalanche Effect Analysis (ALL_OPS)
- heatmap.py
- ra_prng2.py
- c/ra_prng3.c
- src/chacha20.c
- src/philox.c
- Cycle 3 Avalanche Fraction (0.470588)
- Paper heuristic λ ≈ 0.7824·√|S|
- Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))
- robust_reinit.py
- Avalanche Effect Heatmap (Original Algorithm)
- Graphify Knowledge Graph Rule
- Graphify-First Research Workflow Policy
- Cycle 1 vs Cycle 5 Avalanche Fraction Convergence
- Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)
- Greedy Operation-Pruning Search Paradigm
- run_speed_benchmark.sh
- ChaCha20 per-call (benchmarked algorithm)
- ChaCha20 amortized (benchmarked algorithm)
- Original Algorithm Avalanche Bit Distribution & Diffusion
- other_winners_perf.sh
- perf_state_update.sh
- perf_state_update_m.sh
- toy_probe_phase3.sh
- toy_sweep_m.sh
- toy_sweep_phase3.sh
- experiments/2026-8-29_parallelization-research/
- Speed Benchmark Output (philox)
- Speed Benchmark Output (philox amortized)
- CI Build and Test Job
- conftest.py
- Addressable-Init Research (Tahap 0-5)
- /dev/urandom RNG Speed Benchmark Result
- PCG32 RNG Speed Benchmark Result
- Philox4x32 RNG Speed Benchmark Result
- xoshiro256** RNG Speed Benchmark Result
- benchmarks/comparisons/source/ra_prng2.c (unclosed-comment bug)
- benchmarks/comparisons/source/README.md (naming convention)
- Project Root Changelog
- 2025-10-5_scramble-design/scrambler.c (predecessor)
- rows=2 Configuration Log
- Design decision: op/wiring mapping ported to toy scale
- Finding: very short transient tail (μ<=1) across all seeds
- Sweep Output (empty file)
- Kandidat 2: independent L/M keying
- scrambler_addressable.c (CLI)
- ra_core (shuffle loop, scrambler_addressable.c)
- ra_hash (block-reseed, 255-element)
- ra_init_state_addressable(L, M, key)
- scrambler_addressable.py (Python reference port)
- Shuffled Token Output
- run_speed_benchmark.sh
- Finding: addressable-init adds no measurable overhead
- Rationale: -include header workaround for ra_prng2.c comment bug (no source edit)
- Rationale: ZepFold shuffle loop heavier per-token than raw PRNG Fisher-Yates draw
- tokens.txt sample data (100,000 tokens)
- Lemire's Fast Reduction (index swap technique)
- Orbit Addressing (f(key) state-init mechanism)
- PCG32 (benchmarked algorithm)
- PCG32 amortized (benchmarked algorithm)
- ra-prng
- ra_prng2 Package Changelog
- ra_prng3 Package Changelog
- Image Encrypting Documentation
- winner_wired_addressable.c
- xoshiro256** (benchmarked algorithm)
- xoshiro256** amortized (benchmarked algorithm)
- ZepFold (token-shuffling CLI approach)
- RaPrng2
- ra_prng2 (ZepFold, old fixed-init formula)
- Handover: Addressable-Init Data Shuffling & Speed Investigation
- gen_variants.py
- gen_variants_m.py
- full_scale_sanity_check.py
- xor_fold (M) - accumulating XOR fold, retained
- tahap3_collision_scan.py
- tahap3_cross_correlation.py
- Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3

## God Nodes (most connected - your core abstractions)
1. `Params` - 33 edges
2. `ra_prng2 (Array-Based PRNG)` - 29 edges
3. `State-Update Mechanism Research Results (Phase 1-3)` - 23 edges
4. `Parallelization Research Handover` - 21 edges
5. `Candidate` - 20 edges
6. `Operand Position Search Experiment Results` - 19 edges
7. `Wiring` - 18 edges
8. `stream()` - 17 edges
9. `ra_init_state_addressable()` - 17 edges
10. `next_state()` - 14 edges

## Surprising Connections (you probably didn't know these)
- `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` --semantically_similar_to--> `Candidate`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/avalanche_heatmap_pruned_winner_cycle1_vs_cycle2.png → experiments/2026-8-26_operation-pruning-research/pruned_prng.py
- `hamzy hams (GitHub maintainer)` --conceptually_related_to--> `Hamas A. Rahman`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf → README.md
- `winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039)` --conceptually_related_to--> `Avalanche Effect (Bit-Flip Sensitivity)`  [INFERRED]
  experiments/2026-8-27_operand-position-search/avalanche_heatmap_winner_v2.png → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf
- `pruned_winner 4-Operation Fast Variant` --semantically_similar_to--> `ra_prng2 32-bit Generator`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/RESULTS.md → README.md
- `Hamas A. Rahman` --references--> `ra_prng2 (Array-Based PRNG)`  [EXTRACTED]
  README.md → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_philox4x32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]
- **ra_prng2 Statistical Validation Test Suite** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_practrand, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_bigcrush [EXTRACTED 0.90]
- **ra_prng2 State Transition Operator Composition (F = Mutate∘Permute∘Reseed)** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_transition_operator_f, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_mutate_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_permute_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_reseed_operator [EXTRACTED 0.95]
- **Bit-identical self-check chain linking pruned toy model to full-scale reference and winner C source** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_self_check_full_scale, experiments_2026_8_28_periodicity_heuristic_validation2_handover_pruned_wired_toy_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wired_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wiring_module, experiments_2026_8_28_periodicity_heuristic_validation2_handover_winner_wired_v2 [EXTRACTED 1.00]
- **Collision Scan Parallelization Strategy Evolution (ProcessPool deadlock -> ThreadPool fix)** — experiments_2026_8_29_parallelization_research_collision_scan, experiments_2026_8_29_parallelization_research_processpool_deadlock_rationale, experiments_2026_8_29_parallelization_research_threadpool_shared_dict_rationale [EXTRACTED 1.00]
- **2026-08-29 correction of (8,4,rows=4) lambda bound, documented across HANDOVER/RESULTS/STATUS** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_2026_08_29_correction, experiments_2026_8_28_periodicity_heuristic_validation2_handover_stale_lambda_lower_bound, experiments_2026_8_28_periodicity_heuristic_validation2_handover_corrected_lambda_value, experiments_2026_8_28_periodicity_heuristic_validation2_handover_checkpoint_file, experiments_2026_8_28_periodicity_heuristic_validation2_results_footnote_correction, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_summary, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_lower_bound_restated [EXTRACTED 1.00]
- **Double-Swap Hypothesis Tested and Disproven, Leading to I/O Bottleneck Finding** — experiments_2026_8_30_addressable_shuffle_double_swap_hypothesis, experiments_2026_8_30_addressable_shuffle_stream_driven_shuffle_stream_driven_shuffle, experiments_2026_8_30_addressable_shuffle_speed_bench_io_only_isolation_test, experiments_2026_8_30_addressable_shuffle_io_bottleneck_finding [EXTRACTED 1.00]
- **pruned_winner Cycle 1 vs Cycle 90 Avalanche Analysis** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_reseed_diffusion_result, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_avalanche_fraction [EXTRACTED 1.00]
- **winner_wired Avalanche Diffusion Convergence Across Cycles** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_cycle4_avalanche [EXTRACTED 1.00]
- **Winner Wired PRNG Avalanche Effect Analysis** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_avalanche_performance [EXTRACTED 1.00]
- **Avalanche Effect Cycle Comparison** — experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_avalanche_effect_analysis, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_1_avalanche, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_2_avalanche [EXTRACTED 1.00]
- **L Mechanism Spectrum Tested in Phase 1** — experiments_2026_8_28_state_update_mechanism_research_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_overwrite_mechanism [EXTRACTED 1.00]
- **M Mechanism Spectrum Tested in Phase 2** — experiments_2026_8_28_state_update_mechanism_research_xor_fold_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_overwrite_mechanism [EXTRACTED 1.00]
- **Original Algorithm Avalanche Cycle Evolution (Cycle 1 vs Cycle 3)** — experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_heatmap, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_avalanche_diffusion_comparison [EXTRACTED 1.00]
- **Parallelization Validation Harness Suite (Q1-Q4 scripts, all CLI-only wrappers)** — experiments_2026_8_29_parallelization_research_cross_correlation, experiments_2026_8_29_parallelization_research_interleave_practrand, experiments_2026_8_29_parallelization_research_collision_scan, experiments_2026_8_29_parallelization_research_wall_time_scaling, experiments_2026_8_29_parallelization_research_perf_scaling, experiments_2026_8_29_parallelization_research_simd_prototype, experiments_2026_8_29_parallelization_research_common [EXTRACTED 1.00]
- **pruned_winner Avalanche Analysis and Metric Evaluation** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_figure, experiments_2026_8_26_operation_pruning_research_pruned_winner_avalanche_behavior, experiments_2026_8_26_operation_pruning_research_pruned_winner_candidate_configuration [EXTRACTED 1.00]
- **Q1 Stream Independence Validation (Method A + Method B + avalanche rationale)** — experiments_2026_8_29_parallelization_research_method_a_cross_correlation, experiments_2026_8_29_parallelization_research_method_b_interleaved_practrand, experiments_2026_8_29_parallelization_research_avalanche_seed_diffusion, experiments_2026_8_29_parallelization_research_q1_stream_independence [EXTRACTED 1.00]
- **Register-Based PRNG Shuffle CLIs Used as ~2.2x-Faster Comparison Group** — benchmarks_comparisons_source_xoshiro256, benchmarks_comparisons_source_pcg, benchmarks_comparisons_source_philox, benchmarks_comparisons_source_chacha20, experiments_2026_8_30_addressable_shuffle_scrambler_addressable [EXTRACTED 1.00]
- **Prior Research Folders That Tested Only Single-Stream Axes** — concept_periodicity_heuristic_validation_research, concept_operation_pruning_research, concept_operand_position_search_research, concept_state_update_mechanism_research, concept_periodicity_heuristic_validation2_research [EXTRACTED 1.00]
- **Tahap 0 candidate init formulas (Kandidat 1-5) evaluated together** — experiments_2026_8_30_addressable_init_research_handover_kandidat1, experiments_2026_8_30_addressable_init_research_handover_kandidat2, experiments_2026_8_30_addressable_init_research_handover_kandidat3_control, experiments_2026_8_30_addressable_init_research_handover_kandidat4, experiments_2026_8_30_addressable_init_research_handover_kandidat5 [EXTRACTED 1.00]
- **Tahap 3 statistical re-validation methods applied to winner_wired_addressable** — experiments_2026_8_30_addressable_init_research_tahap3_cross_correlation_tahap3_cross_correlation, experiments_2026_8_30_addressable_init_research_tahap3_collision_scan_tahap3_collision_scan, experiments_2026_8_30_addressable_init_research_tahap3_interleave_practrand_tahap3_interleave_practrand, experiments_2026_8_30_addressable_init_research_winner_wired_addressable_ra_init_state_addressable [EXTRACTED 1.00]
- **Tahap 5 ranked init-cost optimization candidates (Rank 1-3)** — experiments_2026_8_30_addressable_init_research_results_rank1_zmm512, experiments_2026_8_30_addressable_init_research_results_rank2_v1_rolv, experiments_2026_8_30_addressable_init_research_results_rank3_funroll [EXTRACTED 1.00]
- **Watchdog process-matching bug, its fix, and the sweep changes it required** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_watchdog_pgrep_bug, experiments_2026_8_28_periodicity_heuristic_validation2_handover_auto_stop_power38, experiments_2026_8_28_periodicity_heuristic_validation2_handover_cmd_pattern_fix, experiments_2026_8_28_periodicity_heuristic_validation2_handover_run_sweep, experiments_2026_8_28_periodicity_heuristic_validation2_handover_manual_only_exclusion [EXTRACTED 1.00]
- **ZepFold Family Shares the Same Per-Token I/O Bottleneck** — experiments_2026_8_30_addressable_shuffle_zepfold_io_helpers, experiments_2026_8_30_addressable_shuffle_scrambler_addressable, benchmarks_comparisons_source_ra_prng2, experiments_2026_8_30_addressable_shuffle_stream_driven_shuffle [EXTRACTED 1.00]
- **Array-Native Data Permutation and Fingerprinting Workflow** — research_iccs2026_ra_prng_avalanche_sensitivity, sub_projects_fingerprinting_tools_concept, benchmarks_comparisons_source_zepfold_cli [INFERRED 0.75]
- **ra_prng2 Original Benchmark Report Set** — benchmarks_results_ra_prng2_original_rnging_perf_stat_, benchmarks_results_ra_prng2_original_testu01, benchmarks_results_ra_prng2_original_dieharder_test, benchmarks_results_ra_prng2_original_shuffling_perf_stat [INFERRED 0.80]
- **ra_prng3 Benchmark Report Set** — benchmarks_results_ra_prng3_nist_sts_test, benchmarks_results_ra_prng3_dieharder_test, benchmarks_results_ra_prng3_ent_test, benchmarks_results_ra_prng3_perf_stat_rnging, benchmarks_results_ra_prng3_perf_stat_scrambling, benchmarks_results_ra_prng3_practrand_test [INFERRED 0.80]
- **ChaCha20 Speed + Entropy Benchmark Suite** — benchmarks_comparisons_rnging_speed_chacha20_result, benchmarks_comparisons_entropy_chacha20_result [INFERRED 0.85]
- **Pruned Winner Avalanche Progression Analysis (Cycle 1 vs Cycle 2)** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_heatmap, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle1_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle2_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_avalanche_strengthening [INFERRED 0.85]
- **Winner Wired Warmup Cycle Avalanche Progression** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_winner_wired_wiring, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle1_avalanche, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle3_avalanche [INFERRED 0.85]
- **Avalanche Effect Bit Diffusion Evaluation** — experiments_others_avalanche_effect_heatmap_figure, experiments_others_avalanche_effect_heatmap_avalanche_effect, experiments_others_avalanche_effect_heatmap_hamming_distance_analysis, experiments_others_avalanche_effect_heatmap_original_algorithm [INFERRED 0.85]
- **Four-Axis Validation Framework (periodicity/speed/avalanche/PractRand)** — experiments_2026_8_25_periodicity_heuristic_validation_toy_prng, experiments_2026_8_28_state_update_mechanism_research_instruction_count_speed_axis, experiments_2026_8_28_state_update_mechanism_research_avalanche_gate_min_bit, experiments_2026_8_28_state_update_mechanism_research_practrand_16gb_tier [INFERRED 0.85]
- **Periodicity Heuristic Empirical Validation Lifecycle** — research_iccs2026_ra_prng_periodicity_heuristic, experiments_2026_8_25_periodicity_heuristic_validation_toy_model_spec, experiments_2026_8_25_periodicity_heuristic_validation_scaling_verdict [INFERRED 0.85]
- **Q1-Q4 Parallelization Research Questions** — concept_stream_independence_testing, concept_seed_collision_probability, concept_throughput_scalability, concept_simd_vectorization_feasibility [INFERRED 0.85]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_practrand, concept_ent [INFERRED 0.85]
- **Pruned Winner Cycle 1 vs Cycle 3 Avalanche Comparison** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_reseed_avalanche_diffusion [INFERRED 0.95]
- **Avalanche Proxy vs Full Empirical Quality Disconnect** — experiments_2026_8_26_operation_pruning_research_results_shift_repair_failure, experiments_2026_8_26_operation_pruning_research_status_gate_size_traps, experiments_2026_8_27_operand_position_search_handover_dead_seed_bits, experiments_2026_8_27_operand_position_search_handover_avalanche_heatmap_baseline [INFERRED]
- **Pruned Winner Multi-battery Statistical Validation** — experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_26_operation_pruning_research_practrand_pruned_winner_1tb_practrand_1tb_log, experiments_2026_8_26_operation_pruning_research_dieharder_pruned_winner_piped_piped_results, experiments_2026_8_26_operation_pruning_research_dieharder_gcd_fixed_gcd_test_log, experiments_2026_8_26_operation_pruning_research_dieharder_rgb_minimum_distance_fixed_rgb_min_dist_log [INFERRED]
- **ra_prng Algorithmic Evolution and Research Pipeline** — readme_ra_prng_architecture, readme_ra_prng2, experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_27_operand_position_search_handover_handover_spec [INFERRED]

## Communities (195 total, 54 thin omitted)

### Community 0 - "Wiring"
Cohesion: 0.07
Nodes (57): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for the winning operand-position rewiring found by…, Mirrors winner_wired.c's ra_reseed()/ra_hash() exactly (identical to… (+49 more)

### Community 1 - "winner_wired_v2.c"
Cohesion: 0.06
Nodes (57): Four-Axis Validation: periodicity, PractRand, avalanche, speed, permute + xor_fold State-Update Mechanism (kept unchanged), FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle() (+49 more)

### Community 2 - "State-Update Mechanism Research Results (Phase 1-3)"
Cohesion: 0.23
Nodes (15): cycle_measure.c (toy model), load_means(), main(), avalanche_gate_min_bit metric (per-bit floor 0.2, band [0.3,0.7]), State-Update Mechanism Research Handover (original spec), Handover Phase 3: L/M Mechanism Follow-up, inject (L) - one-directional overwrite mechanism, Instruction count (perf stat) as primary speed axis (+7 more)

### Community 3 - "others/avalanche_heatmap_original.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_capture(), original_reseed(), plot_comparison(), plot_single() (+9 more)

### Community 4 - "Completed Tasks"
Cohesion: 0.07
Nodes (41): 2026-08-29 correction: (8,4,rows=4) watchdog bug + stale headline number, auto_stop_power38.py (watchdog script), avalanche_check.py, .cycle_measure_ckpt_n8_w4_rows4_seed0.bin (CkptHeader checkpoint), Fix: CMD_PATTERN uses exact argv instead of broad pgrep prefix, Corrected λ > 3.045×10^11 for (8,4,rows=4), cycle_measure.py / cycle_measure.c (Brent's algorithm), enumerate_n2w4.py (+33 more)

### Community 5 - "tahap0_prototype.py"
Cohesion: 0.09
Nodes (38): blake2b8(), init_kandidat1(), init_kandidat2(), init_kandidat3_control(), init_kandidat4(), init_kandidat5(), init_original(), main() (+30 more)

### Community 6 - "Operand Position Search Experiment Results"
Cohesion: 0.07
Nodes (33): baseline_local PractRand 16GB Test Log, v01 PractRand 16GB Test Log (FAIL), v02 PractRand 16GB Test Log (FAIL), v03 PractRand 16GB Test Log (Pass), v04 PractRand 16GB Test Log (Pass), v06 PractRand 16GB Test Log (Pass), v07 PractRand 16GB Test Log (Pass), v08 PractRand 16GB Test Log (Pass) (+25 more)

### Community 7 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.08
Nodes (32): Scrambling Speed Benchmark (perf stat, per-call vs amortized, 30 runs), ra_prng2 NIST STS Test Result, ra_prng2 PractRand Test Result (up to 128 GB, no anomalies), ra_prng Build and Test Workflow, Contributing Guide, ra_prng Implementation Variant Roles, Repository Structure Specification, Brent Cycle Measure Checkpoint/Resume Architecture (+24 more)

### Community 8 - "ra_prng2 (Array-Based PRNG)"
Cohesion: 0.09
Nodes (23): BigCrush (TestU01), Bit Shifting, Rationale: Branchless Design for CPU Pipeline Efficiency, ChaCha20, Claude E. Shannon, Daniel Lemire, ent Entropy Tool, Rationale: Golden-Ratio Constants Reduce Collisions, Dead States, Short Cycles (+15 more)

### Community 9 - "other/avalanche_heatmap_original.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_capture(), original_reseed(), plot_comparison(), plot_single() (+9 more)

### Community 10 - "2026-8-30_addressable-shuffle/scc_test.py"
Cohesion: 0.13
Nodes (14): multi_key_distinctness_check(), Shuffle a fixed-size sample of input_tokens with n_keys different random keys;…, load_token_ids_from_file(), main(), ra_core(), ra_hash(), ra_init_state_addressable(), rot32() (+6 more)

### Community 11 - "2026-8-25_periodicity-heuristic-validation/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 12 - "2026-8-28_periodicity-heuristic-validation2/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 13 - "tahap5_bench.c"
Cohesion: 0.24
Nodes (19): FILE, find_variant(), main(), mode_init_cost(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10() (+11 more)

### Community 14 - "ICCS 2026 Research Paper: Array-Native Randomness"
Cohesion: 0.12
Nodes (19): Benchmark Source Comparisons Documentation, ZepFold Shuffling CLI Tool, Toy PRNG Avalanche Sanity Verification Gate, Brent Cycle Detection Methodology, Periodicity Validation Experiment Handover, Periodicity Heuristic Validation Results Report, Periodicity Heuristic Scalability & Non-Monotonicity Verdict, Initial Sweep Periodicity Measurement Summary (+11 more)

### Community 15 - "tahap4_bench.c"
Cohesion: 0.29
Nodes (17): FILE, main(), mode_init_cost(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10(), philox4x32_round() (+9 more)

### Community 16 - "avalanche_effect_analysis.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_reseed(), plot_comparison(), plot_single(), Path (+9 more)

### Community 17 - "Params"
Cohesion: 0.15
Nodes (10): group_size(), Params, Rescale the four fixed 32-bit plain-shift amounts to width w. s_w = round(s * w…, Top-w-bits truncation of a 32-bit golden-ratio-derived constant. Trap fix:…, G(n, rows): generalization of ra_hash's fixed 8/32 structural constants (256 =…, rescale_shifts(), truncate_const(), enumerate_state_space() (+2 more)

### Community 18 - "quality_gate.py"
Cohesion: 0.18
Nodes (15): ops_to_bitmask(), Encode an op set as pruned_prng.c's bitmask (bit i set = FLAG_ORDER[i] active)., avalanche_gate(), avalanche_gate_min_bit(), avalanche_stats(), _capture_first_cycle(), hamming(), practrand_prefix_gate() (+7 more)

### Community 19 - "Candidate"
Cohesion: 0.24
Nodes (15): Candidate, final_cons(), _build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32() (+7 more)

### Community 20 - "collision_scan.py"
Cohesion: 0.24
Nodes (11): blake2b 64-bit digest fingerprinting (collision probability baseline), digest_collision_prob(), main(), Q2: probability of cross-stream collision/overlap between different seeds'…, Birthday-bound estimate for pure hash-digest collision probability across…, scan_blocksweep(), scan_prefix(), random_seeds() (+3 more)

### Community 21 - "Parallelization Research Handover"
Cohesion: 0.12
Nodes (16): Embarrassingly Parallel Architecture (per-instance state, no locks), Operand-Position Search Research (2026-8-27), Operation Pruning Research (2026-8-26), Periodicity Heuristic Validation 2 (2026-8-28), Periodicity Heuristic Validation Research (2026-8-25), Read-Only Source Constraint (never modify ra_prng2/ra_prng3/winner_wired_v2.c in place), Seed/Cycle Collision Probability (Q2), SIMD Vectorization Feasibility (Q4) (+8 more)

### Community 22 - "toy_prng.py"
Cohesion: 0.26
Nodes (13): brent(), Brent's cycle-detection algorithm applied to toy_prng.next_state. Measures the…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, enumerate_state_space(), main(), Full state-space enumeration for n=2,w=4 (|S|=131072, default rows=2). Directly…, init_state(), next_state() (+5 more)

### Community 23 - "avalanche_heatmap_pruned_winner.py"
Cohesion: 0.21
Nodes (15): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), pruned_winner_capture(), pruned_winner_reseed(), Path (+7 more)

### Community 24 - "next_state"
Cohesion: 0.20
Nodes (13): brent(), Brent's cycle-detection algorithm applied to pruned_wired_toy_prng.next_state.…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, next_state(), State, ra_hash_gen_sequential(), Toy-scale (n, w, rows)-parameterized port of winner_wired_v2.c's permutation…, HASH_ACCESS=sequential port of winner_wired_v2.c's ra_hash: each output word is… (+5 more)

### Community 25 - "common.py"
Cohesion: 0.22
Nodes (10): ensure_ra_prng2_cli(), ndarray, Path, Shared helpers for the parallelization-research harness. Never modifies…, Bounded capture: run `binary --stream seed n`, return n uint32 values. Suitable…, Streaming capture: caller reads/closes proc.stdout incrementally. Use for large…, Compile the optional paper-exact comparator into this folder, if not already…, stream_popen() (+2 more)

### Community 26 - "stream_driven_shuffle.c"
Cohesion: 0.23
Nodes (15): Speed Benchmark Output (stream_driven_shuffle), FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash() (+7 more)

### Community 27 - "interleave_practrand.py"
Cohesion: 0.15
Nodes (11): other_winners_practrand.sh script, Avalanche seed diffusion (1-bit cons diff spreads through L/M in 255 steps), main(), Q1 Method B: interleave K independent `--stream <seed> <n>` streams word-by-…, run_interleave_practrand(), Method A: direct cross-correlation (Pearson + Bonferroni), Method B: interleaved PractRand, PractRand over-supply pattern (avoid short-by-N-words failure via BrokenPipeError cutoff) (+3 more)

### Community 28 - "Handover: Tahap 5 — Optimasi Biaya Init Addressable"
Cohesion: 0.13
Nodes (14): 1. Status & tujuan, 2. Formula & fakta struktural (sudah final, tidak perlu digali ulang), 3. Temuan disassembly (sudah dijalankan read-only, sesi persiapan 2026-08-30), 4. Kandidat optimasi, berperingkat, 5. Rencana file & eksperimen, 6. Gerbang validasi statistik (aturan mekanis, bukan penilaian bebas), 7. Batasan read-only (berlaku juga untuk Tahap 5), 8. Kondisi berhenti / pelaporan eksplisit (+6 more)

### Community 29 - "ablation_search.py"
Cohesion: 0.24
Nodes (12): _log(), main(), phase_a_hash_access_check(), phase_b_greedy_ablation(), Driver for the operation-pruning greedy ablation search. Two phases: Phase A:…, all_ops_baseline(), describe(), Catalog of ablatable operations for the ra_prng2 operation-pruning search.… (+4 more)

### Community 30 - "io_only_isolation_test.c"
Cohesion: 0.26
Nodes (13): FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash(), ra_init_state_addressable() (+5 more)

### Community 31 - "2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py"
Cohesion: 0.23
Nodes (12): decode_batch(), main(), pack_key(), Full state-space enumeration for n=4,w=4 (default rows=4 -> G=1), |S| = 4! *…, Cross-check vec_next_state against the scalar reference on random states drawn…, Pack (L, M, cons, it) arrays into a single uint64 key array, w bits per field,…, Mixed-radix decompose a flat index array into (lperm, m_0..m_{n-1}, it)…, Vectorized rotw: rotate low w bits of x left by r bits (mod w). x, r are numpy… (+4 more)

### Community 32 - "Fisher–Yates Shuffle"
Cohesion: 0.15
Nodes (13): Array Index Shuffling, Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), Bitwise Rotation (rotl32), R. Durstenfeld, Fisher–Yates Shuffle, Mutate Operator, Rationale: Non-Invertible Hash Reseed Trade-off (Reduces Period, Aids Diffusion) (+5 more)

### Community 33 - "Avalanche Effect (Bit-Flip Sensitivity)"
Cohesion: 0.17
Nodes (12): pruned_winner Avalanche Fraction Metric (Cycle 1 vs Cycle 90), Avalanche Effect Heatmap: pruned_winner Cycle 1 vs Cycle 90, Multi-Cycle Reseed Avalanche Diffusion Improvement (0.418 to 0.468), winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039), Avalanche Effect Heatmap (winner_wired_v2), winner_wired_v2 Wiring Parameters (a_xor=d, c_shift=a, rotc_amount=b, rotc_xor=a), Avalanche Effect (Bit-Flip Sensitivity), Joan Daemen and Vincent Rijmen (+4 more)

### Community 34 - "Design decisions (read before touching `pruned_wired_toy_prng.py`)"
Cohesion: 0.17
Nodes (11): 1. Op/wiring mapping ported to toy scale, 2. Tap-survivor generalization: top-2-by-count, and why, 3. Tap shift-width collapse (caught during design, not present in either parent), 4. `ra_hash_gen` divergence from ground truth (found while reading source, not assumed), 5. Self-check (passed) -- MUST stay passing, CRITICAL BUG found and fixed this session -- read before writing more scripts, Design decisions (read before touching `pruned_wired_toy_prng.py`), HANDOVER.md (periodicity-heuristic-validation2) (+3 more)

### Community 35 - "RESULTS.md — Parallelization Research Results"
Cohesion: 0.21
Nodes (10): perf_scaling.sh script, Q3: Multi-process throughput scalability, Q4: SIMD vectorization feasibility (stretch goal), RESULTS.md — Parallelization Research Results, ThreadPoolExecutor + shared dict/Lock fix (no inter-process serialization, stable pattern reused from Q3), Wall-time efficiency degradation is hardware-level (HT/cache/memory-bandwidth contention), not algorithmic (instructions_per_element constant), main(), Q3(a): wall-clock speedup/efficiency when running N independent… (+2 more)

### Community 36 - "BuildPyWithCEngine"
Cohesion: 0.20
Nodes (7): _build_py, Distribution, BinaryDistribution, BuildPyWithCEngine, Imperative build customization pyproject.toml's static [tool.setuptools] table…, After the normal .py copy step, compiles the ra_prng2 C engine and drops…, Declares this distribution as containing compiled, platform-specific code…

### Community 37 - "2025-10-5_scramble-design/scc_test.py"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 38 - "pruned_prng.c"
Cohesion: 0.38
Nodes (10): build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32(), FILE, has() (+2 more)

### Community 39 - "Cycle 1 Initial State Avalanche Fraction (0.487783)"
Cohesion: 0.29
Nodes (11): Avalanche Metrics (overall=0.487783, min_bit=0.472304), Warmup Cycle Avalanche Effect and Bit Diffusion, Cycle 1 Initial State Avalanche Fraction (0.487783), Cycle 3 Converged Avalanche Fraction (0.500751), Winner Wired Avalanche Heatmap Cycle 1 vs Cycle 3, Winner Wired Wiring Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a), Cycle 4 Steady State Avalanche (fraction 0.499713), Avalanche Effect Heatmap Comparison: Cycle 1 vs Cycle 4 (+3 more)

### Community 40 - "Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)"
Cohesion: 0.18
Nodes (10): 1. Summary Comparison: Unpruned Original vs `winner_wired_v2`, 2. Sebaran Siklus Utama (*Distinct $\lambda$ Distribution*), 3. Enumerasi Penuh & Uji Chi-Square vs Poisson(1), 4. Analisis Temuan Kunci (*Key Findings*), 5. Catatan Metodologi & Caveats (Sesuai `HANDOVER.md`), 6. Kesimpulan (*Verdict*), A. Skala $n=2, w=4$ ($|S| = 131.072$) — [`enumerate_n2w4_result.json`](./enumerate_n2w4_result.json), B. Skala $n=4, w=4$ ($|S| = 402.653.184$) — [`enumerate_n4w4_result.json`](./enumerate_n4w4_result.json) (+2 more)

### Community 41 - "cross_correlation.py"
Cohesion: 0.25
Nodes (10): Bonferroni correction (family-wise alpha 0.01 for K x K pair testing), analyze_group(), build_group(), main(), ndarray, Q1 Method A: Pearson cross-correlation between pairs of independent `--stream…, Return float64 matrix (K, n) of stream values for the given seeds., run() (+2 more)

### Community 42 - "tahap5_benchmark.py"
Cohesion: 0.36
Nodes (10): find_crossover(), main(), _parse_rows(), Path, Tahap 5: orchestrator for tahap5_bench / tahap5_bench_zmm512 /…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep() (+2 more)

### Community 43 - "SPEED_BENCHMARK.md"
Cohesion: 0.42
Nodes (9): benchmark_perf.txt (Historical Speed Benchmark), fread + Manual Tokenizer + fwrite I/O Pattern, ra_init_state_addressable Adds No Measurable Speed Cost, Double-Swap Hypothesis (Disproven), I/O Bottleneck Finding, Addressable-Init Data Shuffling Results, scrambler_addressable (Addressable-Init ZepFold Shuffle CLI), speed_benchmark_perf.log (Raw perf Output) (+1 more)

### Community 44 - "pcg_amortized.c"
Cohesion: 0.38
Nodes (9): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), Speed Benchmark Output (pcg amortized) (+1 more)

### Community 45 - "2026-8-28_periodicity-heuristic-validation2/run_sweep.py"
Cohesion: 0.33
Nodes (9): cycle_measure Binary Exit-42 Crash at n=8,w=4,rows=4, log2_fact(), main(), predicted(), Drives the pruned/wired periodicity-heuristic sweep. Same lambda_pred formula…, result_csv_name(), run_c(), run_python() (+1 more)

### Community 46 - "c/ra_prng2.c"
Cohesion: 0.36
Nodes (9): Read-only constraint on src/ra_prng2, src/ra_prng3, winner_wired_v2.c, FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed() (+1 more)

### Community 47 - "tahap4_benchmark.py"
Cohesion: 0.36
Nodes (9): find_crossover(), main(), _parse_rows(), Tahap 4: orchestrator for tahap4_bench (C harness, does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word., run_init_cost(), run_reinit_sweep(), run_throughput() (+1 more)

### Community 48 - "scrambler_addressable.c"
Cohesion: 0.38
Nodes (9): main(), ra_core(), ra_hash(), ra_init_state_addressable(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 49 - "source/ra_prng2.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 50 - "source/ra_prng3.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 51 - "ra_prng3 PRNG Generator"
Cohesion: 0.25
Nodes (9): ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), RNGing perf stat (ra_prng3), PractRand Test Results (ra_prng3), ent Entropy Test Tool, NIST Statistical Test Suite (STS), PractRand Test Suite, ra_prng3 PRNG Generator (+1 more)

### Community 52 - "scrambler.c"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 53 - "2026-8-25_periodicity-heuristic-validation/avalanche_check.py"
Cohesion: 0.31
Nodes (8): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for toy_prng.py. Port of…, One outer iteration, no rehash - mirrors zepfold_capture() in…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), Rotate the low w bits of x left by r bits (mod w)., rotw()

### Community 54 - "pruned_winner.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 55 - "pruned_winner_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 56 - "baseline.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 57 - "baseline_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 58 - "v01.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 59 - "v02.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 60 - "v03.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 61 - "v04.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 62 - "v06.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 63 - "v07.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 64 - "v08.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 65 - "v09.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 66 - "v10.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 67 - "v11.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 68 - "v12.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 69 - "v13.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 70 - "winner_wired.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 71 - "2026-8-28_periodicity-heuristic-validation2/enumerate_n4w4.py"
Cohesion: 0.39
Nodes (8): decode_batch(), main(), pack_key(), Full state-space enumeration for pruned/wired PRNG at n=4,w=4 (default rows=4…, Cross-check vec_next_state against the scalar reference on random states drawn…, rotw_vec(), validate(), vec_next_state()

### Community 72 - "measure_state_update.py"
Cohesion: 0.33
Nodes (8): avalanche_gate_min_bit_binary(), capture_first_cycle(), main(), practrand_prefix_gate_binary(), Path, Avalanche + PractRand measurement for the phase-1 (L-only) state-update-…, `CYCLES` outer cycles' worth of `c` values -- see the CAPTURE_LEN comment above…, # NOTE: unlike pruned_prng.c's CLI (quality_gate.py's convention, where

### Community 73 - "original_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 74 - "original_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 75 - "original_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 76 - "original_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 77 - "original_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 78 - "original_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 79 - "winner_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 80 - "winner_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 81 - "winner_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 82 - "winner_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 83 - "winner_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 84 - "winner_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 85 - "Addressable-Init Data Shuffling — RESULTS"
Cohesion: 0.22
Nodes (9): 1. Smoke test, 2. Parity check (`verify_parity.py`), 3. Statistik kualitas shuffle (`scc_test.py`, key=1, n=100.000), 4. Multi-key distinctness spot-check (`scc_test.py`), Addressable-Init Data Shuffling — RESULTS, Apa ini, Build & run, Follow-up (belum dilakukan sekarang) (+1 more)

### Community 86 - "ra_prng2_struct.c"
Cohesion: 0.44
Nodes (8): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), ra_step(), rot32()

### Community 87 - "ra_prng2_thread.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 88 - "philox_amortized.c"
Cohesion: 0.46
Nodes (7): main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32(), philox_refill_buffer(), rand_bounded()

### Community 89 - "xoshiro256_amortized.c"
Cohesion: 0.43
Nodes (7): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next(), Speed Benchmark Output (xoshiro256 amortized)

### Community 90 - "Dieharder Test Suite"
Cohesion: 0.29
Nodes (8): Dieharder Test Results (ra_prng2 original), RNGing perf stat (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), Dieharder Test Suite, ra_prng2 PRNG Generator, TestU01 BigCrush Test Suite, Robert G. Brown

### Community 91 - "ra_prng2 API Reference"
Cohesion: 0.36
Nodes (8): CPython Binding Documentation, Orphaned CPython Shared Object Rationale, RaPrng2 ctypes Wrapper, ra_prng2 API Reference, ra_core PRNG Core Loop, ra_hash State Reduction Function, RA_PRNG C Struct API, rot32 Left Bitwise Rotation

### Community 92 - "simd_prototype.c"
Cohesion: 0.39
Nodes (7): GCC -O3 -march=native auto-vectorizes 8-lane loop to AVX-512VL (vpternlogq), beating hand-written AVX2 intrinsics by ~15%, main(), rot32_avx2(), rot32_scalar(), run_scalar(), run_simd(), __m256i

### Community 93 - "Hamas A. Rahman"
Cohesion: 0.25
Nodes (8): Hamas A. Rahman, ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex, ra_prng2 and Beyond (Paper CA-162), Scopus, ra_prng GitHub Repository, hamzy hams (GitHub maintainer)

### Community 94 - "ent (Entropy Testing Tool)"
Cohesion: 0.29
Nodes (7): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result

### Community 95 - "pcg.c"
Cohesion: 0.52
Nodes (6): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), Speed Benchmark Output (pcg)

### Community 96 - "source/xoshiro256.c"
Cohesion: 0.48
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), Speed Benchmark Output (xoshiro256)

### Community 97 - "scrambler.py"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 98 - "2026-8-25_periodicity-heuristic-validation/run_sweep.py"
Cohesion: 0.48
Nodes (6): log2_fact(), main(), predicted(), Drives the full periodicity-heuristic-validation sweep. For each measurable (n,…, run_c(), run_python()

### Community 99 - "2026-8-28_periodicity-heuristic-validation2/avalanche_check.py"
Cohesion: 0.38
Nodes (6): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for pruned_wired_toy_prng.py.…, One outer iteration, no rehash -- mirrors next_state's inner loop exactly (tap-…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check()

### Community 100 - "Speed Benchmark: `scrambler_addressable` vs Algoritma Lain"
Cohesion: 0.29
Nodes (7): Hasil (elapsed time, detik, `perf stat -r 30`), Interpretasi, Konteks tabel lama, Metodologi, Pertanyaan, Reproduksi, Speed Benchmark: `scrambler_addressable` vs Algoritma Lain

### Community 101 - "PRNG Periodicity Analysis"
Cohesion: 0.29
Nodes (7): B. Harris, Donald E. Knuth, Makoto Matsumoto and Takuji Nishimura, Mersenne Twister (MT19937), PRNG Periodicity Analysis, Pierre L'Ecuyer, Random Mapping Theorem

### Community 102 - "source/chacha20.c"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 103 - "source/philox.c"
Cohesion: 0.60
Nodes (5): main(), philox4x32_10(), philox4x32_round(), philox_rand32(), rand_bounded()

### Community 104 - "Shuffling perf stat (ra_prng2 original)"
Cohesion: 0.67
Nodes (6): Shuffling perf stat (ra_prng2 original), Scrambling perf stat (ra_prng3), Linux perf stat Tool, Scrambler Tool, tokens.txt Input Corpus, scrambled.txt (scramble-design experiment output)

### Community 105 - "pcg32.c"
Cohesion: 0.70
Nodes (4): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom()

### Community 106 - "src/xoshiro256.c"
Cohesion: 0.70
Nodes (4): main(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 107 - "chacha20_amortized.c"
Cohesion: 0.60
Nodes (3): chacha20_block(), main(), rand_bounded()

### Community 108 - "Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2)"
Cohesion: 0.40
Nodes (5): Reseed-Driven Avalanche Strengthening Effect, Initial State Avalanche Fraction (Cycle 1: 0.418459), Post-Reseed Avalanche Fraction (Cycle 2: 0.468409), Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2), Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)

### Community 109 - "auto_stop_power38.py"
Cohesion: 0.60
Nodes (4): find_pid(), get_status(), main(), Monitors .cycle_measure_ckpt_n8_w4_rows4_seed0.bin and stops cycle_measure when…

### Community 110 - "JAX"
Cohesion: 0.40
Nodes (5): JAX, Philox (counter-mode PRNG), Tahap 4: speed break-even benchmark vs Philox, Threefry2x32, xoshiro256**

### Community 111 - "Orbit Addressing"
Cohesion: 0.40
Nodes (5): Orbit (deterministic trajectory selected by key), Orbit Addressing, Point Addressing (Philox f(counter,key)), CSAI 2025 RA-PRNG Paper, ICCS 2026 RA-PRNG Paper

### Community 112 - "Avalanche Effect Analysis (ALL_OPS)"
Cohesion: 0.50
Nodes (5): Avalanche Effect Analysis (ALL_OPS), Bit Difference Distribution Across 32 Flipped Bits, Cycle 1 Avalanche Fraction (0.497576), Cycle 2 Avalanche Fraction (0.502141), Avalanche Effect Heatmap (Cycle 1 vs Cycle 2)

### Community 113 - "heatmap.py"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 114 - "ra_prng2.py"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 115 - "c/ra_prng3.c"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 117 - "src/philox.c"
Cohesion: 0.83
Nodes (3): main(), philox4x32_10(), philox4x32_round()

### Community 118 - "Cycle 3 Avalanche Fraction (0.470588)"
Cohesion: 0.83
Nodes (4): Cycle 1 Avalanche Fraction (0.418459), Cycle 3 Avalanche Fraction (0.470588), Avalanche Heatmap: Cycle 1 vs Cycle 3 (pruned_winner), Reseed Avalanche Diffusion in Pruned Winner

### Community 119 - "Paper heuristic λ ≈ 0.7824·√|S|"
Cohesion: 0.50
Nodes (4): Paper heuristic λ ≈ 0.7824·√|S|, Research Question: does pruned/rewired algorithm shift the heuristic's λ-vs-λ_pred fit, Central Research Question (RESULTS.md), Verdict 2: heuristic behaves as conservative lower bound at large-scale configs only

### Community 120 - "Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))"
Cohesion: 0.50
Nodes (4): Tap shift-width collapse (e>=w yields identically-zero term), Tap-survivor generalization (top-2-by-count rule, G=8→(6,7)), Finding: non-vacuous tap pruning (G=4) gives closest fit to heuristic, Caveat: tap pruning is a no-op at G<=2 configs

### Community 122 - "Avalanche Effect Heatmap (Original Algorithm)"
Cohesion: 0.67
Nodes (4): Avalanche Effect, Avalanche Effect Heatmap (Original Algorithm), Hamming Distance Analysis, Original PRNG Algorithm

### Community 123 - "Graphify Knowledge Graph Rule"
Cohesion: 0.67
Nodes (3): Graphify Query & Navigation Protocol, Graphify Knowledge Graph Rule, Graphify Pipeline Workflow

### Community 124 - "Graphify-First Research Workflow Policy"
Cohesion: 0.67
Nodes (3): graphify-out/graph.json Knowledge Graph Artifact, graphify Skill, Graphify-First Research Workflow Policy

### Community 125 - "Cycle 1 vs Cycle 5 Avalanche Fraction Convergence"
Cohesion: 1.00
Nodes (3): Cycle 1 vs Cycle 5 Avalanche Fraction Convergence, Avalanche Heatmap: Pruned Winner Cycle 1 vs Cycle 5, Pruned Winner Architecture (TAP6, TAP7, ROT_C, SHR13)

### Community 126 - "Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)"
Cohesion: 1.00
Nodes (3): Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13), pruned_winner Cycle 1 Avalanche Sensitivity and Dead Zones, pruned_winner 4-Op Candidate Configuration (TAP6, TAP7, ROT_C, SHR13)

### Community 127 - "Greedy Operation-Pruning Search Paradigm"
Cohesion: 1.00
Nodes (3): Greedy Operation-Pruning Search Paradigm, Operation-Pruning Research Handover Spec, Limits of Zero-Drift Micro-Optimizations on ra_core

### Community 185 - "RaPrng2"
Cohesion: 0.24
Nodes (8): _find_lib_path(), _lib_handle(), _load_library(), Path, RaPrng2, ctypes binding for the ra_prng2 C engine. This is glue only - it marshals calls…, Thin wrapper around one RA_PRNG instance., CDLL

### Community 186 - "ra_prng2 (ZepFold, old fixed-init formula)"
Cohesion: 0.17
Nodes (8): Speed Benchmark Output (ra_prng2), parametrize, ra_prng2 (ZepFold, old fixed-init formula), Pure-Python unit tests for ra_prng2.ra_hash / ra_core. These do not require the…, Regression test for the historical bug: ra_hash must be a pure function writing…, test_ra_hash_does_not_mutate_input(), Cross-language equivalence: Python reference vs C ground truth. Safety net that…, test_python_reference_matches_c_ground_truth()

### Community 187 - "Handover: Addressable-Init Data Shuffling & Speed Investigation"
Cohesion: 0.22
Nodes (8): 1. Ringkasan alur & tujuan, 2. File & perannya (peta folder `experiments/2026-8-30_addressable-shuffle/`), 3. Hasil kunci (sudah final, jangan diulang), 4. Reproduksi cepat, 5. Constraint yang diwarisi, 6. Follow-up belum dikerjakan (opsional, untuk sesi lanjutan), 7. Setelah mengerjakan follow-up di atas, Handover: Addressable-Init Data Shuffling & Speed Investigation

### Community 188 - "gen_variants.py"
Cohesion: 0.43
Nodes (7): build(), gen_c(), main(), Path, Generate + compile the phase-1 (L-only) state-update-mechanism spectrum…, The "permute" mechanism must reproduce its target's real source exactly (it's…, verify_control_bit_identical()

### Community 189 - "gen_variants_m.py"
Cohesion: 0.39
Nodes (7): build(), gen_c(), main(), Path, Generate + compile the phase-2 (M-only) state-update-mechanism spectrum, per…, The M mechanism only affects the reseed fold (once per 255 steps) -- the first…, verify_first_cycle_matches_control()

### Community 190 - "full_scale_sanity_check.py"
Cohesion: 0.50
Nodes (4): check_no_repeat(), main(), Path, Full-scale (real 256x32-bit) practical periodicity sanity check, per…

### Community 191 - "xor_fold (M) - accumulating XOR fold, retained"
Cohesion: 0.50
Nodes (5): m_inject (M) - injection fold, rejected (catastrophic on winner), m_overwrite (M) - overwrite fold, rejected, m_permute (M) - relocation-only fold, rejected, winner_m_inject catastrophic PractRand failure (p=5.3e-351), xor_fold (M) - accumulating XOR fold, retained

### Community 192 - "tahap3_collision_scan.py"
Cohesion: 0.60
Nodes (4): main(), Tahap 3, Q2 (re-pointed): cross-key collision/overlap scan for…, run_random(), run_sequential()

### Community 193 - "tahap3_cross_correlation.py"
Cohesion: 0.60
Nodes (4): build_group_addr(), main(), Tahap 3, Q1 Method A (re-pointed): Pearson cross-correlation between pairs of…, run()

### Community 194 - "Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3"
Cohesion: 0.67
Nodes (4): Original Algorithm Avalanche Diffusion Comparison (Cycle 1 vs Cycle 3), Original Algorithm Cycle 1 Avalanche Fraction (0.497576), Original Algorithm Cycle 3 Avalanche Fraction (0.499740), Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3

## Ambiguous Edges - Review These
- `other_winners_practrand.sh` → `PractRand over-supply pattern (avoid short-by-N-words failure via BrokenPipeError cutoff)`  [AMBIGUOUS]
  experiments/2026-8-29_parallelization-research/RESULTS.md · relation: conceptually_related_to
- `Hamas A. Rahman` → `hamzy hams (GitHub maintainer)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to

## Knowledge Gaps
- **234 isolated node(s):** `other_winners_perf.sh script`, `other_winners_practrand.sh script`, `perf_state_update.sh script`, `perf_state_update_m.sh script`, `toy_probe_phase3.sh script` (+229 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **54 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `other_winners_practrand.sh` and `PractRand over-supply pattern (avoid short-by-N-words failure via BrokenPipeError cutoff)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `Hamas A. Rahman` and `hamzy hams (GitHub maintainer)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `Avalanche Effect Heatmap (winner_wired_v2)` connect `Avalanche Effect (Bit-Flip Sensitivity)` to `Wiring`, `winner_wired_v2.c`?**
  _High betweenness centrality (0.072) - this node is a cross-community bridge._
- **Why does `Avalanche Effect (Bit-Flip Sensitivity)` connect `Avalanche Effect (Bit-Flip Sensitivity)` to `ra_prng2 (Array-Based PRNG)`?**
  _High betweenness centrality (0.066) - this node is a cross-community bridge._
- **Why does `Parallelization Research Handover` connect `Parallelization Research Handover` to `winner_wired_v2.c`, `State-Update Mechanism Research Results (Phase 1-3)`, `RESULTS.md — Parallelization Research Results`, `Operand Position Search Experiment Results`, `c/ra_prng2.c`, `toy_prng.py`?**
  _High betweenness centrality (0.065) - this node is a cross-community bridge._
- **Are the 7 inferred relationships involving `Params` (e.g. with `capture_inner_outputs()` and `run_avalanche_check()`) actually correct?**
  _`Params` has 7 INFERRED edges - model-reasoned connections that need verification._
- **Are the 8 inferred relationships involving `Candidate` (e.g. with `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` and `avalanche_gate()`) actually correct?**
  _`Candidate` has 8 INFERRED edges - model-reasoned connections that need verification._