# Graph Report - ra_prng  (2026-09-03)

## Corpus Check
- 3 files · ~13,145,075 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 3834 nodes · 6308 edges · 435 communities (329 shown, 106 thin omitted)
- Extraction: 97% EXTRACTED · 3% INFERRED · 0% AMBIGUOUS · INFERRED: 158 edges (avg confidence: 0.86)
- Token cost: 0 input · 95,503 output

## Community Hubs (Navigation)
- Pareto Selection Results (Combo Winners)
- winners_bench.c
- Completed Tasks
- tahap4_bench.c
- tahap5_bench.c
- tahap0_prototype.py
- Singleblock Cycle-Operation Combo Search Handover
- Operand Position Search Experiment Results
- ra_prng2 (Array-Based PRNG)
- ra_core.c
- Langkah 3: Interleaved 1TB for winner_wired_addressable (shared core-loop test)
- ra_prng2 (32-bit generator, CSAI2025)
- winner_wired_v2.c
- benchmark_all.c
- diag_init_candidates.c
- stream
- State-Update Mechanism Research Results (Phase 1-3)
- pruned_winner 4-Operation Fast Variant
- other/avalanche_heatmap_original.py
- 2026-8-25_periodicity-heuristic-validation/cycle_measure.c
- 2026-8-28_periodicity-heuristic-validation2/cycle_measure.c
- ra_core_singleblock Cycle-Operation Combo Search — RESULTS
- recipes.py
- avalanche_stats
- ensure_ra_prng2_cli
- stream_values
- ICCS 2026 Research Paper: Array-Native Randomness
- avalanche_heatmap_winner_v2.py
- avalanche_effect_analysis.py
- others/avalanche_heatmap_original.py
- common.py (TIERS_Q2)
- Params
- Candidate
- winner_wired_addressable.c
- tahap6_bench.c
- scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)
- bench_ra_core.c
- RESULTS: production-candidate battery for `ra_core.c`
- ra_core_singleblock
- RaPrng2
- Parallelization Research Handover
- toy_prng.py
- avalanche_heatmap_pruned_winner.py
- next_state
- stream_driven_shuffle.c
- Karakterisasi ambang K minimum aman — `ra_core_singleblock`
- benchmark_all_run.py
- diag_keyzero_guard.c
- Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect
- RESULTS: fix defect K-kecil `ra_core_singleblock` — perlebar `o` (Kandidat A) + internal warm-up (Kandidat B)
- ra_prng3 PRNG Generator
- collision_scan.py
- PRNG Family Benchmark Results
- Family Productionization Handover
- diag_wideo_warmup_singleblock.c
- avalanche_heatmap_winner.py
- Handover: Tahap 5 — Optimasi Biaya Init Addressable
- io_only_isolation_test.c
- `ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi
- Singleblock cycle-op combo search experiment (2026-09-02)
- 2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py
- Production Readiness Handover — `ra_core_orbit` / `ra_core_singleblock`
- Wiring
- ra_core_singleblock
- diag_wideo_singleblock.c
- HANDOVER: Init-loop speed optimization untuk `ra_core_orbit` / `ra_core_singleblock`
- diag_prereseed.c
- Fisher–Yates Shuffle
- winner_wired_addressable_v2.c
- Design decisions (read before touching `pruned_wired_toy_prng.py`)
- pareto_select.py
- BuildPyWithCEngine
- 2025-10-5_scramble-design/scc_test.py
- Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)
- pruned_prng.c
- Cycle 1 Initial State Avalanche Fraction (0.487783)
- Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)
- tahap5_benchmark.py
- tahap6_benchmark.py
- 2026-8-30_addressable-shuffle/scc_test.py
- RESULTS: key=0 degenerate-state guard-XOR fix
- RESULTS: Priority 1 — Init-loop speed optimization
- ra_prng (Array-Based PRNG Architecture)
- Handover: eliminasi 11 pemenang combo-search via Pareto (speed x avalanche), lalu posisikan vs keluarga PRNG lain
- tahap3_collision_scan.py
- Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`
- quality_gate.py
- 2026-8-28_periodicity-heuristic-validation2/run_sweep.py
- interleave_practrand.py
- tahap4_benchmark.py
- scrambler_addressable.c
- scrambler_wired_addressable.c
- single_mread_isolation_test.c
- c/ra_prng2.c
- diag_hashed_init.c
- diag_orbit_regression.c
- src/chacha20.c
- pcg_amortized.c
- source/ra_prng2.c
- source/ra_prng3.c
- scrambler.c
- 2026-8-25_periodicity-heuristic-validation/avalanche_check.py
- Tahap 2: Folder/Battery Validasi Produksi
- pruned_winner.c
- pruned_winner_refactored.c
- avalanche_gate
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
- Handover: 1TB interleaved-PractRand anomaly follow-up
- benchmarks/comparisons (throughput/entropy comparisons)
- Handover: Addressable-Init Data Shuffling & Speed Investigation
- Hasil validasi
- scrambler_addressable.py
- scrambler_wired_addressable.py
- no_reseed_isolation_test.c
- Handover: PRNG family benchmark (ra_prng semua varian vs PRNG terkenal)
- RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)
- bench_init_avalanche.c
- bench_init_keyterm_mul.c
- bench_warmup_cost.c
- bench_warmup_cost2.c
- diag_init_keyterm_mul.c
- diag_paperlike_cycle.c
- diag_warmup_depth.c
- diag_warmup_discard.c
- 2026-9-1_production-candidate-battery/RESULTS.md
- w2_f0_i0.c
- w2_f10_i0.c
- w2_f11_i0.c
- w2_f12_i0.c
- w2_f13_i0.c
- w2_f14_i0.c
- w2_f15_i0.c
- w2_f16_i0.c
- w2_f17_i0.c
- w2_f18_i0.c
- w2_f19_i0.c
- w2_f1_i0.c
- w2_f20_i0.c
- w2_f21_i0.c
- w2_f22_i0.c
- w2_f23_i0.c
- w2_f24_i0.c
- w2_f25_i0.c
- w2_f26_i0.c
- w2_f27_i0.c
- w2_f28_i0.c
- w2_f29_i0.c
- w2_f2_i0.c
- w2_f30_i0.c
- w2_f31_i0.c
- w2_f32_i0.c
- w2_f33_i0.c
- w2_f34_i0.c
- w2_f35_i0.c
- w2_f3_i0.c
- w2_f4_i0.c
- w2_f5_i0.c
- w2_f6_i0.c
- w2_f7_i0.c
- w2_f8_i0.c
- w2_f9_i0.c
- w4_f0_i0.c
- w4_f10_i0.c
- w4_f11_i0.c
- w4_f12_i0.c
- w4_f13_i0.c
- w4_f14_i0.c
- w4_f15_i0.c
- w4_f16_i0.c
- w4_f17_i0.c
- w4_f18_i0.c
- w4_f19_i0.c
- w4_f1_i0.c
- w4_f20_i0.c
- w4_f21_i0.c
- w4_f22_i0.c
- w4_f23_i0.c
- w4_f24_i0.c
- w4_f25_i0.c
- w4_f26_i0.c
- w4_f27_i0.c
- w4_f28_i0.c
- w4_f29_i0.c
- w4_f2_i0.c
- w4_f30_i0.c
- w4_f31_i0.c
- w4_f32_i0.c
- w4_f33_i0.c
- w4_f34_i0.c
- w4_f35_i0.c
- w4_f3_i0.c
- w4_f4_i0.c
- w4_f5_i0.c
- w4_f6_i0.c
- w4_f7_i0.c
- w4_f8_i0.c
- w4_f9_i0.c
- w8_f0_i0.c
- 2026-9-3_init-loop-optimization/HANDOVER.md
- w8_f11_i0.c
- w8_f12_i0.c
- w8_f13_i0.c
- w8_f14_i0.c
- w8_f15_i0.c
- w8_f16_i0.c
- w8_f17_i0.c
- w8_f18_i0.c
- w8_f19_i0.c
- w8_f1_i0.c
- w8_f20_i0.c
- w8_f21_i0.c
- Axis B - Avalanche Multi-seed + Normality
- w8_f23_i0.c
- Orbit Continuous-Stream Measurement (376.2 MB/s)
- cross_correlation_ra_core_singleblock.py
- ra_core_singleblock (algorithm)
- Finding: MT19937 seeding cost dominates frequent-reseed workloads
- 4. Kandidat optimasi, berperingkat
- w8_f2_i0.c
- w8_f30_i0.c
- w8_f31_i0.c
- w8_f32_i0.c
- avalanche_ra_core.py
- w8_f34_i0.c
- w8_f35_i0.c
- w8_f3_i0.c
- w8_f4_i0.c
- w8_f5_i0.c
- w8_f6_i0.c
- w8_f7_i0.c
- ra_prng Implementation Variant Roles
- HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup
- ra_prng2_struct.c
- ra_prng2_thread.c
- Shuffling perf stat (ra_prng2 original)
- ra_prng2 API Reference
- gen_variants.py
- gen_variants_m.py
- tahap3_cross_correlation.py
- Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche
- other_winners_gen.py
- diag_init_avalanche.c
- diag_init_keyterm_avalanche.c
- diag_init_keyterm_xor.c
- HANDOVER_TAHAP6.md
- ent (Entropy Testing Tool)
- Avalanche Check Validation
- pcg32.c
- src/xoshiro256.c
- xoshiro256_amortized.c
- Orbit KAT Verification
- scrambler.py
- 2026-8-25_periodicity-heuristic-validation/run_sweep.py
- 2026-8-28_periodicity-heuristic-validation2/avalanche_check.py
- simd_prototype.c
- Speed Benchmark: `scrambler_addressable` vs Algoritma Lain
- RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`
- quality_gate.py (avalanche_stats)
- verify_orbit_kat.py
- PRNG Periodicity Analysis
- promotion_search.py
- source/chacha20.c
- chacha20_amortized.c
- pcg.c
- source/xoshiro256.c
- init_kandidat1
- Orbit Addressing Papers
- run_dieharder_battery.py
- run_dieharder_battery_multikey.py
- verify_unification.py
- test_ra_hash.py
- Cycle Combo Avalanche Heatmap
- auto_stop_power38.py
- full_scale_sanity_check.py
- Cycle1 vs Cycle2 Avalanche
- Orbit Addressing
- ra_prng2 Python Reference
- ra_prng3 C Variant
- scrambler_ra_core_singleblock.c
- Periodicity Heuristic Question
- Avalanche heatmap w8_f8_i0 K=255 (full cycle)
- Avalanche Effect Analysis (ALL_OPS)
- heatmap.py
- ra_prng2.py
- c/ra_prng3.c
- Cycle 3 Avalanche Fraction (0.470588)
- Paper heuristic λ ≈ 0.7824·√|S|
- Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))
- CLAUDE.md Graphify Workflow
- robust_reinit.py
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
- Operand Position Avalanche Heatmap
- Avalanche heatmap w8_f22_i0 K=255 (full cycle)
- sub-projects/ (downstream applications)
- State Update Perf Benchmark
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
- Dieharder Orbit Regression Raw Output
- bench_ra_core Throughput/Reinit-Sweep Results Log
- BUILD.sh
- multikey_stream_singleblock_k1.c
- Avalanche heatmap K=255 full cycle: w8_f26_i0
- Cycle-op combo candidate w8_f29_i0
- All 11 combo-search winners show clean, uniform avalanche behavior at K=255 (full singleblock cycle)
- run_confirm_16gb.sh
- run_extended_16gb.sh
- run_warmup_triage.sh
- CI Build and Test Job
- conftest.py
- Addressable-Init Research (Tahap 0-5)
- /dev/urandom RNG Speed Benchmark Result
- xoshiro256** RNG Speed Benchmark Result
- benchmarks/comparisons/source/README.md (naming convention)
- cpython/__init__.py
- bindings/__init__.py
- rows=2 Configuration Log
- Design decision: op/wiring mapping ported to toy scale
- rerun_n8_w4_rows4_stderr.txt
- rerun_n8_w4_rows4_stdout.txt
- Finding: very short transient tail (μ<=1) across all seeds
- Sweep Output (empty file)
- Cross-Correlation Singleblock Postfix Run Log
- Q1: Cross-stream independence
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
- Avalanche heatmap: candidate w8_f28_i0 at K=255 (full cycle)
- Orbit Addressing (f(key) state-init mechanism)
- ra-prng
- benchmarks/results (precomputed statistical output)
- ra_prng2 Package Changelog
- ra_prng3 Package Changelog

## God Nodes (most connected - your core abstractions)
1. `ra_core_singleblock Cycle-Operation Combo Search — RESULTS` - 48 edges
2. `Params` - 33 edges
3. `ra_prng2 (Array-Based PRNG)` - 29 edges
4. `ra_prng2 (32-bit generator, CSAI2025)` - 26 edges
5. `ra_core_singleblock()` - 23 edges
6. `State-Update Mechanism Research Results (Phase 1-3)` - 23 edges
7. `Singleblock Cycle-Operation Combo Search Handover` - 23 edges
8. `Production Readiness Handover` - 22 edges
9. `Candidate` - 20 edges
10. `avalanche_stats()` - 20 edges

## Surprising Connections (you probably didn't know these)
- `Candidate` --semantically_similar_to--> `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/pruned_prng.py → experiments/2026-8-26_operation-pruning-research/avalanche_heatmap_pruned_winner_cycle1_vs_cycle2.png
- `ra_prng2 (Array-Based PRNG)` --cites--> `PractRand`  [EXTRACTED]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf → README.md
- `ra_prng2 (32-bit generator, CSAI2025)` --semantically_similar_to--> `pruned_winner 4-Operation Fast Variant`  [INFERRED] [semantically similar]
  README.md → experiments/2026-8-26_operation-pruning-research/RESULTS.md
- `singleblock vs orbit speed crossover (K≈20-21, singleblock faster above this)` --semantically_similar_to--> `Addressable / Orbit-style Initialization`  [INFERRED] [semantically similar]
  experiments/2026-9-2_singleblock-k-threshold-characterization/RESULTS.md → README.md
- `winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039)` --conceptually_related_to--> `Avalanche Effect (Bit-Flip Sensitivity)`  [INFERRED]
  experiments/2026-8-27_operand-position-search/avalanche_heatmap_winner_v2.png → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **diag_init_candidates.c benchmark core suite** — experiments_2026_9_3_init_loop_optimization_diag_init_candidates, experiments_2026_9_3_init_loop_optimization_diag_init_candidates_orbit_baseline, experiments_2026_9_3_init_loop_optimization_diag_init_candidates_orbit_accum, experiments_2026_9_3_init_loop_optimization_diag_init_candidates_singleblock_baseline, experiments_2026_9_3_init_loop_optimization_diag_init_candidates_singleblock_accum, experiments_2026_9_3_init_loop_optimization_diag_init_candidates_singleblock_oldera, experiments_2026_9_3_init_loop_optimization_diag_init_candidates_philox, experiments_2026_9_3_init_loop_optimization_diag_init_candidates_xoshiro256 [INFERRED 0.85]
- **Singleblock K-small fix candidate selection flow** — singleblock_k_small_structural_defect, combo_winner_pareto_selection, w8_f10_i0, w8_f28_i0, production_candidate_battery_gate [EXTRACTED 1.00]
- **ra_core.c static functions verbatim-copied into diag_init_candidates.c** — experiments_2026_9_3_init_loop_optimization_diag_init_candidates, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_orbit, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_singleblock, experiments_2026_9_1_family_productionization_ra_core_ra_hash, experiments_2026_9_1_family_productionization_ra_core_ra_reseed, experiments_2026_9_1_family_productionization_ra_core_rot32, experiments_2026_9_1_family_productionization_ra_core_fmix32, experiments_2026_9_1_family_productionization_ra_core_guard_l, experiments_2026_9_1_family_productionization_ra_core_guard_m, experiments_2026_9_1_family_productionization_ra_core_ra_init_state_orbit [INFERRED 0.95]
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]
- **ra_prng2 Statistical Validation Test Suite** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_bigcrush [EXTRACTED 0.90]
- **Self-contained diagnostic files following diag_prereseed.c pattern** — experiments_2026_9_2_singleblock_o_width_fix_diag_wideo_singleblock, experiments_2026_9_2_singleblock_o_width_fix_diag_wideo_warmup_singleblock, experiments_2026_9_2_singleblock_prereseed_experiment_diag_prereseed [EXTRACTED 0.90]
- **winner_wired_v2 cycle grafted onto ZepFold shuffle-loop shape (scrambler_wired_addressable)** — experiments_2026_8_30_addressable_shuffle_scrambler_wired_addressable_c, winner_wired_addressable_c, experiments_2026_8_30_addressable_shuffle_scrambler_addressable_c [EXTRACTED 0.90]
- **Validation pipeline for wired addressable shuffle variant** — experiments_2026_8_30_addressable_shuffle_scrambler_wired_addressable_c, experiments_2026_8_30_addressable_shuffle_verify_parity_wired_py, experiments_2026_8_30_addressable_shuffle_scc_test_wired_py [EXTRACTED 0.90]
- **ra_prng2 State Transition Operator Composition (F = Mutate∘Permute∘Reseed)** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_transition_operator_f, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_mutate_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_permute_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_reseed_operator [EXTRACTED 0.95]
- **Byte-for-byte benchmark harness lineage (winner_wired_addressable -> tahap4 -> tahap5 -> tahap6)** — experiments_2026_8_30_addressable_init_research_winner_wired_addressable, experiments_2026_8_30_addressable_init_research_tahap4_bench, experiments_2026_8_30_addressable_init_research_tahap5_bench, experiments_2026_8_30_addressable_init_research_tahap6_bench [EXTRACTED 1.00]
- **Addressable-init research pipeline (Tahap 0-6)** — experiments_2026_8_30_addressable_init_research_results_tahap0, experiments_2026_8_30_addressable_init_research_results_tahap1, experiments_2026_8_30_addressable_init_research_results_tahap2, experiments_2026_8_30_addressable_init_research_results_tahap3, experiments_2026_8_30_addressable_init_research_results_tahap4, experiments_2026_8_30_addressable_init_research_results_tahap5, experiments_2026_8_30_addressable_init_research_handover_tahap6_tahap6 [EXTRACTED 1.00]
- **Candidates that FAILED Axis B normality criterion** — experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f8_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f9_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f22_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f24_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f26_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f27_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f29_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f33_i0 [EXTRACTED 1.00]
- **Bit-identical self-check chain linking pruned toy model to full-scale reference and winner C source** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_self_check_full_scale, experiments_2026_8_28_periodicity_heuristic_validation2_handover_pruned_wired_toy_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wired_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wiring_module, experiments_2026_8_28_periodicity_heuristic_validation2_handover_winner_wired_v2 [EXTRACTED 1.00]
- **Combo-search staged filter pipeline (Tier 0 -> Tier 1 -> Promotion Tier)** — experiments_2026_9_2_singleblock_cycle_combo_search_results_tier0_avalanche_gate, experiments_2026_9_2_singleblock_cycle_combo_search_results_promotion_tier [EXTRACTED 1.00]
- **2026-08-29 correction of (8,4,rows=4) lambda bound, documented across HANDOVER/RESULTS/STATUS** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_2026_08_29_correction, experiments_2026_8_28_periodicity_heuristic_validation2_handover_stale_lambda_lower_bound, experiments_2026_8_28_periodicity_heuristic_validation2_handover_corrected_lambda_value, experiments_2026_8_28_periodicity_heuristic_validation2_handover_checkpoint_file, experiments_2026_8_28_periodicity_heuristic_validation2_results_footnote_correction, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_summary, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_lower_bound_restated [EXTRACTED 1.00]
- **pruned_winner Cycle 1 vs Cycle 90 Avalanche Analysis** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_reseed_diffusion_result, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_avalanche_fraction [EXTRACTED 1.00]
- **winner_wired Avalanche Diffusion Convergence Across Cycles** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_cycle4_avalanche [EXTRACTED 1.00]
- **Winner Wired PRNG Avalanche Effect Analysis** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_avalanche_performance [EXTRACTED 1.00]
- **Avalanche Effect Cycle Comparison** — experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_avalanche_effect_analysis, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_1_avalanche, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_2_avalanche [EXTRACTED 1.00]
- **Family Use-Case to Algorithm Mapping** — experiments_2026_9_1_family_productionization_handover_usecase_mapping, experiments_2026_9_1_family_productionization_handover_winner_wired_unified, experiments_2026_8_30_addressable_init_research_tahap6_bench_ra_core_singleblock, experiments_2026_9_1_family_productionization_handover_ra_prng_family [EXTRACTED 1.00]
- **Documented gate-size-trap incidents across search experiments** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_operation_pruning_research_files, experiments_2026_9_2_singleblock_cycle_combo_search_handover_operand_position_search_files, experiments_2026_9_2_singleblock_cycle_combo_search_handover_singleblock_o_width_fix_experiment, experiments_2026_9_2_singleblock_cycle_combo_search_handover_gate_size_trap [EXTRACTED 1.00]
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
- **Pareto-selection axes (Speed A + Avalanche B feed the frontier)** — experiments_2026_9_3_combo_winner_pareto_selection_handover_axis_a_speed, experiments_2026_9_3_combo_winner_pareto_selection_handover_axis_b_avalanche_normality, experiments_2026_9_3_combo_winner_pareto_selection_handover_pareto_frontier [EXTRACTED 1.00]
- **Three enrichment slots forming the bounded per-round combo search space** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_search_space_dsl, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot1_o_tap_width, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot2_c_finalizer, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot3_extra_inject [EXTRACTED 1.00]
- **12-candidate PRNG family benchmark roster** — experiments_2026_8_31_prng_family_benchmark_handover_candidate_paperorig, experiments_2026_8_31_prng_family_benchmark_handover_candidate_wiredv2, experiments_2026_8_31_prng_family_benchmark_handover_candidate_addrcont, experiments_2026_8_31_prng_family_benchmark_handover_candidate_singleblock_k1, experiments_2026_8_31_prng_family_benchmark_handover_candidate_singleblock_k255, experiments_2026_8_31_prng_family_benchmark_handover_candidate_philox, experiments_2026_8_31_prng_family_benchmark_handover_candidate_xoshiro256, experiments_2026_8_31_prng_family_benchmark_handover_candidate_pcg32, experiments_2026_8_31_prng_family_benchmark_handover_candidate_chacha20, experiments_2026_8_31_prng_family_benchmark_handover_candidate_dev_urandom, experiments_2026_8_31_prng_family_benchmark_handover_candidate_mt19937, experiments_2026_8_31_prng_family_benchmark_handover_candidate_splitmix64 [EXTRACTED 1.00]
- **Cross-family PRNG speed comparison table (ra_prng orbit/singleblock vs other cores)** — experiments_2026_9_1_family_productionization_ra_core_ra_core_singleblock, experiments_2026_9_1_family_productionization_ra_core_ra_core_orbit, experiments_2026_8_31_prng_family_benchmark_benchmark_all_philox, experiments_2026_8_31_prng_family_benchmark_benchmark_all_splitmix64, experiments_2026_8_31_prng_family_benchmark_benchmark_all_pcg32, experiments_2026_8_31_prng_family_benchmark_benchmark_all_paperorig, experiments_2026_8_31_prng_family_benchmark_benchmark_all_xoshiro256, experiments_2026_8_31_prng_family_benchmark_benchmark_all_wiredv2, experiments_2026_8_31_prng_family_benchmark_benchmark_all_addrcont, experiments_2026_8_31_prng_family_benchmark_benchmark_all_chacha20, experiments_2026_8_31_prng_family_benchmark_benchmark_all_mt19937, experiments_2026_8_31_prng_family_benchmark_benchmark_all_dev_urandom [EXTRACTED 1.00]
- **11 Promotion-Tier FULLY CLEAN winners (K in {1,2,4,8,16,32,64,96} @16GB)** — experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f8_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f9_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f10_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f22_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f24_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f25_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f26_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f27_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f28_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f29_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f33_i0 [EXTRACTED 1.00]
- **pruned_winner Avalanche Analysis and Metric Evaluation** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_figure, experiments_2026_8_26_operation_pruning_research_pruned_winner_avalanche_behavior, experiments_2026_8_26_operation_pruning_research_pruned_winner_candidate_configuration [EXTRACTED 1.00]
- **Performance Benchmark Comparison Group** — readme_ra_prng2, readme_ra_prng3, readme_chacha20, readme_philox4x32, readme_dev_urandom [EXTRACTED 1.00]
- **Repository Structure Mapping (docs/STRUCTURE.md)** — readme_docs_structure, readme_ra_prng2, readme_ra_prng3, readme_benchmarks_comparisons_source [EXTRACTED 1.00]
- **Statistical Validation of ra_prng2** — readme_ra_prng2, readme_dieharder, readme_nist_sts, readme_testu01_bigcrush, readme_practrand [EXTRACTED 1.00]
- **Prior experiments that ruled out the init/seed fix direction** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_singleblock_prereseed_experiment, experiments_2026_9_2_singleblock_cycle_combo_search_handover_ra_core_orbit_crosscheck, experiments_2026_9_2_singleblock_cycle_combo_search_handover_kandidat_a_wideo, experiments_2026_9_2_singleblock_cycle_combo_search_handover_kandidat_b_wideo_warmup [EXTRACTED 1.00]
- **winner_wired_v2 and winner_wired_addressable both implement the same shared core generation loop** — experiments_2026_8_29_parallelization_research_handover_1tb_followup_winner_wired_v2, experiments_2026_8_29_parallelization_research_handover_1tb_followup_winner_wired_addressable, experiments_2026_8_29_parallelization_research_handover_1tb_followup_core_generation_loop [EXTRACTED 1.00]
- **Prior Research Folders That Tested Only Single-Stream Axes** — concept_periodicity_heuristic_validation_research, concept_operation_pruning_research, concept_operand_position_search_research, concept_state_update_mechanism_research, concept_periodicity_heuristic_validation2_research [EXTRACTED 1.00]
- **Tahap 0 candidate init formulas (Kandidat 1-5) evaluated together** — experiments_2026_8_30_addressable_init_research_handover_kandidat1, experiments_2026_8_30_addressable_init_research_handover_kandidat2, experiments_2026_8_30_addressable_init_research_handover_kandidat3_control, experiments_2026_8_30_addressable_init_research_handover_kandidat4, experiments_2026_8_30_addressable_init_research_handover_kandidat5 [EXTRACTED 1.00]
- **Tahap 1 Bit-Identical Unification** — experiments_2026_9_1_family_productionization_handover_tahap1_unification, experiments_2026_8_27_operand_position_search_winner_wired_v2_winner_wired_v2, experiments_2026_8_30_addressable_init_research_winner_wired_addressable_winner_wired_addressable [EXTRACTED 1.00]
- **Tahap 2 Production Validation Battery** — experiments_2026_9_1_family_productionization_handover_tahap2_validation_battery, experiments_2026_9_1_family_productionization_handover_winner_wired_unified, experiments_2026_8_30_addressable_init_research_tahap6_bench_ra_core_singleblock, quality_gate_avalanche_gate_min_bit [EXTRACTED 1.00]
- **Two-Stage Fix Strategy (Wide-o + Warm-up Fallback)** — experiments_2026_9_2_singleblock_o_width_fix_handover_wide_o_candidate, experiments_2026_9_2_singleblock_o_width_fix_handover_warmup_fallback_candidate, project_multikey_remix_search [EXTRACTED 1.00]
- **Watchdog process-matching bug, its fix, and the sweep changes it required** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_watchdog_pgrep_bug, experiments_2026_8_28_periodicity_heuristic_validation2_handover_auto_stop_power38, experiments_2026_8_28_periodicity_heuristic_validation2_handover_cmd_pattern_fix, experiments_2026_8_28_periodicity_heuristic_validation2_handover_run_sweep, experiments_2026_8_28_periodicity_heuristic_validation2_handover_manual_only_exclusion [EXTRACTED 1.00]
- **10 of 11 Promotion Tier winners share an XORSHIFT-based finalizer stage as the dominant recurring pattern** — w8_f8_i0, w8_f9_i0, w8_f10_i0, w8_f22_i0, w8_f24_i0, w8_f25_i0, w8_f26_i0, w8_f27_i0, w8_f28_i0, w8_f29_i0 [EXTRACTED 1.00]
- **Array-Native Data Permutation and Fingerprinting Workflow** — research_iccs2026_ra_prng_avalanche_sensitivity, sub_projects_fingerprinting_tools_concept, benchmarks_comparisons_source_zepfold_cli [INFERRED 0.75]
- **PractRand anomaly signals observed across both candidates** — experiments_2026_9_2_singleblock_o_width_fix_results_bcfn, experiments_2026_9_2_singleblock_o_width_fix_results_tmfn, experiments_2026_9_2_singleblock_o_width_fix_results_fpf14 [INFERRED 0.75]
- **K-kecil Defect Discovery Chain** — experiments_2026_9_2_singleblock_o_width_fix_handover_k_kecil_defect, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_singleblock, experiments_2026_9_2_singleblock_k_threshold_characterization_results, experiments_2026_9_1_production_candidate_battery [INFERRED 0.80]
- **ra_prng2 Original Benchmark Report Set** — benchmarks_results_ra_prng2_original_rnging_perf_stat_, benchmarks_results_ra_prng2_original_testu01, benchmarks_results_ra_prng2_original_dieharder_test, benchmarks_results_ra_prng2_original_shuffling_perf_stat [INFERRED 0.80]
- **ra_prng3 Benchmark Report Set** — benchmarks_results_ra_prng3_nist_sts_test, benchmarks_results_ra_prng3_dieharder_test, benchmarks_results_ra_prng3_ent_test, benchmarks_results_ra_prng3_perf_stat_rnging, benchmarks_results_ra_prng3_perf_stat_scrambling, benchmarks_results_ra_prng3_practrand_test [INFERRED 0.80]
- **Candidate A Diagnostic Build Pattern** — experiments_2026_9_2_singleblock_o_width_fix_handover_wide_o_candidate, experiments_2026_9_2_singleblock_o_width_fix_diag_wideo_singleblock, experiments_2026_8_26_operation_pruning_research_pruned_prng_build_o, experiments_2026_9_2_singleblock_k_threshold_characterization_multikey_stream_k [INFERRED 0.85]
- **ChaCha20 Speed + Entropy Benchmark Suite** — benchmarks_comparisons_rnging_speed_chacha20_result, benchmarks_comparisons_entropy_chacha20_result [INFERRED 0.85]
- **Cumulative gap-explanation chain: I/O (~85%) -> reseed/M-width (~13% of residual) -> recurrence pruning (~5%)** — experiments_2026_8_30_addressable_shuffle_speed_benchmark_io_bottleneck_finding, experiments_2026_8_30_addressable_shuffle_speed_benchmark_gap_decomposition_finding, experiments_2026_8_30_addressable_shuffle_speed_benchmark_recurrence_pruning_finding [INFERRED 0.85]
- **Four-step 1TB diagnostic sequence (Langkah 1-4) forming the decision procedure** — experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah1_singlestream_v2_1tb, experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah2_reseed_confirm, experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah3_addressable_1tb, experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah4_sintesis [INFERRED 0.85]
- **Pruned Winner Avalanche Progression Analysis (Cycle 1 vs Cycle 2)** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_heatmap, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle1_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle2_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_avalanche_strengthening [INFERRED 0.85]
- **Winner Wired Warmup Cycle Avalanche Progression** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_winner_wired_wiring, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle1_avalanche, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle3_avalanche [INFERRED 0.85]
- **Avalanche Effect Bit Diffusion Evaluation** — experiments_others_avalanche_effect_heatmap_figure, experiments_others_avalanche_effect_heatmap_avalanche_effect, experiments_others_avalanche_effect_heatmap_hamming_distance_analysis, experiments_others_avalanche_effect_heatmap_original_algorithm [INFERRED 0.85]
- **Four-Axis Validation Framework (periodicity/speed/avalanche/PractRand)** — experiments_2026_8_25_periodicity_heuristic_validation_toy_prng, experiments_2026_8_28_state_update_mechanism_research_instruction_count_speed_axis, experiments_2026_8_28_state_update_mechanism_research_avalanche_gate_min_bit, experiments_2026_8_28_state_update_mechanism_research_practrand_16gb_tier [INFERRED 0.85]
- **Periodicity Heuristic Empirical Validation Lifecycle** — research_iccs2026_ra_prng_periodicity_heuristic, experiments_2026_8_25_periodicity_heuristic_validation_toy_model_spec, experiments_2026_8_25_periodicity_heuristic_validation_scaling_verdict [INFERRED 0.85]
- **winner_wired/ra_core evolutionary lineage of the pruned+wired recurrence** — experiments_2026_8_27_operand_position_search_baseline, experiments_2026_8_27_operand_position_search_winner_wired, experiments_2026_8_27_operand_position_search_winner_wired_v2, experiments_2026_8_30_addressable_init_research_winner_wired_addressable, experiments_2026_9_1_family_productionization_ra_core [INFERRED 0.85]
- **K=255 avalanche heatmap re-check flow: Promotion Tier winners verified against the dead-bit defect class using a ported operand-position-search script** — promotion_tier, experiments_2026_9_2_singleblock_cycle_combo_search_results_avalanche_heatmap_k255_recheck, experiments_2026_9_2_singleblock_cycle_combo_search_avalanche_heatmap_winners_k255, experiments_2026_9_2_singleblock_cycle_combo_search_results_dead_bit_defect [INFERRED 0.85]
- **Q1-Q4 Parallelization Research Questions** — concept_stream_independence_testing, concept_seed_collision_probability, concept_throughput_scalability, concept_simd_vectorization_feasibility [INFERRED 0.85]
- **Pareto Frontier Selection over Combo-Winner Candidates** — experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f10_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f28_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f25_i0, experiments_2026_9_3_combo_winner_pareto_selection_pareto_select [INFERRED 0.85]
- **Q1 cross-stream independence investigation (Method A + Method B)** — experiments_2026_8_29_parallelization_research_results_q1_independence, experiments_2026_8_29_parallelization_research_results_method_a_cross_correlation, experiments_2026_8_29_parallelization_research_results_method_b_interleaved_practrand, experiments_2026_8_29_parallelization_research_results_cross_correlation_py, experiments_2026_8_29_parallelization_research_results_interleave_practrand_py [INFERRED 0.85]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_practrand, concept_ent [INFERRED 0.85]
- **K-small BCFN defect fix search (Kandidat A/B -> no-recommendation)** — experiments_2026_9_2_singleblock_o_width_fix_results_kandidat_a, experiments_2026_9_2_singleblock_o_width_fix_results_kandidat_b, experiments_2026_9_2_singleblock_o_width_fix_results_rekomendasi, experiments_2026_9_2_singleblock_k_threshold_characterization_results [INFERRED 0.85]
- **Rantai investigasi defect K kecil ra_core_singleblock** — experiments_2026_9_1_keyzero_guard_fix_results_k1_singleblock_defect, experiments_2026_9_2_singleblock_prereseed_experiment_results, experiments_2026_9_2_singleblock_k_threshold_characterization_results_safe_k_threshold [INFERRED 0.90]
- **Pruned Winner Cycle 1 vs Cycle 3 Avalanche Comparison** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_reseed_avalanche_diffusion [INFERRED 0.95]
- **Avalanche Proxy vs Full Empirical Quality Disconnect** — experiments_2026_8_26_operation_pruning_research_results_shift_repair_failure, experiments_2026_8_26_operation_pruning_research_status_gate_size_traps, experiments_2026_8_27_operand_position_search_handover_dead_seed_bits, experiments_2026_8_27_operand_position_search_handover_avalanche_heatmap_baseline [INFERRED]
- **Pruned Winner Multi-battery Statistical Validation** — experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_26_operation_pruning_research_practrand_pruned_winner_1tb_practrand_1tb_log, experiments_2026_8_26_operation_pruning_research_dieharder_pruned_winner_piped_piped_results, experiments_2026_8_26_operation_pruning_research_dieharder_gcd_fixed_gcd_test_log, experiments_2026_8_26_operation_pruning_research_dieharder_rgb_minimum_distance_fixed_rgb_min_dist_log [INFERRED]
- **ra_prng Algorithmic Evolution and Research Pipeline** — readme_ra_prng2, experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_27_operand_position_search_handover_handover_spec [INFERRED]

## Communities (435 total, 106 thin omitted)

### Community 0 - "Pareto Selection Results (Combo Winners)"
Cohesion: 0.06
Nodes (59): CLAUDE.md (project instructions), docs/ERRATA.md, dieharder_pruned_winner_piped.txt, parallelization-research experiment (2026-8-29), HANDOVER_TAHAP6.md, Tahap 6 hard scope limit (rng<=255, hard-abort guard), core_entry_t, FILE (+51 more)

### Community 1 - "winners_bench.c"
Cohesion: 0.08
Nodes (45): CORES[] function-pointer dispatch table (ra_core.c), Cycle-operation combo DSL search space (756 combos), generate_all_tier01(), Path, Compare compiled binary's --single output (multi-round, rng=37) against…, The ONE true hand-trace: width=2, finalizer=empty(idx0), inject=off(idx0) must…, render_c(), sanity_check() (+37 more)

### Community 2 - "Completed Tasks"
Cohesion: 0.08
Nodes (46): Accum candidate (ring-arithmetic strength reduction), Cross-correlation / collision scan test, docs/ERRATA.md (planned, not created), Axis A - Speed Benchmark Methodology, Deviation: K=1 noise fix via 7x min-of-repeats reinit-sweep, operation-pruning-research experiment (gate-trap example), main(), main() (+38 more)

### Community 3 - "tahap4_bench.c"
Cohesion: 0.04
Nodes (45): 1. Status & tujuan, 2. Formula & fakta struktural (sudah final, tidak perlu digali ulang), 3. Temuan disassembly (sudah dijalankan read-only, sesi persiapan 2026-08-30), 4. Kandidat optimasi, berperingkat, 5. Rencana file & eksperimen, 6. Gerbang validasi statistik (aturan mekanis, bukan penilaian bebas), 7. Batasan read-only (berlaku juga untuk Tahap 5), 8. Kondisi berhenti / pelaporan eksplisit (+37 more)

### Community 4 - "tahap5_bench.c"
Cohesion: 0.13
Nodes (42): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32() (+34 more)

### Community 5 - "tahap0_prototype.py"
Cohesion: 0.07
Nodes (41): 2026-08-29 correction: (8,4,rows=4) watchdog bug + stale headline number, auto_stop_power38.py (watchdog script), avalanche_check.py, .cycle_measure_ckpt_n8_w4_rows4_seed0.bin (CkptHeader checkpoint), Fix: CMD_PATTERN uses exact argv instead of broad pgrep prefix, Corrected λ > 3.045×10^11 for (8,4,rows=4), cycle_measure.py / cycle_measure.c (Brent's algorithm), enumerate_n2w4.py (+33 more)

### Community 6 - "Singleblock Cycle-Operation Combo Search Handover"
Cohesion: 0.10
Nodes (36): main(), philox4x32_10(), philox4x32_round(), main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32() (+28 more)

### Community 7 - "Operand Position Search Experiment Results"
Cohesion: 0.10
Nodes (34): blake2b8(), init_kandidat3_control(), init_kandidat4(), init_kandidat5(), init_original(), main(), multiset_digest(), pack_u32() (+26 more)

### Community 8 - "ra_prng2 (Array-Based PRNG)"
Cohesion: 0.11
Nodes (33): BCFN structural defect (K small, ra_core_singleblock) — FAILs up to K≈72, clean from K≈96, operand_search.py (operation-pruning-research), pruned_winner — dead-bit defect candidate from operand-position-search: seed bits 5/6 nearly non-avalanching across 253/255 output positions, mean per-bit fraction 0.0078, passed old scalar gate but caught by full K=255 heatmap, combo_search_log.jsonl, promotion_log.jsonl / promotion_logs/*.log, ra_core_singleblock Cycle-Operation Combo Search — RESULTS, Dead-bit defect class — near-zero avalanche fraction for specific seed bits across the full output cycle, invisible to single-word (K=1) scalar avalanche gates, detectable only via full-cycle (K=255) heatmap, Promotion Tier (29 candidates x 8 K-values x 16GB, VPS) (+25 more)

### Community 9 - "ra_core.c"
Cohesion: 0.09
Nodes (34): Singleblock Cycle-Operation Combo Search Handover, avalanche_gate_min_bit (vs scalar-average avalanche_gate), BCFN/TMFn/FPF structural defect at small K, bench_ra_core.c (reinit-sweep microbench pattern), CLAUDE.md workflow rule: run /graphify --update before closing task, diag_wideo_singleblock.c / diag_wideo_warmup_singleblock.c (multikey --stream driver pattern), fmix32, Gate-size-trap incidents (long-stream gate misses defects) (+26 more)

### Community 10 - "Langkah 3: Interleaved 1TB for winner_wired_addressable (shared core-loop test)"
Cohesion: 0.09
Nodes (33): Scrambling Speed Benchmark (perf stat, per-call vs amortized, 30 runs), ra_prng2 NIST STS Test Result, ra_prng2 PractRand Test Result (up to 128 GB, no anomalies), ra_prng Build and Test Workflow, Contributing Guide, ra_prng Implementation Variant Roles, Repository Structure Specification, ra_prng README (+25 more)

### Community 11 - "ra_prng2 (32-bit generator, CSAI2025)"
Cohesion: 0.07
Nodes (33): baseline_local PractRand 16GB Test Log, v01 PractRand 16GB Test Log (FAIL), v02 PractRand 16GB Test Log (FAIL), v03 PractRand 16GB Test Log (Pass), v04 PractRand 16GB Test Log (Pass), v06 PractRand 16GB Test Log (Pass), v07 PractRand 16GB Test Log (Pass), v08 PractRand 16GB Test Log (Pass) (+25 more)

### Community 12 - "winner_wired_v2.c"
Cohesion: 0.16
Nodes (30): core_entry_t, find_core(), fmix32(), main(), now_seconds(), philox core (diag_init_candidates.c), philox4x32_10(), philox4x32_round() (+22 more)

### Community 13 - "benchmark_all.c"
Cohesion: 0.15
Nodes (28): FILE, kat_philox(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), ra_core_baseline(), ra_core_p1(), ra_core_p2() (+20 more)

### Community 14 - "diag_init_candidates.c"
Cohesion: 0.10
Nodes (28): FILE, fmix32(), main(), narrow_o(), now_seconds(), pick_use_wide(), ra_core_singleblock_warmup(), ra_init_state_singleblock() (+20 more)

### Community 15 - "stream"
Cohesion: 0.08
Nodes (29): Hamas A. Rahman (author), ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex, ra_prng2 and Beyond (Paper CA-162), Scopus, BigCrush (TestU01), Bit Shifting (+21 more)

### Community 16 - "State-Update Mechanism Research Results (Phase 1-3)"
Cohesion: 0.10
Nodes (23): Belum dikerjakan / perlu keputusan user, File yang dihasilkan sesi ini, RESULTS: Tahap 2 — Battery validasi produksi (`ra_core_orbit` + `ra_core_singleblock`), Status, ra_init_state_addressable (winner_wired_addressable.c / scrambler_addressable.c, additive combine), BCFN PractRand Test, BRank (binary-rank) PractRand Test, Guard-XOR Fix (key=0 defect) (+15 more)

### Community 17 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.13
Nodes (26): winner_wired_addressable Method A+B validated clean up to 128GB (xlarge tier), re-verified raw stdout this session, interleave_practrand_1tb_confirm.py (planned; reuse run_interleave_practrand with alt seeds), Shared core generation loop: ra_permutation_cycle / ra_reseed / ra_core (identical between winner_wired_v2.c and winner_wired_addressable.c; only ra_init_state[_addressable] differs), 1TB Anomaly Diagnostic Plan (Langkah 1-4), GATED_TIERS explicit-invocation gate (1tb/xlarge require explicit arg, unlike smoke/medium/full), HANDOVER_TAHAP5.md §6 convention: full Tahap 0-3 addressable-init repeat required if init formula changes, Harness bug: JSON 'passed' field mismatches lowercase PractRand 'suspicious'/'very suspicious' tags (case-sensitivity), not fixed in-place, (a) Cross-stream correlation specific to winner_wired_v2 init formula (+18 more)

### Community 18 - "other/avalanche_heatmap_original.py"
Cohesion: 0.17
Nodes (24): Manual accumulator (strength reduction) ruled out -- 4.4-5x slower, defeats auto-vectorization, Rank 1: -mprefer-vector-width=512 flag-only speedup (not adopted), Rank 3: -funroll-loops flag-only speedup (not adopted), Tahap 5: Init Cost Optimization, FILE, find_variant(), main(), mode_init_cost() (+16 more)

### Community 19 - "2026-8-25_periodicity-heuristic-validation/cycle_measure.c"
Cohesion: 0.11
Nodes (24): BCFN structural defect (K-small), Avalanche test: per-bit avalanche fraction heatmap (flipped seed bit x output word index), Avalanche heatmap K=255 (full cycle): w8_f24_i0, Candidate w8_f24_i0 (tap width=8, finalizer variant f24), compile_candidate(), iter_checkpoints(), _log(), main() (+16 more)

### Community 20 - "2026-8-28_periodicity-heuristic-validation2/cycle_measure.c"
Cohesion: 0.12
Nodes (22): pruned_prng.c, build_o() (original 8-tap formula), multikey_stream.c Weyl-increment multikey pattern (base pattern), ra_permutation_cycle_singleblock(), Production-Candidate Battery Gate, Cycle-operation combo-search DSL (756-combination search space: tap width x finalizer x inject), main(), singleblock vs orbit speed crossover (K≈20-21, singleblock faster above this) (+14 more)

### Community 21 - "ra_core_singleblock Cycle-Operation Combo Search — RESULTS"
Cohesion: 0.16
Nodes (22): Avalanche multi-seed normality test, Combo-winner Pareto-selection, Commit d2f1675 (combo-winner Pareto-selection), Cycle-op combo search (11 candidates), dieharder test battery, avalanche_heatmap_winners_k255.png — heatmap visualization (32x255 cells per candidate), uniform noise pattern for all 11 winners, no dark horizontal lines (no dead bits), Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect, 7 extra-inject variants (slot 3 DSL combo-search) (+14 more)

### Community 22 - "recipes.py"
Cohesion: 0.10
Nodes (16): common.py (TIERS_Q2), Path, Langkah 2 of HANDOVER_1TB_FOLLOWUP.md: re-run the interleaved 1TB Method B test…, run_live(), main(), Q1 Method B: interleave K independent `--stream <seed> <n>` streams word-by-…, run_interleave_practrand(), main() (+8 more)

### Community 23 - "avalanche_stats"
Cohesion: 0.18
Nodes (20): cycle_measure.c (toy model), load_means(), main(), avalanche_gate_min_bit metric (per-bit floor 0.2, band [0.3,0.7]), State-Update Mechanism Research Handover (original spec), Handover Phase 3: L/M Mechanism Follow-up, inject (L) - one-directional overwrite mechanism, Instruction count (perf stat) as primary speed axis (+12 more)

### Community 24 - "ensure_ra_prng2_cli"
Cohesion: 0.11
Nodes (22): Brent Cycle Measure Checkpoint/Resume Architecture, Hash Depth Domination on Cycle Length, Exact State-Space In-Degree Poisson(1) Rejection, Periodicity Heuristic Validation Status Report, Dieharder DNA Test Log (Suspect/EOF Excluded), Dieharder Marsaglia-Tsang GCD Test Results, Dieharder 27 Piped Test Results, Dieharder Rewind Artifact Failure Log (+14 more)

### Community 25 - "stream_values"
Cohesion: 0.14
Nodes (21): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), Original Algorithm Avalanche Diffusion Comparison (Cycle 1 vs Cycle 3), Original Algorithm Cycle 1 Avalanche Fraction (0.497576), Original Algorithm Cycle 3 Avalanche Fraction (0.499740), Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3, main() (+13 more)

### Community 26 - "ICCS 2026 Research Paper: Array-Native Randomness"
Cohesion: 0.13
Nodes (21): Parallelization Research Results, 1TB Interleaved PractRand Anomaly (3/304 tests flagged, unconfirmed), common.py (shared harness config, TIERS_Q1B), cross_correlation.py, cross_correlation_ra_prng2.py, interleave_practrand.py, interleave_practrand_ra_prng2.py, Method A: Cross-correlation analysis (lag-0 Pearson, Bonferroni) (+13 more)

### Community 27 - "avalanche_heatmap_winner_v2.py"
Cohesion: 0.16
Nodes (20): Four-Axis Validation: periodicity, PractRand, avalanche, speed, permute + xor_fold State-Update Mechanism (kept unchanged), FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle() (+12 more)

### Community 28 - "avalanche_effect_analysis.py"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 29 - "others/avalanche_heatmap_original.py"
Cohesion: 0.19
Nodes (19): Candidate, final_cons(), _build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32() (+11 more)

### Community 30 - "common.py (TIERS_Q2)"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 31 - "Params"
Cohesion: 0.14
Nodes (17): ensure_ra_prng2_cli(), ndarray, Path, Shared helpers for the parallelization-research harness. Never modifies…, Bounded capture: run `binary --stream seed n`, return n uint32 values. Suitable…, Streaming capture: caller reads/closes proc.stdout incrementally. Use for large…, Compile the optional paper-exact comparator into this folder, if not already…, stream_popen() (+9 more)

### Community 32 - "Candidate"
Cohesion: 0.17
Nodes (17): analyze_group(), build_group(), main(), build_group(), main(), Non-regression check for `ra_init_orbit` (the multikey mix-init fix candidate,…, run(), ndarray (+9 more)

### Community 33 - "winner_wired_addressable.c"
Cohesion: 0.12
Nodes (19): Benchmark Source Comparisons Documentation, ZepFold Shuffling CLI Tool, Toy PRNG Avalanche Sanity Verification Gate, Brent Cycle Detection Methodology, Periodicity Validation Experiment Handover, Periodicity Heuristic Validation Results Report, Periodicity Heuristic Scalability & Non-Monotonicity Verdict, Initial Sweep Periodicity Measurement Summary (+11 more)

### Community 34 - "tahap6_bench.c"
Cohesion: 0.19
Nodes (16): _log(), main(), phase_a_hash_access_check(), phase_b_greedy_ablation(), Driver for the operation-pruning greedy ablation search. Two phases: Phase A:…, all_ops_baseline(), describe(), ops_to_bitmask() (+8 more)

### Community 35 - "scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)"
Cohesion: 0.18
Nodes (18): Sanity-check: orbit's implausible 2.4x speed vs old singleblock was a cross-session machine-speed artifact, Deviation: baseline copied from ra_core.c not benchmark_all.c (stale init formula), addrcont core, chacha20 core, dev_urandom baseline, mt19937 core, paperorig core, pcg32 core (+10 more)

### Community 36 - "bench_ra_core.c"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for the winning operand-position rewiring found by…, Mirrors winner_wired.c's ra_reseed()/ra_hash() exactly (identical to… (+9 more)

### Community 37 - "RESULTS: production-candidate battery for `ra_core.c`"
Cohesion: 0.11
Nodes (17): Ambang, Batasan &amp; catatan, Hasil sweep, Karakterisasi ambang K minimum aman — `ra_core_singleblock`, Konteks, Metodologi, Overlay trade-off performa, Pertanyaan terbuka untuk user (+9 more)

### Community 38 - "ra_core_singleblock"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_reseed(), plot_comparison(), plot_single(), Path (+9 more)

### Community 39 - "RaPrng2"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_capture(), original_reseed(), plot_comparison(), plot_single() (+9 more)

### Community 40 - "Parallelization Research Handover"
Cohesion: 0.15
Nodes (10): group_size(), Params, Rescale the four fixed 32-bit plain-shift amounts to width w. s_w = round(s * w…, Top-w-bits truncation of a 32-bit golden-ratio-derived constant. Trap fix:…, G(n, rows): generalization of ra_hash's fixed 8/32 structural constants (256 =…, rescale_shifts(), truncate_const(), enumerate_state_space() (+2 more)

### Community 41 - "toy_prng.py"
Cohesion: 0.18
Nodes (16): Orbit Engine, L[256] array (dead-code discovery), Tahap 2: C Implementation, FILE, main(), Orbit Addressing formula, ra_core(), ra_hash() (+8 more)

### Community 42 - "avalanche_heatmap_pruned_winner.py"
Cohesion: 0.14
Nodes (17): 7. Follow-up: cycle `winner_wired_v2` + addressable init — SELESAI (2026-08-31), Double-swap hypothesis (disproven), Follow-up: varian cycle `winner_wired_v2` + addressable init (2026-08-31), scc_test_wired.py (statistical & distinctness validation, wired variant), scrambler_addressable.c (CLI), ra_init_state_addressable(L, M, key), scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI), scrambler_wired_addressable.py (Python reference port, wired variant) (+9 more)

### Community 43 - "next_state"
Cohesion: 0.24
Nodes (15): core_entry_t, find_core(), main(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10(), philox4x32_round() (+7 more)

### Community 44 - "stream_driven_shuffle.c"
Cohesion: 0.12
Nodes (17): clean, but a SEPARATE, more severe defect (K=1 pattern) now blocks, CRITICAL DEFECT: key=0 produces a permanent all-zero stream (both modes), further progress — gate STILL NOT PASSED, history, superseded by the rerun above for Steps 0 and 3 specifically, Original Step 0-3 run (2026-09-01, before the key=0 fix) — kept for, Provenance (Step 0), Recommendation (K=1 structural defect — NEW, open, needs a decision), Recommendation (key=0 — RESOLVED, kept for history) (+9 more)

### Community 45 - "Karakterisasi ambang K minimum aman — `ra_core_singleblock`"
Cohesion: 0.14
Nodes (14): chacha20_block(), main(), Candidate: chacha20, PRNG Family Benchmark Results, chacha20.c RFC 8439 nonce byte-order bug, Closing note, K=1 (frequent reinit) vs largest-K (steady state), sorted by K=1 cost ascending, MB/s sanity check, sorted descending (+6 more)

### Community 46 - "benchmark_all_run.py"
Cohesion: 0.18
Nodes (11): _find_lib_path(), _lib_handle(), _load_library(), Path, RaPrng2, ctypes binding for the ra_prng2 C engine. This is glue only - it marshals calls…, Thin wrapper around one RA_PRNG instance., CDLL (+3 more)

### Community 47 - "diag_keyzero_guard.c"
Cohesion: 0.12
Nodes (16): Embarrassingly Parallel Architecture (per-instance state, no locks), Operand-Position Search Research (2026-8-27), Operation Pruning Research (2026-8-26), Periodicity Heuristic Validation 2 (2026-8-28), Periodicity Heuristic Validation Research (2026-8-25), Read-Only Source Constraint (never modify ra_prng2/ra_prng3/winner_wired_v2.c in place), Seed/Cycle Collision Probability (Q2), SIMD Vectorization Feasibility (Q4) (+8 more)

### Community 48 - "Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect"
Cohesion: 0.26
Nodes (13): brent(), Brent's cycle-detection algorithm applied to toy_prng.next_state. Measures the…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, enumerate_state_space(), main(), Full state-space enumeration for n=2,w=4 (|S|=131072, default rows=2). Directly…, init_state(), next_state() (+5 more)

### Community 49 - "RESULTS: fix defect K-kecil `ra_core_singleblock` — perlebar `o` (Kandidat A) + internal warm-up (Kandidat B)"
Cohesion: 0.21
Nodes (15): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), pruned_winner_capture(), pruned_winner_reseed(), Path (+7 more)

### Community 50 - "ra_prng3 PRNG Generator"
Cohesion: 0.22
Nodes (14): capture(), main(), Per-bit avalanche stats (cycle 1) for all 13 winners from RESULTS.md's table,…, final_cons(), init_state(), permutation_cycle(), Operand-position-parameterized reimplementation of `baseline.c`'s…, DEFAULT_WIRING must reproduce baseline.c / pruned_prng.py's… (+6 more)

### Community 51 - "collision_scan.py"
Cohesion: 0.20
Nodes (13): brent(), Brent's cycle-detection algorithm applied to pruned_wired_toy_prng.next_state.…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, next_state(), State, ra_hash_gen_sequential(), Toy-scale (n, w, rows)-parameterized port of winner_wired_v2.c's permutation…, HASH_ACCESS=sequential port of winner_wired_v2.c's ra_hash: each output word is… (+5 more)

### Community 52 - "PRNG Family Benchmark Results"
Cohesion: 0.23
Nodes (15): Speed Benchmark Output (stream_driven_shuffle), FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash() (+7 more)

### Community 53 - "Family Productionization Handover"
Cohesion: 0.27
Nodes (15): build_bench_all(), build_crosscheck_originals(), main(), ols_fit(), paperorig/wiredv2 originals: `bin --stream key n`. addrcont original…, Like run(), but keeps stdout as raw bytes -- for --stream binary output., run(), run_crosscheck() (+7 more)

### Community 54 - "diag_wideo_warmup_singleblock.c"
Cohesion: 0.33
Nodes (15): FILE, fmix32(), is_all_zero(), main(), multikey_chain(), ra_core_orbit_stream(), ra_core_singleblock_block(), ra_hash() (+7 more)

### Community 55 - "avalanche_heatmap_winner.py"
Cohesion: 0.16
Nodes (15): Dieharder Test Results (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), PractRand Test Results (ra_prng3), Dieharder Test Suite, ent Entropy Test Tool (+7 more)

### Community 56 - "Handover: Tahap 5 — Optimasi Biaya Init Addressable"
Cohesion: 0.22
Nodes (13): digest_collision_prob(), main(), main(), Tahap 2 (family-productionization) follow-up, Q2 re-pointed at…, scan_fullblock(), Q2: probability of cross-stream collision/overlap between different seeds'…, Birthday-bound estimate for pure hash-digest collision probability across…, scan_blocksweep() (+5 more)

### Community 57 - "io_only_isolation_test.c"
Cohesion: 0.26
Nodes (14): core_fn_t, FILE, fmix32(), main(), now_seconds(), pick_variant(), ra_core_singleblock_narrow(), ra_core_singleblock_wideo() (+6 more)

### Community 58 - "`ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi"
Cohesion: 0.25
Nodes (13): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for `winner_wired_v2.c` (wiring "v08" in RESULTS.md's…, Mirrors winner_wired_v2.c's ra_reseed()/ra_hash() exactly (identical to… (+5 more)

### Community 59 - "Singleblock cycle-op combo search experiment (2026-09-02)"
Cohesion: 0.20
Nodes (12): avalanche_gate_min_bit_binary(), capture_first_cycle(), main(), practrand_prefix_gate_binary(), Path, Avalanche + PractRand measurement for the phase-1 (L-only) state-update-…, `CYCLES` outer cycles' worth of `c` values -- see the CAPTURE_LEN comment above…, # NOTE: unlike pruned_prng.c's CLI (quality_gate.py's convention, where (+4 more)

### Community 60 - "2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py"
Cohesion: 0.26
Nodes (13): FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash(), ra_init_state_addressable() (+5 more)

### Community 61 - "Production Readiness Handover — `ra_core_orbit` / `ra_core_singleblock`"
Cohesion: 0.18
Nodes (13): Avalanche effect diagnostic (seed-bit-flip vs output-bit-difference), Avalanche heatmap test method (seed-bit-flip vs output-word bit-diff), Singleblock K-small structural defect (fixed via cycle-op combo search), w8_f10_i0 cycle-op combo candidate (tap width=8, finalizer variant 10, i0), PRNG cycle-op candidate w8_f33_i0 (tap width=8, finalizer #33, i0), Singleblock cycle-op combo search experiment (2026-09-02), Avalanche Heatmap w8_f10_i0 (K=255), Avalanche heatmap: w8_f25_i0 @ K=255 (full cycle) (+5 more)

### Community 62 - "Wiring"
Cohesion: 0.23
Nodes (12): decode_batch(), main(), pack_key(), Full state-space enumeration for n=4,w=4 (default rows=4 -> G=1), |S| = 4! *…, Cross-check vec_next_state against the scalar reference on random states drawn…, Pack (L, M, cons, it) arrays into a single uint64 key array, w bits per field,…, Mixed-radix decompose a flat index array into (lperm, m_0..m_{n-1}, it)…, Vectorized rotw: rotate low w bits of x left by r bits (mod w). x, r are numpy… (+4 more)

### Community 63 - "ra_core_singleblock"
Cohesion: 0.32
Nodes (11): _capture_first_cycle(), _log(), main(), Driver for the operand-position rewiring search over wiring.py's 108 candidate…, tier0_avalanche(), tier1_practrand_small(), all_wirings(), describe() (+3 more)

### Community 64 - "diag_wideo_singleblock.c"
Cohesion: 0.31
Nodes (12): core_fn_t, FILE, fmix32(), main(), pick_variant(), ra_core_singleblock_baseline(), ra_core_singleblock_prereseed(), ra_hash() (+4 more)

### Community 65 - "HANDOVER: Init-loop speed optimization untuk `ra_core_orbit` / `ra_core_singleblock`"
Cohesion: 0.15
Nodes (13): Array Index Shuffling, Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), Bitwise Rotation (rotl32), R. Durstenfeld, Fisher–Yates Shuffle, Mutate Operator, Rationale: Non-Invertible Hash Reseed Trade-off (Reduces Period, Aids Diffusion) (+5 more)

### Community 66 - "diag_prereseed.c"
Cohesion: 0.17
Nodes (12): pruned_winner Avalanche Fraction Metric (Cycle 1 vs Cycle 90), Avalanche Effect Heatmap: pruned_winner Cycle 1 vs Cycle 90, Multi-Cycle Reseed Avalanche Diffusion Improvement (0.418 to 0.468), winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039), Avalanche Effect Heatmap (winner_wired_v2), winner_wired_v2 Wiring Parameters (a_xor=d, c_shift=a, rotc_amount=b, rotc_xor=a), Avalanche Effect (Bit-Flip Sensitivity), Joan Daemen and Vincent Rijmen (+4 more)

### Community 67 - "Fisher–Yates Shuffle"
Cohesion: 0.17
Nodes (11): 1. Op/wiring mapping ported to toy scale, 2. Tap-survivor generalization: top-2-by-count, and why, 3. Tap shift-width collapse (caught during design, not present in either parent), 4. `ra_hash_gen` divergence from ground truth (found while reading source, not assumed), 5. Self-check (passed) -- MUST stay passing, CRITICAL BUG found and fixed this session -- read before writing more scripts, Design decisions (read before touching `pruned_wired_toy_prng.py`), HANDOVER.md (periodicity-heuristic-validation2) (+3 more)

### Community 68 - "winner_wired_addressable_v2.c"
Cohesion: 0.23
Nodes (10): dominates(), load_jsonl(), main(), Path, Pareto frontier selection (HANDOVER.md sect 4) over the 11 combo-search…, True if candidate a dominates candidate b: a is >= b on all 3 objectives (in…, main(), Axis A orchestrator (HANDOVER.md sect 2): K=1 / K=255 reinit-sweep and… (+2 more)

### Community 69 - "Design decisions (read before touching `pruned_wired_toy_prng.py`)"
Cohesion: 0.17
Nodes (12): Axis A — Speed, Closing note, Deviations from HANDOVER.md (documented, both investigated before deviating), K=1 (frequent reinit) vs largest-K (steady state), Konteks, MB/s sanity check, Pareto frontier, Perbandingan vs keluarga PRNG lain (+4 more)

### Community 70 - "pareto_select.py"
Cohesion: 0.17
Nodes (12): Against the HANDOVER talking-point thresholds, Assembly diff (`gcc -O3 -march=native -S`, extracted per-function), Benchmark (K=1, `--bench <core> 50000000 1`, 3 runs each, min/best-of-3 internal to each run), Benchmark (K=255, `--bench <core> 50000000 255`, 3 runs each) — the candidate is NOT a strict win across all K, Benchmark (orbit `--stream`, single continuous call, no forced reinit), Candidate implemented, Open question for the user, `ra_hash`/`ra_reseed` (+4 more)

### Community 71 - "BuildPyWithCEngine"
Cohesion: 0.17
Nodes (12): Closing note, Continuous-stream, sesi/mesin yang sama (hari ini), Deviasi dari rencana: sanity-check tambahan, Diskusi, Hasil, Kesimpulan, Konteks, Metodologi (+4 more)

### Community 72 - "2025-10-5_scramble-design/scc_test.py"
Cohesion: 0.20
Nodes (7): _build_py, Distribution, BinaryDistribution, BuildPyWithCEngine, Imperative build customization pyproject.toml's static [tool.setuptools] table…, After the normal .py copy step, compiles the ra_prng2 C engine and drops…, Declares this distribution as containing compiled, platform-specific code…

### Community 73 - "Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 74 - "pruned_prng.c"
Cohesion: 0.38
Nodes (10): build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32(), FILE, has() (+2 more)

### Community 75 - "Cycle 1 Initial State Avalanche Fraction (0.487783)"
Cohesion: 0.29
Nodes (11): Avalanche Metrics (overall=0.487783, min_bit=0.472304), Warmup Cycle Avalanche Effect and Bit Diffusion, Cycle 1 Initial State Avalanche Fraction (0.487783), Cycle 3 Converged Avalanche Fraction (0.500751), Winner Wired Avalanche Heatmap Cycle 1 vs Cycle 3, Winner Wired Wiring Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a), Cycle 4 Steady State Avalanche (fraction 0.499713), Avalanche Effect Heatmap Comparison: Cycle 1 vs Cycle 4 (+3 more)

### Community 76 - "Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)"
Cohesion: 0.18
Nodes (10): 1. Summary Comparison: Unpruned Original vs `winner_wired_v2`, 2. Sebaran Siklus Utama (*Distinct $\lambda$ Distribution*), 3. Enumerasi Penuh & Uji Chi-Square vs Poisson(1), 4. Analisis Temuan Kunci (*Key Findings*), 5. Catatan Metodologi & Caveats (Sesuai `HANDOVER.md`), 6. Kesimpulan (*Verdict*), A. Skala $n=2, w=4$ ($|S| = 131.072$) — [`enumerate_n2w4_result.json`](./enumerate_n2w4_result.json), B. Skala $n=4, w=4$ ($|S| = 402.653.184$) — [`enumerate_n4w4_result.json`](./enumerate_n4w4_result.json) (+2 more)

### Community 77 - "tahap5_benchmark.py"
Cohesion: 0.36
Nodes (10): find_crossover(), main(), _parse_rows(), Path, Tahap 5: orchestrator for tahap5_bench / tahap5_bench_zmm512 /…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep() (+2 more)

### Community 78 - "tahap6_benchmark.py"
Cohesion: 0.33
Nodes (10): find_crossover(), main(), _parse_rows(), Tahap 6: orchestrator for tahap6_bench (C harness does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep(), run_throughput() (+2 more)

### Community 79 - "2026-8-30_addressable-shuffle/scc_test.py"
Cohesion: 0.25
Nodes (9): chi_square_uniform(), entropy(), multi_key_distinctness_check(), Shuffle a fixed-size sample of input_tokens with n_keys different random keys;…, read_tokens(), runs_test(), serial_correlation(), multi_key_distinctness_check() (+1 more)

### Community 80 - "RESULTS: key=0 degenerate-state guard-XOR fix"
Cohesion: 0.18
Nodes (11): 1. Status matrix ringkas, 2. Blocker utama: singleblock K-small structural defect, 2 kandidat fix co-frontier (commit `d2f1675`, hari ini), 3. Action item yang belum dikerjakan (blocking promosi), 4. Selesai/tertutup sesi ini (2026-09-03) — jangan ditelusuri ulang, 5. Item terbuka lain, prioritas lebih rendah — TIDAK memblokir gate korektnes, 6. Eksplisit di luar scope gate ini (track terpisah), 7. Referensi cepat (+3 more)

### Community 81 - "RESULTS: Priority 1 — Init-loop speed optimization"
Cohesion: 0.18
Nodes (11): 1. Diagnostic candidate — `diag_keyzero_guard.c`, 2. Edge-case check — PASS, 3. BCFN non-regression — K=255 multikey pattern: PASS, 32GB clean, 4. Non-regression — `ra_core validate` after applying to `ra_core.c`, Applied to `ra_core.c`, CRITICAL — separate, pre-existing defect found: K=1 singleblock chaining catastrophically FAILs PractRand, Guard constants, RESULTS: key=0 degenerate-state guard-XOR fix (+3 more)

### Community 82 - "ra_prng (Array-Based PRNG Architecture)"
Cohesion: 0.18
Nodes (11): Critical files to reference, Deliverables, Goal -- three-tier strategy, in priority order, HANDOVER: Init-loop speed optimization untuk `ra_core_orbit` / `ra_core_singleblock`, Non-goals / hard constraints, Priority 1 (try first): find redundant/reducible operations with ZERO output change, Priority 2 (if P1 fails to help significantly): search for a new init formula (output allowed to change), Priority 3 (if both P1 and P2 fail): re-evaluate the tradeoff (+3 more)

### Community 83 - "Handover: eliminasi 11 pemenang combo-search via Pareto (speed x avalanche), lalu posisikan vs keluarga PRNG lain"
Cohesion: 0.20
Nodes (11): Addressable / Orbit-style Initialization Research, benchmarks/comparisons (throughput/entropy comparisons), ChaCha20, /dev/urandom, experiments/ (active research), ICCS2026 paper: Array-Native Randomness, MT19937, PCG32 (+3 more)

### Community 84 - "tahap3_collision_scan.py"
Cohesion: 0.27
Nodes (8): collision_scan.py, main(), Non-regression check for `ra_init_orbit` (the multikey mix-init fix candidate,…, run_sequential(), main(), Tahap 3, Q2 (re-pointed): cross-key collision/overlap scan for…, run_random(), run_sequential()

### Community 85 - "Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`"
Cohesion: 0.38
Nodes (9): Pareto Frontier Selection (3 objectives), FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo() (+1 more)

### Community 86 - "quality_gate.py"
Cohesion: 0.31
Nodes (9): avalanche_stats(), hamming(), Generic per-bit avalanche measurement: flip each of the 32 seed bits, capture…, capture_cycle(), evaluate_candidate(), main(), per_seed_stats(), Path (+1 more)

### Community 87 - "2026-8-28_periodicity-heuristic-validation2/run_sweep.py"
Cohesion: 0.33
Nodes (9): cycle_measure Binary Exit-42 Crash at n=8,w=4,rows=4, log2_fact(), main(), predicted(), Drives the pruned/wired periodicity-heuristic sweep. Same lambda_pred formula…, result_csv_name(), run_c(), run_python() (+1 more)

### Community 88 - "interleave_practrand.py"
Cohesion: 0.36
Nodes (9): Baseline decision: winner_wired_addressable.c (portable), not _v2.c, FILE, main(), ra_core(), ra_hash(), ra_init_state_addressable(), ra_permutation_cycle(), ra_reseed() (+1 more)

### Community 89 - "tahap4_benchmark.py"
Cohesion: 0.36
Nodes (9): find_crossover(), main(), _parse_rows(), Tahap 4: orchestrator for tahap4_bench (C harness, does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word., run_init_cost(), run_reinit_sweep(), run_throughput() (+1 more)

### Community 90 - "scrambler_addressable.c"
Cohesion: 0.38
Nodes (9): main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 91 - "scrambler_wired_addressable.c"
Cohesion: 0.38
Nodes (9): main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 92 - "single_mread_isolation_test.c"
Cohesion: 0.38
Nodes (9): Options, load_tokens_from_file(), main(), parse_args(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32() (+1 more)

### Community 93 - "c/ra_prng2.c"
Cohesion: 0.24
Nodes (10): chacha20_block(), kat_chacha20(), kat_pcg32(), kat_splitmix64(), kat_xoshiro256(), ra_core_chacha20(), ra_core_xoshiro256(), run_verify_all() (+2 more)

### Community 94 - "diag_hashed_init.c"
Cohesion: 0.40
Nodes (9): FILE, main(), ra_core_diag(), ra_hash(), ra_init_state_hashed_key(), ra_permutation_cycle(), ra_reseed(), rot32() (+1 more)

### Community 95 - "diag_orbit_regression.c"
Cohesion: 0.40
Nodes (9): FILE, fmix32(), main(), ra_core_orbit_stream(), ra_hash(), ra_init_orbit(), ra_permutation_cycle_orbit(), ra_reseed() (+1 more)

### Community 96 - "src/chacha20.c"
Cohesion: 0.20
Nodes (10): 1. Konteks & tujuan, 2. Axis A — Speed: K=1, K=255, continuous-stream, 3. Axis B — Avalanche multi-seed + uji normalitas, 4. Pareto frontier selection, 5. Perbandingan vs keluarga PRNG lain — REUSE tabel lama, JANGAN tes ulang, 6. File yang TIDAK BOLEH diubah (read-only precedent), 7. Struktur file baru (folder ini), 8. Non-goals (+2 more)

### Community 97 - "pcg_amortized.c"
Cohesion: 0.44
Nodes (8): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), pcg32_buffered_t

### Community 98 - "source/ra_prng2.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 99 - "source/ra_prng3.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 100 - "scrambler.c"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 101 - "2026-8-25_periodicity-heuristic-validation/avalanche_check.py"
Cohesion: 0.31
Nodes (8): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for toy_prng.py. Port of…, One outer iteration, no rehash - mirrors zepfold_capture() in…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), Rotate the low w bits of x left by r bits (mod w)., rotw()

### Community 102 - "Tahap 2: Folder/Battery Validasi Produksi"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 103 - "pruned_winner.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 104 - "pruned_winner_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 105 - "avalanche_gate"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 106 - "baseline.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 107 - "baseline_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 108 - "v01.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 109 - "v02.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 110 - "v03.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 111 - "v04.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 112 - "v06.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 113 - "v07.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 114 - "v08.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 115 - "v09.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 116 - "v10.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 117 - "v11.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 118 - "v12.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 119 - "v13.c"
Cohesion: 0.39
Nodes (8): decode_batch(), main(), pack_key(), Full state-space enumeration for pruned/wired PRNG at n=4,w=4 (default rows=4…, Cross-check vec_next_state against the scalar reference on random states drawn…, rotw_vec(), validate(), vec_next_state()

### Community 120 - "winner_wired.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 121 - "2026-8-28_periodicity-heuristic-validation2/enumerate_n4w4.py"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 122 - "measure_state_update.py"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 123 - "original_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 124 - "original_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 125 - "original_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 126 - "original_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 127 - "original_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 128 - "original_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 129 - "winner_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 130 - "winner_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 131 - "winner_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 132 - "winner_m_permute.c"
Cohesion: 0.22
Nodes (8): 1. Status & apa yang sudah terjadi (2026-08-31), 2. Pertanyaan yang belum terjawab, 3. Rencana diagnostik (belum dikerjakan, urutan disarankan), 4. Setelah selesai, Handover: 1TB interleaved-PractRand anomaly follow-up, Langkah 1 — Single-stream `winner_wired_v2` ke 1TB (tanpa interleave), Langkah 2 — Re-run interleaved 1TB dengan seed set berbeda, Langkah 3 — Sintesis & keputusan

### Community 133 - "winner_overwrite.c"
Cohesion: 0.22
Nodes (8): 1. Ringkasan alur & tujuan, 2. File & perannya (peta folder `experiments/2026-8-30_addressable-shuffle/`), 3. Hasil kunci (sudah final, jangan diulang), 4. Reproduksi cepat, 5. Constraint yang diwarisi, 6. Follow-up: SELESAI (2026-08-30, sesi lanjutan), 7. Setelah mengerjakan follow-up di atas, Handover: Addressable-Init Data Shuffling & Speed Investigation

### Community 134 - "winner_permute.c"
Cohesion: 0.22
Nodes (9): Addressable-Init Data Shuffling Results, 1. Smoke test, 2. Parity check (`verify_parity.py`), 3. Statistik kualitas shuffle (`scc_test.py`, key=1, n=100.000), 4. Multi-key distinctness spot-check (`scc_test.py`), Apa ini, Build & run, Follow-up (belum dilakukan sekarang) (+1 more)

### Community 135 - "Handover: 1TB interleaved-PractRand anomaly follow-up"
Cohesion: 0.42
Nodes (8): load_token_ids_from_file(), main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Byte-for-byte port of winner_wired_addressable.c's function of the same name…, save_token_ids_to_file()

### Community 136 - "benchmarks/comparisons (throughput/entropy comparisons)"
Cohesion: 0.42
Nodes (8): load_token_ids_from_file(), main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Byte-for-byte port of winner_wired_addressable.c's function of the same name…, save_token_ids_to_file()

### Community 137 - "Handover: Addressable-Init Data Shuffling & Speed Investigation"
Cohesion: 0.42
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), ra_init_state_addressable(), ra_shuffle(), rot32(), save_tokens_to_file()

### Community 138 - "Hasil validasi"
Cohesion: 0.22
Nodes (8): Catatan scope eksplisit, File yang dihasilkan sesi ini, Keputusan penamaan & scope (dikonfirmasi user, sesi ini), Non-goals (tidak dikerjakan sesi ini, sesuai HANDOVER §3), RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock), Status, Struktur `ra_core.c`, Validasi

### Community 139 - "scrambler_addressable.py"
Cohesion: 0.22
Nodes (9): 1. Avalanche (`avalanche_ra_core.py`, reuse `quality_gate.py`'s `avalanche_gate_min_bit()`), 2. Speed (`bench_ra_core.c`, pola `tahap6_bench.c`), 3. Cross-correlation, multi-key (`cross_correlation_ra_core_singleblock.py`, tier "full": K=512, n=255), 4. Collision-scan, multi-key (`collision_scan_ra_core_singleblock.py`, tier "full": M=50.000, satu blok penuh 255-kata per key), 5. Dieharder "Good" battery, stream multi-key (`multikey_stream` + `run_dieharder_battery.py` reused), 6. PractRand bertahap, stream multi-key (`multikey_stream` -> `RNG_test stdin32`), `ra_core_orbit`: SEMUA axis terwarisi, tidak ada pekerjaan baru, `ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi (+1 more)

### Community 140 - "scrambler_wired_addressable.py"
Cohesion: 0.50
Nodes (8): core_avalanche(), core_baseline(), fmix32(), main(), ra_cycle(), ra_init_state_avalanche(), ra_init_state_orbit(), rot32()

### Community 141 - "no_reseed_isolation_test.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_keyterm_mul(), fmix32(), main(), ra_cycle(), ra_init_state_keyterm_mul(), ra_init_state_orbit(), rot32()

### Community 142 - "Handover: PRNG family benchmark (ra_prng semua varian vs PRNG terkenal)"
Cohesion: 0.50
Nodes (8): core_baseline(), core_warmup(), main(), ra_cycle(), ra_hash(), ra_init_state_orbit(), ra_reseed(), rot32()

### Community 143 - "RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)"
Cohesion: 0.50
Nodes (8): core_baseline(), core_warmup(), main(), ra_cycle(), ra_hash(), ra_init_state_orbit(), ra_reseed(), rot32()

### Community 144 - "bench_init_avalanche.c"
Cohesion: 0.47
Nodes (8): FILE, fmix32(), main(), ra_core_orbit(), ra_init_orbit(), ra_init_singleblock(), ra_permutation_cycle_orbit(), rot32()

### Community 145 - "bench_init_keyterm_mul.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_diag(), ra_hash(), ra_init_state_addressable(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 146 - "bench_warmup_cost.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_warmup(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed(), rot32()

### Community 147 - "bench_warmup_cost2.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_warmup(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed(), rot32()

### Community 148 - "diag_init_keyterm_mul.c"
Cohesion: 0.22
Nodes (9): Candidates tried so far, Context: where this came from, Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey), Non-goals, Non-regression verification results (2026-09-01) -- verification bar item 2 DONE, Process note (per explicit user feedback this session), Root cause (conclusively diagnosed this session — do not re-litigate), The task: find something cheaper than a full/partial warm-up cycle (+1 more)

### Community 149 - "diag_paperlike_cycle.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 150 - "diag_warmup_depth.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 151 - "diag_warmup_discard.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 152 - "2026-9-1_production-candidate-battery/RESULTS.md"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 153 - "w2_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 154 - "w2_f10_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 155 - "w2_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 156 - "w2_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 157 - "w2_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 158 - "w2_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 159 - "w2_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 160 - "w2_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 161 - "w2_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 162 - "w2_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 163 - "w2_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 164 - "w2_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 165 - "w2_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 166 - "w2_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 167 - "w2_f22_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 168 - "w2_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 169 - "w2_f24_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 170 - "w2_f25_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 171 - "w2_f26_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 172 - "w2_f27_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 173 - "w2_f28_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 174 - "w2_f29_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 175 - "w2_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 176 - "w2_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 177 - "w2_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 178 - "w2_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 179 - "w2_f33_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 180 - "w2_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 181 - "w2_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 182 - "w2_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 183 - "w2_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 184 - "w2_f5_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 185 - "w2_f6_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 186 - "w2_f7_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 187 - "w2_f8_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 188 - "w2_f9_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 189 - "w4_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 190 - "w4_f10_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 191 - "w4_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 192 - "w4_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 193 - "w4_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 194 - "w4_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 195 - "w4_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 196 - "w4_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 197 - "w4_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 198 - "w4_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 199 - "w4_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 200 - "w4_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 201 - "w4_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 202 - "w4_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 203 - "w4_f22_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 204 - "w4_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 205 - "w4_f24_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 206 - "w4_f25_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 207 - "w4_f26_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 208 - "w4_f27_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 209 - "w4_f28_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 210 - "w4_f29_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 211 - "w4_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 212 - "w4_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 213 - "w4_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 214 - "w4_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 215 - "w4_f33_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 216 - "w4_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 217 - "w4_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 218 - "w4_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 219 - "w4_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 220 - "w4_f5_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 221 - "w4_f6_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 222 - "w4_f7_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 223 - "w4_f8_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 224 - "w4_f9_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 225 - "w8_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 226 - "2026-9-3_init-loop-optimization/HANDOVER.md"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 227 - "w8_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 228 - "w8_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 229 - "w8_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 230 - "w8_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 231 - "w8_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 232 - "w8_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 233 - "w8_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 234 - "w8_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 235 - "w8_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 236 - "w8_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 237 - "w8_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 238 - "w8_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 239 - "Axis B - Avalanche Multi-seed + Normality"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 240 - "w8_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 241 - "Orbit Continuous-Stream Measurement (376.2 MB/s)"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 242 - "cross_correlation_ra_core_singleblock.py"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 243 - "ra_core_singleblock (algorithm)"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 244 - "Finding: MT19937 seeding cost dominates frequent-reseed workloads"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 245 - "4. Kandidat optimasi, berperingkat"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 246 - "w8_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 247 - "w8_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 248 - "w8_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 249 - "w8_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 250 - "avalanche_ra_core.py"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 251 - "w8_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 252 - "w8_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 253 - "w8_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 254 - "w8_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 255 - "w8_f5_i0.c"
Cohesion: 0.22
Nodes (9): Cara lanjut di sesi berikutnya, Context, HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup, Ide fix dari user (2 tahap, verbatim diparafrase), Kandidat A — perlebar `o` ke 8-tap (isolasi 1 variabel), Kandidat B (fallback, hanya jalan jika Kandidat A gagal/belum cukup), Non-goals (eksplisit), Rencana eksekusi (belum dijalankan) (+1 more)

### Community 256 - "w8_f6_i0.c"
Cohesion: 0.44
Nodes (8): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), ra_step(), rot32()

### Community 257 - "w8_f7_i0.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 258 - "ra_prng Implementation Variant Roles"
Cohesion: 0.25
Nodes (8): Backend-bound execution stalls (K=255 singleblock_accum regression), BCFN-style PractRand defect pattern, singleblock_accum core, perf stat topdown microarchitecture analysis, Execution-port contention hypothesis (unconfirmed), Production Candidate Battery Gate, Safe-K floor formula (K>=80-96), Singleblock K-small structural defect

### Community 259 - "HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup"
Cohesion: 0.43
Nodes (8): RNGing perf stat (ra_prng2 original), Shuffling perf stat (ra_prng2 original), RNGing perf stat (ra_prng3), Scrambling perf stat (ra_prng3), Linux perf stat Tool, Scrambler Tool, tokens.txt Input Corpus, scrambled.txt (scramble-design experiment output)

### Community 260 - "ra_prng2_struct.c"
Cohesion: 0.36
Nodes (8): CPython Binding Documentation, Orphaned CPython Shared Object Rationale, RaPrng2 ctypes Wrapper, ra_prng2 API Reference, ra_core PRNG Core Loop, ra_hash State Reduction Function, RA_PRNG C Struct API, rot32 Left Bitwise Rotation

### Community 261 - "ra_prng2_thread.c"
Cohesion: 0.43
Nodes (7): build(), gen_c(), main(), Path, Generate + compile the phase-1 (L-only) state-update-mechanism spectrum…, The "permute" mechanism must reproduce its target's real source exactly (it's…, verify_control_bit_identical()

### Community 262 - "Shuffling perf stat (ra_prng2 original)"
Cohesion: 0.39
Nodes (7): build(), gen_c(), main(), Path, Generate + compile the phase-2 (M-only) state-update-mechanism spectrum, per…, The M mechanism only affects the reseed fold (once per 255 steps) -- the first…, verify_first_cycle_matches_control()

### Community 263 - "ra_prng2 API Reference"
Cohesion: 0.32
Nodes (7): parallelization-research infra (referenced), Devil's-Advocate Session (challenged parallelization research value), Tahap 3: Statistical Validation, build_group_addr(), main(), Tahap 3, Q1 Method A (re-pointed): Pearson cross-correlation between pairs of…, run()

### Community 264 - "gen_variants.py"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_avalanche_init(), ra_init_state_avalanche(), ra_permutation_cycle_orbit(), rot32()

### Community 265 - "gen_variants_m.py"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_keyterm_add(), ra_init_state_keyterm_add(), ra_permutation_cycle_orbit(), rot32()

### Community 266 - "tahap3_cross_correlation.py"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_keyterm_xor(), ra_init_state_keyterm_xor(), ra_permutation_cycle_orbit(), rot32()

### Community 267 - "Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche"
Cohesion: 0.29
Nodes (7): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result

### Community 268 - "other_winners_gen.py"
Cohesion: 0.43
Nodes (6): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), Candidate: pcg32, Official PCG reference (O'Neill, pcg-random.org)

### Community 269 - "diag_init_avalanche.c"
Cohesion: 0.43
Nodes (6): main(), rotl(), xoshiro256_init(), xoshiro256_next(), Candidate: xoshiro256**, Official xoshiro256** reference (Blackman & Vigna)

### Community 270 - "diag_init_keyterm_avalanche.c"
Cohesion: 0.52
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next()

### Community 271 - "diag_init_keyterm_xor.c"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 272 - "HANDOVER_TAHAP6.md"
Cohesion: 0.48
Nodes (6): log2_fact(), main(), predicted(), Drives the full periodicity-heuristic-validation sweep. For each measurable (n,…, run_c(), run_python()

### Community 273 - "ent (Entropy Testing Tool)"
Cohesion: 0.38
Nodes (6): gen_c(), main(), Path, Generate + compile hardcoded C binaries for the 12 non-selected winning wirings…, Compare the compiled binary's --stream output against wired_prng.py's stream()…, verify_bit_identical()

### Community 274 - "Avalanche Check Validation"
Cohesion: 0.38
Nodes (6): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for pruned_wired_toy_prng.py.…, One outer iteration, no rehash -- mirrors next_state's inner loop exactly (tap-…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check()

### Community 275 - "pcg32.c"
Cohesion: 0.48
Nodes (6): main(), rot32_avx2(), rot32_scalar(), run_scalar(), run_simd(), __m256i

### Community 276 - "src/xoshiro256.c"
Cohesion: 0.29
Nodes (6): Context, Files produced this session, Interpretation, Method, Results, RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`

### Community 277 - "xoshiro256_amortized.c"
Cohesion: 0.52
Nodes (6): capture_cycle(), compute_matrix(), main(), plot_single(), Path, Avalanche heatmap (K=255, full single-key cycle) for the 11 Promotion Tier…

### Community 278 - "Orbit KAT Verification"
Cohesion: 0.38
Nodes (6): last_word(), main(), Path, Byte-identical + KAT verification for orbit_bench.c's extern-linked…, # NOTE: for orbit, requesting rng=n does NOT guarantee exactly n words are, stream_bytes()

### Community 279 - "scrambler.py"
Cohesion: 0.29
Nodes (7): B. Harris, Donald E. Knuth, Makoto Matsumoto and Takuji Nishimura, Mersenne Twister (MT19937), PRNG Periodicity Analysis, Pierre L'Ecuyer, Random Mapping Theorem

### Community 280 - "2026-8-25_periodicity-heuristic-validation/run_sweep.py"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 281 - "2026-8-28_periodicity-heuristic-validation2/avalanche_check.py"
Cohesion: 0.47
Nodes (4): chacha20_block(), main(), rand_bounded(), Speed Benchmark Output (chacha20 amortized)

### Community 282 - "simd_prototype.c"
Cohesion: 0.67
Nodes (5): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded()

### Community 283 - "Speed Benchmark: `scrambler_addressable` vs Algoritma Lain"
Cohesion: 0.60
Nodes (5): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 284 - "RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`"
Cohesion: 0.40
Nodes (6): init_kandidat1(), init_kandidat2(), HANDOVER.md Kandidat 1: only L keyed by (key, counter) via splitmix64; M kept…, HANDOVER.md Kandidat 2: L and M independently keyed, domain-separated., splitmix64_next(), splitmix64_seed()

### Community 285 - "quality_gate.py (avalanche_stats)"
Cohesion: 0.47
Nodes (6): core_entry_t, find_core(), main(), mode_reinit_sweep(), mode_urandom_flat(), now_seconds()

### Community 286 - "verify_orbit_kat.py"
Cohesion: 0.60
Nodes (5): main(), Path, Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"- reliability…, run_one(), run_test()

### Community 287 - "PRNG Periodicity Analysis"
Cohesion: 0.60
Nodes (5): main(), Path, Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"- reliability…, run_one(), run_test()

### Community 288 - "promotion_search.py"
Cohesion: 0.60
Nodes (5): compile_new(), compile_old(), main(), run_stream_new(), run_stream_old()

### Community 289 - "source/chacha20.c"
Cohesion: 0.33
Nodes (3): Pure-Python unit tests for ra_prng2.ra_hash / ra_core. These do not require the…, Regression test for the historical bug: ra_hash must be a pure function writing…, test_ra_hash_does_not_mutate_input()

### Community 290 - "chacha20_amortized.c"
Cohesion: 0.40
Nodes (5): Reseed-Driven Avalanche Strengthening Effect, Initial State Avalanche Fraction (Cycle 1: 0.418459), Post-Reseed Avalanche Fraction (Cycle 2: 0.468409), Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2), Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)

### Community 291 - "pcg.c"
Cohesion: 0.60
Nodes (4): avalanche_gate(), main(), Post-convergence "shift repair" search. User's request after seeing the winning…, score()

### Community 292 - "source/xoshiro256.c"
Cohesion: 0.60
Nodes (4): find_pid(), get_status(), main(), Monitors .cycle_measure_ckpt_n8_w4_rows4_seed0.bin and stops cycle_measure when…

### Community 293 - "init_kandidat1"
Cohesion: 0.50
Nodes (4): check_no_repeat(), main(), Path, Full-scale (real 256x32-bit) practical periodicity sanity check, per…

### Community 294 - "Orbit Addressing Papers"
Cohesion: 0.40
Nodes (5): Orbit (deterministic trajectory selected by key), Orbit Addressing, Point Addressing (Philox f(counter,key)), CSAI 2025 RA-PRNG Paper, ICCS 2026 RA-PRNG Paper

### Community 295 - "run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): kat_mt19937(), mt19937_genrand_int32(), mt19937_init_genrand(), ra_core_mt19937(), mt19937_state_t

### Community 296 - "run_dieharder_battery_multikey.py"
Cohesion: 0.60
Nodes (4): capture_cycle(), main(), Tahap 2 (family-productionization): avalanche gate for ra_core.c's two…, run_gate()

### Community 297 - "verify_unification.py"
Cohesion: 0.60
Nodes (4): capture_cycle(), main(), production-candidate-battery, Step 1: avalanche gate for `orbit` and…, run_gate()

### Community 298 - "test_ra_hash.py"
Cohesion: 0.70
Nodes (4): fisher_yates_k1(), main(), pull_n(), pull_one()

### Community 299 - "Cycle Combo Avalanche Heatmap"
Cohesion: 0.40
Nodes (5): Avalanche heatmap w8_f8_i0 K=255 (full cycle), Avalanche heatmap w8_f9_i0 K=255 (full cycle), Avalanche test method: seed-bit-flip vs. output-word bit-difference heatmap, Cycle-op combo candidate w8_f8_i0 (tap width=8, finalizer f8, i0), Candidate w8_f9_i0 (tap width=8, finalizer c^=c>>16, cycle-op combo)

### Community 300 - "auto_stop_power38.py"
Cohesion: 0.50
Nodes (4): main(), Path, Byte-identical verification for diag_init_candidates.c's four cores…, stream_bytes()

### Community 301 - "full_scale_sanity_check.py"
Cohesion: 0.40
Nodes (5): Deliverable, Deviasi dari rencana (didokumentasikan), HANDOVER: Continuous-stream real benchmark untuk `ra_core_orbit`, Konteks, Rencana implementasi (plan yang disetujui)

### Community 302 - "Cycle1 vs Cycle2 Avalanche"
Cohesion: 0.50
Nodes (5): Avalanche Effect Analysis (ALL_OPS), Bit Difference Distribution Across 32 Flipped Bits, Cycle 1 Avalanche Fraction (0.497576), Cycle 2 Avalanche Fraction (0.502141), Avalanche Effect Heatmap (Cycle 1 vs Cycle 2)

### Community 303 - "Orbit Addressing"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 304 - "ra_prng2 Python Reference"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 305 - "ra_prng3 C Variant"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 306 - "scrambler_ra_core_singleblock.c"
Cohesion: 0.83
Nodes (4): Cycle 1 Avalanche Fraction (0.418459), Cycle 3 Avalanche Fraction (0.470588), Avalanche Heatmap: Cycle 1 vs Cycle 3 (pruned_winner), Reseed Avalanche Diffusion in Pruned Winner

### Community 307 - "Periodicity Heuristic Question"
Cohesion: 0.50
Nodes (4): Paper heuristic λ ≈ 0.7824·√|S|, Research Question: does pruned/rewired algorithm shift the heuristic's λ-vs-λ_pred fit, Central Research Question (RESULTS.md), Verdict 2: heuristic behaves as conservative lower bound at large-scale configs only

### Community 308 - "Avalanche heatmap w8_f8_i0 K=255 (full cycle)"
Cohesion: 0.50
Nodes (4): Tap shift-width collapse (e>=w yields identically-zero term), Tap-survivor generalization (top-2-by-count rule, G=8→(6,7)), Finding: non-vacuous tap pruning (G=4) gives closest fit to heuristic, Caveat: tap pruning is a no-op at G<=2 configs

### Community 309 - "Avalanche Effect Analysis (ALL_OPS)"
Cohesion: 0.67
Nodes (4): Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche, Kandidat 3 (negative control: L rotated only, multiset unchanged), Kandidat 5 (user's rot32-based L/M formula, winning candidate), L_MIX_CONST (frozen to 0x9e3779b7)

### Community 310 - "heatmap.py"
Cohesion: 0.50
Nodes (4): JAX, Philox (counter-mode PRNG), Tahap 4: speed break-even benchmark vs Philox, Threefry2x32

### Community 312 - "c/ra_prng3.c"
Cohesion: 0.67
Nodes (3): main(), Continuous-stream (real, syscall-inclusive) MB/s for ra_core_orbit via…, run_continuous_stream()

### Community 313 - "Cycle 3 Avalanche Fraction (0.470588)"
Cohesion: 0.67
Nodes (4): Avalanche Effect, Avalanche Effect Heatmap (Original Algorithm), Hamming Distance Analysis, Original PRNG Algorithm

### Community 314 - "Paper heuristic λ ≈ 0.7824·√|S|"
Cohesion: 0.67
Nodes (3): Graphify Query & Navigation Protocol, Graphify Knowledge Graph Rule, Graphify Pipeline Workflow

### Community 316 - "CLAUDE.md Graphify Workflow"
Cohesion: 0.67
Nodes (3): graphify-out/graph.json Knowledge Graph Artifact, graphify Skill, Graphify-First Research Workflow Policy

### Community 317 - "robust_reinit.py"
Cohesion: 1.00
Nodes (3): Cycle 1 vs Cycle 5 Avalanche Fraction Convergence, Avalanche Heatmap: Pruned Winner Cycle 1 vs Cycle 5, Pruned Winner Architecture (TAP6, TAP7, ROT_C, SHR13)

### Community 318 - "Avalanche Effect Heatmap (Original Algorithm)"
Cohesion: 1.00
Nodes (3): Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13), pruned_winner Cycle 1 Avalanche Sensitivity and Dead Zones, pruned_winner 4-Op Candidate Configuration (TAP6, TAP7, ROT_C, SHR13)

### Community 319 - "Graphify Knowledge Graph Rule"
Cohesion: 1.00
Nodes (3): Greedy Operation-Pruning Search Paradigm, Operation-Pruning Research Handover Spec, Limits of Zero-Drift Micro-Optimizations on ra_core

### Community 320 - "dev_urandom.c"
Cohesion: 0.67
Nodes (3): collision_scan.py, Q2: Cross-stream collision probability, ProcessPoolExecutor deadlock -> ThreadPoolExecutor+lock fix

### Community 321 - "Graphify-First Research Workflow Policy"
Cohesion: 0.67
Nodes (3): perf_scaling.sh, Q3: Multi-process throughput scalability, wall_time_scaling.py

### Community 322 - "Cycle 1 vs Cycle 5 Avalanche Fraction Convergence"
Cohesion: 0.67
Nodes (3): out_pcg.txt (output artifact of pcg speed-benchmark run), out_pcg_amortized.txt (output artifact of pcg amortized speed-benchmark run), pcg.c (PCG32 comparison binary)

### Community 323 - "Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)"
Cohesion: 0.67
Nodes (3): out_xoshiro256.txt (output artifact of xoshiro256 speed-benchmark run), out_xoshiro256_amortized.txt (output artifact of xoshiro256 amortized speed-benchmark run), xoshiro256.c (fread+tokenizer comparison binary)

### Community 329 - "run_speed_benchmark.sh"
Cohesion: 1.00
Nodes (3): Avalanche heatmap w8_f22_i0 K=255 (full cycle), Candidate w8_f22_i0 (tap width=8, finalizer f22, cycle-op combo search), Result: uniform avalanche (overall=0.5010, min_bit=0.4915), no visible weak bit/word structure

### Community 330 - "verify_parity.py"
Cohesion: 0.67
Nodes (3): fingerprinting_tools sub-project, image_encrypting sub-project, sub-projects/ (downstream applications)

## Ambiguous Edges - Review These
- `Orbit Continuous-Bench Results` → `RESULTS_TAHAP2.md (orbit K=1 historical figure)`  [AMBIGUOUS]
  experiments/2026-9-3_orbit-continuous-bench/RESULTS.md · relation: references
- `hamzy hams (GitHub maintainer)` → `Hamas A. Rahman (author)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to

## Knowledge Gaps
- **565 isolated node(s):** `Threefry2x32`, `operand_search.py (operation-pruning-research)`, `combo_search_log.jsonl`, `promotion_log.jsonl / promotion_logs/*.log`, `tier0_survivors.jsonl` (+560 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **106 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `Orbit Continuous-Bench Results` and `RESULTS_TAHAP2.md (orbit K=1 historical figure)`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `hamzy hams (GitHub maintainer)` and `Hamas A. Rahman (author)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `ra_core_singleblock Cycle-Operation Combo Search — RESULTS` connect `ra_prng2 (Array-Based PRNG)` to `winners_bench.c`, `Completed Tasks`, `tahap6_bench.c`, `scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)`, `RESULTS: production-candidate battery for `ra_core.c``, `ra_core.c`, `2026-8-25_periodicity-heuristic-validation/cycle_measure.c`, `2026-8-28_periodicity-heuristic-validation2/cycle_measure.c`, `ra_core_singleblock Cycle-Operation Combo Search — RESULTS`, `quality_gate.py`, `io_only_isolation_test.c`, `others/avalanche_heatmap_original.py`?**
  _High betweenness centrality (0.050) - this node is a cross-community bridge._
- **Why does `Production Readiness Handover` connect `Completed Tasks` to `Pareto Selection Results (Combo Winners)`, `toy_prng.py`, `winner_wired_v2.c`, `RESULTS: key=0 degenerate-state guard-XOR fix`, `State-Update Mechanism Research Results (Phase 1-3)`, `ra_core_singleblock Cycle-Operation Combo Search — RESULTS`, `recipes.py`?**
  _High betweenness centrality (0.039) - this node is a cross-community bridge._
- **Why does `avalanche_stats()` connect `quality_gate.py` to `tahap6_bench.c`, `pcg.c`, `bench_ra_core.c`, `Operand Position Search Experiment Results`, `ra_prng2 (Array-Based PRNG)`, `run_dieharder_battery_multikey.py`, `verify_unification.py`, `ra_prng3 PRNG Generator`, `xoshiro256_amortized.c`, ``ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi`, `Singleblock cycle-op combo search experiment (2026-09-02)`, `others/avalanche_heatmap_original.py`, `ra_core_singleblock`?**
  _High betweenness centrality (0.026) - this node is a cross-community bridge._
- **Are the 7 inferred relationships involving `Params` (e.g. with `capture_inner_outputs()` and `run_avalanche_check()`) actually correct?**
  _`Params` has 7 INFERRED edges - model-reasoned connections that need verification._
- **Are the 3 inferred relationships involving `ra_prng2 (32-bit generator, CSAI2025)` (e.g. with `ra_prng2 PractRand Test Result (up to 128 GB, no anomalies)` and `pruned_winner 4-Operation Fast Variant`) actually correct?**
  _`ra_prng2 (32-bit generator, CSAI2025)` has 3 INFERRED edges - model-reasoned connections that need verification._