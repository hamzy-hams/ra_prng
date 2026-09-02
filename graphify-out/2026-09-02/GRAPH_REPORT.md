# Graph Report - ra_prng  (2026-09-02)

## Corpus Check
- 1 files · ~12,917,301 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 2329 nodes · 3578 edges · 291 communities (196 shown, 95 thin omitted)
- Extraction: 96% EXTRACTED · 4% INFERRED · 0% AMBIGUOUS · INFERRED: 126 edges (avg confidence: 0.87)
- Token cost: 38,575 input · 0 output

## Community Hubs (Navigation)
- benchmark_all.c
- tahap6_bench.c
- Speed Benchmark: `scrambler_addressable` vs Algoritma Lain
- Completed Tasks
- tahap0_prototype.py
- tahap4_bench.c
- pruned_winner 4-Operation Fast Variant
- Operand Position Search Experiment Results
- winner_wired_addressable.c
- Langkah 3: Interleaved 1TB for winner_wired_addressable (shared core-loop test)
- ra_prng2 (Array-Based PRNG)
- State-Update Mechanism Research Results (Phase 1-3)
- other/avalanche_heatmap_original.py
- cross_correlation.py
- 2026-8-25_periodicity-heuristic-validation/cycle_measure.c
- 2026-8-28_periodicity-heuristic-validation2/cycle_measure.c
- tahap5_bench.c
- winner_wired_v2.c
- ICCS 2026 Research Paper: Array-Native Randomness
- Karakterisasi ambang K minimum aman — `ra_core_singleblock`
- Wiring
- avalanche_effect_analysis.py
- others/avalanche_heatmap_original.py
- Params
- Candidate
- scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)
- RESULTS: production-candidate battery for `ra_core.c`
- RaPrng2
- Parallelization Research Handover
- toy_prng.py
- avalanche_heatmap_pruned_winner.py
- avalanche_stats
- HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup
- next_state
- stream_driven_shuffle.c
- benchmark_all_run.py
- diag_keyzero_guard.c
- quality_gate.py (avalanche_stats)
- collision_scan.py
- 2026-9-1_production-candidate-battery/RESULTS.md
- common.py (TIERS_Q2)
- Handover: PRNG family benchmark (ra_prng semua varian vs PRNG terkenal)
- io_only_isolation_test.c
- `ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi
- diag_prereseed.c
- 2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py
- stream_values
- c/ra_prng2.c
- ra_core.c
- Fisher–Yates Shuffle
- Avalanche Effect (Bit-Flip Sensitivity)
- Design decisions (read before touching `pruned_wired_toy_prng.py`)
- BuildPyWithCEngine
- 2025-10-5_scramble-design/scc_test.py
- pruned_prng.c
- Cycle 1 Initial State Avalanche Fraction (0.487783)
- Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)
- ensure_ra_prng2_cli
- tahap5_benchmark.py
- tahap6_benchmark.py
- 2026-8-30_addressable-shuffle/scc_test.py
- bench_ra_core.c
- ra_core_singleblock
- RESULTS: key=0 degenerate-state guard-XOR fix
- tahap3_collision_scan.py
- ablation_search.py
- 2026-8-28_periodicity-heuristic-validation2/run_sweep.py
- interleave_practrand.py
- tahap4_benchmark.py
- scrambler_addressable.c
- scrambler_wired_addressable.c
- single_mread_isolation_test.c
- diag_hashed_init.c
- diag_orbit_regression.c
- verify_fresh_build.py
- pcg_amortized.c
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
- Handover: 1TB interleaved-PractRand anomaly follow-up
- Handover: Addressable-Init Data Shuffling & Speed Investigation
- Hasil validasi
- scrambler_addressable.py
- scrambler_wired_addressable.py
- no_reseed_isolation_test.c
- RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)
- bench_init_avalanche.c
- bench_init_keyterm_mul.c
- bench_warmup_cost.c
- bench_warmup_cost2.c
- diag_init_keyterm_mul.c
- diag_paperlike_cycle.c
- diag_warmup_depth.c
- diag_warmup_discard.c
- Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)
- ra_prng2_struct.c
- ra_prng2_thread.c
- Dieharder Test Suite
- ra_prng2 API Reference
- HANDOVER_TAHAP6.md
- Handover: Tahap 5 — Optimasi Biaya Init Addressable
- tahap3_cross_correlation.py
- Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`
- diag_init_avalanche.c
- diag_init_keyterm_avalanche.c
- diag_init_keyterm_xor.c
- RESULTS: pre-reseed experiment untuk defect K=1 `ra_core_singleblock`
- ent (Entropy Testing Tool)
- pcg32.c
- src/xoshiro256.c
- xoshiro256_amortized.c
- scrambler.py
- 2026-8-25_periodicity-heuristic-validation/run_sweep.py
- src/chacha20.c
- 2026-8-28_periodicity-heuristic-validation2/avalanche_check.py
- PRNG Family Benchmark Results
- simd_prototype.c
- RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`
- PRNG Periodicity Analysis
- source/chacha20.c
- chacha20_amortized.c
- pcg.c
- source/xoshiro256.c
- Shuffling perf stat (ra_prng2 original)
- run_dieharder_battery.py
- run_dieharder_battery_multikey.py
- verify_unification.py
- test_ra_hash.py
- Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2)
- avalanche_gate
- auto_stop_power38.py
- cross_correlation_ra_prng2.py
- JAX
- Orbit Addressing
- avalanche_orbit_singleblock_k255.py
- scrambler_ra_core_singleblock.c
- Avalanche Effect Analysis (ALL_OPS)
- heatmap.py
- Hamas A. Rahman
- ra_prng2.py
- c/ra_prng3.c
- Cycle 3 Avalanche Fraction (0.470588)
- Paper heuristic λ ≈ 0.7824·√|S|
- Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))
- Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche
- robust_reinit.py
- avalanche_ra_core_singleblock_k1.py
- Avalanche Effect Heatmap (Original Algorithm)
- Graphify Knowledge Graph Rule
- dev_urandom.c
- Graphify-First Research Workflow Policy
- Cycle 1 vs Cycle 5 Avalanche Fraction Convergence
- Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)
- Greedy Operation-Pruning Search Paradigm
- collision_scan.py
- Q3: Multi-process throughput scalability
- pcg.c (PCG32 comparison binary)
- xoshiro256.c (fread+tokenizer comparison binary)
- run_speed_benchmark.sh
- verify_parity.py
- verify_parity_wired.py
- Guard-XOR Fix (key=0 defect)
- Original Algorithm Avalanche Bit Distribution & Diffusion
- other_winners_perf.sh
- other_winners_practrand.sh
- perf_state_update.sh
- perf_state_update_m.sh
- toy_probe_phase3.sh
- toy_sweep_m.sh
- toy_sweep_phase3.sh
- perf_scaling.sh
- Q4: SIMD vectorization microbenchmark (stretch goal)
- Kandidat 5 (user candidate, adopted)
- Dieharder Orbit Regression Raw Output
- bench_ra_core Throughput/Reinit-Sweep Results Log
- BUILD.sh
- multikey_stream_singleblock_k1.c
- CI Build and Test Job
- conftest.py
- Addressable-Init Research (Tahap 0-5)
- PCG32 RNG Speed Benchmark Result
- Philox4x32 RNG Speed Benchmark Result
- xoshiro256** RNG Speed Benchmark Result
- benchmarks/comparisons/source/README.md (naming convention)
- cpython/__init__.py
- bindings/__init__.py
- Project Root Changelog
- 2025-10-5_scramble-design/scrambler.c (predecessor)
- rerun_n8_w4_rows4_stderr.txt
- rerun_n8_w4_rows4_stdout.txt
- Finding: very short transient tail (μ<=1) across all seeds
- Sweep Output (empty file)
- Kandidat 1 (L=f(seed,counter) splitmix, M unchanged)
- Kandidat 2 (L and M independently keyed)
- Kandidat 4 (L mult from seed, additive counter term)
- Kandidat 2: independent L/M keying
- Kandidat 1
- Kandidat 2
- Kandidat 4
- v0_baseline (control init)
- 8. Setelah mengerjakan follow-up di atas
- Addressable-Init Shuffle Handover
- I/O text parsing as root cause of ~2.2x slowdown
- io_only_isolation_test.c (I/O cost isolation diagnostic)
- I/O optimization: fread + manual tokenizer + fwrite (done)
- Rename ra_core to ra_shuffle (done)
- ra_shuffle (renamed shuffle function, formerly ra_core)
- scc_test.py (statistical & distinctness validation)
- scrambler_addressable (Shuffle CLI)
- speed_bench/ benchmark harness
- stream_driven_shuffle.c (single-swap hypothesis test CLI)
- verify_parity.py (C vs Python parity check)
- Multi-key distinctness spot-check (200 keys, 0 collisions)
- Orbit Addressing integrated into shuffle state init
- Parity validation (63/63 PASS, C vs Python bit-identical)
- Speed summary: ZepFold ~2.2x slower, root cause is I/O
- Statistical validation (entropy/chi2/runs/SCC, no regression)
- ra_core (shuffle loop, scrambler_addressable.c)
- ra_hash (block-reseed, 255-element)
- scrambler_addressable.py (Python reference port)
- Shuffled Token Output
- Speed Benchmark Output (chacha20)
- Speed Benchmark Output (philox)
- Speed Benchmark Output (ra_prng2)
- Speed Benchmark Output (scrambler_addressable)
- chacha20.c (ChaCha20 comparison binary)
- I/O (fscanf/fprintf per-token) is the dominant ~85% bottleneck, not the PRNG
- io_only_isolation_test.c (I/O-only isolation harness)
- load_tokens_from_file (fscanf per-token -> fread+tokenizer)
- no_reseed_isolation_test.c (diagnostic-only, reseed removed)
- philox.c (Philox4x32 comparison binary)
- ra_core(key, rng, raw_stream) — renamed to ra_shuffle
- ra_init_state_addressable(key)
- run_speed_benchmark.sh (perf stat -r 30 over 12 binaries)
- save_tokens_to_file (fprintf per-token -> single fwrite)
- scrambler_addressable (addressable-init shuffle CLI)
- single_mread_isolation_test.c (diagnostic-only, 1-elem M[] read)
- Single-run benchmark variance caution (stream_driven_shuffle ±8.27%)
- stream_driven_shuffle.c (single-swap Fisher-Yates driven by ra_core)
- winner_wired_addressable.c (source of ra_core generator)
- tokens.txt sample data (100,000 tokens)
- dieharder ra_core_singleblock multikey piped results
- Avalanche Orbit/Singleblock K=255 Rerun Log
- Avalanche Singleblock K=1 Log
- Orbit Addressing (f(key) state-init mechanism)
- ra-prng
- ra_prng2 Package Changelog
- ra_prng3 Package Changelog
- Image Encrypting Documentation
- ZepFold (token-shuffling CLI approach)
- 4. Kandidat optimasi, berperingkat
- wall_time_scaling.py
- ra_core_singleblock_prereseed Candidate
- ZepFold (token-shuffling CLI approach)

## God Nodes (most connected - your core abstractions)
1. `Params` - 33 edges
2. `ra_prng2 (Array-Based PRNG)` - 29 edges
3. `State-Update Mechanism Research Results (Phase 1-3)` - 23 edges
4. `Candidate` - 20 edges
5. `Parallelization Research Handover` - 20 edges
6. `Operand Position Search Experiment Results` - 19 edges
7. `Wiring` - 18 edges
8. `stream()` - 17 edges
9. `RESULTS: production-candidate battery for `ra_core.c`` - 17 edges
10. `common.py (TIERS_Q2)` - 17 edges

## Surprising Connections (you probably didn't know these)
- `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` --semantically_similar_to--> `Candidate`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/avalanche_heatmap_pruned_winner_cycle1_vs_cycle2.png → experiments/2026-8-26_operation-pruning-research/pruned_prng.py
- `hamzy hams (GitHub maintainer)` --conceptually_related_to--> `Hamas A. Rahman`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf → README.md
- `winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039)` --conceptually_related_to--> `Avalanche Effect (Bit-Flip Sensitivity)`  [INFERRED]
  experiments/2026-8-27_operand-position-search/avalanche_heatmap_winner_v2.png → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf
- `Hamas A. Rahman` --references--> `ra_prng2 (Array-Based PRNG)`  [EXTRACTED]
  README.md → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf
- `ra_prng2 (Array-Based PRNG)` --cites--> `Dieharder Test Suite`  [EXTRACTED]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf → benchmarks/results/ra_prng2_original/dieharder_test.txt

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Candidate A Diagnostic Build Pattern** — experiments_2026_9_2_singleblock_o_width_fix_handover_wide_o_candidate, experiments_2026_9_2_singleblock_o_width_fix_diag_wideo_singleblock, experiments_2026_8_26_operation_pruning_research_pruned_prng_build_o, experiments_2026_9_2_singleblock_k_threshold_characterization_multikey_stream_k [INFERRED 0.85]
- **K-kecil Defect Discovery Chain** — experiments_2026_9_2_singleblock_o_width_fix_handover_k_kecil_defect, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_singleblock, experiments_2026_9_2_singleblock_k_threshold_characterization_results, experiments_2026_9_1_production_candidate_battery [INFERRED 0.80]
- **Two-Stage Fix Strategy (Wide-o + Warm-up Fallback)** — experiments_2026_9_2_singleblock_o_width_fix_handover_wide_o_candidate, experiments_2026_9_2_singleblock_o_width_fix_handover_warmup_fallback_candidate, project_multikey_remix_search [EXTRACTED 1.00]
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_philox4x32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]
- **ra_prng2 Statistical Validation Test Suite** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_practrand, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_bigcrush [EXTRACTED 0.90]
- **winner_wired_v2 cycle grafted onto ZepFold shuffle-loop shape (scrambler_wired_addressable)** — experiments_2026_8_30_addressable_shuffle_scrambler_wired_addressable_c, winner_wired_addressable_c, experiments_2026_8_30_addressable_shuffle_scrambler_addressable_c [EXTRACTED 0.90]
- **Validation pipeline for wired addressable shuffle variant** — experiments_2026_8_30_addressable_shuffle_scrambler_wired_addressable_c, experiments_2026_8_30_addressable_shuffle_verify_parity_wired_py, experiments_2026_8_30_addressable_shuffle_scc_test_wired_py [EXTRACTED 0.90]
- **ra_prng2 State Transition Operator Composition (F = Mutate∘Permute∘Reseed)** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_transition_operator_f, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_mutate_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_permute_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_reseed_operator [EXTRACTED 0.95]
- **Byte-for-byte benchmark harness lineage (winner_wired_addressable -> tahap4 -> tahap5 -> tahap6)** — experiments_2026_8_30_addressable_init_research_winner_wired_addressable, experiments_2026_8_30_addressable_init_research_tahap4_bench, experiments_2026_8_30_addressable_init_research_tahap5_bench, experiments_2026_8_30_addressable_init_research_tahap6_bench [EXTRACTED 1.00]
- **Addressable-init research pipeline (Tahap 0-6)** — experiments_2026_8_30_addressable_init_research_results_tahap0, experiments_2026_8_30_addressable_init_research_results_tahap1, experiments_2026_8_30_addressable_init_research_results_tahap2, experiments_2026_8_30_addressable_init_research_results_tahap3, experiments_2026_8_30_addressable_init_research_results_tahap4, experiments_2026_8_30_addressable_init_research_results_tahap5, experiments_2026_8_30_addressable_init_research_handover_tahap6_tahap6 [EXTRACTED 1.00]
- **Bit-identical self-check chain linking pruned toy model to full-scale reference and winner C source** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_self_check_full_scale, experiments_2026_8_28_periodicity_heuristic_validation2_handover_pruned_wired_toy_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wired_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wiring_module, experiments_2026_8_28_periodicity_heuristic_validation2_handover_winner_wired_v2 [EXTRACTED 1.00]
- **2026-08-29 correction of (8,4,rows=4) lambda bound, documented across HANDOVER/RESULTS/STATUS** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_2026_08_29_correction, experiments_2026_8_28_periodicity_heuristic_validation2_handover_stale_lambda_lower_bound, experiments_2026_8_28_periodicity_heuristic_validation2_handover_corrected_lambda_value, experiments_2026_8_28_periodicity_heuristic_validation2_handover_checkpoint_file, experiments_2026_8_28_periodicity_heuristic_validation2_results_footnote_correction, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_summary, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_lower_bound_restated [EXTRACTED 1.00]
- **pruned_winner Cycle 1 vs Cycle 90 Avalanche Analysis** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_reseed_diffusion_result, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_avalanche_fraction [EXTRACTED 1.00]
- **winner_wired Avalanche Diffusion Convergence Across Cycles** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_cycle4_avalanche [EXTRACTED 1.00]
- **Winner Wired PRNG Avalanche Effect Analysis** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_avalanche_performance [EXTRACTED 1.00]
- **Avalanche Effect Cycle Comparison** — experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_avalanche_effect_analysis, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_1_avalanche, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_2_avalanche [EXTRACTED 1.00]
- **Family Use-Case to Algorithm Mapping** — experiments_2026_9_1_family_productionization_handover_usecase_mapping, experiments_2026_9_1_family_productionization_handover_winner_wired_unified, experiments_2026_8_30_addressable_init_research_tahap6_bench_ra_core_singleblock, experiments_2026_9_1_family_productionization_handover_ra_prng_family [EXTRACTED 1.00]
- **12-candidate PRNG family benchmark roster (ra_prng family vs literature PRNGs)** — src_ra_prng2_c_ra_prng2, experiments_2026_8_27_operand_position_search_winner_wired_v2, experiments_2026_8_30_addressable_init_research_tahap6_bench [EXTRACTED 1.00]
- **1TB interleaved-PractRand anomaly diagnostic investigation** — experiments_2026_8_29_parallelization_research_handover_1tb_followup, experiments_2026_8_29_parallelization_research_results, experiments_2026_8_30_addressable_init_research_results [EXTRACTED 1.00]
- **Addressable-init research pipeline Tahap 0 through Tahap 6** — experiments_2026_8_30_addressable_init_research_results_kandidat5 [EXTRACTED 1.00]
- **Guard-XOR Fix Implementation & Verification Chain** — experiments_2026_9_1_keyzero_guard_fix_results_guard_xor_fix, experiments_2026_9_1_keyzero_guard_fix_diag_keyzero_guard, experiments_2026_9_1_keyzero_guard_fix_pick_guard_constants, experiments_2026_9_1_family_productionization_ra_core_guard_l, experiments_2026_9_1_family_productionization_ra_core_guard_m [EXTRACTED 1.00]
- **Production-Candidate Battery Verification Axes (Step 0-3)** — experiments_2026_9_1_production_candidate_battery_results, experiments_2026_9_1_production_candidate_battery_quality_gate, experiments_2026_8_29_parallelization_research_cross_correlation_ra_core_singleblock, experiments_2026_8_29_parallelization_research_collision_scan_ra_core_singleblock, experiments_2026_9_1_production_candidate_battery_results_key_zero_defect [EXTRACTED 1.00]
- **(a)/(a')/(b)/(c) hypothesis decision matrix synthesized in Langkah 4** — experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah4_sintesis, experiments_2026_8_29_parallelization_research_handover_1tb_followup_hypothesis_a, experiments_2026_8_29_parallelization_research_handover_1tb_followup_hypothesis_a_prime, experiments_2026_8_29_parallelization_research_handover_1tb_followup_hypothesis_b, experiments_2026_8_29_parallelization_research_handover_1tb_followup_hypothesis_c [EXTRACTED 1.00]
- **K=255 Throughput Benchmark: ra_core vs Philox** — experiments_2026_9_1_production_candidate_battery_bench_ra_core_results_philox_baseline, experiments_2026_9_1_production_candidate_battery_bench_ra_core_results_log [EXTRACTED 1.00]
- **L Mechanism Spectrum Tested in Phase 1** — experiments_2026_8_28_state_update_mechanism_research_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_overwrite_mechanism [EXTRACTED 1.00]
- **M Mechanism Spectrum Tested in Phase 2** — experiments_2026_8_28_state_update_mechanism_research_xor_fold_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_overwrite_mechanism [EXTRACTED 1.00]
- **Original Algorithm Avalanche Cycle Evolution (Cycle 1 vs Cycle 3)** — experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_heatmap, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_avalanche_diffusion_comparison [EXTRACTED 1.00]
- **12-candidate PRNG family benchmark roster** — experiments_2026_8_31_prng_family_benchmark_handover_candidate_paperorig, experiments_2026_8_31_prng_family_benchmark_handover_candidate_wiredv2, experiments_2026_8_31_prng_family_benchmark_handover_candidate_addrcont, experiments_2026_8_31_prng_family_benchmark_handover_candidate_singleblock_k1, experiments_2026_8_31_prng_family_benchmark_handover_candidate_singleblock_k255, experiments_2026_8_31_prng_family_benchmark_handover_candidate_philox, experiments_2026_8_31_prng_family_benchmark_handover_candidate_xoshiro256, experiments_2026_8_31_prng_family_benchmark_handover_candidate_pcg32, experiments_2026_8_31_prng_family_benchmark_handover_candidate_chacha20, experiments_2026_8_31_prng_family_benchmark_handover_candidate_dev_urandom, experiments_2026_8_31_prng_family_benchmark_handover_candidate_mt19937, experiments_2026_8_31_prng_family_benchmark_handover_candidate_splitmix64 [EXTRACTED 1.00]
- **pruned_winner Avalanche Analysis and Metric Evaluation** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_figure, experiments_2026_8_26_operation_pruning_research_pruned_winner_avalanche_behavior, experiments_2026_8_26_operation_pruning_research_pruned_winner_candidate_configuration [EXTRACTED 1.00]
- **winner_wired_v2 and winner_wired_addressable both implement the same shared core generation loop** — experiments_2026_8_29_parallelization_research_handover_1tb_followup_winner_wired_v2, experiments_2026_8_29_parallelization_research_handover_1tb_followup_winner_wired_addressable, experiments_2026_8_29_parallelization_research_handover_1tb_followup_core_generation_loop [EXTRACTED 1.00]
- **Prior Research Folders That Tested Only Single-Stream Axes** — concept_periodicity_heuristic_validation_research, concept_operation_pruning_research, concept_operand_position_search_research, concept_state_update_mechanism_research, concept_periodicity_heuristic_validation2_research [EXTRACTED 1.00]
- **Tahap 0 candidate init formulas (Kandidat 1-5) evaluated together** — experiments_2026_8_30_addressable_init_research_handover_kandidat1, experiments_2026_8_30_addressable_init_research_handover_kandidat2, experiments_2026_8_30_addressable_init_research_handover_kandidat3_control, experiments_2026_8_30_addressable_init_research_handover_kandidat4, experiments_2026_8_30_addressable_init_research_handover_kandidat5 [EXTRACTED 1.00]
- **Tahap 1 Bit-Identical Unification** — experiments_2026_9_1_family_productionization_handover_tahap1_unification, experiments_2026_8_27_operand_position_search_winner_wired_v2_winner_wired_v2, experiments_2026_8_30_addressable_init_research_winner_wired_addressable_winner_wired_addressable [EXTRACTED 1.00]
- **Tahap 2 Production Validation Battery** — experiments_2026_9_1_family_productionization_handover_tahap2_validation_battery, experiments_2026_9_1_family_productionization_handover_winner_wired_unified, experiments_2026_8_30_addressable_init_research_tahap6_bench_ra_core_singleblock, quality_gate_avalanche_gate_min_bit [EXTRACTED 1.00]
- **Watchdog process-matching bug, its fix, and the sweep changes it required** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_watchdog_pgrep_bug, experiments_2026_8_28_periodicity_heuristic_validation2_handover_auto_stop_power38, experiments_2026_8_28_periodicity_heuristic_validation2_handover_cmd_pattern_fix, experiments_2026_8_28_periodicity_heuristic_validation2_handover_run_sweep, experiments_2026_8_28_periodicity_heuristic_validation2_handover_manual_only_exclusion [EXTRACTED 1.00]
- **Array-Native Data Permutation and Fingerprinting Workflow** — research_iccs2026_ra_prng_avalanche_sensitivity, sub_projects_fingerprinting_tools_concept, benchmarks_comparisons_source_zepfold_cli [INFERRED 0.75]
- **ra_prng2 Original Benchmark Report Set** — benchmarks_results_ra_prng2_original_rnging_perf_stat_, benchmarks_results_ra_prng2_original_testu01, benchmarks_results_ra_prng2_original_dieharder_test, benchmarks_results_ra_prng2_original_shuffling_perf_stat [INFERRED 0.80]
- **ra_prng3 Benchmark Report Set** — benchmarks_results_ra_prng3_nist_sts_test, benchmarks_results_ra_prng3_dieharder_test, benchmarks_results_ra_prng3_ent_test, benchmarks_results_ra_prng3_perf_stat_rnging, benchmarks_results_ra_prng3_perf_stat_scrambling, benchmarks_results_ra_prng3_practrand_test [INFERRED 0.80]
- **ChaCha20 Speed + Entropy Benchmark Suite** — benchmarks_comparisons_rnging_speed_chacha20_result, benchmarks_comparisons_entropy_chacha20_result [INFERRED 0.85]
- **Cumulative gap-explanation chain: I/O (~85%) -> reseed/M-width (~13% of residual) -> recurrence pruning (~5%)** — experiments_2026_8_30_addressable_shuffle_speed_benchmark_io_bottleneck_finding, experiments_2026_8_30_addressable_shuffle_speed_benchmark_gap_decomposition_finding, experiments_2026_8_30_addressable_shuffle_speed_benchmark_recurrence_pruning_finding [INFERRED 0.85]
- **Four-step 1TB diagnostic sequence (Langkah 1-4) forming the decision procedure** — experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah1_singlestream_v2_1tb, experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah2_reseed_confirm, experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah3_addressable_1tb, experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah4_sintesis [INFERRED 0.85]
- **Pruned Winner Avalanche Progression Analysis (Cycle 1 vs Cycle 2)** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_heatmap, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle1_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle2_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_avalanche_strengthening [INFERRED 0.85]
- **Winner Wired Warmup Cycle Avalanche Progression** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_winner_wired_wiring, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle1_avalanche, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle3_avalanche [INFERRED 0.85]
- **Avalanche Effect Bit Diffusion Evaluation** — experiments_others_avalanche_effect_heatmap_figure, experiments_others_avalanche_effect_heatmap_avalanche_effect, experiments_others_avalanche_effect_heatmap_hamming_distance_analysis, experiments_others_avalanche_effect_heatmap_original_algorithm [INFERRED 0.85]
- **Four-Axis Validation Framework (periodicity/speed/avalanche/PractRand)** — experiments_2026_8_25_periodicity_heuristic_validation_toy_prng, experiments_2026_8_28_state_update_mechanism_research_instruction_count_speed_axis, experiments_2026_8_28_state_update_mechanism_research_avalanche_gate_min_bit, experiments_2026_8_28_state_update_mechanism_research_practrand_16gb_tier [INFERRED 0.85]
- **Periodicity Heuristic Empirical Validation Lifecycle** — research_iccs2026_ra_prng_periodicity_heuristic, experiments_2026_8_25_periodicity_heuristic_validation_toy_model_spec, experiments_2026_8_25_periodicity_heuristic_validation_scaling_verdict [INFERRED 0.85]
- **winner_wired/ra_core evolutionary lineage of the pruned+wired recurrence** — experiments_2026_8_27_operand_position_search_baseline, experiments_2026_8_27_operand_position_search_winner_wired, experiments_2026_8_27_operand_position_search_winner_wired_v2, experiments_2026_8_30_addressable_init_research_winner_wired_addressable, experiments_2026_9_1_family_productionization_ra_core [INFERRED 0.85]
- **Q1-Q4 Parallelization Research Questions** — concept_stream_independence_testing, concept_seed_collision_probability, concept_throughput_scalability, concept_simd_vectorization_feasibility [INFERRED 0.85]
- **Q1 cross-stream independence investigation (Method A + Method B)** — experiments_2026_8_29_parallelization_research_results_q1_independence, experiments_2026_8_29_parallelization_research_results_method_a_cross_correlation, experiments_2026_8_29_parallelization_research_results_method_b_interleaved_practrand, experiments_2026_8_29_parallelization_research_results_cross_correlation_py, experiments_2026_8_29_parallelization_research_results_interleave_practrand_py [INFERRED 0.85]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_practrand, concept_ent [INFERRED 0.85]
- **Rantai investigasi defect K kecil ra_core_singleblock** — experiments_2026_9_1_keyzero_guard_fix_results_k1_singleblock_defect, experiments_2026_9_2_singleblock_prereseed_experiment_results, experiments_2026_9_2_singleblock_k_threshold_characterization_results_safe_k_threshold [INFERRED 0.90]
- **Pruned Winner Cycle 1 vs Cycle 3 Avalanche Comparison** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_reseed_avalanche_diffusion [INFERRED 0.95]
- **Avalanche Proxy vs Full Empirical Quality Disconnect** — experiments_2026_8_26_operation_pruning_research_results_shift_repair_failure, experiments_2026_8_26_operation_pruning_research_status_gate_size_traps, experiments_2026_8_27_operand_position_search_handover_dead_seed_bits, experiments_2026_8_27_operand_position_search_handover_avalanche_heatmap_baseline [INFERRED]
- **Pruned Winner Multi-battery Statistical Validation** — experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_26_operation_pruning_research_practrand_pruned_winner_1tb_practrand_1tb_log, experiments_2026_8_26_operation_pruning_research_dieharder_pruned_winner_piped_piped_results, experiments_2026_8_26_operation_pruning_research_dieharder_gcd_fixed_gcd_test_log, experiments_2026_8_26_operation_pruning_research_dieharder_rgb_minimum_distance_fixed_rgb_min_dist_log [INFERRED]
- **ra_prng Algorithmic Evolution and Research Pipeline** — readme_ra_prng_architecture, readme_ra_prng2, experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_27_operand_position_search_handover_handover_spec [INFERRED]

## Communities (291 total, 95 thin omitted)

### Community 0 - "benchmark_all.c"
Cohesion: 0.06
Nodes (61): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for the winning operand-position rewiring found by…, Mirrors winner_wired.c's ra_reseed()/ra_hash() exactly (identical to… (+53 more)

### Community 1 - "tahap6_bench.c"
Cohesion: 0.09
Nodes (49): core_entry_t, FILE, chacha20_block(), find_core(), kat_chacha20(), kat_mt19937(), kat_pcg32(), kat_philox() (+41 more)

### Community 2 - "Speed Benchmark: `scrambler_addressable` vs Algoritma Lain"
Cohesion: 0.08
Nodes (38): cycle_measure.c (toy model), load_means(), main(), avalanche_gate_min_bit metric (per-bit floor 0.2, band [0.3,0.7]), check_no_repeat(), main(), Path, Full-scale (real 256x32-bit) practical periodicity sanity check, per… (+30 more)

### Community 3 - "Completed Tasks"
Cohesion: 0.07
Nodes (41): 2026-08-29 correction: (8,4,rows=4) watchdog bug + stale headline number, auto_stop_power38.py (watchdog script), avalanche_check.py, .cycle_measure_ckpt_n8_w4_rows4_seed0.bin (CkptHeader checkpoint), Fix: CMD_PATTERN uses exact argv instead of broad pgrep prefix, Corrected λ > 3.045×10^11 for (8,4,rows=4), cycle_measure.py / cycle_measure.c (Brent's algorithm), enumerate_n2w4.py (+33 more)

### Community 4 - "tahap0_prototype.py"
Cohesion: 0.09
Nodes (40): blake2b8(), init_kandidat1(), init_kandidat2(), init_kandidat3_control(), init_kandidat4(), init_kandidat5(), init_original(), main() (+32 more)

### Community 5 - "tahap4_bench.c"
Cohesion: 0.10
Nodes (36): main(), philox4x32_10(), philox4x32_round(), main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32() (+28 more)

### Community 6 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.12
Nodes (32): Manual accumulator (strength reduction) ruled out -- 4.4-5x slower, defeats auto-vectorization, Rank 1: -mprefer-vector-width=512 flag-only speedup (not adopted), Rank 3: -funroll-loops flag-only speedup (not adopted), Tahap 5: Init Cost Optimization, FILE, find_variant(), main(), mode_init_cost() (+24 more)

### Community 7 - "Operand Position Search Experiment Results"
Cohesion: 0.07
Nodes (33): baseline_local PractRand 16GB Test Log, v01 PractRand 16GB Test Log (FAIL), v02 PractRand 16GB Test Log (FAIL), v03 PractRand 16GB Test Log (Pass), v04 PractRand 16GB Test Log (Pass), v06 PractRand 16GB Test Log (Pass), v07 PractRand 16GB Test Log (Pass), v08 PractRand 16GB Test Log (Pass) (+25 more)

### Community 8 - "winner_wired_addressable.c"
Cohesion: 0.08
Nodes (32): Scrambling Speed Benchmark (perf stat, per-call vs amortized, 30 runs), ra_prng2 NIST STS Test Result, ra_prng2 PractRand Test Result (up to 128 GB, no anomalies), ra_prng Build and Test Workflow, Contributing Guide, ra_prng Implementation Variant Roles, Repository Structure Specification, Brent Cycle Measure Checkpoint/Resume Architecture (+24 more)

### Community 9 - "Langkah 3: Interleaved 1TB for winner_wired_addressable (shared core-loop test)"
Cohesion: 0.13
Nodes (26): winner_wired_addressable Method A+B validated clean up to 128GB (xlarge tier), re-verified raw stdout this session, interleave_practrand_1tb_confirm.py (planned; reuse run_interleave_practrand with alt seeds), Shared core generation loop: ra_permutation_cycle / ra_reseed / ra_core (identical between winner_wired_v2.c and winner_wired_addressable.c; only ra_init_state[_addressable] differs), 1TB Anomaly Diagnostic Plan (Langkah 1-4), GATED_TIERS explicit-invocation gate (1tb/xlarge require explicit arg, unlike smoke/medium/full), HANDOVER_TAHAP5.md §6 convention: full Tahap 0-3 addressable-init repeat required if init formula changes, Harness bug: JSON 'passed' field mismatches lowercase PractRand 'suspicious'/'very suspicious' tags (case-sensitivity), not fixed in-place, (a) Cross-stream correlation specific to winner_wired_v2 init formula (+18 more)

### Community 10 - "ra_prng2 (Array-Based PRNG)"
Cohesion: 0.09
Nodes (23): BigCrush (TestU01), Bit Shifting, Rationale: Branchless Design for CPU Pipeline Efficiency, ChaCha20, Claude E. Shannon, Daniel Lemire, ent Entropy Tool, Rationale: Golden-Ratio Constants Reduce Collisions, Dead States, Short Cycles (+15 more)

### Community 11 - "State-Update Mechanism Research Results (Phase 1-3)"
Cohesion: 0.14
Nodes (21): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), Original Algorithm Avalanche Diffusion Comparison (Cycle 1 vs Cycle 3), Original Algorithm Cycle 1 Avalanche Fraction (0.497576), Original Algorithm Cycle 3 Avalanche Fraction (0.499740), Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3, main() (+13 more)

### Community 12 - "other/avalanche_heatmap_original.py"
Cohesion: 0.16
Nodes (19): ndarray, Bounded capture: run `binary --stream seed n`, return n uint32 values. Suitable…, stream_values(), analyze_group(), build_group(), main(), build_group(), main() (+11 more)

### Community 13 - "cross_correlation.py"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 14 - "2026-8-25_periodicity-heuristic-validation/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 15 - "2026-8-28_periodicity-heuristic-validation2/cycle_measure.c"
Cohesion: 0.14
Nodes (16): main(), splitmix32(), main(), main(), core_entry_t, FILE, find_core(), main() (+8 more)

### Community 16 - "tahap5_bench.c"
Cohesion: 0.16
Nodes (19): Four-Axis Validation: periodicity, PractRand, avalanche, speed, permute + xor_fold State-Update Mechanism (kept unchanged), FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle() (+11 more)

### Community 17 - "winner_wired_v2.c"
Cohesion: 0.12
Nodes (19): Benchmark Source Comparisons Documentation, ZepFold Shuffling CLI Tool, Toy PRNG Avalanche Sanity Verification Gate, Brent Cycle Detection Methodology, Periodicity Validation Experiment Handover, Periodicity Heuristic Validation Results Report, Periodicity Heuristic Scalability & Non-Monotonicity Verdict, Initial Sweep Periodicity Measurement Summary (+11 more)

### Community 18 - "ICCS 2026 Research Paper: Array-Native Randomness"
Cohesion: 0.15
Nodes (18): pruned_prng.c, build_o() (original 8-tap formula), ra_init_state_singleblock(), ra_permutation_cycle_singleblock(), rot32(), main(), diag_wideo_singleblock.c (planned), diag_wideo_warmup_singleblock.c (planned) (+10 more)

### Community 19 - "Karakterisasi ambang K minimum aman — `ra_core_singleblock`"
Cohesion: 0.18
Nodes (16): fmix32(), GUARD_L (0x38916df4), GUARD_M (0x6c26fc92), ra_init_state_orbit(), ra_init_state_addressable (winner_wired_addressable.c / scrambler_addressable.c, additive combine), BCFN PractRand Test, BRank (binary-rank) PractRand Test, Guard Constant Derivation (popcount==16, SHA-256 scan) (+8 more)

### Community 20 - "Wiring"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_reseed(), plot_comparison(), plot_single(), Path (+9 more)

### Community 21 - "avalanche_effect_analysis.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_capture(), original_reseed(), plot_comparison(), plot_single() (+9 more)

### Community 22 - "others/avalanche_heatmap_original.py"
Cohesion: 0.15
Nodes (10): group_size(), Params, Rescale the four fixed 32-bit plain-shift amounts to width w. s_w = round(s * w…, Top-w-bits truncation of a 32-bit golden-ratio-derived constant. Trap fix:…, G(n, rows): generalization of ra_hash's fixed 8/32 structural constants (256 =…, rescale_shifts(), truncate_const(), enumerate_state_space() (+2 more)

### Community 23 - "Params"
Cohesion: 0.24
Nodes (15): Candidate, final_cons(), _build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32() (+7 more)

### Community 24 - "Candidate"
Cohesion: 0.18
Nodes (16): Orbit Engine, L[256] array (dead-code discovery), Baseline decision: winner_wired_addressable.c (portable), not _v2.c, Tahap 2: C Implementation, FILE, main(), Orbit Addressing formula, ra_core() (+8 more)

### Community 25 - "scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)"
Cohesion: 0.28
Nodes (16): core_entry_t, find_core(), main(), mode_init_cost(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10() (+8 more)

### Community 26 - "RESULTS: production-candidate battery for `ra_core.c`"
Cohesion: 0.14
Nodes (17): 7. Follow-up: cycle `winner_wired_v2` + addressable init — SELESAI (2026-08-31), Double-swap hypothesis (disproven), Follow-up: varian cycle `winner_wired_v2` + addressable init (2026-08-31), scc_test_wired.py (statistical & distinctness validation, wired variant), scrambler_addressable.c (CLI), ra_init_state_addressable(L, M, key), scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI), scrambler_wired_addressable.py (Python reference port, wired variant) (+9 more)

### Community 27 - "RaPrng2"
Cohesion: 0.12
Nodes (17): clean, but a SEPARATE, more severe defect (K=1 pattern) now blocks, CRITICAL DEFECT: key=0 produces a permanent all-zero stream (both modes), further progress — gate STILL NOT PASSED, history, superseded by the rerun above for Steps 0 and 3 specifically, Original Step 0-3 run (2026-09-01, before the key=0 fix) — kept for, Provenance (Step 0), Recommendation (K=1 structural defect — NEW, open, needs a decision), Recommendation (key=0 — RESOLVED, kept for history) (+9 more)

### Community 28 - "Parallelization Research Handover"
Cohesion: 0.12
Nodes (16): Ambang, Batasan &amp; catatan, Hasil sweep, Karakterisasi ambang K minimum aman — `ra_core_singleblock`, Konteks, Metodologi, Overlay trade-off performa, Pertanyaan terbuka untuk user (+8 more)

### Community 29 - "toy_prng.py"
Cohesion: 0.18
Nodes (11): _find_lib_path(), _lib_handle(), _load_library(), Path, RaPrng2, ctypes binding for the ra_prng2 C engine. This is glue only - it marshals calls…, Thin wrapper around one RA_PRNG instance., CDLL (+3 more)

### Community 30 - "avalanche_heatmap_pruned_winner.py"
Cohesion: 0.12
Nodes (16): Embarrassingly Parallel Architecture (per-instance state, no locks), Operand-Position Search Research (2026-8-27), Operation Pruning Research (2026-8-26), Periodicity Heuristic Validation 2 (2026-8-28), Periodicity Heuristic Validation Research (2026-8-25), Read-Only Source Constraint (never modify ra_prng2/ra_prng3/winner_wired_v2.c in place), Seed/Cycle Collision Probability (Q2), SIMD Vectorization Feasibility (Q4) (+8 more)

### Community 31 - "avalanche_stats"
Cohesion: 0.26
Nodes (13): brent(), Brent's cycle-detection algorithm applied to toy_prng.next_state. Measures the…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, enumerate_state_space(), main(), Full state-space enumeration for n=2,w=4 (|S|=131072, default rows=2). Directly…, init_state(), next_state() (+5 more)

### Community 32 - "HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup"
Cohesion: 0.21
Nodes (15): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), pruned_winner_capture(), pruned_winner_reseed(), Path (+7 more)

### Community 33 - "next_state"
Cohesion: 0.17
Nodes (14): all_ops_baseline(), ops_to_bitmask(), Catalog of ablatable operations for the ra_prng2 operation-pruning search.…, The full, unpruned operation set -- must reproduce ra_prng2 exactly., Encode an op set as pruned_prng.c's bitmask (bit i set = FLAG_ORDER[i] active)., avalanche_gate_min_bit(), avalanche_stats(), _capture_first_cycle() (+6 more)

### Community 34 - "stream_driven_shuffle.c"
Cohesion: 0.20
Nodes (13): brent(), Brent's cycle-detection algorithm applied to pruned_wired_toy_prng.next_state.…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, next_state(), State, ra_hash_gen_sequential(), Toy-scale (n, w, rows)-parameterized port of winner_wired_v2.c's permutation…, HASH_ACCESS=sequential port of winner_wired_v2.c's ra_hash: each output word is… (+5 more)

### Community 35 - "benchmark_all_run.py"
Cohesion: 0.23
Nodes (15): Speed Benchmark Output (stream_driven_shuffle), FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash() (+7 more)

### Community 36 - "diag_keyzero_guard.c"
Cohesion: 0.27
Nodes (15): build_bench_all(), build_crosscheck_originals(), main(), ols_fit(), paperorig/wiredv2 originals: `bin --stream key n`. addrcont original…, Like run(), but keeps stdout as raw bytes -- for --stream binary output., run(), run_crosscheck() (+7 more)

### Community 37 - "quality_gate.py (avalanche_stats)"
Cohesion: 0.33
Nodes (15): FILE, fmix32(), is_all_zero(), main(), multikey_chain(), ra_core_orbit_stream(), ra_core_singleblock_block(), ra_hash() (+7 more)

### Community 38 - "collision_scan.py"
Cohesion: 0.19
Nodes (13): avalanche_gate_min_bit_binary(), capture_first_cycle(), main(), practrand_prefix_gate_binary(), Path, Avalanche + PractRand measurement for the phase-1 (L-only) state-update-…, `CYCLES` outer cycles' worth of `c` values -- see the CAPTURE_LEN comment above…, # NOTE: unlike pruned_prng.c's CLI (quality_gate.py's convention, where (+5 more)

### Community 39 - "2026-9-1_production-candidate-battery/RESULTS.md"
Cohesion: 0.22
Nodes (13): digest_collision_prob(), main(), main(), Tahap 2 (family-productionization) follow-up, Q2 re-pointed at…, scan_fullblock(), Q2: probability of cross-stream collision/overlap between different seeds'…, Birthday-bound estimate for pure hash-digest collision probability across…, scan_blocksweep() (+5 more)

### Community 40 - "common.py (TIERS_Q2)"
Cohesion: 0.14
Nodes (14): CLAUDE.md (project instructions), docs/ERRATA.md, Family Productionization Handover, 0. Latar belakang (supaya sesi eksekusi tidak perlu riset ulang), 1. Tahap 1 (WAJIB PERTAMA): Unifikasi `winner_wired_v2.c` + `winner_wired_addressable.c`, 2. Tahap 2 (SETELAH Tahap 1 selesai & dikonfirmasi user): Folder/battery validasi produksi, 3. Non-goals eksplisit (supaya sesi eksekusi tidak overscope), 4. Open decisions untuk sesi eksekusi (BELUM diputuskan user — jangan diasumsikan, tanya dulu) (+6 more)

### Community 41 - "Handover: PRNG family benchmark (ra_prng semua varian vs PRNG terkenal)"
Cohesion: 0.15
Nodes (11): common.py (TIERS_Q2), Path, Langkah 2 of HANDOVER_1TB_FOLLOWUP.md: re-run the interleaved 1TB Method B test…, run_live(), main(), Q3(a): wall-clock speedup/efficiency when running N independent…, Launch n concurrent --stream processes (seeds 0..n-1), each emitting n_per_proc…, run_n_concurrent() (+3 more)

### Community 42 - "io_only_isolation_test.c"
Cohesion: 0.26
Nodes (13): FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash(), ra_init_state_addressable() (+5 more)

### Community 43 - "`ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi"
Cohesion: 0.14
Nodes (13): 1. Avalanche (`avalanche_ra_core.py`, reuse `quality_gate.py`'s `avalanche_gate_min_bit()`), 2. Speed (`bench_ra_core.c`, pola `tahap6_bench.c`), 3. Cross-correlation, multi-key (`cross_correlation_ra_core_singleblock.py`, tier "full": K=512, n=255), 4. Collision-scan, multi-key (`collision_scan_ra_core_singleblock.py`, tier "full": M=50.000, satu blok penuh 255-kata per key), 5. Dieharder "Good" battery, stream multi-key (`multikey_stream` + `run_dieharder_battery.py` reused), 6. PractRand bertahap, stream multi-key (`multikey_stream` -> `RNG_test stdin32`), Belum dikerjakan / perlu keputusan user, File yang dihasilkan sesi ini (+5 more)

### Community 44 - "diag_prereseed.c"
Cohesion: 0.31
Nodes (12): core_fn_t, FILE, fmix32(), main(), pick_variant(), ra_core_singleblock_baseline(), ra_core_singleblock_prereseed(), ra_hash() (+4 more)

### Community 45 - "2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py"
Cohesion: 0.23
Nodes (12): decode_batch(), main(), pack_key(), Full state-space enumeration for n=4,w=4 (default rows=4 -> G=1), |S| = 4! *…, Cross-check vec_next_state against the scalar reference on random states drawn…, Pack (L, M, cons, it) arrays into a single uint64 key array, w bits per field,…, Mixed-radix decompose a flat index array into (lperm, m_0..m_{n-1}, it)…, Vectorized rotw: rotate low w bits of x left by r bits (mod w). x, r are numpy… (+4 more)

### Community 46 - "stream_values"
Cohesion: 0.22
Nodes (13): winner_wired_v2.c, HANDOVER_TAHAP6.md, Tahap 6 hard scope limit (rng<=255, hard-abort guard), ra_core_singleblock(), winner_wired_addressable.c, Candidate: addressable agresif K=255 (singleblock), prng-family-benchmark experiment (12 candidates), ra_prng-family (unified codebase concept) (+5 more)

### Community 47 - "c/ra_prng2.c"
Cohesion: 0.17
Nodes (12): Production-Candidate Battery Gate, Singleblock O-Width Fix Handover, Cara lanjut di sesi berikutnya, Context, HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup, Ide fix dari user (2 tahap, verbatim diparafrase), Kandidat A — perlebar `o` ke 8-tap (isolasi 1 variabel), Kandidat B (fallback, hanya jalan jika Kandidat A gagal/belum cukup) (+4 more)

### Community 48 - "ra_core.c"
Cohesion: 0.15
Nodes (13): Array Index Shuffling, Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), Bitwise Rotation (rotl32), R. Durstenfeld, Fisher–Yates Shuffle, Mutate Operator, Rationale: Non-Invertible Hash Reseed Trade-off (Reduces Period, Aids Diffusion) (+5 more)

### Community 49 - "Fisher–Yates Shuffle"
Cohesion: 0.17
Nodes (12): pruned_winner Avalanche Fraction Metric (Cycle 1 vs Cycle 90), Avalanche Effect Heatmap: pruned_winner Cycle 1 vs Cycle 90, Multi-Cycle Reseed Avalanche Diffusion Improvement (0.418 to 0.468), winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039), Avalanche Effect Heatmap (winner_wired_v2), winner_wired_v2 Wiring Parameters (a_xor=d, c_shift=a, rotc_amount=b, rotc_xor=a), Avalanche Effect (Bit-Flip Sensitivity), Joan Daemen and Vincent Rijmen (+4 more)

### Community 50 - "Avalanche Effect (Bit-Flip Sensitivity)"
Cohesion: 0.17
Nodes (11): 1. Op/wiring mapping ported to toy scale, 2. Tap-survivor generalization: top-2-by-count, and why, 3. Tap shift-width collapse (caught during design, not present in either parent), 4. `ra_hash_gen` divergence from ground truth (found while reading source, not assumed), 5. Self-check (passed) -- MUST stay passing, CRITICAL BUG found and fixed this session -- read before writing more scripts, Design decisions (read before touching `pruned_wired_toy_prng.py`), HANDOVER.md (periodicity-heuristic-validation2) (+3 more)

### Community 51 - "Design decisions (read before touching `pruned_wired_toy_prng.py`)"
Cohesion: 0.20
Nodes (7): _build_py, Distribution, BinaryDistribution, BuildPyWithCEngine, Imperative build customization pyproject.toml's static [tool.setuptools] table…, After the normal .py copy step, compiles the ra_prng2 C engine and drops…, Declares this distribution as containing compiled, platform-specific code…

### Community 52 - "BuildPyWithCEngine"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 53 - "2025-10-5_scramble-design/scc_test.py"
Cohesion: 0.38
Nodes (10): build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32(), FILE, has() (+2 more)

### Community 54 - "pruned_prng.c"
Cohesion: 0.29
Nodes (11): Avalanche Metrics (overall=0.487783, min_bit=0.472304), Warmup Cycle Avalanche Effect and Bit Diffusion, Cycle 1 Initial State Avalanche Fraction (0.487783), Cycle 3 Converged Avalanche Fraction (0.500751), Winner Wired Avalanche Heatmap Cycle 1 vs Cycle 3, Winner Wired Wiring Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a), Cycle 4 Steady State Avalanche (fraction 0.499713), Avalanche Effect Heatmap Comparison: Cycle 1 vs Cycle 4 (+3 more)

### Community 55 - "Cycle 1 Initial State Avalanche Fraction (0.487783)"
Cohesion: 0.18
Nodes (10): 1. Summary Comparison: Unpruned Original vs `winner_wired_v2`, 2. Sebaran Siklus Utama (*Distinct $\lambda$ Distribution*), 3. Enumerasi Penuh & Uji Chi-Square vs Poisson(1), 4. Analisis Temuan Kunci (*Key Findings*), 5. Catatan Metodologi & Caveats (Sesuai `HANDOVER.md`), 6. Kesimpulan (*Verdict*), A. Skala $n=2, w=4$ ($|S| = 131.072$) — [`enumerate_n2w4_result.json`](./enumerate_n2w4_result.json), B. Skala $n=4, w=4$ ($|S| = 402.653.184$) — [`enumerate_n4w4_result.json`](./enumerate_n4w4_result.json) (+2 more)

### Community 56 - "Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)"
Cohesion: 0.22
Nodes (9): ensure_ra_prng2_cli(), Path, Shared helpers for the parallelization-research harness. Never modifies…, Streaming capture: caller reads/closes proc.stdout incrementally. Use for large…, Compile the optional paper-exact comparator into this folder, if not already…, stream_popen(), main(), Checkpoint follow-up (2026-08-31), Q1 Method B re-pointed at `ra_prng2.c`… (+1 more)

### Community 57 - "ensure_ra_prng2_cli"
Cohesion: 0.36
Nodes (10): find_crossover(), main(), _parse_rows(), Path, Tahap 5: orchestrator for tahap5_bench / tahap5_bench_zmm512 /…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep() (+2 more)

### Community 58 - "tahap5_benchmark.py"
Cohesion: 0.33
Nodes (10): find_crossover(), main(), _parse_rows(), Tahap 6: orchestrator for tahap6_bench (C harness does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep(), run_throughput() (+2 more)

### Community 59 - "tahap6_benchmark.py"
Cohesion: 0.25
Nodes (9): chi_square_uniform(), entropy(), multi_key_distinctness_check(), Shuffle a fixed-size sample of input_tokens with n_keys different random keys;…, read_tokens(), runs_test(), serial_correlation(), multi_key_distinctness_check() (+1 more)

### Community 60 - "2026-8-30_addressable-shuffle/scc_test.py"
Cohesion: 0.18
Nodes (11): Candidate: MT19937, Candidate: addressable agresif K=1 (singleblock), PRNG Family Benchmark Results, Closing note, K=1 (frequent reinit) vs largest-K (steady state), sorted by K=1 cost ascending, MB/s sanity check, sorted descending, Finding: MT19937 seeding cost dominates frequent-reseed workloads, Seeding cost — OLS fit `call_ns(K) = a + b·K`, sorted by fixed cost `a` ascending (+3 more)

### Community 61 - "bench_ra_core.c"
Cohesion: 0.42
Nodes (10): core_entry_t, find_core(), main(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10(), philox4x32_round() (+2 more)

### Community 62 - "ra_core_singleblock"
Cohesion: 0.18
Nodes (11): 1. Diagnostic candidate — `diag_keyzero_guard.c`, 2. Edge-case check — PASS, 3. BCFN non-regression — K=255 multikey pattern: PASS, 32GB clean, 4. Non-regression — `ra_core validate` after applying to `ra_core.c`, Applied to `ra_core.c`, CRITICAL — separate, pre-existing defect found: K=1 singleblock chaining catastrophically FAILs PractRand, Guard constants, RESULTS: key=0 degenerate-state guard-XOR fix (+3 more)

### Community 63 - "RESULTS: key=0 degenerate-state guard-XOR fix"
Cohesion: 0.27
Nodes (8): collision_scan.py, main(), Non-regression check for `ra_init_orbit` (the multikey mix-init fix candidate,…, run_sequential(), main(), Tahap 3, Q2 (re-pointed): cross-key collision/overlap scan for…, run_random(), run_sequential()

### Community 64 - "tahap3_collision_scan.py"
Cohesion: 0.38
Nodes (9): _log(), main(), phase_a_hash_access_check(), phase_b_greedy_ablation(), Driver for the operation-pruning greedy ablation search. Two phases: Phase A:…, describe(), Human-readable summary of a candidate for logging in STATUS.md/RESULTS.md., quality_gate() (+1 more)

### Community 65 - "ablation_search.py"
Cohesion: 0.33
Nodes (9): cycle_measure Binary Exit-42 Crash at n=8,w=4,rows=4, log2_fact(), main(), predicted(), Drives the pruned/wired periodicity-heuristic sweep. Same lambda_pred formula…, result_csv_name(), run_c(), run_python() (+1 more)

### Community 66 - "2026-8-28_periodicity-heuristic-validation2/run_sweep.py"
Cohesion: 0.20
Nodes (10): 1TB Interleaved PractRand Anomaly (3/304 tests flagged, unconfirmed), common.py (shared harness config, TIERS_Q1B), cross_correlation.py, cross_correlation_ra_prng2.py, interleave_practrand.py, interleave_practrand_ra_prng2.py, Method A: Cross-correlation analysis (lag-0 Pearson, Bonferroni), Method B: Interleaved cross-stream PractRand test (+2 more)

### Community 67 - "interleave_practrand.py"
Cohesion: 0.20
Nodes (9): 1. Status & tujuan, 2. Formula & fakta struktural (sudah final, tidak perlu digali ulang), 3. Temuan disassembly (sudah dijalankan read-only, sesi persiapan 2026-08-30), 5. Rencana file & eksperimen, 6. Gerbang validasi statistik (aturan mekanis, bukan penilaian bebas), 7. Batasan read-only (berlaku juga untuk Tahap 5), 8. Kondisi berhenti / pelaporan eksplisit, Handover: Tahap 5 — Optimasi Biaya Init Addressable (+1 more)

### Community 68 - "tahap4_benchmark.py"
Cohesion: 0.20
Nodes (9): 1. Trigger dan motivasi, 2. Analisis (dikonfirmasi sebelum menulis kode apa pun), 3. File & perannya, 4. Batas scope keras (WAJIB dibaca sebelum memakai `ra_core_singleblock`), 5. Hasil kunci (angka lengkap di `RESULTS.md` "## Tahap 6"), 6. Status & langkah selanjutnya, 7. Setelah mengerjakan follow-up di atas, Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255` (+1 more)

### Community 69 - "scrambler_addressable.c"
Cohesion: 0.36
Nodes (9): find_crossover(), main(), _parse_rows(), Tahap 4: orchestrator for tahap4_bench (C harness, does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word., run_init_cost(), run_reinit_sweep(), run_throughput() (+1 more)

### Community 70 - "scrambler_wired_addressable.c"
Cohesion: 0.38
Nodes (9): main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 71 - "single_mread_isolation_test.c"
Cohesion: 0.38
Nodes (9): main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 72 - "diag_hashed_init.c"
Cohesion: 0.38
Nodes (9): Options, load_tokens_from_file(), main(), parse_args(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32() (+1 more)

### Community 73 - "diag_orbit_regression.c"
Cohesion: 0.36
Nodes (9): Candidate: ra_prng2 original (paperorig), FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed() (+1 more)

### Community 74 - "verify_fresh_build.py"
Cohesion: 0.40
Nodes (9): FILE, main(), ra_core_diag(), ra_hash(), ra_init_state_hashed_key(), ra_permutation_cycle(), ra_reseed(), rot32() (+1 more)

### Community 75 - "pcg_amortized.c"
Cohesion: 0.40
Nodes (9): FILE, fmix32(), main(), ra_core_orbit_stream(), ra_hash(), ra_init_orbit(), ra_permutation_cycle_orbit(), ra_reseed() (+1 more)

### Community 76 - "source/ra_prng2.c"
Cohesion: 0.24
Nodes (8): bench_ra_core, BUILD.sh, multikey_stream (K=255 chaining binary), quality_gate.py, check_multikey_cross(), check_validate(), main(), production-candidate-battery, Step 0: passive correctness gate on the FRESH…

### Community 77 - "source/ra_prng3.c"
Cohesion: 0.44
Nodes (8): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), pcg32_buffered_t

### Community 78 - "ra_prng3 PRNG Generator"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 79 - "scrambler.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 80 - "2026-8-25_periodicity-heuristic-validation/avalanche_check.py"
Cohesion: 0.25
Nodes (9): ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), RNGing perf stat (ra_prng3), PractRand Test Results (ra_prng3), ent Entropy Test Tool, NIST Statistical Test Suite (STS), PractRand Test Suite, ra_prng3 PRNG Generator (+1 more)

### Community 81 - "pruned_winner.c"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 82 - "pruned_winner_refactored.c"
Cohesion: 0.31
Nodes (8): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for toy_prng.py. Port of…, One outer iteration, no rehash - mirrors zepfold_capture() in…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), Rotate the low w bits of x left by r bits (mod w)., rotw()

### Community 83 - "baseline.c"
Cohesion: 0.22
Nodes (9): dieharder_pruned_winner_piped.txt, parallelization-research experiment (2026-8-29), scc_test.py, verify_parity.py, Min-of-Trials Benchmarking Pattern, Tahap 2: Folder/Battery Validasi Produksi, avalanche_gate_min_bit(), quality_gate.py (+1 more)

### Community 84 - "baseline_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 85 - "v01.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 86 - "v02.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 87 - "v03.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 88 - "v04.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 89 - "v06.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 90 - "v07.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 91 - "v08.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 92 - "v09.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 93 - "v10.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 94 - "v11.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 95 - "v12.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 96 - "v13.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 97 - "winner_wired.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 98 - "2026-8-28_periodicity-heuristic-validation2/enumerate_n4w4.py"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 99 - "original_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 100 - "original_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 101 - "original_m_overwrite.c"
Cohesion: 0.39
Nodes (8): decode_batch(), main(), pack_key(), Full state-space enumeration for pruned/wired PRNG at n=4,w=4 (default rows=4…, Cross-check vec_next_state against the scalar reference on random states drawn…, rotw_vec(), validate(), vec_next_state()

### Community 102 - "original_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 103 - "original_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 104 - "original_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 105 - "winner_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 106 - "winner_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 107 - "winner_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 108 - "winner_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 109 - "winner_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 110 - "winner_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 111 - "Handover: 1TB interleaved-PractRand anomaly follow-up"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 112 - "Handover: Addressable-Init Data Shuffling & Speed Investigation"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 113 - "Hasil validasi"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 114 - "scrambler_addressable.py"
Cohesion: 0.22
Nodes (8): 1. Status & apa yang sudah terjadi (2026-08-31), 2. Pertanyaan yang belum terjawab, 3. Rencana diagnostik (belum dikerjakan, urutan disarankan), 4. Setelah selesai, Handover: 1TB interleaved-PractRand anomaly follow-up, Langkah 1 — Single-stream `winner_wired_v2` ke 1TB (tanpa interleave), Langkah 2 — Re-run interleaved 1TB dengan seed set berbeda, Langkah 3 — Sintesis & keputusan

### Community 115 - "scrambler_wired_addressable.py"
Cohesion: 0.22
Nodes (8): Addressable-Init Research Results, Hasil (elapsed time, detik, `perf stat -r 30`), Interpretasi, Konteks tabel lama, Metodologi, Pertanyaan, Reproduksi, Speed Benchmark: `scrambler_addressable` vs Algoritma Lain

### Community 116 - "no_reseed_isolation_test.c"
Cohesion: 0.22
Nodes (8): 1. Ringkasan alur & tujuan, 2. File & perannya (peta folder `experiments/2026-8-30_addressable-shuffle/`), 3. Hasil kunci (sudah final, jangan diulang), 4. Reproduksi cepat, 5. Constraint yang diwarisi, 6. Follow-up: SELESAI (2026-08-30, sesi lanjutan), 7. Setelah mengerjakan follow-up di atas, Handover: Addressable-Init Data Shuffling & Speed Investigation

### Community 117 - "RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)"
Cohesion: 0.22
Nodes (9): Addressable-Init Data Shuffling Results, 1. Smoke test, 2. Parity check (`verify_parity.py`), 3. Statistik kualitas shuffle (`scc_test.py`, key=1, n=100.000), 4. Multi-key distinctness spot-check (`scc_test.py`), Apa ini, Build & run, Follow-up (belum dilakukan sekarang) (+1 more)

### Community 118 - "bench_init_avalanche.c"
Cohesion: 0.42
Nodes (8): load_token_ids_from_file(), main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Byte-for-byte port of winner_wired_addressable.c's function of the same name…, save_token_ids_to_file()

### Community 119 - "bench_init_keyterm_mul.c"
Cohesion: 0.42
Nodes (8): load_token_ids_from_file(), main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Byte-for-byte port of winner_wired_addressable.c's function of the same name…, save_token_ids_to_file()

### Community 120 - "bench_warmup_cost.c"
Cohesion: 0.42
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), ra_init_state_addressable(), ra_shuffle(), rot32(), save_tokens_to_file()

### Community 121 - "bench_warmup_cost2.c"
Cohesion: 0.22
Nodes (9): 1. Tujuan, 2. Roster (12 kandidat, path source persis), 3. Arsitektur harness (perluasan pola `tahap5_bench.c`/`tahap6_bench.c`), 4. Metodologi benchmark, 5. Gate validasi (wajib, berlapis per kategori sumber), 6. File yang TIDAK BOLEH diubah (read-only precedent), 7. Non-goals (eksplisit, di luar scope), 8. Penutup sesi (+1 more)

### Community 122 - "diag_init_keyterm_mul.c"
Cohesion: 0.22
Nodes (8): Catatan scope eksplisit, File yang dihasilkan sesi ini, Keputusan penamaan & scope (dikonfirmasi user, sesi ini), Non-goals (tidak dikerjakan sesi ini, sesuai HANDOVER §3), RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock), Status, Struktur `ra_core.c`, Validasi

### Community 123 - "diag_paperlike_cycle.c"
Cohesion: 0.50
Nodes (8): core_avalanche(), core_baseline(), fmix32(), main(), ra_cycle(), ra_init_state_avalanche(), ra_init_state_orbit(), rot32()

### Community 124 - "diag_warmup_depth.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_keyterm_mul(), fmix32(), main(), ra_cycle(), ra_init_state_keyterm_mul(), ra_init_state_orbit(), rot32()

### Community 125 - "diag_warmup_discard.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_warmup(), main(), ra_cycle(), ra_hash(), ra_init_state_orbit(), ra_reseed(), rot32()

### Community 126 - "Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)"
Cohesion: 0.50
Nodes (8): core_baseline(), core_warmup(), main(), ra_cycle(), ra_hash(), ra_init_state_orbit(), ra_reseed(), rot32()

### Community 127 - "ra_prng2_struct.c"
Cohesion: 0.47
Nodes (8): FILE, fmix32(), main(), ra_core_orbit(), ra_init_orbit(), ra_init_singleblock(), ra_permutation_cycle_orbit(), rot32()

### Community 128 - "ra_prng2_thread.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_diag(), ra_hash(), ra_init_state_addressable(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 129 - "Dieharder Test Suite"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_warmup(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed(), rot32()

### Community 130 - "ra_prng2 API Reference"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_warmup(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed(), rot32()

### Community 131 - "HANDOVER_TAHAP6.md"
Cohesion: 0.22
Nodes (9): Candidates tried so far, Context: where this came from, Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey), Non-goals, Non-regression verification results (2026-09-01) -- verification bar item 2 DONE, Process note (per explicit user feedback this session), Root cause (conclusively diagnosed this session — do not re-litigate), The task: find something cheaper than a full/partial warm-up cycle (+1 more)

### Community 132 - "Handover: Tahap 5 — Optimasi Biaya Init Addressable"
Cohesion: 0.44
Nodes (8): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), ra_step(), rot32()

### Community 133 - "tahap3_cross_correlation.py"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 134 - "Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`"
Cohesion: 0.29
Nodes (8): Dieharder Test Results (ra_prng2 original), RNGing perf stat (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), Dieharder Test Suite, ra_prng2 PRNG Generator, TestU01 BigCrush Test Suite, Robert G. Brown

### Community 135 - "diag_init_avalanche.c"
Cohesion: 0.36
Nodes (8): CPython Binding Documentation, Orphaned CPython Shared Object Rationale, RaPrng2 ctypes Wrapper, ra_prng2 API Reference, ra_core PRNG Core Loop, ra_hash State Reduction Function, RA_PRNG C Struct API, rot32 Left Bitwise Rotation

### Community 136 - "diag_init_keyterm_avalanche.c"
Cohesion: 0.32
Nodes (7): parallelization-research infra (referenced), Devil's-Advocate Session (challenged parallelization research value), Tahap 3: Statistical Validation, build_group_addr(), main(), Tahap 3, Q1 Method A (re-pointed): Pearson cross-correlation between pairs of…, run()

### Community 137 - "diag_init_keyterm_xor.c"
Cohesion: 0.29
Nodes (5): main(), Q1 Method B: interleave K independent `--stream <seed> <n>` streams word-by-…, run_interleave_practrand(), Tahap 3, Q1 Method B (re-pointed): interleave K independent…, PractRand (RNG_stdin32)

### Community 138 - "RESULTS: pre-reseed experiment untuk defect K=1 `ra_core_singleblock`"
Cohesion: 0.36
Nodes (8): FILE, ra_core_baseline(), ra_init_state_addressable_baseline(), ra_permutation_cycle(), ra_permutation_cycle_full(), rot32(), run_validate_singleblock(), Candidate: addressable continuing (addrcont)

### Community 139 - "ent (Entropy Testing Tool)"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_avalanche_init(), ra_init_state_avalanche(), ra_permutation_cycle_orbit(), rot32()

### Community 140 - "pcg32.c"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_keyterm_add(), ra_init_state_keyterm_add(), ra_permutation_cycle_orbit(), rot32()

### Community 141 - "src/xoshiro256.c"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_keyterm_xor(), ra_init_state_keyterm_xor(), ra_permutation_cycle_orbit(), rot32()

### Community 142 - "xoshiro256_amortized.c"
Cohesion: 0.25
Nodes (8): Hamas A. Rahman, ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex, ra_prng2 and Beyond (Paper CA-162), Scopus, ra_prng GitHub Repository, hamzy hams (GitHub maintainer)

### Community 143 - "scrambler.py"
Cohesion: 0.29
Nodes (7): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result

### Community 144 - "2026-8-25_periodicity-heuristic-validation/run_sweep.py"
Cohesion: 0.38
Nodes (5): chacha20_block(), main(), Candidate: chacha20, chacha20.c RFC 8439 nonce byte-order bug, RFC 8439 (ChaCha20 spec)

### Community 145 - "src/chacha20.c"
Cohesion: 0.43
Nodes (6): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), Candidate: pcg32, Official PCG reference (O'Neill, pcg-random.org)

### Community 146 - "2026-8-28_periodicity-heuristic-validation2/avalanche_check.py"
Cohesion: 0.43
Nodes (6): main(), rotl(), xoshiro256_init(), xoshiro256_next(), Candidate: xoshiro256**, Official xoshiro256** reference (Blackman & Vigna)

### Community 147 - "PRNG Family Benchmark Results"
Cohesion: 0.52
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next()

### Community 148 - "simd_prototype.c"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 149 - "RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`"
Cohesion: 0.48
Nodes (6): log2_fact(), main(), predicted(), Drives the full periodicity-heuristic-validation sweep. For each measurable (n,…, run_c(), run_python()

### Community 150 - "PRNG Periodicity Analysis"
Cohesion: 0.38
Nodes (6): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for pruned_wired_toy_prng.py.…, One outer iteration, no rehash -- mirrors next_state's inner loop exactly (tap-…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check()

### Community 151 - "source/chacha20.c"
Cohesion: 0.48
Nodes (6): build_group(), main(), ndarray, Tahap 2 (family-productionization) follow-up, Q1 Method A re-pointed at…, run(), stream_values_singleblock()

### Community 152 - "chacha20_amortized.c"
Cohesion: 0.48
Nodes (6): main(), rot32_avx2(), rot32_scalar(), run_scalar(), run_simd(), __m256i

### Community 153 - "pcg.c"
Cohesion: 0.29
Nodes (6): Context, Files produced this session, Interpretation, Method, Results, RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`

### Community 154 - "source/xoshiro256.c"
Cohesion: 0.29
Nodes (7): B. Harris, Donald E. Knuth, Makoto Matsumoto and Takuji Nishimura, Mersenne Twister (MT19937), PRNG Periodicity Analysis, Pierre L'Ecuyer, Random Mapping Theorem

### Community 155 - "Shuffling perf stat (ra_prng2 original)"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 156 - "run_dieharder_battery.py"
Cohesion: 0.47
Nodes (4): chacha20_block(), main(), rand_bounded(), Speed Benchmark Output (chacha20 amortized)

### Community 157 - "run_dieharder_battery_multikey.py"
Cohesion: 0.67
Nodes (5): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded()

### Community 158 - "verify_unification.py"
Cohesion: 0.60
Nodes (5): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 159 - "test_ra_hash.py"
Cohesion: 0.67
Nodes (6): Shuffling perf stat (ra_prng2 original), Scrambling perf stat (ra_prng3), Linux perf stat Tool, Scrambler Tool, tokens.txt Input Corpus, scrambled.txt (scramble-design experiment output)

### Community 160 - "Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2)"
Cohesion: 0.33
Nodes (6): 4. Kandidat optimasi, berperingkat, Rank 1 — `-mprefer-vector-width=512` (flag saja, tanpa ubah kode), Rank 2 — Substitusi `vprolvd`/`vprorvd` intrinsic khusus untuk `rot32`, Rank 3 — Unroll loop, Rank 4 (deprioritized, referensi saja) — Rewrite SIMD manual penuh, Ruled out — Mengurangi jumlah iterasi (<256)

### Community 161 - "avalanche_gate"
Cohesion: 0.60
Nodes (5): main(), Path, Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"- reliability…, run_one(), run_test()

### Community 162 - "auto_stop_power38.py"
Cohesion: 0.60
Nodes (5): main(), Path, Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"- reliability…, run_one(), run_test()

### Community 163 - "cross_correlation_ra_prng2.py"
Cohesion: 0.60
Nodes (5): compile_new(), compile_old(), main(), run_stream_new(), run_stream_old()

### Community 164 - "JAX"
Cohesion: 0.33
Nodes (3): Pure-Python unit tests for ra_prng2.ra_hash / ra_core. These do not require the…, Regression test for the historical bug: ra_hash must be a pure function writing…, test_ra_hash_does_not_mutate_input()

### Community 165 - "Orbit Addressing"
Cohesion: 0.40
Nodes (5): Reseed-Driven Avalanche Strengthening Effect, Initial State Avalanche Fraction (Cycle 1: 0.418459), Post-Reseed Avalanche Fraction (Cycle 2: 0.468409), Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2), Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)

### Community 166 - "avalanche_orbit_singleblock_k255.py"
Cohesion: 0.60
Nodes (4): avalanche_gate(), main(), Post-convergence "shift repair" search. User's request after seeing the winning…, score()

### Community 167 - "scrambler_ra_core_singleblock.c"
Cohesion: 0.60
Nodes (4): find_pid(), get_status(), main(), Monitors .cycle_measure_ckpt_n8_w4_rows4_seed0.bin and stops cycle_measure when…

### Community 168 - "Avalanche Effect Analysis (ALL_OPS)"
Cohesion: 0.40
Nodes (5): JAX, Philox (counter-mode PRNG), Tahap 4: speed break-even benchmark vs Philox, Threefry2x32, xoshiro256**

### Community 169 - "heatmap.py"
Cohesion: 0.40
Nodes (5): Orbit (deterministic trajectory selected by key), Orbit Addressing, Point Addressing (Philox f(counter,key)), CSAI 2025 RA-PRNG Paper, ICCS 2026 RA-PRNG Paper

### Community 170 - "Hamas A. Rahman"
Cohesion: 0.60
Nodes (4): capture_cycle(), main(), production-candidate-battery, Step 1: avalanche gate for `orbit` and…, run_gate()

### Community 171 - "ra_prng2.py"
Cohesion: 0.70
Nodes (4): fisher_yates_k1(), main(), pull_n(), pull_one()

### Community 172 - "c/ra_prng3.c"
Cohesion: 0.50
Nodes (5): Avalanche Effect Analysis (ALL_OPS), Bit Difference Distribution Across 32 Flipped Bits, Cycle 1 Avalanche Fraction (0.497576), Cycle 2 Avalanche Fraction (0.502141), Avalanche Effect Heatmap (Cycle 1 vs Cycle 2)

### Community 173 - "Cycle 3 Avalanche Fraction (0.470588)"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 174 - "Paper heuristic λ ≈ 0.7824·√|S|"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 175 - "Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 176 - "Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche"
Cohesion: 0.83
Nodes (4): Cycle 1 Avalanche Fraction (0.418459), Cycle 3 Avalanche Fraction (0.470588), Avalanche Heatmap: Cycle 1 vs Cycle 3 (pruned_winner), Reseed Avalanche Diffusion in Pruned Winner

### Community 177 - "robust_reinit.py"
Cohesion: 0.50
Nodes (4): Paper heuristic λ ≈ 0.7824·√|S|, Research Question: does pruned/rewired algorithm shift the heuristic's λ-vs-λ_pred fit, Central Research Question (RESULTS.md), Verdict 2: heuristic behaves as conservative lower bound at large-scale configs only

### Community 178 - "avalanche_ra_core_singleblock_k1.py"
Cohesion: 0.50
Nodes (4): Tap shift-width collapse (e>=w yields identically-zero term), Tap-survivor generalization (top-2-by-count rule, G=8→(6,7)), Finding: non-vacuous tap pruning (G=4) gives closest fit to heuristic, Caveat: tap pruning is a no-op at G<=2 configs

### Community 179 - "Avalanche Effect Heatmap (Original Algorithm)"
Cohesion: 0.67
Nodes (4): Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche, Kandidat 3 (negative control: L rotated only, multiset unchanged), Kandidat 5 (user's rot32-based L/M formula, winning candidate), L_MIX_CONST (frozen to 0x9e3779b7)

### Community 181 - "dev_urandom.c"
Cohesion: 0.67
Nodes (3): capture_one(), main(), production-candidate-battery, Step 1: avalanche gate for `singleblock` K=1 mode…

### Community 182 - "Graphify-First Research Workflow Policy"
Cohesion: 0.67
Nodes (4): Avalanche Effect, Avalanche Effect Heatmap (Original Algorithm), Hamming Distance Analysis, Original PRNG Algorithm

### Community 183 - "Cycle 1 vs Cycle 5 Avalanche Fraction Convergence"
Cohesion: 0.67
Nodes (3): Graphify Query & Navigation Protocol, Graphify Knowledge Graph Rule, Graphify Pipeline Workflow

### Community 185 - "Greedy Operation-Pruning Search Paradigm"
Cohesion: 0.67
Nodes (3): graphify-out/graph.json Knowledge Graph Artifact, graphify Skill, Graphify-First Research Workflow Policy

### Community 186 - "collision_scan.py"
Cohesion: 1.00
Nodes (3): Cycle 1 vs Cycle 5 Avalanche Fraction Convergence, Avalanche Heatmap: Pruned Winner Cycle 1 vs Cycle 5, Pruned Winner Architecture (TAP6, TAP7, ROT_C, SHR13)

### Community 187 - "Q3: Multi-process throughput scalability"
Cohesion: 1.00
Nodes (3): Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13), pruned_winner Cycle 1 Avalanche Sensitivity and Dead Zones, pruned_winner 4-Op Candidate Configuration (TAP6, TAP7, ROT_C, SHR13)

### Community 188 - "pcg.c (PCG32 comparison binary)"
Cohesion: 1.00
Nodes (3): Greedy Operation-Pruning Search Paradigm, Operation-Pruning Research Handover Spec, Limits of Zero-Drift Micro-Optimizations on ra_core

### Community 189 - "xoshiro256.c (fread+tokenizer comparison binary)"
Cohesion: 0.67
Nodes (3): collision_scan.py, Q2: Cross-stream collision probability, ProcessPoolExecutor deadlock -> ThreadPoolExecutor+lock fix

### Community 190 - "run_speed_benchmark.sh"
Cohesion: 0.67
Nodes (3): perf_scaling.sh, Q3: Multi-process throughput scalability, wall_time_scaling.py

### Community 191 - "verify_parity.py"
Cohesion: 0.67
Nodes (3): out_pcg.txt (output artifact of pcg speed-benchmark run), out_pcg_amortized.txt (output artifact of pcg amortized speed-benchmark run), pcg.c (PCG32 comparison binary)

### Community 192 - "verify_parity_wired.py"
Cohesion: 0.67
Nodes (3): out_xoshiro256.txt (output artifact of xoshiro256 speed-benchmark run), out_xoshiro256_amortized.txt (output artifact of xoshiro256 amortized speed-benchmark run), xoshiro256.c (fread+tokenizer comparison binary)

## Ambiguous Edges - Review These
- `Hamas A. Rahman` → `hamzy hams (GitHub maintainer)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to

## Knowledge Gaps
- **428 isolated node(s):** `Threefry2x32`, `xoshiro256**`, `0. Latar belakang (supaya sesi eksekusi tidak perlu riset ulang)`, `1. Tahap 1 (WAJIB PERTAMA): Unifikasi `winner_wired_v2.c` + `winner_wired_addressable.c``, `2. Tahap 2 (SETELAH Tahap 1 selesai & dikonfirmasi user): Folder/battery validasi produksi` (+423 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **95 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Hamas A. Rahman` and `hamzy hams (GitHub maintainer)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `Avalanche Effect Heatmap (winner_wired_v2)` connect `Fisher–Yates Shuffle` to `benchmark_all.c`, `tahap5_bench.c`?**
  _High betweenness centrality (0.057) - this node is a cross-community bridge._
- **Why does `quality_gate.py (avalanche_stats)` connect `collision_scan.py` to `tahap3_collision_scan.py`, `benchmark_all.c`, `HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup`, `tahap0_prototype.py`, `avalanche_orbit_singleblock_k255.py`, `Hamas A. Rahman`, `dev_urandom.c`?**
  _High betweenness centrality (0.049) - this node is a cross-community bridge._
- **Why does `common.py (TIERS_Q2)` connect `Handover: PRNG family benchmark (ra_prng semua varian vs PRNG terkenal)` to `benchmark_all.c`, `tahap0_prototype.py`, `2026-9-1_production-candidate-battery/RESULTS.md`, `diag_init_keyterm_avalanche.c`, `diag_init_keyterm_xor.c`, `other/avalanche_heatmap_original.py`, `source/chacha20.c`, `Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)`, `RESULTS: key=0 degenerate-state guard-XOR fix`?**
  _High betweenness centrality (0.045) - this node is a cross-community bridge._
- **Are the 7 inferred relationships involving `Params` (e.g. with `capture_inner_outputs()` and `run_avalanche_check()`) actually correct?**
  _`Params` has 7 INFERRED edges - model-reasoned connections that need verification._
- **Are the 8 inferred relationships involving `Candidate` (e.g. with `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` and `avalanche_gate()`) actually correct?**
  _`Candidate` has 8 INFERRED edges - model-reasoned connections that need verification._
- **What connects `Threefry2x32`, `xoshiro256**`, `0. Latar belakang (supaya sesi eksekusi tidak perlu riset ulang)` to the rest of the system?**
  _428 weakly-connected nodes found - possible documentation gaps or missing edges._