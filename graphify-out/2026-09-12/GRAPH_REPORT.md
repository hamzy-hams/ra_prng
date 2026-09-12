# Graph Report - ra_prng  (2026-09-12)

## Corpus Check
- 723 files · ~23,692,573 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 6087 nodes · 10090 edges · 630 communities (491 shown, 139 thin omitted)
- Extraction: 98% EXTRACTED · 2% INFERRED · 0% AMBIGUOUS · INFERRED: 227 edges (avg confidence: 0.85)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `c148bfde`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- winners_bench.c
- avalanche_stats
- HANDOVER: uji BCFN pasca-batas-reseed untuk ra_reseed/ra_hash
- combo_gen_orbit.py
- Parallelization Research Results (2026-08-29)
- benchmark_all.c
- tahap4_bench.c
- ra_core_orbit
- ra_permutation_cycle_singleblock
- Completed Tasks
- tahap0_prototype.py
- RESULTS — State-Size / Speed Pareto Frontier (N∈{256,128,64,32,16})
- Handover: uji 2 kandidat fix tertarget (seed `d0` di fase init) untuk defect K=1+hashed pada `ra_core_orbit`
- c/ra_core.c
- stream
- ra_prng README
- tahap5_bench.c
- avalanche_heatmap_pruned_winner.py
- ra_core_orbit multi-cycle off-by-one bug
- combo_gen.py
- ra_prng2 (Array-Based PRNG)
- Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)
- PRNG Family Benchmark Results
- combo_prng_warmup.py
- diag_init_candidates.c
- 2026-9-4_orbit-init-cycle-combo-search/RESULTS.md
- singleblock-cycle-combo-search RESULTS.md
- ra_reseed
- ra_core_orbit
- Langkah 3: Interleaved 1TB for winner_wired_addressable (shared core-loop test)
- exhaustive_analysis.c
- winner_wired_v2.c
- tahap6_bench.c
- next_state
- Random
- ra_core_param.c
- cycle_analysis_weylfinal.py
- bench_release.c
- ra_core_v3.c
- 2026-9-4_orbit-kmin-battery/run_dieharder_battery.py
- HANDOVER — Refinement akhir `ra_core_singleblock`: difusi-di-akhir, optimasi, eksperimen kecil
- pruned_winner 4-Operation Fast Variant
- other/avalanche_heatmap_original.py
- ERRATA
- 2026-8-25_periodicity-heuristic-validation/cycle_measure.c
- Operand-Position Rewiring Results
- 2026-8-28_periodicity-heuristic-validation2/cycle_measure.c
- measure_state_update.py
- 64-bit Address Widening Handover
- stream_values
- cross_correlation.py
- interleave_practrand.py
- 2026-9-1_family-productionization/ra_core.c
- candidate_d.c
- ICCS 2026 Research Paper: Array-Native Randomness
- Dieharder Test Suite
- interleave_practrand.py
- ra_core_v2.c
- candidate_a.c
- candidate_c.c
- RESULTS: Priority 1 — Init-loop speed optimization
- State-Size Pareto Frontier Results
- winner_wired_addressable.c
- scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)
- Pareto Selection Results (Combo Winners)
- practrand_gate.py
- ra_core_engine.c
- ra_core_baseline.c
- combo_prng_ofmix.py
- avalanche_effect_analysis.py
- others/avalanche_heatmap_original.py
- pip-ra-core README
- API
- RA-PRNG
- ra_core (addressable/orbit-addressing successor)
- State-Update Mechanism Research Results (Phase 1-3)
- Params
- Candidate
- RESULTS: production-candidate battery for `ra_core.c`
- Singleblock Cycle-Operation Combo Search Handover
- combo_prng_d0raw.py
- test_ra_core.py
- avalanche.py
- RaPrng2
- Orbit SplitMix Finalizer Redesign Handover
- Parallelization Research Handover
- toy_prng.py
- tier0_quality.py
- stream_driven_shuffle.c
- benchmark_all_run.py
- diag_keyzero_guard.c
- ADDENDUM_POST_FIX_STATUS.md — Step 4-8, post `w8_f10_i0` promotion
- diag_wideo_warmup_singleblock.c
- combo_prng_admul.py
- RaCoreEngine
- RA-PRNG — Landing Page Pitch (Draft)
- random_seeds
- candidate_b.c
- candidate_e.c
- common.py (TIERS_Q2)
- Singleblock cycle-op combo search experiment (2026-09-02)
- Axis B - Avalanche Multi-seed + Normality
- avalanche_heatmap_winner.py
- avalanche_heatmap_winner_v2.py
- Handover: Tahap 5 — Optimasi Biaya Init Addressable
- io_only_isolation_test.c
- Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect
- ra_prng-family Productionization Handover
- K=1 Exhaustive Function Analysis Handover
- Fisher–Yates shuffle
- 2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py
- 2026-8-27_operand-position-search/baseline.c
- 4. Rencana implementasi
- microbench_orbit_vs_singleblock.c
- avalanche_ra_core.py
- workload_a_singleblock_pi.c
- diag_prereseed.c
- HANDOVER: Orbit K-kecil defect fix + rolling-register optimasi wide-o
- combo_prng_orbit.py
- bit_bias_analysis.py
- position_split_orbit.c
- ra_core/__init__.py
- BuildPyWithCEngine
- quality_gate.py (avalanche_stats)
- Avalanche Effect (Bit-Flip Sensitivity)
- 2026-8-28_periodicity-heuristic-validation2/run_sweep.py
- Design decisions (read before touching `pruned_wired_toy_prng.py`)
- Tahap 5 goal: reduce ra_init_state_addressable init cost (~49.87ns/call, 4.96x Philox) to push N*=49/K*=71 break-even lower, formula-preserving only (optimization, not formula redesign)
- ra_core_singleblock
- pareto_select.py
- Wiring
- control.c
- d0_hash.c
- d0_realround.c
- o_fmix_every_round.c
- o_fmix_init_only.c
- d0_raw_cons.c
- winner_wired.c
- RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)
- 2025-10-5_scramble-design/scc_test.py
- pruned_prng.c
- Cycle 1 Initial State Avalanche Fraction (0.487783)
- Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)
- tahap5_benchmark.py
- tahap6_benchmark.py
- RESULTS: key=0 degenerate-state guard-XOR fix
- combo_prng.py
- source/philox.c
- ad_mul_combine.c
- init_hashed_per_index__w8_f10_i0.c
- init_hashed_per_index__w8_f28_i0.c
- init_hashed_per_index__w8_f33_i0.c
- init_hashed_per_index__w8_f9_i0.c
- init_hashed_shared__w8_f10_i0.c
- init_hashed_shared__w8_f28_i0.c
- init_hashed_shared__w8_f33_i0.c
- init_hashed_shared__w8_f9_i0.c
- init_linear_p1_per_index__w8_f10_i0.c
- init_linear_p1_per_index__w8_f28_i0.c
- init_linear_p1_per_index__w8_f33_i0.c
- init_linear_p1_per_index__w8_f9_i0.c
- init_linear_p1_shared__w8_f10_i0.c
- init_linear_p1_shared__w8_f28_i0.c
- init_linear_p1_shared__w8_f33_i0.c
- init_linear_p1_shared__w8_f9_i0.c
- init_linear_per_index__w8_f10_i0.c
- init_linear_per_index__w8_f28_i0.c
- init_linear_per_index__w8_f33_i0.c
- init_linear_per_index__w8_f9_i0.c
- init_linear_shared__w8_f10_i0.c
- init_linear_shared__w8_f28_i0.c
- init_linear_shared__w8_f33_i0.c
- init_linear_shared__w8_f9_i0.c
- init_odd_per_index__w8_f10_i0.c
- init_odd_per_index__w8_f28_i0.c
- init_odd_per_index__w8_f33_i0.c
- init_odd_per_index__w8_f9_i0.c
- init_odd_shared__w8_f10_i0.c
- init_odd_shared__w8_f28_i0.c
- init_odd_shared__w8_f33_i0.c
- init_odd_shared__w8_f9_i0.c
- Hasil: search init-formula × cycle-transform untuk `ra_core_orbit` (dual key-ordering)
- tier1_gate_warmup.py
- multikey_stream_orbit_altconst.c
- bench_addr_hi.c
- tier1_gate_d0raw.py
- Handover: redesign arsitektur "Weyl-add + finalizer 32-bit murni" untuk defect K-small `ra_core_orbit`
- BuildPyWithCEngine
- tahap3_collision_scan.py
- orbit-round1-collision-position-isolation -- hasil (diagnosis only)
- w8_f25_i0.c
- tahap4_benchmark.py
- scrambler_addressable.c
- scrambler_wired_addressable.c
- single_mread_isolation_test.c
- diag_hashed_init.c
- diag_orbit_regression.c
- promotion_search.py
- Handover: eliminasi 11 pemenang combo-search via Pareto (speed x avalanche), lalu posisikan vs keluarga PRNG lain
- Hasil: kandidat fix `fmix32(o)` untuk defect `ra_core_orbit` K=1+hashed
- fpf_scan_addr_hi.c
- RESULTS: Weyl-add + fmix32/triple32 finalizer redesign — both candidates FAIL
- _engine.py
- pcg_amortized.c
- source/ra_prng2.c
- source/ra_prng3.c
- scrambler.c
- 2026-8-25_periodicity-heuristic-validation/avalanche_check.py
- Tahap 2: Folder/Battery Validasi Produksi
- pruned_winner.c
- pruned_winner_refactored.c
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
- simd_prototype.c
- Speed Benchmark: `scrambler_addressable` vs Algoritma Lain
- Handover: Addressable-Init Data Shuffling & Speed Investigation
- Hasil validasi
- scrambler_addressable.py
- scrambler_wired_addressable.py
- no_reseed_isolation_test.c
- period_precheck.py
- `ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi
- bench_init_avalanche.c
- bench_init_keyterm_mul.c
- bench_warmup_cost.c
- bench_warmup_cost2.c
- diag_init_keyterm_mul.c
- diag_paperlike_cycle.c
- diag_warmup_depth.c
- diag_warmup_discard.c
- 2026-9-2_singleblock-cycle-combo-search/candidates/w2_f0_i0.c
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
- w8_f10_i0.c
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
- w8_f22_i0.c
- w8_f23_i0.c
- w8_f24_i0.c
- w8_f26_i0.c
- w8_f27_i0.c
- w8_f28_i0.c
- w8_f29_i0.c
- w8_f2_i0.c
- w8_f30_i0.c
- w8_f31_i0.c
- w8_f32_i0.c
- w8_f33_i0.c
- w8_f34_i0.c
- w8_f35_i0.c
- w8_f3_i0.c
- w8_f4_i0.c
- w8_f5_i0.c
- w8_f6_i0.c
- w8_f7_i0.c
- RESULTS: continuous-stream real benchmark untuk `ra_core_orbit`
- w8_f9_i0.c
- BCFN/TMFn/FPF structural defect at small K
- HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup
- 2026-9-3_dieharder-inject-crossing/candidates/w2_f0_i0.c
- w8_f10_i1.c
- w8_f10_i2.c
- w8_f10_i3.c
- w8_f10_i4.c
- w8_f10_i5.c
- w8_f10_i6.c
- w8_f28_i1.c
- w8_f28_i2.c
- w8_f28_i3.c
- w8_f28_i4.c
- w8_f28_i5.c
- w8_f28_i6.c
- promotion_search_inject.py
- tier1_gate_ofmix.py
- Hasil: diagnosis resonansi-konstanta Weyl-stepping vs `ra_init_state_orbit`
- HANDOVER: karakterisasi ekshaustif fungsi round-1 K=1 (`ra_core_orbit`)
- ra_init_state_orbit
- tier1_gate_weylfinal.py
- Promosi Kandidat D ke `ra_core.c` kanonik — RESULTS
- ra_prng2_struct.c
- ra_prng2_thread.c
- Shuffling perf stat (ra_prng2 original)
- ra_prng2 API Reference
- gen_variants.py
- gen_variants_m.py
- State Update Mechanism Spectrum
- tahap3_cross_correlation.py
- Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`
- diag_init_avalanche.c
- diag_init_keyterm_avalanche.c
- diag_init_keyterm_xor.c
- tier1_gate_orbit.py
- c/ra_prng2.c
- 2. Inject-crossing (action item 2)
- staged_inject_screening.py
- 2026-9-6_release-benchmark-prep/avalanche_curve_ra_core.py
- ent (Entropy Testing Tool)
- pcg32.c
- src/xoshiro256.c
- xoshiro256_amortized.c
- ra_permutation_cycle_singleblock
- Cross-check against ra_core_orbit
- philox_amortized.c
- scrambler.py
- 2026-8-25_periodicity-heuristic-validation/run_sweep.py
- 2026-8-28_periodicity-heuristic-validation2/avalanche_check.py
- Temuan metodologi: benchmark reinit-K vs steady-state untuk orbit/singleblock
- RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`
- avalanche_heatmap_winners_k255.py
- Validasi `ra_core` dengan workload Monte Carlo nyata (untuk paper baru)
- avalanche_multiseed.py
- verify_orbit_kat.py
- src/chacha20.c
- orbit-d0-raw-cons-fix -- hasil
- source/chacha20.c
- chacha20_amortized.c
- pcg.c
- source/xoshiro256.c
- orbit-kmin-battery gate (multistream K=1/K=255 statistical re-validation)
- Avalanche Heatmap: singleblock N=16 (K=15 full block)
- 2026-9-1_dieharder-battery/run_dieharder_battery.py
- run_dieharder_battery_multikey.py
- verify_unification.py
- 2026-9-1_production-candidate-battery/run_dieharder_battery.py
- scramble_w8_f28_i0_lookonly.py
- 2026-9-3_dieharder-inject-crossing/run_dieharder_battery.py
- orbit-ad-mul-combine-fix -- hasil
- other_winners_gen.py
- run_altconst_practrand.py
- run_dieharder_battery_addr_hi.py
- Hasil: K=1 orbit dengan key dari `/dev/urandom` (proxy TRNG)
- Hasil: `o` sebagai XOR 8 word mentah (tanpa bit-shift per-tap)
- Hasil Workload A — Monte Carlo π estimation (niche `singleblock`)
- HANDOVER: baseline dual-core (orbit+singleblock) siap-edit
- weyl_fmix32.c
- weyl_triple32.c
- Promosi Kandidat D (64-bit address widening) ke `ra_core.c` kanonik — HANDOVER
- test_ra_hash.py
- auto_stop_power38.py
- full_scale_sanity_check.py
- avalanche_orbit_singleblock_k255.py
- multikey_stream (K=255 gate binary)
- score_shuffle.py
- scrambler_ra_core_singleblock.c
- Avalanche heatmap w8_f8_i0 K=255 (full cycle)
- Avalanche Effect Analysis (ALL_OPS)
- heatmap.py
- ra_prng2.py
- c/ra_prng3.c
- Avalanche Effect Curve: ra_core_orbit (K=255)
- Cycle 3 Avalanche Fraction (0.470588)
- 108-Wiring Exhaustive Search
- Paper heuristic λ ≈ 0.7824·√|S|
- Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))
- dieharder_battery_param.py
- robust_reinit.py
- recipes.py
- Address Widening 64-bit RESULTS.md
- Avalanche Effect Heatmap (Original Algorithm)
- Graphify Knowledge Graph Rule
- dev_urandom.c
- Graphify-First Research Workflow Policy
- Two-mode decision: orbit + singleblock only, no third fixed-init mode
- Cycle 1 vs Cycle 5 Avalanche Fraction Convergence
- Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)
- Greedy Operation-Pruning Search Paradigm
- Seed Bit 5/6 Avalanche-Dead Defect
- avalanche_3way_addr_hi.py
- collision_scan.py
- Q3: Multi-process throughput scalability
- pcg.c (PCG32 comparison binary)
- xoshiro256.c (fread+tokenizer comparison binary)
- run_speed_benchmark.sh
- tier1_sweep.sh
- tier2_run.sh
- exhaustive_word_count_check.py
- Avalanche heatmap w8_f22_i0 K=255 (full cycle)
- Validasi pola addr_hi-increment chaining untuk fitur shuffle pip-ra-core
- pick_guard_constant_d.py
- bench_init_only.c
- tools/battery/
- sub-projects/ (downstream applications)
- ra_prng2 NIST STS Test Result
- Bit-bias marginal (single-bit) diagnostic test
- collision_scan_ra_core_singleblock.py (K=255 collision-scan script)
- combo_gen_warmup.py (C codegen)
- combo_prng.py (Python oracle for combo-search candidates)
- verify_unification.py (bit-identical cross-check script)
- ra_prng Build and Test Workflow
- End-diffusion idea: add new diffusion stage near output instead of only at round start
- Original Algorithm Avalanche Bit Distribution & Diffusion
- perf_state_update.sh
- perf_state_update_m.sh
- toy_probe_phase3.sh
- kat_mt19937
- toy_sweep_m.sh
- toy_sweep_phase3.sh
- perf_scaling.sh
- Q4: SIMD vectorization microbenchmark (stretch goal)
- 2026-9-11_state-size-pareto-frontier/BUILD.sh
- bench_ra_core Throughput/Reinit-Sweep Results Log
- 2026-9-1_production-candidate-battery/BUILD.sh
- Avalanche heatmap K=255 full cycle: w8_f26_i0
- Cycle-op combo candidate w8_f29_i0
- All 11 combo-search winners show clean, uniform avalanche behavior at K=255 (full singleblock cycle)
- run_confirm_16gb.sh
- run_extended_16gb.sh
- run_warmup_triage.sh
- 2026-9-4_orbit-init-zerotap-fix/BUILD.sh
- 2026-9-4_orbit-reseed-postboundary-bcfn/BUILD.sh
- 2026-9-5_orbit-k1-exhaustive-function-analysis/BUILD.sh
- 2026-9-7_addr-hi-chaining-validation/BUILD.sh
- CI Build and Test Job
- conftest.py
- Addressable-Init Research (Tahap 0-5)
- /dev/urandom RNG Speed Benchmark Result
- PCG32 RNG Speed Benchmark Result
- Philox4x32 RNG Speed Benchmark Result
- xoshiro256** RNG Speed Benchmark Result
- Scrambling Speed Benchmark (perf stat, per-call vs amortized, 30 runs)
- benchmarks/comparisons/source/README.md (naming convention)
- Project Root Changelog
- checksum_key() (ra_core.c KAT-checksum capture function)
- BCFN(2+0,13-0,T) PractRand failure (first-cycle key-affine correlation)
- Multikey / aggressive-reinit ("call-and-discard") usage pattern
- 'Orbit Addressing' naming policy reversal (docs-only -> code identifier)
- addr_hi (64-bit address parameter)
- 2025-10-5_scramble-design/scrambler.c (predecessor)
- rows=2 Configuration Log
- operation-pruning-research experiment (gate-trap example)
- Design decision: op/wiring mapping ported to toy scale
- Finding: very short transient tail (μ<=1) across all seeds
- Sweep Output (empty file)
- Cross-Correlation Singleblock Postfix Run Log
- Q1: Cross-stream independence
- Kandidat 1 (L=f(seed,counter) splitmix, M unchanged)
- Kandidat 2 (L and M independently keyed)
- Kandidat 4 (L mult from seed, additive counter term)
- Kandidat 2: independent L/M keying
- Orbit (deterministic trajectory selected by key)
- Disassembly finding: algebraic factoring i*C+key*C -> C*(i+key) already applied by compiler (vpaddd once, vpmulld reused) -- no precompute/cache win left
- Tahap 5 mechanical revalidation gate: bit-identical --stream output vs winner_wired_addressable means Tahap 3 need not re-run (formula-preserving, statistical guarantees carry over); any output difference mandates full Tahap 0 + Tahap 3 re-validation before adoption
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
- Avalanche Heatmap: orbit N=128 LOWONLY (Tier0 gate)
- Avalanche Heatmap: orbit N=256 MUTUAL (K=255 full block)
- Avalanche Heatmap: singleblock N=256 (K=255 full block)
- Avalanche Orbit/Singleblock K=255 Rerun Log
- Avalanche Singleblock K=1 Log
- Avalanche heatmap: candidate w8_f28_i0 at K=255 (full cycle)
- Addressable / anti-weak-key init property
- Modular-arithmetic strength reduction (multiply-chain to accumulator)
- w8_f33_i0 (ADD->ROT, no xorshift)
- w8_f9_i0 (XORSHIFT(16))
- Orbit Addressing (f(key) state-init mechanism)
- Philox4x32-10 (comparison generator)
- ra-core
- ra-prng
- production-candidate-battery gate (Step 0-8)
- benchmarks/results (precomputed statistical output)
- research/ (conference manuscripts)
- CSAI 2025 RA-PRNG Paper
- ICCS 2026 RA-PRNG Paper
- scrambler_wired_addressable.c (wired+addressable shuffle combo)
- src/ra_core CHANGELOG (empty)
- ra_prng2 Package Changelog
- ra_prng3 Package Changelog
- Image Encrypting Documentation
- tahap6_bench.c (benchmark methodology reused for bench_ra_core.c)
- ZepFold (token-shuffling CLI approach)
- verify_fresh_build.py
- HANDOVER: Continuous-stream real benchmark untuk `ra_core_orbit`
- CycleStage

## God Nodes (most connected - your core abstractions)
1. `singleblock-cycle-combo-search RESULTS.md` - 50 edges
2. `Params` - 34 edges
3. `HANDOVER: uji BCFN pasca-batas-reseed untuk ra_reseed/ra_hash` - 32 edges
4. `ra_prng README` - 30 edges
5. `HANDOVER: Orbit K-kecil defect fix + rolling-register optimasi wide-o` - 29 edges
6. `ra_core_orbit()` - 27 edges
7. `ra_prng2 (Array-Based PRNG)` - 27 edges
8. `ra_core_orbit()` - 26 edges
9. `State-Size/Speed Pareto Frontier HANDOVER Plan` - 26 edges
10. `avalanche_stats()` - 25 edges

## Surprising Connections (you probably didn't know these)
- `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)` --semantically_similar_to--> `Candidate`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/avalanche_heatmap_pruned_winner_cycle1_vs_cycle2.png → experiments/2026-8-26_operation-pruning-research/pruned_prng.py
- `hamzy hams (GitHub maintainer)` --conceptually_related_to--> `Hamas A. Rahman`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf → docs/PITCH.md
- `orbit-MUTUAL reseed mode` --conceptually_related_to--> `ra_reseed()`  [INFERRED]
  experiments/2026-9-11_state-size-pareto-frontier/RESULTS.md → src/ra_core/c/ra_core.c
- `always_inline Ablation Experiment on ra_permutation_cycle_orbit` --references--> `ra_permutation_cycle_orbit()`  [EXTRACTED]
  experiments/2026-9-12_orbit-multicycle-offbyone-fix/BENCHMARK_METHODOLOGY_FINDINGS.md → src/ra_core/c/ra_core.c
- `Fairness fix effect on singleblock K=255 throughput` --references--> `ra_core_singleblock()`  [EXTRACTED]
  experiments/2026-9-1_family-productionization/bench_release_results.txt → src/ra_core/c/ra_core.c

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **Verification evidence suite for the off-by-one fix** — experiments_2026_9_12_orbit_multicycle_offbyone_fix_results_fix_boolflag, experiments_2026_9_12_orbit_multicycle_offbyone_fix_results_exhaustive_word_count_check, experiments_2026_9_12_orbit_multicycle_offbyone_fix_results_parity_check_engine [EXTRACTED 0.85]
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]
- **addr_hi-chaining vs key-increment chaining pattern investigation for pip-ra-core shuffle** — experiments_2026_9_7_addr_hi_chaining_validation_results_doc, concept_addr_hi_chaining_pattern, concept_key_increment_chaining_pattern, pip_ra_core_readme_doc, concept_practrand [EXTRACTED 0.90]
- **Off-by-one termination bug and its fix in ra_core_orbit** — experiments_2026_9_12_orbit_multicycle_offbyone_fix_results_offbyone_bug, src_ra_core_c_ra_core_ra_permutation_cycle_orbit, src_ra_core_c_ra_core_ra_core_orbit, experiments_2026_9_12_orbit_multicycle_offbyone_fix_results_fix_boolflag [EXTRACTED 0.90]
- **Investigation into orbit vs singleblock benchmark gap** — experiments_2026_9_12_orbit_multicycle_offbyone_fix_benchmark_methodology_findings, experiments_2026_9_12_orbit_multicycle_offbyone_fix_dce_microbenchmark_bug, experiments_2026_9_12_orbit_multicycle_offbyone_fix_perf_stat_division_error, experiments_2026_9_12_orbit_multicycle_offbyone_fix_ilp_cross_call_effect, src_ra_core_c_ra_core_ra_core_orbit, src_ra_core_c_ra_core_ra_core_singleblock [EXTRACTED 0.90]
- **ra_core_param.c parameterized core family (orbit/singleblock x N x reseed-mode)** — experiments_2026_9_11_state_size_pareto_frontier_handover_orbit_core, experiments_2026_9_11_state_size_pareto_frontier_handover_singleblock_core, experiments_2026_9_11_state_size_pareto_frontier_handover_ra_n_parameterization, experiments_2026_9_11_state_size_pareto_frontier_handover_reseed_mode_lowonly, experiments_2026_9_11_state_size_pareto_frontier_handover_reseed_mode_mutual [EXTRACTED 0.90]
- **Tiered quality validation pipeline (period gate -> Tier0 -> Tier1 -> Tier2 -> safe-K floor)** — experiments_2026_9_11_state_size_pareto_frontier_handover_period_theoretical_gate, experiments_2026_9_11_state_size_pareto_frontier_handover_tier0_validation, experiments_2026_9_11_state_size_pareto_frontier_handover_tier1_validation, experiments_2026_9_11_state_size_pareto_frontier_handover_tier2_validation, experiments_2026_9_11_state_size_pareto_frontier_handover_safe_k_floor [EXTRACTED 0.90]
- **ra_prng2 Statistical Validation Test Suite** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, concept_nist_sts, concept_dieharder, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_bigcrush [EXTRACTED 0.90]
- **Reinit-K vs Steady-State methodology comparison** — experiments_2026_9_12_orbit_multicycle_offbyone_fix_reinit_k_benchmark_methodology, experiments_2026_9_12_orbit_multicycle_offbyone_fix_steady_state_benchmark_methodology, experiments_2026_9_1_family_productionization_bench_release_bench_real_reinit4, experiments_2026_9_12_orbit_multicycle_offbyone_fix_microbench_orbit_vs_singleblock_c [EXTRACTED 0.90]
- **Self-contained diagnostic files following diag_prereseed.c pattern** — experiments_2026_9_2_singleblock_o_width_fix_diag_wideo_singleblock, experiments_2026_9_2_singleblock_o_width_fix_diag_wideo_warmup_singleblock, experiments_2026_9_2_singleblock_prereseed_experiment_diag_prereseed [EXTRACTED 0.90]
- **winner_wired_v2 cycle grafted onto ZepFold shuffle-loop shape (scrambler_wired_addressable)** — experiments_2026_8_30_addressable_shuffle_scrambler_wired_addressable_c, winner_wired_addressable_c, experiments_2026_8_30_addressable_shuffle_scrambler_addressable_c [EXTRACTED 0.90]
- **Validation pipeline for wired addressable shuffle variant** — experiments_2026_8_30_addressable_shuffle_scrambler_wired_addressable_c, experiments_2026_8_30_addressable_shuffle_verify_parity_wired_py, experiments_2026_8_30_addressable_shuffle_scc_test_wired_py [EXTRACTED 0.90]
- **Candidate D 64-bit address-widening promotion to ra_core.c** — concept_candidate_d, experiments_2026_9_6_orbit_address_widen_64bit_promotion_handover_doc, experiments_2026_9_6_orbit_address_widen_64bit_promotion_results_doc, concept_addr_hi, readme_doc [EXTRACTED 0.95]
- **Orbit K-small defect discovery -> fix -> battery validation -> promotion to ra_core.c** — concept_orbit_kmin_multistream_defect, concept_ra_core_v2_c [EXTRACTED 0.95]
- **ra_prng2 State Transition Operator Composition (F = Mutate∘Permute∘Reseed)** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_transition_operator_f, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_mutate_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_permute_operator, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_reseed_operator [EXTRACTED 0.95]
- **Candidates that FAILED Axis B normality criterion** — experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f8_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f9_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f22_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f24_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f26_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f27_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f29_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f33_i0 [EXTRACTED 1.00]
- **Bit-identical self-check chain linking pruned toy model to full-scale reference and winner C source** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_self_check_full_scale, experiments_2026_8_28_periodicity_heuristic_validation2_handover_pruned_wired_toy_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wired_prng, experiments_2026_8_28_periodicity_heuristic_validation2_handover_wiring_module, experiments_2026_8_28_periodicity_heuristic_validation2_handover_winner_wired_v2 [EXTRACTED 1.00]
- **Combo-search staged filter pipeline (Tier 0 -> Tier 1 -> Promotion Tier)** — experiments_2026_9_2_singleblock_cycle_combo_search_results_tier0_avalanche_gate, experiments_2026_9_2_singleblock_cycle_combo_search_results_promotion_tier [EXTRACTED 1.00]
- **2026-08-29 correction of (8,4,rows=4) lambda bound, documented across HANDOVER/RESULTS/STATUS** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_2026_08_29_correction, experiments_2026_8_28_periodicity_heuristic_validation2_handover_stale_lambda_lower_bound, experiments_2026_8_28_periodicity_heuristic_validation2_handover_corrected_lambda_value, experiments_2026_8_28_periodicity_heuristic_validation2_handover_checkpoint_file, experiments_2026_8_28_periodicity_heuristic_validation2_results_footnote_correction, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_summary, experiments_2026_8_28_periodicity_heuristic_validation2_status_corrected_lower_bound_restated [EXTRACTED 1.00]
- **Cross-session bug discovery and workaround trail** — experiments_2026_9_12_orbit_multicycle_offbyone_fix_handover, experiments_2026_9_11_state_size_pareto_frontier_results, experiments_2026_9_11_state_size_pareto_frontier_results_temuan_2, tools_battery_practrand_gate [EXTRACTED 1.00]
- **Cycle-transform DSL finalizer candidates (curated set of 4)** — experiments_2026_9_4_orbit_init_cycle_combo_search_handover_w8_f9_i0, experiments_2026_9_4_orbit_init_cycle_combo_search_handover_w8_f33_i0 [EXTRACTED 1.00]
- **Dieharder validation runs across K=1/K=96/K=255 for w8_f10_i0 and w8_f28_i0** — experiments_2026_9_3_dieharder_inject_crossing_dieharder_w8_f10_i0_k1_piped, experiments_2026_9_3_dieharder_inject_crossing_dieharder_w8_f10_i0_k96_piped, experiments_2026_9_3_dieharder_inject_crossing_dieharder_w8_f28_i0_k1_piped, experiments_2026_9_3_dieharder_inject_crossing_dieharder_w8_f28_i0_k96_piped, experiments_2026_9_3_combo_winner_pareto_selection_dieharder_w8_f28_i0_k255_piped [EXTRACTED 1.00]
- **Classic PRNG comparison table (Section 3)** — docs_pitch_ra_prng, docs_pitch_mersenne_twister, docs_pitch_pcg, docs_pitch_xorshift, docs_pitch_philox4x32_10 [EXTRACTED 1.00]
- **RA-PRNG research provenance / authorship** — docs_pitch_ra_prng2_paper, docs_pitch_hamas_a_rahman, docs_pitch_state_polytechnic_of_malang, docs_pitch_csai_2025 [EXTRACTED 1.00]
- **RA-PRNG statistical validation battery** — docs_pitch_ra_prng, concept_dieharder, concept_practrand, docs_pitch_nist_sts, docs_pitch_testu01_bigcrush [EXTRACTED 1.00]
- **pruned_winner Cycle 1 vs Cycle 90 Avalanche Analysis** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_reseed_diffusion_result, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_avalanche_fraction [EXTRACTED 1.00]
- **winner_wired Avalanche Diffusion Convergence Across Cycles** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle1_avalanche, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_cycle4_avalanche [EXTRACTED 1.00]
- **Winner Wired PRNG Avalanche Effect Analysis** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_avalanche_performance [EXTRACTED 1.00]
- **Avalanche Effect Cycle Comparison** — experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_avalanche_effect_analysis, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_1_avalanche, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_2_avalanche [EXTRACTED 1.00]
- **Code locations affected by the glibc fmemopen exact-buffer-size clobber bug** — scrambler_ra_core_singleblock_c, checksum_key_function [EXTRACTED 1.00]
- **Documented gate-size-trap incidents across search experiments** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_operation_pruning_research_files, experiments_2026_9_2_singleblock_cycle_combo_search_handover_operand_position_search_files, experiments_2026_9_2_singleblock_cycle_combo_search_handover_singleblock_o_width_fix_experiment, experiments_2026_9_2_singleblock_cycle_combo_search_handover_gate_size_trap [EXTRACTED 1.00]
- **12-candidate PRNG family benchmark roster (ra_prng family vs literature PRNGs)** — src_ra_prng2_c_ra_prng2, experiments_2026_8_27_operand_position_search_winner_wired_v2, experiments_2026_8_30_addressable_init_research_tahap6_bench, concept_pcg32, concept_chacha20, concept_splitmix64 [EXTRACTED 1.00]
- **1TB interleaved-PractRand anomaly diagnostic investigation** — experiments_2026_8_29_parallelization_research_handover_1tb_followup, experiments_2026_8_30_addressable_init_research_results, rationale_1tb_anomaly_open_question [EXTRACTED 1.00]
- **1TB PractRand anomaly matrix across 4 configurations** — experiments_2026_8_29_parallelization_research_results_winner_wired_v2, experiments_2026_8_29_parallelization_research_results_winner_wired_addressable, experiments_2026_8_29_parallelization_research_results_fpf14_6_16_signal, experiments_2026_8_29_parallelization_research_results_bcfn_signal, experiments_2026_8_29_parallelization_research_results_langkah4_synthesis [EXTRACTED 1.00]
- **Addressable-init research pipeline Tahap 0 through Tahap 6** — experiments_2026_8_30_addressable_init_research_results_kandidat5, experiments_2026_8_30_addressable_init_research_results_winner_wired_addressable, experiments_2026_8_30_addressable_init_research_results_tahap3_validation, experiments_2026_8_30_addressable_init_research_results_tahap4_philox_benchmark, experiments_2026_8_30_addressable_init_research_results_tahap5_init_optimization [EXTRACTED 1.00]
- **Guard-XOR Fix Implementation & Verification Chain** — experiments_2026_9_1_keyzero_guard_fix_results_guard_xor_fix, experiments_2026_9_1_keyzero_guard_fix_diag_keyzero_guard, experiments_2026_9_1_keyzero_guard_fix_pick_guard_constants, experiments_2026_9_1_family_productionization_ra_core_guard_l, experiments_2026_9_1_family_productionization_ra_core_guard_m [EXTRACTED 1.00]
- **Multikey remix mixing-fix candidate search sequence** — experiments_2026_9_1_multikey_remix_search_handover_candidate1_fmix32, experiments_2026_9_1_multikey_remix_search_handover_candidate2_keyterm_additive, experiments_2026_9_1_multikey_remix_search_handover_candidate3_keyterm_xor, experiments_2026_9_1_multikey_remix_search_handover_candidate4_keyterm_mul, experiments_2026_9_1_multikey_remix_search_handover_ra_init_orbit [EXTRACTED 1.00]
- **Production-Candidate Battery Verification Axes (Step 0-3)** — experiments_2026_9_1_production_candidate_battery_results, experiments_2026_9_1_production_candidate_battery_quality_gate, experiments_2026_8_29_parallelization_research_cross_correlation_ra_core_singleblock, experiments_2026_8_29_parallelization_research_collision_scan_ra_core_singleblock, experiments_2026_9_1_production_candidate_battery_results_key_zero_defect [EXTRACTED 1.00]
- **K=255 Throughput Benchmark: ra_core vs Philox** — experiments_2026_9_1_production_candidate_battery_bench_ra_core_results_philox_baseline, experiments_2026_9_1_production_candidate_battery_bench_ra_core_results_log [EXTRACTED 1.00]
- **L Mechanism Spectrum Tested in Phase 1** — experiments_2026_8_28_state_update_mechanism_research_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_overwrite_mechanism [EXTRACTED 1.00]
- **M Mechanism Spectrum Tested in Phase 2** — experiments_2026_8_28_state_update_mechanism_research_xor_fold_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_overwrite_mechanism [EXTRACTED 1.00]
- **Off-by-one bug diagnosis and fix flow** — experiments_2026_9_12_orbit_multicycle_offbyone_fix_handover_offbyone_bug, src_ra_core_c_ra_permutation_cycle_orbit, src_ra_core_c_ra_core_orbit, experiments_2026_9_12_orbit_multicycle_offbyone_fix_handover_fix_plan [EXTRACTED 1.00]
- **64-bit Init Mixing-Formula Candidates (A/B/C) Bake-off** — experiments_2026_9_5_orbit_address_widen_64bit_candidate_a, experiments_2026_9_5_orbit_address_widen_64bit_candidate_b, experiments_2026_9_5_orbit_address_widen_64bit_candidate_c [EXTRACTED 1.00]
- **Four-Analysis Investigation Flow Resolving K=1 Orbit Defect** — experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_bijectivity_analysis, experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_fpf_histogram_analysis, experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_weyl_cancellation_refuted, experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_key_duplication_hypothesis [EXTRACTED 1.00]
- **Orbit K=1+hashed-key defect root-cause investigation chain** — experiments_2026_9_4_orbit_kmin_battery_addendum_hashed_key_gap, experiments_2026_9_4_orbit_init_zerotap_fix_results_zero_tap_hypothesis, experiments_2026_9_4_orbit_init_cycle_combo_search_handover_init_formula_axis, experiments_2026_9_4_orbit_init_cycle_combo_search_handover_cycle_transform_axis [EXTRACTED 1.00]
- **Original Algorithm Avalanche Cycle Evolution (Cycle 1 vs Cycle 3)** — experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_heatmap, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_avalanche_diffusion_comparison [EXTRACTED 1.00]
- **State-size Pareto frontier variant quality tiers** — experiments_2026_9_11_state_size_pareto_frontier_results_pareto_frontier, experiments_2026_9_11_state_size_pareto_frontier_results_orbit_lowonly, experiments_2026_9_11_state_size_pareto_frontier_results_orbit_mutual, experiments_2026_9_11_state_size_pareto_frontier_results_singleblock_param, experiments_2026_9_11_state_size_pareto_frontier_results_n16_failure [EXTRACTED 1.00]
- **Pareto-selection axes (Speed A + Avalanche B feed the frontier)** — experiments_2026_9_3_combo_winner_pareto_selection_handover_axis_a_speed, experiments_2026_9_3_combo_winner_pareto_selection_handover_axis_b_avalanche_normality, experiments_2026_9_3_combo_winner_pareto_selection_handover_pareto_frontier [EXTRACTED 1.00]
- **Three enrichment slots forming the bounded per-round combo search space** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_search_space_dsl, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot1_o_tap_width, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot2_c_finalizer, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot3_extra_inject [EXTRACTED 1.00]
- **Cross-family PRNG speed comparison table (ra_prng orbit/singleblock vs other cores)** — experiments_2026_9_1_family_productionization_ra_core_ra_core_singleblock, experiments_2026_9_1_family_productionization_ra_core_ra_core_orbit, experiments_2026_8_31_prng_family_benchmark_benchmark_all_philox, experiments_2026_8_31_prng_family_benchmark_benchmark_all_splitmix64, experiments_2026_8_31_prng_family_benchmark_benchmark_all_pcg32, experiments_2026_8_31_prng_family_benchmark_benchmark_all_paperorig, experiments_2026_8_31_prng_family_benchmark_benchmark_all_xoshiro256, experiments_2026_8_31_prng_family_benchmark_benchmark_all_wiredv2, experiments_2026_8_31_prng_family_benchmark_benchmark_all_addrcont, experiments_2026_8_31_prng_family_benchmark_benchmark_all_chacha20, experiments_2026_8_31_prng_family_benchmark_benchmark_all_mt19937, experiments_2026_8_31_prng_family_benchmark_benchmark_all_dev_urandom [EXTRACTED 1.00]
- **Production readiness gate closure blockers** — experiments_2026_9_1_family_productionization_production_readiness_handover_gate, experiments_2026_9_1_family_productionization_production_readiness_handover_singleblock_kmin_defect, experiments_2026_9_1_family_productionization_production_readiness_handover_orbit_kmin_multistream_defect, experiments_2026_9_1_family_productionization_production_readiness_handover_birthday_paradox_root_cause, experiments_2026_9_1_family_productionization_production_readiness_handover_postreseed_bcfn_item [EXTRACTED 1.00]
- **11 Promotion-Tier FULLY CLEAN winners (K in {1,2,4,8,16,32,64,96} @16GB)** — experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f8_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f9_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f10_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f22_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f24_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f25_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f26_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f27_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f28_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f29_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f33_i0 [EXTRACTED 1.00]
- **pruned_winner Avalanche Analysis and Metric Evaluation** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_figure, experiments_2026_8_26_operation_pruning_research_pruned_winner_avalanche_behavior, experiments_2026_8_26_operation_pruning_research_pruned_winner_candidate_configuration [EXTRACTED 1.00]
- **Release Benchmark Prep Gate Components for ra_core.c Promotion** — experiments_2026_9_6_release_benchmark_prep_avalanche_curve_task, experiments_2026_9_6_release_benchmark_prep_throughput_reconfirmation, experiments_2026_9_6_release_benchmark_prep_production_promotion_decision [EXTRACTED 1.00]
- **Prior experiments that ruled out the init/seed fix direction** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_singleblock_prereseed_experiment, experiments_2026_9_2_singleblock_cycle_combo_search_handover_ra_core_orbit_crosscheck, experiments_2026_9_2_singleblock_cycle_combo_search_handover_kandidat_a_wideo, experiments_2026_9_2_singleblock_cycle_combo_search_handover_kandidat_b_wideo_warmup [EXTRACTED 1.00]
- **Prior Research Folders That Tested Only Single-Stream Axes** — concept_periodicity_heuristic_validation_research, concept_operation_pruning_research, concept_operand_position_search_research, concept_state_update_mechanism_research, concept_periodicity_heuristic_validation2_research [EXTRACTED 1.00]
- **Singleblock K-small-defect fix promotion and battery-gate closure flow** — production_candidate_battery_gate, experiments_2026_9_1_production_candidate_battery_addendum_post_fix_status, experiments_2026_9_1_family_productionization_production_readiness_handover [EXTRACTED 1.00]
- **Tahap 5 ranked optimization candidates for ra_init_state_addressable init cost** — experiments_2026_8_30_addressable_init_research_handover_tahap5_rank1_vector_width_512, experiments_2026_8_30_addressable_init_research_handover_tahap5_rank2_vprolvd_intrinsic, experiments_2026_8_30_addressable_init_research_handover_tahap5_rank3_loop_unrolling, experiments_2026_8_30_addressable_init_research_handover_tahap5_rank4_hand_simd_rewrite, experiments_2026_8_30_addressable_init_research_handover_tahap5_ruled_out_iteration_count [EXTRACTED 1.00]
- **Tahap 5 bit-identical revalidation gate: candidate correctness check determines whether Tahap 0/Tahap 3 must be redone before adoption** — experiments_2026_8_30_addressable_init_research_handover_tahap5_validation_gate, experiments_2026_8_30_addressable_init_research_handover_tahap5_goal [EXTRACTED 1.00]
- **Two-Stage Fix Strategy (Wide-o + Warm-up Fallback)** — experiments_2026_9_2_singleblock_o_width_fix_handover_wide_o_candidate, experiments_2026_9_2_singleblock_o_width_fix_handover_warmup_fallback_candidate, project_multikey_remix_search [EXTRACTED 1.00]
- **Fix verification flow (KAT, exhaustive check, parity, tooling)** — experiments_2026_9_12_orbit_multicycle_offbyone_fix_handover_verification_plan, src_ra_core_c_checksum_key_core, src_ra_core_c_ra_core_engine_ra_core_engine_fill_block, tools_battery_practrand_gate [EXTRACTED 1.00]
- **Validation pipeline for d0/a0 warmup candidates (Tier 0 -> Tier 1)** — tier0_avalanche_gate, tier1_practrand_gate, kandidat_a_d0_hash, kandidat_b_d0_realround [EXTRACTED 1.00]
- **Watchdog process-matching bug, its fix, and the sweep changes it required** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_watchdog_pgrep_bug, experiments_2026_8_28_periodicity_heuristic_validation2_handover_auto_stop_power38, experiments_2026_8_28_periodicity_heuristic_validation2_handover_cmd_pattern_fix, experiments_2026_8_28_periodicity_heuristic_validation2_handover_run_sweep, experiments_2026_8_28_periodicity_heuristic_validation2_handover_manual_only_exclusion [EXTRACTED 1.00]
- **D vs E Final Decision: Identical Statistical Gates, Decided by GPU Portability Tie-break** — experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_d, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_e, experiments_2026_9_5_orbit_address_widen_64bit_results_gpu_portability_tiebreak, experiments_2026_9_5_orbit_address_widen_64bit_results_avalanche_3way_test, experiments_2026_9_5_orbit_address_widen_64bit_results_practrand_gap16_test, experiments_2026_9_5_orbit_address_widen_64bit_results_fpf_collision_scan_test, experiments_2026_9_5_orbit_address_widen_64bit_results_dieharder_battery [INFERRED 0.75]
- **Array-Native Data Permutation and Fingerprinting Workflow** — research_iccs2026_ra_prng_avalanche_sensitivity, sub_projects_fingerprinting_tools_concept, benchmarks_comparisons_source_zepfold_cli [INFERRED 0.75]
- **PractRand anomaly signals observed across both candidates** — experiments_2026_9_2_singleblock_o_width_fix_results_bcfn, experiments_2026_9_2_singleblock_o_width_fix_results_tmfn, experiments_2026_9_2_singleblock_o_width_fix_results_fpf14 [INFERRED 0.75]
- **Cross-session convergence on the a=(d^o)^(cons+a) combination point as root-cause locus** — experiments_2026_9_5_orbit_o_noshift_fix_results_combination_point_concept, experiments_2026_9_5_orbit_o_noshift_fix_results_staggering_shift_hypothesis, experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_handover_k1_orbit_defect, experiments_2026_9_5_orbit_o_noshift_fix_results_o_noshift_candidate [INFERRED 0.80]
- **K-kecil Defect Discovery Chain** — experiments_2026_9_2_singleblock_o_width_fix_handover_k_kecil_defect, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_singleblock, experiments_2026_9_2_singleblock_k_threshold_characterization_results, experiments_2026_9_1_production_candidate_battery [INFERRED 0.80]
- **ra_prng2 Original Benchmark Report Set** — benchmarks_results_ra_prng2_original_rnging_perf_stat_, benchmarks_results_ra_prng2_original_testu01, benchmarks_results_ra_prng2_original_dieharder_test, benchmarks_results_ra_prng2_original_shuffling_perf_stat [INFERRED 0.80]
- **ra_prng3 Benchmark Report Set** — benchmarks_results_ra_prng3_nist_sts_test, benchmarks_results_ra_prng3_dieharder_test, benchmarks_results_ra_prng3_ent_test, benchmarks_results_ra_prng3_perf_stat_rnging, benchmarks_results_ra_prng3_perf_stat_scrambling, benchmarks_results_ra_prng3_practrand_test [INFERRED 0.80]
- **addr_hi 64-bit widening fix and downstream re-validation** — src_ra_core_c_ra_core_addr_hi_widening, experiments_2026_9_1_family_productionization_production_readiness_handover_birthday_paradox_root_cause, experiments_2026_9_4_orbit_reseed_postboundary_bcfn_results_test, experiments_2026_9_4_orbit_reseed_postboundary_bcfn_results_driver_addr_hi_bug, experiments_2026_9_11_state_size_pareto_frontier_results_addr_hi_staleness, tools_battery_readme_addr_hi_contract [INFERRED 0.85]
- **Candidate A Diagnostic Build Pattern** — experiments_2026_9_2_singleblock_o_width_fix_handover_wide_o_candidate, experiments_2026_9_2_singleblock_o_width_fix_diag_wideo_singleblock, experiments_2026_8_26_operation_pruning_research_pruned_prng_build_o, experiments_2026_9_2_singleblock_k_threshold_characterization_multikey_stream_k [INFERRED 0.85]
- **ChaCha20 Speed + Entropy Benchmark Suite** — benchmarks_comparisons_rnging_speed_chacha20_result, benchmarks_comparisons_entropy_chacha20_result [INFERRED 0.85]
- **Cumulative gap-explanation chain: I/O (~85%) -> reseed/M-width (~13% of residual) -> recurrence pruning (~5%)** — experiments_2026_8_30_addressable_shuffle_speed_benchmark_io_bottleneck_finding, experiments_2026_8_30_addressable_shuffle_speed_benchmark_gap_decomposition_finding, experiments_2026_8_30_addressable_shuffle_speed_benchmark_recurrence_pruning_finding [INFERRED 0.85]
- **Pruned Winner Avalanche Progression Analysis (Cycle 1 vs Cycle 2)** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_heatmap, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle1_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_cycle2_avalanche, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle2_avalanche_strengthening [INFERRED 0.85]
- **Winner Wired Warmup Cycle Avalanche Progression** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_winner_wired_wiring, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle1_avalanche, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle3_cycle3_avalanche [INFERRED 0.85]
- **Avalanche Effect Bit Diffusion Evaluation** — experiments_others_avalanche_effect_heatmap_figure, experiments_others_avalanche_effect_heatmap_avalanche_effect, experiments_others_avalanche_effect_heatmap_hamming_distance_analysis, experiments_others_avalanche_effect_heatmap_original_algorithm [INFERRED 0.85]
- **Four-Axis Validation Framework (periodicity/speed/avalanche/PractRand)** — experiments_2026_8_25_periodicity_heuristic_validation_toy_prng, experiments_2026_8_28_state_update_mechanism_research_instruction_count_speed_axis, experiments_2026_8_28_state_update_mechanism_research_avalanche_gate_min_bit, experiments_2026_8_28_state_update_mechanism_research_practrand_16gb_tier [INFERRED 0.85]
- **Periodicity Heuristic Empirical Validation Lifecycle** — research_iccs2026_ra_prng_periodicity_heuristic, experiments_2026_8_25_periodicity_heuristic_validation_toy_model_spec, experiments_2026_8_25_periodicity_heuristic_validation_scaling_verdict [INFERRED 0.85]
- **ra_core addressable API surface (orbit/singleblock + pip binding)** — readme_ra_core, readme_pip_ra_core, src_ra_core_c_ra_core_ra_core_orbit, src_ra_core_c_ra_core_ra_core_singleblock [INFERRED 0.85]
- **Weyl vs SplitMix32-hashed key-ordering parity testing that exposed the K=1 gap** — concept_weyl_stepped_key_ordering, concept_splitmix32_hashed_key_ordering, concept_k1_hashed_key_gap, experiments_2026_9_4_orbit_kmin_battery_addendum_hashed_key_gap [INFERRED 0.85]
- **5 Candidates Evaluated Through the Same Statistical Gate Battery (avalanche, Gap-16, FPF, dieharder)** — experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_a, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_b, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_c, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_d, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_e, experiments_2026_9_5_orbit_address_widen_64bit_results_avalanche_3way_test, experiments_2026_9_5_orbit_address_widen_64bit_results_practrand_gap16_test, experiments_2026_9_5_orbit_address_widen_64bit_results_fpf_collision_scan_test, experiments_2026_9_5_orbit_address_widen_64bit_results_dieharder_battery [INFERRED 0.85]
- **Q1-Q4 Parallelization Research Questions** — concept_stream_independence_testing, concept_seed_collision_probability, concept_throughput_scalability, concept_simd_vectorization_feasibility [INFERRED 0.85]
- **Pareto Frontier Selection over Combo-Winner Candidates** — experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f10_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f28_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f25_i0, experiments_2026_9_3_combo_winner_pareto_selection_pareto_select [INFERRED 0.85]
- **Candidate D Implements Modified ra_core.c Init/Signature Functions, 255-Round Engine Unchanged** — experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_d, experiments_2026_9_1_family_productionization_ra_core_ra_init_state_orbit, experiments_2026_9_1_family_productionization_ra_core_ra_init_state_singleblock, experiments_2026_9_1_family_productionization_ra_core_ra_core_orbit, experiments_2026_9_1_family_productionization_ra_core_ra_core_singleblock, experiments_2026_9_5_orbit_address_widen_64bit_results_ra_permutation_cycle_verbatim [INFERRED 0.85]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_practrand, concept_ent [INFERRED 0.85]
- **K-small BCFN defect fix search (Kandidat A/B -> no-recommendation)** — experiments_2026_9_2_singleblock_o_width_fix_results_kandidat_a, experiments_2026_9_2_singleblock_o_width_fix_results_kandidat_b, experiments_2026_9_2_singleblock_o_width_fix_results_rekomendasi, experiments_2026_9_2_singleblock_k_threshold_characterization_results [INFERRED 0.85]
- **Pruned Winner Cycle 1 vs Cycle 3 Avalanche Comparison** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_reseed_avalanche_diffusion [INFERRED 0.95]
- **ra_core.c static functions verbatim-copied into diag_init_candidates.c** — experiments_2026_9_3_init_loop_optimization_diag_init_candidates, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_orbit, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_singleblock, experiments_2026_9_1_family_productionization_ra_core_ra_hash, experiments_2026_9_1_family_productionization_ra_core_ra_reseed, experiments_2026_9_1_family_productionization_ra_core_rot32, experiments_2026_9_1_family_productionization_ra_core_fmix32, experiments_2026_9_1_family_productionization_ra_core_guard_l, experiments_2026_9_1_family_productionization_ra_core_guard_m, experiments_2026_9_1_family_productionization_ra_core_ra_init_state_orbit [INFERRED 0.95]
- **Avalanche Proxy vs Full Empirical Quality Disconnect** — experiments_2026_8_26_operation_pruning_research_results_shift_repair_failure, experiments_2026_8_26_operation_pruning_research_status_gate_size_traps, experiments_2026_8_27_operand_position_search_handover_dead_seed_bits, experiments_2026_8_27_operand_position_search_handover_avalanche_heatmap_baseline [INFERRED]
- **Pruned Winner Multi-battery Statistical Validation** — experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_26_operation_pruning_research_practrand_pruned_winner_1tb_practrand_1tb_log, experiments_2026_8_26_operation_pruning_research_dieharder_pruned_winner_piped_piped_results, experiments_2026_8_26_operation_pruning_research_dieharder_gcd_fixed_gcd_test_log, experiments_2026_8_26_operation_pruning_research_dieharder_rgb_minimum_distance_fixed_rgb_min_dist_log [INFERRED]
- **ra_prng Algorithmic Evolution and Research Pipeline** — readme_ra_prng2, experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_27_operand_position_search_handover_handover_spec [INFERRED]

## Communities (630 total, 139 thin omitted)

### Community 0 - "winners_bench.c"
Cohesion: 0.08
Nodes (57): core_entry_t, find_core(), main(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10(), philox4x32_round() (+49 more)

### Community 1 - "avalanche_stats"
Cohesion: 0.13
Nodes (24): _log(), main(), phase_a_hash_access_check(), phase_b_greedy_ablation(), Driver for the operation-pruning greedy ablation search. Two phases: Phase A:…, all_ops_baseline(), describe(), ops_to_bitmask() (+16 more)

### Community 2 - "HANDOVER: uji BCFN pasca-batas-reseed untuk ra_reseed/ra_hash"
Cohesion: 0.04
Nodes (62): BCFN-style escalating PractRand failure pattern (e.g. FPF-14+6/16), dieharder battery (27 Good-reliability tests, run_dieharder_battery.py), orbit K=1 + hashed(SplitMix32) key ordering PractRand failure (unresolved), no-retry-on-FAIL policy (one PractRand FAIL is final, no seed retry), Orbit/singleblock benchmark standard (5-number report: multistream K=1/K=255/--stream, singleblock K=1/K=255), post-reseed-boundary BCFN test design (isolate first cycle after ra_reseed), ra_hash (hash mechanism used inside ra_reseed, HASH_DEPTH=32), ra_reseed (state reseed mechanism, orbit mode, XOR-fold 256->8 words) (+54 more)

### Community 3 - "combo_gen_orbit.py"
Cohesion: 0.19
Nodes (20): generate_all(), Path, Per-candidate C codegen for the orbit init-formula x cycle-transform combo…, Compare compiled binary's --single output (multi-round, rng=37) against…, The mandatory hand-trace: (init_linear_shared, w8_f10_i0) must reproduce…, render_c(), sanity_check(), verify_control_matches_production() (+12 more)

### Community 4 - "Parallelization Research Results (2026-08-29)"
Cohesion: 0.05
Nodes (52): 1TB interleaved PractRand anomaly (3/304 flagged), BCFN(2+0,13-0,T) seed-dependent noise finding, Q2 collision-scan (sequential + random seed space), Method A: cross-correlation independence test, Parallelization Research Results (2026-08-29), FPF-14+6/16 reproducible signal across 4 configurations, Harness bug: case-sensitive SUSPICIOUS tag match, Method B: interleaved PractRand test (+44 more)

### Community 5 - "benchmark_all.c"
Cohesion: 0.10
Nodes (44): FILE, chacha20_block(), find_core(), kat_chacha20(), kat_pcg32(), kat_philox(), kat_splitmix64(), kat_xoshiro256() (+36 more)

### Community 6 - "tahap4_bench.c"
Cohesion: 0.21
Nodes (21): tahap5_bench.c (planned new file, fork of tahap4_bench.c, benchmarks multiple ra_init_state_addressable_vN_* variants side by side via mode_init_cost/mode_throughput/mode_reinit_sweep), Disassembly finding: GCC -O3 -march=native already auto-vectorizes ra_init_state_addressable loop to AVX-512VL (8-lane/YMM), incl. vpternlogd fusion for (i^key)&31, vpmulld, vpsrlvd/vpsllvd+vpord for rot32, and aligned vmovdqa stores, Test machine: Intel Core i3-1115G4 (Tiger Lake, 2C/4T, AVX-512-capable), Tahap 4: Speed Benchmark vs Philox, FILE, main(), mode_init_cost(), mode_reinit_sweep() (+13 more)

### Community 7 - "ra_core_orbit"
Cohesion: 0.05
Nodes (54): Avalanche test: per-bit avalanche fraction heatmap (flipped seed bit x output word index), Axis A - Speed Benchmark Methodology, Deviation: K=1 noise fix via 7x min-of-repeats reinit-sweep, main(), splitmix32(), main(), main(), FILE (+46 more)

### Community 8 - "ra_permutation_cycle_singleblock"
Cohesion: 0.06
Nodes (41): pruned_prng.c, build_o() (original 8-tap formula), multikey_stream.c Weyl-increment multikey pattern (base pattern), ra_permutation_cycle_singleblock(), Production-Candidate Battery Gate, Cycle-operation combo-search DSL (756-combination search space: tap width x finalizer x inject), main(), singleblock-k-threshold-characterization RESULTS.md (+33 more)

### Community 9 - "Completed Tasks"
Cohesion: 0.07
Nodes (41): 2026-08-29 correction: (8,4,rows=4) watchdog bug + stale headline number, auto_stop_power38.py (watchdog script), avalanche_check.py, .cycle_measure_ckpt_n8_w4_rows4_seed0.bin (CkptHeader checkpoint), Fix: CMD_PATTERN uses exact argv instead of broad pgrep prefix, Corrected λ > 3.045×10^11 for (8,4,rows=4), cycle_measure.py / cycle_measure.c (Brent's algorithm), enumerate_n2w4.py (+33 more)

### Community 10 - "tahap0_prototype.py"
Cohesion: 0.09
Nodes (40): blake2b8(), init_kandidat1(), init_kandidat2(), init_kandidat3_control(), init_kandidat4(), init_kandidat5(), init_original(), main() (+32 more)

### Community 11 - "RESULTS — State-Size / Speed Pareto Frontier (N∈{256,128,64,32,16})"
Cohesion: 0.11
Nodes (18): 10. Rekomendasi untuk publikasi, 11. Index file, 1. Metodologi ringkas, 2. Gerbang period teoretis (Tahap B), 3. Tier 0 — Avalanche 3-arah + heatmap, 4. Tier 1 — PractRand screen (256M/2G/2G), 5. Safe-K floor scan, khusus N=16 (Tahap E), 6. Tier 2 — PractRand deep (2G→8G→16G dual-base-key) + dieharder 27-test (VPS) (+10 more)

### Community 12 - "Handover: uji 2 kandidat fix tertarget (seed `d0` di fase init) untuk defect K=1+hashed pada `ra_core_orbit`"
Cohesion: 0.06
Nodes (38): singleblock-prereseed-experiment, 0. WAJIB: checkpoint ke user sebelum implementasi, 10. Referensi cepat, 1. Kenapa search ini perlu (ringkasan, baca sumber untuk detail lengkap), 2. Scope, 3. Kandidat A — `d0_hash` (sintetis, murah), 4. Kandidat B — `d0_realround` (satu ronde asli i=255, dibuang, dihitung di fase init), 5. Perubahan kode (minimal, hanya signature — lihat §0.5) (+30 more)

### Community 13 - "c/ra_core.c"
Cohesion: 0.09
Nodes (40): Pareto Frontier Speed Benchmark Raw Results, PCG32 baseline (newly added to harness), State-Size/Speed Pareto Frontier HANDOVER Plan, orbit core algorithm variant (full L+M, ra_reseed every N-1 words), Theoretical period pre-check gate (lambda ~ 0.7824*sqrt|S|), RA_N/RA_MASK/RA_RESEED_MODE macro parameterization design, State-size shrink Pareto research questions (RQ1-RQ5), RESEED_MODE LOWONLY (production-equivalent reseed) (+32 more)

### Community 14 - "stream"
Cohesion: 0.22
Nodes (14): capture(), main(), Per-bit avalanche stats (cycle 1) for all 13 winners from RESULTS.md's table,…, final_cons(), init_state(), permutation_cycle(), Operand-position-parameterized reimplementation of `baseline.c`'s…, DEFAULT_WIRING must reproduce baseline.c / pruned_prng.py's… (+6 more)

### Community 15 - "ra_prng README"
Cohesion: 0.07
Nodes (40): Hamas A. Rahman (author/maintainer), 32-bit key birthday-paradox collision defect, Philox4x32-10 (reference counter-based PRNG), ra_prng Implementation Variant Roles, Repository Structure Specification, CSAI2025 manuscript: ra_prng2 and Beyond, ICCS2026 manuscript: Array-Native Randomness, Addressable/Orbit-style Initialization (research line) (+32 more)

### Community 16 - "tahap5_bench.c"
Cohesion: 0.12
Nodes (32): Manual accumulator (strength reduction) ruled out -- 4.4-5x slower, defeats auto-vectorization, Rank 1: -mprefer-vector-width=512 flag-only speedup (not adopted), Rank 3: -funroll-loops flag-only speedup (not adopted), Tahap 5: Init Cost Optimization, FILE, find_variant(), main(), mode_init_cost() (+24 more)

### Community 17 - "avalanche_heatmap_pruned_winner.py"
Cohesion: 0.14
Nodes (20): analyze_cycle(), compute_avalanche_matrix(), Reseed-Driven Avalanche Strengthening Effect, Initial State Avalanche Fraction (Cycle 1: 0.418459), Post-Reseed Avalanche Fraction (Cycle 2: 0.468409), Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2), Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13), main() (+12 more)

### Community 18 - "ra_core_orbit multi-cycle off-by-one bug"
Cohesion: 0.08
Nodes (32): Birthday-paradox key-repetition defect (32-bit key domain), Temuan #2 (state-size study), Orbit Multi-cycle Off-by-one Fix Handover, 1. Latar belakang, 2. Root cause (diverifikasi lewat trace manual + baca kode, sesi ini), 3. Rencana fix (disarankan, BELUM diterapkan), 4. Rencana verifikasi (WAJIB sebelum promosi, urutan), 5. Peringatan penting — ini PERUBAHAN PERILAKU (breaking change kecil) (+24 more)

### Community 19 - "combo_gen.py"
Cohesion: 0.18
Nodes (21): CORES[] function-pointer dispatch table (ra_core.c), generate_all_tier01(), Path, Compare compiled binary's --single output (multi-round, rng=37) against…, The ONE true hand-trace: width=2, finalizer=empty(idx0), inject=off(idx0) must…, render_c(), sanity_check(), verify_control_matches_production() (+13 more)

### Community 20 - "ra_prng2 (Array-Based PRNG)"
Cohesion: 0.07
Nodes (33): CSAI 2025 (Beijing), Hamas A. Rahman, ra_core, "ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation", State Polytechnic of Malang, Indonesia, ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex (+25 more)

### Community 21 - "Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)"
Cohesion: 0.22
Nodes (9): Candidates tried so far, Context: where this came from, Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey), Non-goals, Non-regression verification results (2026-09-01) -- verification bar item 2 DONE, Process note (per explicit user feedback this session), Root cause (conclusively diagnosed this session — do not re-litigate), The task: find something cheaper than a full/partial warm-up cycle (+1 more)

### Community 22 - "PRNG Family Benchmark Results"
Cohesion: 0.07
Nodes (29): RA-PRNG Repo Workflow Guide, ChaCha20, graphify knowledge-graph workflow, pcg32, SplitMix64, VPS for long-running research jobs, Tahap 6: no-L[] fast path (rng<=255), PRNG Family Benchmark Handover (+21 more)

### Community 23 - "combo_prng_warmup.py"
Cohesion: 0.11
Nodes (28): generate_all(), Path, Per-variant C codegen for the orbit init-warmup fix experiment (HANDOVER.md),…, Compare compiled binary's --single output against combo_prng_warmup.py's…, The mandatory hand-trace: control (production behavior byte-for-byte) must…, render_c(), sanity_check(), verify_control_matches_production() (+20 more)

### Community 24 - "diag_init_candidates.c"
Cohesion: 0.13
Nodes (34): Accum candidate (ring-arithmetic strength reduction), Backend-bound execution stalls (K=255 singleblock_accum regression), core_entry_t, FILE, find_core(), fmix32(), main(), now_seconds() (+26 more)

### Community 25 - "2026-9-4_orbit-init-cycle-combo-search/RESULTS.md"
Cohesion: 0.09
Nodes (23): candidates/init_linear_shared__w8_f10_i0 (kontrol/produksi binary), linear_p1 IDX_XFORM candidate (index i+1), Retracted claim: ra_hash/ra_reseed pointed as most likely root cause, init_linear_p1_shared x w8_f10/f9_i0 FAIL even under Weyl ordering (secondary finding), Corrected insight: K=1 tests quality of orbit_round1 as hash function key->1 word, Zero-Tap Init Fix Results, Hypothesis (see conversation, 2026-09-04), Patch tested (+15 more)

### Community 26 - "singleblock-cycle-combo-search RESULTS.md"
Cohesion: 0.09
Nodes (40): BCFN structural defect (K small, ra_core_singleblock) — FAILs up to K≈72, clean from K≈96, operand_search.py (operation-pruning-research), pruned_winner dead-bit defect (seed bit 5/6 nearly non-avalanche, operand-position-search), core_entry_t, mode_reinit_sweep(), combo_search_log.jsonl, promotion_log.jsonl / promotion_logs/*.log, singleblock-cycle-combo-search RESULTS.md (+32 more)

### Community 27 - "ra_reseed"
Cohesion: 0.11
Nodes (25): addr_hi contract staleness in battery tooling, Birthday-paradox key-repetition root cause, ra_reseed/ra_hash post-reseed BCFN item, Post-Reseed BCFN Test Results, Driver addr_hi hardcoded-0 bug, Hasil: 4 run utama post-reseed (8GB, target HANDOVER.md §2.4), Hasil: kontrol sanity (2GB, wajib per HANDOVER.md §2.3), Kenapa baru selesai sekarang (bukan 2026-09-04) (+17 more)

### Community 28 - "ra_core_orbit"
Cohesion: 0.18
Nodes (23): Erratum: Orbit-vs-Singleblock Benchmark Self-Correction, always_inline Ablation Experiment on ra_permutation_cycle_orbit, Benchmark Methodology Findings (Reinit-K vs Steady-State), bench_reinit(), bench_stream_direct(), main(), now_seconds(), Dead-Code-Elimination Bug in Ad-hoc Microbenchmark (+15 more)

### Community 29 - "Langkah 3: Interleaved 1TB for winner_wired_addressable (shared core-loop test)"
Cohesion: 0.13
Nodes (26): winner_wired_addressable Method A+B validated clean up to 128GB (xlarge tier), re-verified raw stdout this session, interleave_practrand_1tb_confirm.py (planned; reuse run_interleave_practrand with alt seeds), Shared core generation loop: ra_permutation_cycle / ra_reseed / ra_core (identical between winner_wired_v2.c and winner_wired_addressable.c; only ra_init_state[_addressable] differs), 1TB Anomaly Diagnostic Plan (Langkah 1-4), GATED_TIERS explicit-invocation gate (1tb/xlarge require explicit arg, unlike smoke/medium/full), HANDOVER_TAHAP5.md §6 convention: full Tahap 0-3 addressable-init repeat required if init formula changes, Harness bug: JSON 'passed' field mismatches lowercase PractRand 'suspicious'/'very suspicious' tags (case-sensitivity), not fixed in-place, (a) Cross-stream correlation specific to winner_wired_v2 init formula (+18 more)

### Community 30 - "exhaustive_analysis.c"
Cohesion: 0.17
Nodes (23): alloc_all(), chi2_independence(), count_low_zeroes32(), counter1_bump(), counter1_report(), ct_bump(), f_key(), fmix32() (+15 more)

### Community 31 - "winner_wired_v2.c"
Cohesion: 0.11
Nodes (27): Four-Axis Validation: periodicity, PractRand, avalanche, speed, Orbit Addressing terminology, permute + xor_fold State-Update Mechanism (kept unchanged), Point Addressing (Philox f(counter,key)), Orbit Addressing init formula (Kandidat 5), FILE, main(), ra_core() (+19 more)

### Community 32 - "tahap6_bench.c"
Cohesion: 0.19
Nodes (24): core_entry_t, FILE, find_core(), main(), mode_init_cost(), mode_reinit_sweep(), mode_throughput(), now_seconds() (+16 more)

### Community 33 - "next_state"
Cohesion: 0.15
Nodes (20): Brent's cycle-detection algorithm applied to pruned_wired_toy_prng.next_state.…, decode_batch(), main(), pack_key(), Full state-space enumeration for pruned/wired PRNG at n=4,w=4 (default rows=4…, Cross-check vec_next_state against the scalar reference on random states drawn…, rotw_vec(), validate() (+12 more)

### Community 34 - "Random"
Cohesion: 0.12
Nodes (19): chi_square_uniform(), entropy(), multi_key_distinctness_check(), Shuffle a fixed-size sample of input_tokens with n_keys different random keys;…, read_tokens(), runs_test(), serial_correlation(), multi_key_distinctness_check() (+11 more)

### Community 35 - "ra_core_param.c"
Cohesion: 0.05
Nodes (61): bench_init_generic3(), bench_init_orbit(), bench_init_singleblock(), bench_real_reinit(), bench_real_reinit4(), bench_real_stream(), bench_real_stream4(), FILE (+53 more)

### Community 36 - "cycle_analysis_weylfinal.py"
Cohesion: 0.16
Nodes (21): emit(), fmix32(), main(), make_finalizer(), Matches multikey_stream_hashed.c splitmix32(uint64_t*) exactly -- 64-bit…, splitmix32_stream(), triple32(), weyl_core() (+13 more)

### Community 37 - "bench_release.c"
Cohesion: 0.20
Nodes (22): bench_init_orbit(), bench_init_philox(), bench_init_singleblock(), bench_init_xoshiro(), bench_real_reinit(), bench_real_stream(), bench_real_stream4(), FILE (+14 more)

### Community 38 - "ra_core_v3.c"
Cohesion: 0.18
Nodes (21): main(), splitmix32(), core_entry_t, core_fn_t, FILE, checksum_key_core(), find_core(), fmix32() (+13 more)

### Community 39 - "2026-9-4_orbit-kmin-battery/run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): main(), Path, experiments/2026-9-4_orbit-kmin-battery -- dieharder for the two multikey-…, run_one(), run_test()

### Community 40 - "HANDOVER — Refinement akhir `ra_core_singleblock`: difusi-di-akhir, optimasi, eksperimen kecil"
Cohesion: 0.10
Nodes (21): Benchmark 5-angka standard (orbit K1/K255/--stream + singleblock K1/K255), Candidate w8_f10_i3 (add_a offset 64, FAIL at K=4), combo_gen.write_and_compile (candidate binary generator), shuffled_k1.txt (raw shuffle output data, K=1), shuffled_k255.txt (raw shuffle output data, K=255), 1. Baseline: `ra_core_baseline.c` (REFERENSI BEKU, jangan diedit), 2. Struktur round saat ini — di mana "difusi" terjadi, 3. Arah kerja (dari user, DICATAT SEBAGAI ARAH — bukan spek final) (+13 more)

### Community 41 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.11
Nodes (22): Brent Cycle Measure Checkpoint/Resume Architecture, Hash Depth Domination on Cycle Length, Exact State-Space In-Degree Poisson(1) Rejection, Periodicity Heuristic Validation Status Report, Dieharder DNA Test Log (Suspect/EOF Excluded), Dieharder Marsaglia-Tsang GCD Test Results, Dieharder 27 Piped Test Results, Dieharder Rewind Artifact Failure Log (+14 more)

### Community 42 - "other/avalanche_heatmap_original.py"
Cohesion: 0.14
Nodes (21): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), Original Algorithm Avalanche Diffusion Comparison (Cycle 1 vs Cycle 3), Original Algorithm Cycle 1 Avalanche Fraction (0.497576), Original Algorithm Cycle 3 Avalanche Fraction (0.499740), Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3, main() (+13 more)

### Community 43 - "ERRATA"
Cohesion: 0.11
Nodes (20): 2026-09-12 — Kesimpulan awal investigasi "orbit vs singleblock" salah (self-correction), 2026-09-12 — Off-by-one di `ra_core_orbit()` multi-cycle, ERRATA, Erratum: ra_core_orbit() Multi-cycle Off-by-one Bug, RESULTS: orbit multi-cycle off-by-one fix, Breaking change: output differs for rng≡1 mod 255, rng>255, Bukti verifikasi, pip-ra-core ctypes output-buffer +1-byte quirk (+12 more)

### Community 44 - "2026-8-25_periodicity-heuristic-validation/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 45 - "Operand-Position Rewiring Results"
Cohesion: 0.10
Nodes (21): baseline_local PractRand 16GB Test Log, v01 PractRand 16GB Test Log (FAIL), v02 PractRand 16GB Test Log (FAIL), v03 PractRand 16GB Test Log (Pass), v04 PractRand 16GB Test Log (Pass), v06 PractRand 16GB Test Log (Pass), v07 PractRand 16GB Test Log (Pass), v08 PractRand 16GB Test Log (Pass) (+13 more)

### Community 46 - "2026-8-28_periodicity-heuristic-validation2/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 47 - "measure_state_update.py"
Cohesion: 0.33
Nodes (8): avalanche_gate_min_bit_binary(), capture_first_cycle(), main(), practrand_prefix_gate_binary(), Path, Avalanche + PractRand measurement for the phase-1 (L-only) state-update-…, `CYCLES` outer cycles' worth of `c` values -- see the CAPTURE_LEN comment above…, # NOTE: unlike pruned_prng.c's CLI (quality_gate.py's convention, where

### Community 48 - "64-bit Address Widening Handover"
Cohesion: 0.13
Nodes (22): 64-bit Widening Test Battery Plan (avalanche/collision/PractRand/dieharder/bench), Candidate A: Dual-Keyterm Multiplicative Combine, Candidate B: SplitMix64-style 64-bit Pre-mix, Candidate C: param2-in-Rotation-Only, Execution Guardrails / Prohibitions for 64-bit Widening, 64-bit Address Widening Handover, Second-Parameter Naming Debate (counter vs salt/addr_hi), Section 5: 64-bit Address Space Recommendation (quantitative projection) (+14 more)

### Community 49 - "stream_values"
Cohesion: 0.14
Nodes (17): ensure_ra_prng2_cli(), ndarray, Path, Shared helpers for the parallelization-research harness. Never modifies…, Bounded capture: run `binary --stream seed n`, return n uint32 values. Suitable…, Streaming capture: caller reads/closes proc.stdout incrementally. Use for large…, Compile the optional paper-exact comparator into this folder, if not already…, stream_popen() (+9 more)

### Community 50 - "cross_correlation.py"
Cohesion: 0.17
Nodes (17): analyze_group(), build_group(), main(), build_group(), main(), Non-regression check for `ra_init_orbit` (the multikey mix-init fix candidate,…, run(), ndarray (+9 more)

### Community 51 - "interleave_practrand.py"
Cohesion: 0.20
Nodes (10): 1TB Interleaved PractRand Anomaly (3/304 tests flagged, unconfirmed), common.py (shared harness config, TIERS_Q1B), cross_correlation.py, cross_correlation_ra_prng2.py, interleave_practrand.py, interleave_practrand_ra_prng2.py, Method A: Cross-correlation analysis (lag-0 Pearson, Bonferroni), Method B: Interleaved cross-stream PractRand test (+2 more)

### Community 52 - "2026-9-1_family-productionization/ra_core.c"
Cohesion: 0.07
Nodes (48): core_entry_t, core_fn_t, checksum_key_core(), find_core(), fmix32(), GUARD_L (0x38916df4), GUARD_M (0x6c26fc92), main() (+40 more)

### Community 53 - "candidate_d.c"
Cohesion: 0.29
Nodes (14): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+6 more)

### Community 54 - "ICCS 2026 Research Paper: Array-Native Randomness"
Cohesion: 0.12
Nodes (19): Benchmark Source Comparisons Documentation, ZepFold Shuffling CLI Tool, Toy PRNG Avalanche Sanity Verification Gate, Brent Cycle Detection Methodology, Periodicity Validation Experiment Handover, Periodicity Heuristic Validation Results Report, Periodicity Heuristic Scalability & Non-Monotonicity Verdict, Initial Sweep Periodicity Measurement Summary (+11 more)

### Community 55 - "Dieharder Test Suite"
Cohesion: 0.13
Nodes (19): Dieharder Test Results (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), PractRand Test Results (ra_prng3), Dieharder Test Suite, ent Entropy Test Tool (+11 more)

### Community 56 - "interleave_practrand.py"
Cohesion: 0.11
Nodes (15): Shared ra_permutation_cycle/ra_reseed/ra_core loop, 1. Status & apa yang sudah terjadi (2026-08-31), 2. Pertanyaan yang belum terjawab, 3. Rencana diagnostik (belum dikerjakan, urutan disarankan), 4. Setelah selesai, Handover: 1TB interleaved-PractRand anomaly follow-up, Langkah 1 — Single-stream `winner_wired_v2` ke 1TB (tanpa interleave), Langkah 2 — Re-run interleaved 1TB dengan seed set berbeda (+7 more)

### Community 57 - "ra_core_v2.c"
Cohesion: 0.23
Nodes (18): core_entry_t, core_fn_t, FILE, checksum_key_core(), find_core(), fmix32(), main(), ra_core_orbit() (+10 more)

### Community 58 - "candidate_a.c"
Cohesion: 0.22
Nodes (18): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+10 more)

### Community 59 - "candidate_c.c"
Cohesion: 0.22
Nodes (17): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+9 more)

### Community 60 - "RESULTS: Priority 1 — Init-loop speed optimization"
Cohesion: 0.06
Nodes (34): PractRand gate-size trap (small gate passes, large gate FAILs), HANDOVER: Init-loop speed optimization, Critical files to reference, Deliverables, Goal -- three-tier strategy, in priority order, HANDOVER: Init-loop speed optimization untuk `ra_core_orbit` / `ra_core_singleblock`, Non-goals / hard constraints, Priority 1 (try first): find redundant/reducible operations with ZERO output change (+26 more)

### Community 61 - "State-Size Pareto Frontier Results"
Cohesion: 0.19
Nodes (15): N16 orbit: avalanche/Tier0 PASS but downstream PractRand FAIL (K=1 lane, MUTUAL continuous lane), Avalanche Heatmap: orbit N16 LOWONLY, State-Size Pareto Frontier Results, Benchmark fairness bug (extern-link vs static-inline), N=16 quality failure, orbit-LOWONLY reseed mode, orbit-MUTUAL reseed mode, off-by-one bug in ra_core_orbit (mod N-1) (+7 more)

### Community 62 - "winner_wired_addressable.c"
Cohesion: 0.17
Nodes (17): Orbit Engine, winner_wired_addressable_v2.c (planned new file if a Tahap 5 candidate is adopted -- formula-preserving + bit-identical-verified, header cites the adopted Rank number), L[256] array (dead-code discovery), Tahap 2: C Implementation, FILE, main(), Orbit Addressing formula, ra_core() (+9 more)

### Community 63 - "scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)"
Cohesion: 0.13
Nodes (18): 7. Follow-up: cycle `winner_wired_v2` + addressable init — SELESAI (2026-08-31), Double-swap hypothesis (disproven), Follow-up: varian cycle `winner_wired_v2` + addressable init (2026-08-31), scc_test_wired.py (statistical & distinctness validation, wired variant), scrambler_addressable.c (CLI), ra_init_state_addressable(L, M, key), scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI), scrambler_wired_addressable.py (Python reference port, wired variant) (+10 more)

### Community 64 - "Pareto Selection Results (Combo Winners)"
Cohesion: 0.18
Nodes (18): Sanity-check: orbit's implausible 2.4x speed vs old singleblock was a cross-session machine-speed artifact, Deviation: baseline copied from ra_core.c not benchmark_all.c (stale init formula), addrcont core, chacha20 core, dev_urandom baseline, mt19937 core, paperorig core, pcg32 core (+10 more)

### Community 65 - "practrand_gate.py"
Cohesion: 0.20
Nodes (13): HANDOVER.md Tahap E (4.4): safe-K floor scan khusus N=16 (dipicu oleh Tier1…, build_lanes(), derive_addr_hi(), iter_checkpoints(), Lane, main(), parse_size(), Path (+5 more)

### Community 66 - "ra_core_engine.c"
Cohesion: 0.20
Nodes (15): check_one(), main(), ra_core_engine_t, ra_core_engine_addr_hi(), ra_core_engine_fill_block(), ra_core_engine_fork(), ra_core_engine_init(), ra_core_engine_key() (+7 more)

### Community 67 - "ra_core_baseline.c"
Cohesion: 0.25
Nodes (17): core_entry_t, FILE, checksum_key(), find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock() (+9 more)

### Community 68 - "combo_prng_ofmix.py"
Cohesion: 0.22
Nodes (16): fmix32(), orbit_round1(), orbit_stream_full(), _production_round1(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_ofmix(), ra_reseed() (+8 more)

### Community 69 - "avalanche_effect_analysis.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_reseed(), plot_comparison(), plot_single(), Path (+9 more)

### Community 70 - "others/avalanche_heatmap_original.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_capture(), original_reseed(), plot_comparison(), plot_single() (+9 more)

### Community 71 - "pip-ra-core README"
Cohesion: 0.16
Nodes (18): addr_hi (64-bit address widening parameter), addr_hi-increment chaining pattern (key fixed, addr_hi++) - FAILED, Candidate D (64-bit address widening fix), generate(key, addr_hi, n) - stateless random-access API, SINGLEBLOCK_KAT_CHECKSUMS / ORBIT_KAT_CHECKSUMS (known-answer test vectors), key-increment chaining pattern (Weyl 0x9E3779B9, addr_hi=0) - validated safe, Lemire's fast multiply-shift range reduction method, RaCoreEngine (stateful streaming/fork/save-resume class) (+10 more)

### Community 72 - "API"
Cohesion: 0.20
Nodes (10): RaCoreBitGenerator (numpy.random.BitGenerator integration, Cython), API, Development, `generate(key, addr_hi, n, *, mode="auto") -> bytes`, Install, ra-core, `RaCoreEngine` - stateful streaming, fork, save/resume, `random_key() -> int` (+2 more)

### Community 73 - "RA-PRNG"
Cohesion: 0.12
Nodes (17): jax.random.split-style GPU parallel streams, Mersenne Twister (MT19937), NIST STS test suite, PCG, Philox4x32-10, RA-PRNG, ra_prng3 ("ZepFold", 64-bit), Same-machine, same-session benchmarking policy (+9 more)

### Community 74 - "ra_core (addressable/orbit-addressing successor)"
Cohesion: 0.29
Nodes (11): Addressable / Orbit-style Initialization Research, 2026-09-04 Release Benchmark Table, experiments/ (active research), 12-candidate PRNG family benchmark (2026-08-31), MT19937, Orbit Addressing, Philox4x32, experiments/2026-8-31_prng-family-benchmark (+3 more)

### Community 75 - "State-Update Mechanism Research Results (Phase 1-3)"
Cohesion: 0.18
Nodes (20): cycle_measure.c (C Brent cycle-length measurement), load_means(), main(), avalanche_gate_min_bit metric (per-bit floor 0.2, band [0.3,0.7]), State-Update Mechanism Research Handover (original spec), Handover Phase 3: L/M Mechanism Follow-up, inject (L) - one-directional overwrite mechanism, Instruction count (perf stat) as primary speed axis (+12 more)

### Community 76 - "Params"
Cohesion: 0.15
Nodes (10): group_size(), Params, Rescale the four fixed 32-bit plain-shift amounts to width w. s_w = round(s * w…, Top-w-bits truncation of a 32-bit golden-ratio-derived constant. Trap fix:…, G(n, rows): generalization of ra_hash's fixed 8/32 structural constants (256 =…, rescale_shifts(), truncate_const(), enumerate_state_space() (+2 more)

### Community 77 - "Candidate"
Cohesion: 0.24
Nodes (15): Candidate, final_cons(), _build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32() (+7 more)

### Community 78 - "RESULTS: production-candidate battery for `ra_core.c`"
Cohesion: 0.12
Nodes (17): clean, but a SEPARATE, more severe defect (K=1 pattern) now blocks, CRITICAL DEFECT: key=0 produces a permanent all-zero stream (both modes), further progress — gate STILL NOT PASSED, history, superseded by the rerun above for Steps 0 and 3 specifically, Original Step 0-3 run (2026-09-01, before the key=0 fix) — kept for, Provenance (Step 0), Recommendation (K=1 structural defect — NEW, open, needs a decision), Recommendation (key=0 — RESOLVED, kept for history) (+9 more)

### Community 79 - "Singleblock Cycle-Operation Combo Search Handover"
Cohesion: 0.18
Nodes (17): Singleblock Cycle-Operation Combo Search Handover, avalanche_gate_min_bit (vs scalar-average avalanche_gate), CLAUDE.md workflow rule: run /graphify --update before closing task, diag_wideo_singleblock.c / diag_wideo_warmup_singleblock.c (multikey --stream driver pattern), fmix32, Gate-size-trap incidents (long-stream gate misses defects), 2026-8-27 operand-position-search files (wiring.py, operand_search.py, other_winners_gen.py, quality_gate.py), 2026-8-26 operation-pruning-research files (quality_gate.py, operations.py, ablation_search.py, pruned_prng.c) (+9 more)

### Community 80 - "combo_prng_d0raw.py"
Cohesion: 0.22
Nodes (15): fmix32(), orbit_round1(), orbit_stream_full(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_warmup(), ra_reseed(), Pure-Python reference oracle for orbit-d0-raw-cons-fix (d0_raw_cons candidate).… (+7 more)

### Community 81 - "test_ra_core.py"
Cohesion: 0.14
Nodes (6): needs_cli, _cli_stream(), parametrize, pytest suite for pip-ra-core. Byte-identity tests shell out to the repo's own…, test_engine_next_byte_identical_to_cli_across_multiple_cycles(), test_generate_byte_identical_to_cli()

### Community 82 - "avalanche.py"
Cohesion: 0.18
Nodes (15): avalanche_gate_min_bit(), avalanche_stats(), capture_via_binary(), hamming(), main(), Path, Run `<binary> [<core>] --single <seed> <addr_hi> <cycle_words>` and parse raw…, Flip each of the 32 seed bits, capture one cycle via `capture_fn(seed) ->… (+7 more)

### Community 83 - "RaPrng2"
Cohesion: 0.18
Nodes (11): _find_lib_path(), _lib_handle(), _load_library(), CDLL, Path, RaPrng2, ctypes binding for the ra_prng2 C engine. This is glue only - it marshals calls…, Thin wrapper around one RA_PRNG instance. (+3 more)

### Community 84 - "Orbit SplitMix Finalizer Redesign Handover"
Cohesion: 0.21
Nodes (16): a=(d*o)^(cons+a) MUL-combine candidate, DC6-9x1Bytes-1 PractRand test (new K=255 regression signature), fmix32 finalizer (MurmurHash3-style), o_fmix_every_round candidate (fmix32 on o every round), o_fmix_init_only candidate (fmix32 on initial o), triple32 finalizer (Chris Wellons / hash-prospector), Weyl-constant resonance hypothesis (0x9E3779B9 vs 0x9e3779b7), weyl_fmix32 redesign candidate (+8 more)

### Community 85 - "Parallelization Research Handover"
Cohesion: 0.12
Nodes (16): Embarrassingly Parallel Architecture (per-instance state, no locks), Operand-Position Search Research (2026-8-27), Operation Pruning Research (2026-8-26), Periodicity Heuristic Validation 2 (2026-8-28), Periodicity Heuristic Validation Research (2026-8-25), Read-Only Source Constraint (never modify ra_prng2/ra_prng3/winner_wired_v2.c in place), Seed/Cycle Collision Probability (Q2), SIMD Vectorization Feasibility (Q4) (+8 more)

### Community 86 - "toy_prng.py"
Cohesion: 0.26
Nodes (13): brent(), Brent's cycle-detection algorithm applied to toy_prng.next_state. Measures the…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, enumerate_state_space(), main(), Full state-space enumeration for n=2,w=4 (|S|=131072, default rows=2). Directly…, init_state(), next_state() (+5 more)

### Community 87 - "tier0_quality.py"
Cohesion: 0.35
Nodes (9): Avalanche Heatmap: orbit N64 MUTUAL, capture(), compute_matrix(), main(), plot_heatmap(), Path, HANDOVER.md Tahap C (4.3, Tier 0): avalanche 3-arah + heatmap 32x(N-1) untuk…, run_direction() (+1 more)

### Community 88 - "stream_driven_shuffle.c"
Cohesion: 0.23
Nodes (15): Speed Benchmark Output (stream_driven_shuffle), FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash() (+7 more)

### Community 89 - "benchmark_all_run.py"
Cohesion: 0.27
Nodes (15): build_bench_all(), build_crosscheck_originals(), main(), ols_fit(), paperorig/wiredv2 originals: `bin --stream key n`. addrcont original…, Like run(), but keeps stdout as raw bytes -- for --stream binary output., run(), run_crosscheck() (+7 more)

### Community 90 - "diag_keyzero_guard.c"
Cohesion: 0.33
Nodes (15): FILE, fmix32(), is_all_zero(), main(), multikey_chain(), ra_core_orbit_stream(), ra_core_singleblock_block(), ra_hash() (+7 more)

### Community 91 - "ADDENDUM_POST_FIX_STATUS.md — Step 4-8, post `w8_f10_i0` promotion"
Cohesion: 0.12
Nodes (14): ADDENDUM_POST_FIX_STATUS.md — Step 4-8, post `w8_f10_i0` promotion, Bug found and fixed: glibc `fmemopen` exact-buffer-size clobber, Overall gate verdict: PASS, Post-fix scoring, Step 0 (rebuild) — PASS, Step 4 — collision-scan K=1: PASS, Step 5 — dieharder, K=255 & K=1: PASS, Step 6 — PractRand, K=255 & K=1, 16GB (VPS): PASS (+6 more)

### Community 92 - "diag_wideo_warmup_singleblock.c"
Cohesion: 0.10
Nodes (28): FILE, fmix32(), main(), narrow_o(), now_seconds(), pick_use_wide(), ra_core_singleblock_warmup(), ra_init_state_singleblock() (+20 more)

### Community 93 - "combo_prng_admul.py"
Cohesion: 0.23
Nodes (14): fmix32(), orbit_round1(), orbit_stream_full(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_admul(), ra_reseed(), Pure-Python reference oracle for the orbit-ad-mul-combine-fix experiment… (+6 more)

### Community 94 - "RaCoreEngine"
Cohesion: 0.14
Nodes (7): RaCoreEngine, Stateful orbit-mode stream: continues across .next() calls, can be forked,…, Total words emitted by this engine so far ("print address engine": combine with…, Generate n little-endian uint32 words (as bytes), continuing from this engine's…, A new, independent RaCoreEngine that has gone through one forced early reseed -…, Dump this engine's full state to `path` as raw bytes (binary compact format -…, Restore an engine previously written by .save(), resuming from exactly the…

### Community 95 - "RA-PRNG — Landing Page Pitch (Draft)"
Cohesion: 0.13
Nodes (15): 10. Footer, 1. Hero, 2. What it is, 3. Why it's different, 4. By the numbers, 5. How it works — two modes, pick the right one, 6. Use cases, 7. Research credibility (+7 more)

### Community 96 - "random_seeds"
Cohesion: 0.13
Nodes (20): digest_collision_prob(), main(), main(), Tahap 2 (family-productionization) follow-up, Q2 re-pointed at…, scan_fullblock(), Q2: probability of cross-stream collision/overlap between different seeds'…, Birthday-bound estimate for pure hash-digest collision probability across…, scan_blocksweep() (+12 more)

### Community 97 - "candidate_b.c"
Cohesion: 0.29
Nodes (14): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+6 more)

### Community 98 - "candidate_e.c"
Cohesion: 0.26
Nodes (15): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+7 more)

### Community 99 - "common.py (TIERS_Q2)"
Cohesion: 0.11
Nodes (18): common.py (TIERS_Q2), Path, Langkah 2 of HANDOVER_1TB_FOLLOWUP.md: re-run the interleaved 1TB Method B test…, run_live(), main(), Q3(a): wall-clock speedup/efficiency when running N independent…, Launch n concurrent --stream processes (seeds 0..n-1), each emitting n_per_proc…, run_n_concurrent() (+10 more)

### Community 100 - "Singleblock cycle-op combo search experiment (2026-09-02)"
Cohesion: 0.16
Nodes (14): Avalanche effect diagnostic (seed-bit-flip vs output-bit-difference), Avalanche heatmap test method (seed-bit-flip vs output-word bit-diff), Orbit K-small multistream defect (2026-09-04 discovery), Singleblock K-small structural defect (fixed via cycle-op combo search), w8_f10_i0 cycle-op combo candidate (tap width=8, finalizer variant 10, i0), PRNG cycle-op candidate w8_f33_i0 (tap width=8, finalizer #33, i0), Singleblock cycle-op combo search experiment (2026-09-02), Avalanche Heatmap w8_f10_i0 (K=255) (+6 more)

### Community 101 - "Axis B - Avalanche Multi-seed + Normality"
Cohesion: 0.14
Nodes (14): Axis B - Avalanche Multi-seed + Normality, Methodology caveat: Shapiro-Wilk on quantized per_bit_fraction may be a discreteness artifact, Axis A — Speed, Closing note, Deviations from HANDOVER.md (documented, both investigated before deviating), K=1 (frequent reinit) vs largest-K (steady state), Konteks, MB/s sanity check (+6 more)

### Community 102 - "avalanche_heatmap_winner.py"
Cohesion: 0.25
Nodes (13): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for the winning operand-position rewiring found by…, Mirrors winner_wired.c's ra_reseed()/ra_hash() exactly (identical to… (+5 more)

### Community 103 - "avalanche_heatmap_winner_v2.py"
Cohesion: 0.25
Nodes (13): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for `winner_wired_v2.c` (wiring "v08" in RESULTS.md's…, Mirrors winner_wired_v2.c's ra_reseed()/ra_hash() exactly (identical to… (+5 more)

### Community 104 - "Handover: Tahap 5 — Optimasi Biaya Init Addressable"
Cohesion: 0.14
Nodes (14): 1. Status & tujuan, 2. Formula & fakta struktural (sudah final, tidak perlu digali ulang), 3. Temuan disassembly (sudah dijalankan read-only, sesi persiapan 2026-08-30), 4. Kandidat optimasi, berperingkat, 5. Rencana file & eksperimen, 6. Gerbang validasi statistik (aturan mekanis, bukan penilaian bebas), 7. Batasan read-only (berlaku juga untuk Tahap 5), 8. Kondisi berhenti / pelaporan eksplisit (+6 more)

### Community 105 - "io_only_isolation_test.c"
Cohesion: 0.26
Nodes (13): FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash(), ra_init_state_addressable() (+5 more)

### Community 106 - "Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect"
Cohesion: 0.25
Nodes (14): avalanche_heatmap_winners_k255.png — heatmap visualization (32x255 cells per candidate), uniform noise pattern for all 11 winners, no dark horizontal lines (no dead bits), Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect, Promotion Tier — 29 candidates x 8 K-values x 16GB PractRand on VPS, 11/29 fully clean, Tier 0 — avalanche gate (min-per-bit, K=1, 108/108 passed), Tier 1 — multikey PractRand staged (K=1, 256MB->2GB, 30/108 survivors), w8_f22_i0 — width=8, finalizer XORSHIFT(16)->MUL(0x85ebca6b); Promotion Tier CLEAN all K@16GB; K=255 avalanche overall=0.501 min_bit=0.492 (bit5), w8_f24_i0 — width=8, finalizer XORSHIFT(16)->MUL(0x9e3779b7); Promotion Tier CLEAN all K@16GB; K=255 avalanche overall=0.500 min_bit=0.488 (bit11), w8_f25_i0 — width=8, finalizer XORSHIFT(16)->MUL(0x06a0dd9b); Promotion Tier CLEAN all K@16GB; K=255 avalanche overall=0.498 min_bit=0.486 (bit18) (+6 more)

### Community 107 - "ra_prng-family Productionization Handover"
Cohesion: 0.15
Nodes (13): CLAUDE.md (project instructions), ra_prng-family Productionization Handover, 0. Latar belakang (supaya sesi eksekusi tidak perlu riset ulang), 1. Tahap 1 (WAJIB PERTAMA): Unifikasi `winner_wired_v2.c` + `winner_wired_addressable.c`, 2. Tahap 2 (SETELAH Tahap 1 selesai & dikonfirmasi user): Folder/battery validasi produksi, 3. Non-goals eksplisit (supaya sesi eksekusi tidak overscope), 4. Open decisions untuk sesi eksekusi (BELUM diputuskan user — jangan diasumsikan, tanya dulu), 5. Penutup sesi (+5 more)

### Community 108 - "K=1 Exhaustive Function Analysis Handover"
Cohesion: 0.22
Nodes (13): /dev/urandom key-source diagnosis (defect is intrinsic, not SplitMix32-specific), K=1 Exhaustive Function Analysis Handover, PractRand FPF-14+6/16 test, Hashed key (SplitMix32-chain), Orbit K=1 PractRand Defect, 10-11 exhausted guess-and-check fix axes, /dev/urandom key, Weyl-ordering key (key=base+idx*0x9E3779B9) (+5 more)

### Community 109 - "Fisher–Yates shuffle"
Cohesion: 0.15
Nodes (13): Fisher–Yates shuffle, Array Index Shuffling, Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), Bitwise Rotation (rotl32), R. Durstenfeld, Mutate Operator, Rationale: Non-Invertible Hash Reseed Trade-off (Reduces Period, Aids Diffusion) (+5 more)

### Community 110 - "2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py"
Cohesion: 0.23
Nodes (12): decode_batch(), main(), pack_key(), Full state-space enumeration for n=4,w=4 (default rows=4 -> G=1), |S| = 4! *…, Cross-check vec_next_state against the scalar reference on random states drawn…, Pack (L, M, cons, it) arrays into a single uint64 key array, w bits per field,…, Mixed-radix decompose a flat index array into (lperm, m_0..m_{n-1}, it)…, Vectorized rotw: rotate low w bits of x left by r bits (mod w). x, r are numpy… (+4 more)

### Community 111 - "2026-8-27_operand-position-search/baseline.c"
Cohesion: 0.24
Nodes (10): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32() (+2 more)

### Community 112 - "4. Rencana implementasi"
Cohesion: 0.15
Nodes (13): 1. Latar belakang & tujuan riset, 2. Keputusan desain — SUDAH DIKONFIRMASI USER (AskUserQuestion, 2026-09-11), 3. Latar teknis kunci (dari eksplorasi kode, sesi 2026-09-11), 4.1 Core terparametrisasi — `ra_core_param.c`, 4.2 Gerbang period teoretis (pre-check, sebelum PractRand), 4.3 Validasi kualitas bertingkat, 4.4 Safe-K floor per-N, 4.5 Benchmark kecepatan (+5 more)

### Community 113 - "microbench_orbit_vs_singleblock.c"
Cohesion: 0.36
Nodes (12): bench_orbit_true(), bench_singleblock_true(), cycle_orbit(), cycle_singleblock(), fmix32(), init_state_orbit(), init_state_singleblock(), main() (+4 more)

### Community 114 - "avalanche_ra_core.py"
Cohesion: 0.26
Nodes (11): capture_cycle(), main(), Tahap 2 (family-productionization): avalanche gate for ra_core.c's two…, run_gate(), compute_curve(), main(), plot_core(), ndarray (+3 more)

### Community 115 - "workload_a_singleblock_pi.c"
Cohesion: 0.21
Nodes (16): bench_and_report(), pcg32_random_t, main(), now_seconds(), pcg32_into(), pcg32_random_r(), pcg32_srandom(), ra_core_singleblock_into() (+8 more)

### Community 116 - "diag_prereseed.c"
Cohesion: 0.31
Nodes (12): core_fn_t, FILE, fmix32(), main(), pick_variant(), ra_core_singleblock_baseline(), ra_core_singleblock_prereseed(), ra_hash() (+4 more)

### Community 117 - "HANDOVER: Orbit K-kecil defect fix + rolling-register optimasi wide-o"
Cohesion: 0.07
Nodes (37): dieharder_winner_wired_v2_piped.txt (precedent: -d 200 empty, same WEAK pattern), dieharder ra_core_singleblock multikey piped results, HANDOVER: Orbit K-kecil defect fix + rolling-register optimasi wide-o, 1. Temuan awal: `ra_core_orbit` rentan defek K-kecil, 2. Keputusan user (AskUserQuestion + pesan lanjutan), 3. Insight user: rolling-register untuk wide-`o`, 4. Perubahan di `ra_core_v2.c`, 5. Verifikasi yang sudah dijalankan (+29 more)

### Community 118 - "combo_prng_orbit.py"
Cohesion: 0.21
Nodes (16): apply_cycle(), _cycle_variant_lookup(), fmix32(), _init_variant_lookup(), orbit_stream_full(), ra_hash(), ra_init_state_orbit_variant(), ra_permutation_cycle_orbit_variant() (+8 more)

### Community 119 - "bit_bias_analysis.py"
Cohesion: 0.15
Nodes (21): bit_frequencies(), bit_frequencies_fast(), iter_words(), main(), Path, Part B: bit/position bias localization for the K=1+hashed-key defect. Reads raw…, Return (n_words, counts[32]) where counts[b] = number of words with bit b set., Vectorized-ish version: unpack in bulk, tally via bit masks per chunk. (+13 more)

### Community 120 - "position_split_orbit.c"
Cohesion: 0.37
Nodes (12): FILE, fmix32(), main(), ra_core_orbit(), ra_core_orbit_split(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit() (+4 more)

### Community 121 - "ra_core/__init__.py"
Cohesion: 0.18
Nodes (10): generate(), _mask32(), random_key(), ra_core: pip binding for the ra_core PRNG (orbit + singleblock). Headline…, Random-access generate: n little-endian uint32 words (as bytes) from the…, n independent RaCoreEngine streams for the same key, addr_hi = addr_hi_start,…, Deterministic Fisher-Yates shuffle of seq, driven by ra_core. Same (key, seq…, A fresh 32-bit key from OS entropy, for callers that don't want to hardcode one. (+2 more)

### Community 122 - "BuildPyWithCEngine"
Cohesion: 0.18
Nodes (9): BinaryDistribution, BuildPyWithCEngine, _build_py, Distribution, Path, Imperative build customization pyproject.toml's static [tool.setuptools] table…, After the normal .py copy step, compiles the ra_core engine and drops…, Declares this distribution as containing compiled, platform-specific code… (+1 more)

### Community 123 - "quality_gate.py (avalanche_stats)"
Cohesion: 0.19
Nodes (11): main(), Post-convergence "shift repair" search. User's request after seeing the winning…, score(), capture_one(), main(), production-candidate-battery, Step 1: avalanche gate for `singleblock` K=1 mode…, capture(), main() (+3 more)

### Community 124 - "Avalanche Effect (Bit-Flip Sensitivity)"
Cohesion: 0.17
Nodes (12): pruned_winner Avalanche Fraction Metric (Cycle 1 vs Cycle 90), Avalanche Effect Heatmap: pruned_winner Cycle 1 vs Cycle 90, Multi-Cycle Reseed Avalanche Diffusion Improvement (0.418 to 0.468), winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039), Avalanche Effect Heatmap (winner_wired_v2), winner_wired_v2 Wiring Parameters (a_xor=d, c_shift=a, rotc_amount=b, rotc_xor=a), Avalanche Effect (Bit-Flip Sensitivity), Joan Daemen and Vincent Rijmen (+4 more)

### Community 125 - "2026-8-28_periodicity-heuristic-validation2/run_sweep.py"
Cohesion: 0.26
Nodes (11): brent(), cycle_measure Binary Exit-42 Crash at n=8,w=4,rows=4, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, log2_fact(), main(), predicted(), Drives the pruned/wired periodicity-heuristic sweep. Same lambda_pred formula…, result_csv_name() (+3 more)

### Community 126 - "Design decisions (read before touching `pruned_wired_toy_prng.py`)"
Cohesion: 0.17
Nodes (11): 1. Op/wiring mapping ported to toy scale, 2. Tap-survivor generalization: top-2-by-count, and why, 3. Tap shift-width collapse (caught during design, not present in either parent), 4. `ra_hash_gen` divergence from ground truth (found while reading source, not assumed), 5. Self-check (passed) -- MUST stay passing, CRITICAL BUG found and fixed this session -- read before writing more scripts, Design decisions (read before touching `pruned_wired_toy_prng.py`), HANDOVER.md (periodicity-heuristic-validation2) (+3 more)

### Community 127 - "Tahap 5 goal: reduce ra_init_state_addressable init cost (~49.87ns/call, 4.96x Philox) to push N*=49/K*=71 break-even lower, formula-preserving only (optimization, not formula redesign)"
Cohesion: 0.17
Nodes (12): JAX, Philox (counter-mode PRNG), Tahap 5 goal: reduce ra_init_state_addressable init cost (~49.87ns/call, 4.96x Philox) to push N*=49/K*=71 break-even lower, formula-preserving only (optimization, not formula redesign), Rank 1: -mprefer-vector-width=512 recompile test (flag only, no code change; must be measured end-to-end via mode_throughput/mode_reinit_sweep for AVX-512 downclocking risk, not mode_init_cost alone), Rank 2: vprolvd/vprorvd (_mm256_rolv_epi32) intrinsic substitution for rot32 in ra_init_state_addressable -- structurally different from Q4's failed case since this loop is pure independent write with no cross-lane dependency, Rank 3: loop unrolling (-funroll-loops flag first, then manual 2-chunk dual-accumulator unroll to break the single loop-carried dependency chain observed in disassembly), Rank 4 (deprioritized, reference only): full hand-SIMD rewrite of ra_init_state_addressable using AVX-512 (ZMM 16-lane) -- only if Rank 1-3 are exhausted and user explicitly wants to continue, Ruled out for Tahap 5: reducing the 256-iteration count of L[256]/M[256] -- would require a new formula since ra_permutation_cycle/ra_reseed index the full array 0-255, deferred as a possible separate 'Tahap 6' (full formula redesign + Tahap 0-3 re-run) (+4 more)

### Community 128 - "ra_core_singleblock"
Cohesion: 0.22
Nodes (13): winner_wired_v2.c, HANDOVER_TAHAP6.md, Tahap 6 hard scope limit (rng<=255, hard-abort guard), ra_core_singleblock(), winner_wired_addressable.c, Candidate: addressable agresif K=255 (singleblock), prng-family-benchmark experiment (12 candidates), ra_prng-family (unified codebase concept) (+5 more)

### Community 129 - "pareto_select.py"
Cohesion: 0.23
Nodes (10): dominates(), load_jsonl(), main(), Path, Pareto frontier selection (HANDOVER.md sect 4) over the 11 combo-search…, True if candidate a dominates candidate b: a is >= b on all 3 objectives (in…, main(), Axis A orchestrator (HANDOVER.md sect 2): K=1 / K=255 reinit-sweep and… (+2 more)

### Community 130 - "Wiring"
Cohesion: 0.33
Nodes (11): _capture_first_cycle(), _log(), main(), Driver for the operand-position rewiring search over wiring.py's 108 candidate…, tier0_avalanche(), tier1_practrand_small(), all_wirings(), describe() (+3 more)

### Community 131 - "control.c"
Cohesion: 0.33
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_init_warmup_orbit(), ra_permutation_cycle_orbit_combo() (+3 more)

### Community 132 - "d0_hash.c"
Cohesion: 0.35
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_init_warmup_orbit(), ra_permutation_cycle_orbit_combo() (+3 more)

### Community 133 - "d0_realround.c"
Cohesion: 0.35
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_init_warmup_orbit(), ra_permutation_cycle_orbit_combo() (+3 more)

### Community 134 - "o_fmix_every_round.c"
Cohesion: 0.35
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_ofmix(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_ofmix(), ra_reseed() (+3 more)

### Community 135 - "o_fmix_init_only.c"
Cohesion: 0.35
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_ofmix(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_ofmix(), ra_reseed() (+3 more)

### Community 136 - "d0_raw_cons.c"
Cohesion: 0.33
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit(), ra_init_warmup_orbit(), ra_permutation_cycle_orbit_combo() (+3 more)

### Community 137 - "winner_wired.c"
Cohesion: 0.31
Nodes (10): avalanche_gate_min_bit() / avalanche_stats() min-bit-floor gate, FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed() (+2 more)

### Community 138 - "RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)"
Cohesion: 0.20
Nodes (11): Split-loop fix for variable-length M[] init (avoids masked-wrap vectorization regression), variable-length M[] init optimization (singleblock, split-loop fix), RESULTS: Tahap 1 Unifikasi ra_core.c, Catatan scope eksplisit, File yang dihasilkan sesi ini, Keputusan penamaan & scope (dikonfirmasi user, sesi ini), Non-goals (tidak dikerjakan sesi ini, sesuai HANDOVER §3), RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock) (+3 more)

### Community 139 - "2025-10-5_scramble-design/scc_test.py"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 140 - "pruned_prng.c"
Cohesion: 0.38
Nodes (10): build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32(), FILE, has() (+2 more)

### Community 141 - "Cycle 1 Initial State Avalanche Fraction (0.487783)"
Cohesion: 0.29
Nodes (11): Avalanche Metrics (overall=0.487783, min_bit=0.472304), Warmup Cycle Avalanche Effect and Bit Diffusion, Cycle 1 Initial State Avalanche Fraction (0.487783), Cycle 3 Converged Avalanche Fraction (0.500751), Winner Wired Avalanche Heatmap Cycle 1 vs Cycle 3, Winner Wired Wiring Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a), Cycle 4 Steady State Avalanche (fraction 0.499713), Avalanche Effect Heatmap Comparison: Cycle 1 vs Cycle 4 (+3 more)

### Community 142 - "Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)"
Cohesion: 0.18
Nodes (10): 1. Summary Comparison: Unpruned Original vs `winner_wired_v2`, 2. Sebaran Siklus Utama (*Distinct $\lambda$ Distribution*), 3. Enumerasi Penuh & Uji Chi-Square vs Poisson(1), 4. Analisis Temuan Kunci (*Key Findings*), 5. Catatan Metodologi & Caveats (Sesuai `HANDOVER.md`), 6. Kesimpulan (*Verdict*), A. Skala $n=2, w=4$ ($|S| = 131.072$) — [`enumerate_n2w4_result.json`](./enumerate_n2w4_result.json), B. Skala $n=4, w=4$ ($|S| = 402.653.184$) — [`enumerate_n4w4_result.json`](./enumerate_n4w4_result.json) (+2 more)

### Community 143 - "tahap5_benchmark.py"
Cohesion: 0.36
Nodes (10): find_crossover(), main(), _parse_rows(), Path, Tahap 5: orchestrator for tahap5_bench / tahap5_bench_zmm512 /…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep() (+2 more)

### Community 144 - "tahap6_benchmark.py"
Cohesion: 0.33
Nodes (10): find_crossover(), main(), _parse_rows(), Tahap 6: orchestrator for tahap6_bench (C harness does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep(), run_throughput() (+2 more)

### Community 145 - "RESULTS: key=0 degenerate-state guard-XOR fix"
Cohesion: 0.18
Nodes (11): 1. Diagnostic candidate — `diag_keyzero_guard.c`, 2. Edge-case check — PASS, 3. BCFN non-regression — K=255 multikey pattern: PASS, 32GB clean, 4. Non-regression — `ra_core validate` after applying to `ra_core.c`, Applied to `ra_core.c`, CRITICAL — separate, pre-existing defect found: K=1 singleblock chaining catastrophically FAILs PractRand, Guard constants, RESULTS: key=0 degenerate-state guard-XOR fix (+3 more)

### Community 146 - "combo_prng.py"
Cohesion: 0.22
Nodes (16): apply_finalizer(), fmix32(), ra_init_state_singleblock(), Pure-Python reference oracle for the cycle-operation combo DSL. Used by Tier 0…, Single round1 output word: matches ra_core_singleblock(key, rng=1) with…, Full multi-round stream for one ra_core_singleblock_combo(key, rng) call, rng…, rot32(), round1() (+8 more)

### Community 147 - "source/philox.c"
Cohesion: 0.26
Nodes (10): main(), philox4x32_10(), philox4x32_round(), main(), philox4x32_10(), philox4x32_round(), philox_rand32(), rand_bounded() (+2 more)

### Community 148 - "ad_mul_combine.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_admul(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_admul(), ra_reseed() (+2 more)

### Community 149 - "init_hashed_per_index__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 150 - "init_hashed_per_index__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 151 - "init_hashed_per_index__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 152 - "init_hashed_per_index__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 153 - "init_hashed_shared__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 154 - "init_hashed_shared__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 155 - "init_hashed_shared__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 156 - "init_hashed_shared__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 157 - "init_linear_p1_per_index__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 158 - "init_linear_p1_per_index__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 159 - "init_linear_p1_per_index__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 160 - "init_linear_p1_per_index__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 161 - "init_linear_p1_shared__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 162 - "init_linear_p1_shared__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 163 - "init_linear_p1_shared__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 164 - "init_linear_p1_shared__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 165 - "init_linear_per_index__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 166 - "init_linear_per_index__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 167 - "init_linear_per_index__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 168 - "init_linear_per_index__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 169 - "init_linear_shared__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 170 - "init_linear_shared__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 171 - "init_linear_shared__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 172 - "init_linear_shared__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 173 - "init_odd_per_index__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 174 - "init_odd_per_index__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 175 - "init_odd_per_index__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 176 - "init_odd_per_index__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 177 - "init_odd_shared__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 178 - "init_odd_shared__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 179 - "init_odd_shared__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 180 - "init_odd_shared__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 181 - "Hasil: search init-formula × cycle-transform untuk `ra_core_orbit` (dual key-ordering)"
Cohesion: 0.18
Nodes (11): 1. Konteks, 2. Validasi infra (langkah 1-2 pipeline), 3. Tier 0 — avalanche gate (K=1), 4. Tier 1 — PractRand dual-ordering lokal, 5. Analisis pola kegagalan, 6. Kesimpulan dan langkah berikut, 7. File terkait, Hasil: search init-formula × cycle-transform untuk `ra_core_orbit` (dual key-ordering) (+3 more)

### Community 182 - "tier1_gate_warmup.py"
Cohesion: 0.31
Nodes (10): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the orbit init-warmup fix experiment,…, Run both orderings for each (variant, K) pair; a pair survives only if BOTH are…, Yield one full checkpoint block (str) at a time from RNG_test's stdout, as… (+2 more)

### Community 183 - "multikey_stream_orbit_altconst.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed() (+2 more)

### Community 184 - "bench_addr_hi.c"
Cohesion: 0.42
Nodes (10): core_entry_t, find_core(), main(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10(), philox4x32_round() (+2 more)

### Community 185 - "tier1_gate_d0raw.py"
Cohesion: 0.31
Nodes (10): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the orbit init-warmup fix experiment,…, Run both orderings for each (variant, K) pair; a pair survives only if BOTH are…, Yield one full checkpoint block (str) at a time from RNG_test's stdout, as… (+2 more)

### Community 186 - "Handover: redesign arsitektur "Weyl-add + finalizer 32-bit murni" untuk defect K-small `ra_core_orbit`"
Cohesion: 0.18
Nodes (11): 0. WAJIB: checkpoint ke user sebelum implementasi, 1. Kenapa handover ini ditulis (ringkasan, baca sumber untuk detail lengkap — JANGAN re-derive dari nol), 2. Insight arsitektural (dari diskusi sesi ini) dan scope, 3. Kandidat A — `weyl_fmix32` (finalizer termurah, precedent sudah ada di repo), 4. Kandidat B — `weyl_triple32` (finalizer lebih kuat, fallback kalau A gagal Tier 1), 5. Struktur kode yang harus dibuat, 6. Pipeline validasi (urutan wajib, disiplin sama seperti semua sesi K-small sebelumnya), 7. Kriteria hasil (+3 more)

### Community 187 - "BuildPyWithCEngine"
Cohesion: 0.20
Nodes (7): BinaryDistribution, BuildPyWithCEngine, _build_py, Distribution, Imperative build customization pyproject.toml's static [tool.setuptools] table…, After the normal .py copy step, compiles the ra_prng2 C engine and drops…, Declares this distribution as containing compiled, platform-specific code…

### Community 188 - "tahap3_collision_scan.py"
Cohesion: 0.27
Nodes (8): collision_scan.py, main(), Non-regression check for `ra_init_orbit` (the multikey mix-init fix candidate,…, run_sequential(), main(), Tahap 3, Q2 (re-pointed): cross-key collision/overlap scan for…, run_random(), run_sequential()

### Community 189 - "orbit-round1-collision-position-isolation -- hasil (diagnosis only)"
Cohesion: 0.22
Nodes (10): d0_raw_cons candidate (d = cons & 0xFF, no hash), Round-1 a=cons self-collision hypothesis, Orbit d0-Raw-Cons Fix Results, Orbit Round1-Collision Position Isolation Results, 1. Driver `position_split_orbit.c` -- verifikasi wajib, 2. Stage 1: K=2, hashed, base_key=0x12345, staged 64MB->1GB per substream, 3. Interpretasi: hipotesis collision REFUTED sebagai penjelasan tunggal, 4. Status Stage 2 (K=255 asli) (+2 more)

### Community 190 - "w8_f25_i0.c"
Cohesion: 0.38
Nodes (9): Pareto Frontier Selection (3 objectives), FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo() (+1 more)

### Community 191 - "tahap4_benchmark.py"
Cohesion: 0.36
Nodes (9): find_crossover(), main(), _parse_rows(), Tahap 4: orchestrator for tahap4_bench (C harness, does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word., run_init_cost(), run_reinit_sweep(), run_throughput() (+1 more)

### Community 192 - "scrambler_addressable.c"
Cohesion: 0.38
Nodes (9): main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 193 - "scrambler_wired_addressable.c"
Cohesion: 0.38
Nodes (9): main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 194 - "single_mread_isolation_test.c"
Cohesion: 0.38
Nodes (9): Options, load_tokens_from_file(), main(), parse_args(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32() (+1 more)

### Community 195 - "diag_hashed_init.c"
Cohesion: 0.40
Nodes (9): FILE, main(), ra_core_diag(), ra_hash(), ra_init_state_hashed_key(), ra_permutation_cycle(), ra_reseed(), rot32() (+1 more)

### Community 196 - "diag_orbit_regression.c"
Cohesion: 0.40
Nodes (9): FILE, fmix32(), main(), ra_core_orbit_stream(), ra_hash(), ra_init_orbit(), ra_permutation_cycle_orbit(), ra_reseed() (+1 more)

### Community 197 - "promotion_search.py"
Cohesion: 0.16
Nodes (18): BCFN structural defect (K-small), compile_candidate(), iter_checkpoints(), _log(), main(), Path, Promotion tier: runs ON THE VPS (not locally -- 8 K-values x up to 29…, Yield one full checkpoint block (str) at a time from RNG_test's stdout, as… (+10 more)

### Community 198 - "Handover: eliminasi 11 pemenang combo-search via Pareto (speed x avalanche), lalu posisikan vs keluarga PRNG lain"
Cohesion: 0.20
Nodes (10): 1. Konteks & tujuan, 2. Axis A — Speed: K=1, K=255, continuous-stream, 3. Axis B — Avalanche multi-seed + uji normalitas, 4. Pareto frontier selection, 5. Perbandingan vs keluarga PRNG lain — REUSE tabel lama, JANGAN tes ulang, 6. File yang TIDAK BOLEH diubah (read-only precedent), 7. Struktur file baru (folder ini), 8. Non-goals (+2 more)

### Community 199 - "Hasil: kandidat fix `fmix32(o)` untuk defect `ra_core_orbit` K=1+hashed"
Cohesion: 0.20
Nodes (10): 1. Konteks dan motivasi, 2. Kandidat yang diuji, 3. Byte-exact C vs oracle (WAJIB lolos sebelum PractRand), 4. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2), 5. Tier 1: PractRand dual-ordering staged, K=1 dan K=255, skala 2GB, 6. Overhead (ringan, timing tunggal K=255 stream 20M word), 7. Verdict dan insight penting, 8. Yang TIDAK dilakukan sesi ini (di luar scope, per persetujuan awal) (+2 more)

### Community 200 - "fpf_scan_addr_hi.c"
Cohesion: 0.38
Nodes (9): count_low_zeroes32(), fpf_bump(), fpf_process_word(), fpf_report(), gen_urandom_word(), main(), now_sec(), real_orbit_word() (+1 more)

### Community 201 - "RESULTS: Weyl-add + fmix32/triple32 finalizer redesign — both candidates FAIL"
Cohesion: 0.20
Nodes (10): 0. Scope executed vs handover, 1. Core code (both candidates, full generator — no L[]/M[]/255-round loop), 2.1 Byte-exact oracle vs C (`combo_prng_weylfinal.py`), 2.2 Tier 0 avalanche (K=1, band 0.3-0.7, min-bit floor 0.2), 2.3 Tier 1 PractRand, 8GB, dual-ordering, staged (A/B/C), 2.4 Benchmark, 2. Pipeline results, 3. Analysis (+2 more)

### Community 202 - "_engine.py"
Cohesion: 0.29
Nodes (9): alloc_aligned_state(), engine_struct_size(), _find_lib_path(), _lib_handle(), _load_library(), CDLL, Path, ctypes binding for the ra_core engine (src/ra_core/c/ra_core_engine.c). This is… (+1 more)

### Community 203 - "pcg_amortized.c"
Cohesion: 0.44
Nodes (8): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), pcg32_buffered_t

### Community 204 - "source/ra_prng2.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 205 - "source/ra_prng3.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 206 - "scrambler.c"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 207 - "2026-8-25_periodicity-heuristic-validation/avalanche_check.py"
Cohesion: 0.31
Nodes (8): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for toy_prng.py. Port of…, One outer iteration, no rehash - mirrors zepfold_capture() in…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), Rotate the low w bits of x left by r bits (mod w)., rotw()

### Community 208 - "Tahap 2: Folder/Battery Validasi Produksi"
Cohesion: 0.22
Nodes (9): dieharder_pruned_winner_piped.txt, parallelization-research experiment (2026-8-29), scc_test.py, verify_parity.py, Min-of-Trials Benchmarking Pattern, Tahap 2: Folder/Battery Validasi Produksi, avalanche_gate_min_bit(), quality_gate.py (+1 more)

### Community 209 - "pruned_winner.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 210 - "pruned_winner_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 211 - "baseline_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 212 - "v01.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 213 - "v02.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 214 - "v03.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 215 - "v04.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 216 - "v06.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 217 - "v07.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 218 - "v08.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 219 - "v09.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 220 - "v10.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 221 - "v11.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 222 - "v12.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 223 - "v13.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 224 - "original_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 225 - "original_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 226 - "original_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 227 - "original_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 228 - "original_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 229 - "original_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 230 - "winner_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 231 - "winner_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 232 - "winner_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 233 - "winner_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 234 - "winner_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 235 - "winner_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 236 - "simd_prototype.c"
Cohesion: 0.33
Nodes (8): main(), rot32_avx2(), rot32_scalar(), run_scalar(), run_simd(), Read-only constraint (never modify src/ra_prng2, ra_prng3, winner_wired_v2.c), Tahap 5 read-only constraints: winner_wired_addressable.c and tahap4_bench.c (established read-only precedent), plus earlier-stage read-only files/dirs (src/ra_prng2, src/ra_prng3, winner_wired*.c, 2026-8-29_parallelization-research/*.py and simd_prototype.c, 3 buggy Philox files) -- all Tahap 5 candidates must be new files, __m256i

### Community 237 - "Speed Benchmark: `scrambler_addressable` vs Algoritma Lain"
Cohesion: 0.22
Nodes (8): addressable-init-research/RESULTS.md (Tahap 3/6 PractRand 128GB + K* reinit-sweep crossover), Hasil (elapsed time, detik, `perf stat -r 30`), Interpretasi, Konteks tabel lama, Metodologi, Pertanyaan, Reproduksi, Speed Benchmark: `scrambler_addressable` vs Algoritma Lain

### Community 238 - "Handover: Addressable-Init Data Shuffling & Speed Investigation"
Cohesion: 0.22
Nodes (8): 1. Ringkasan alur & tujuan, 2. File & perannya (peta folder `experiments/2026-8-30_addressable-shuffle/`), 3. Hasil kunci (sudah final, jangan diulang), 4. Reproduksi cepat, 5. Constraint yang diwarisi, 6. Follow-up: SELESAI (2026-08-30, sesi lanjutan), 7. Setelah mengerjakan follow-up di atas, Handover: Addressable-Init Data Shuffling & Speed Investigation

### Community 239 - "Hasil validasi"
Cohesion: 0.22
Nodes (9): Addressable-Init Data Shuffling Results, 1. Smoke test, 2. Parity check (`verify_parity.py`), 3. Statistik kualitas shuffle (`scc_test.py`, key=1, n=100.000), 4. Multi-key distinctness spot-check (`scc_test.py`), Apa ini, Build & run, Follow-up (belum dilakukan sekarang) (+1 more)

### Community 240 - "scrambler_addressable.py"
Cohesion: 0.42
Nodes (8): load_token_ids_from_file(), main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Byte-for-byte port of winner_wired_addressable.c's function of the same name…, save_token_ids_to_file()

### Community 241 - "scrambler_wired_addressable.py"
Cohesion: 0.42
Nodes (8): load_token_ids_from_file(), main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Byte-for-byte port of winner_wired_addressable.c's function of the same name…, save_token_ids_to_file()

### Community 242 - "no_reseed_isolation_test.c"
Cohesion: 0.42
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), ra_init_state_addressable(), ra_shuffle(), rot32(), save_tokens_to_file()

### Community 243 - "period_precheck.py"
Cohesion: 0.33
Nodes (8): fmt_log2(), log2_fact(), main(), predicted_orbit_log2(), predicted_singleblock_log2(), HANDOVER.md Tahap B (4.2): gerbang period teoretis, sebelum kompilasi…, Returns (log2|S|, log2(lambda_pred)). lambda_pred itself overflows a Python…, Render a huge log2-scale value as 'a x 10^b' without ever materializing the…

### Community 244 - "`ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi"
Cohesion: 0.15
Nodes (13): 1. Avalanche (`avalanche_ra_core.py`, reuse `quality_gate.py`'s `avalanche_gate_min_bit()`), 2. Speed (`bench_ra_core.c`, pola `tahap6_bench.c`), 3. Cross-correlation, multi-key (`cross_correlation_ra_core_singleblock.py`, tier "full": K=512, n=255), 4. Collision-scan, multi-key (`collision_scan_ra_core_singleblock.py`, tier "full": M=50.000, satu blok penuh 255-kata per key), 5. Dieharder "Good" battery, stream multi-key (`multikey_stream` + `run_dieharder_battery.py` reused), 6. PractRand bertahap, stream multi-key (`multikey_stream` -> `RNG_test stdin32`), Belum dikerjakan / perlu keputusan user, File yang dihasilkan sesi ini (+5 more)

### Community 245 - "bench_init_avalanche.c"
Cohesion: 0.50
Nodes (8): core_avalanche(), core_baseline(), fmix32(), main(), ra_cycle(), ra_init_state_avalanche(), ra_init_state_orbit(), rot32()

### Community 246 - "bench_init_keyterm_mul.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_keyterm_mul(), fmix32(), main(), ra_cycle(), ra_init_state_keyterm_mul(), ra_init_state_orbit(), rot32()

### Community 247 - "bench_warmup_cost.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_warmup(), main(), ra_cycle(), ra_hash(), ra_init_state_orbit(), ra_reseed(), rot32()

### Community 248 - "bench_warmup_cost2.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_warmup(), main(), ra_cycle(), ra_hash(), ra_init_state_orbit(), ra_reseed(), rot32()

### Community 249 - "diag_init_keyterm_mul.c"
Cohesion: 0.47
Nodes (8): FILE, fmix32(), main(), ra_core_orbit(), ra_init_orbit(), ra_init_singleblock(), ra_permutation_cycle_orbit(), rot32()

### Community 250 - "diag_paperlike_cycle.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_diag(), ra_hash(), ra_init_state_addressable(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 251 - "diag_warmup_depth.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_warmup(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed(), rot32()

### Community 252 - "diag_warmup_discard.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_warmup(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed(), rot32()

### Community 253 - "2026-9-2_singleblock-cycle-combo-search/candidates/w2_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 254 - "w2_f10_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 255 - "w2_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 256 - "w2_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 257 - "w2_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 258 - "w2_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 259 - "w2_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 260 - "w2_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 261 - "w2_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 262 - "w2_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 263 - "w2_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 264 - "w2_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 265 - "w2_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 266 - "w2_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 267 - "w2_f22_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 268 - "w2_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 269 - "w2_f24_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 270 - "w2_f25_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 271 - "w2_f26_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 272 - "w2_f27_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 273 - "w2_f28_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 274 - "w2_f29_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 275 - "w2_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 276 - "w2_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 277 - "w2_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 278 - "w2_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 279 - "w2_f33_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 280 - "w2_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 281 - "w2_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 282 - "w2_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 283 - "w2_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 284 - "w2_f5_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 285 - "w2_f6_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 286 - "w2_f7_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 287 - "w2_f8_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 288 - "w2_f9_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 289 - "w4_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 290 - "w4_f10_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 291 - "w4_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 292 - "w4_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 293 - "w4_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 294 - "w4_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 295 - "w4_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 296 - "w4_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 297 - "w4_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 298 - "w4_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 299 - "w4_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 300 - "w4_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 301 - "w4_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 302 - "w4_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 303 - "w4_f22_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 304 - "w4_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 305 - "w4_f24_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 306 - "w4_f25_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 307 - "w4_f26_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 308 - "w4_f27_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 309 - "w4_f28_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 310 - "w4_f29_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 311 - "w4_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 312 - "w4_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 313 - "w4_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 314 - "w4_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 315 - "w4_f33_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 316 - "w4_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 317 - "w4_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 318 - "w4_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 319 - "w4_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 320 - "w4_f5_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 321 - "w4_f6_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 322 - "w4_f7_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 323 - "w4_f8_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 324 - "w4_f9_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 325 - "w8_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 326 - "w8_f10_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 327 - "w8_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 328 - "w8_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 329 - "w8_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 330 - "w8_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 331 - "w8_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 332 - "w8_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 333 - "w8_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 334 - "w8_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 335 - "w8_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 336 - "w8_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 337 - "w8_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 338 - "w8_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 339 - "w8_f22_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 340 - "w8_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 341 - "w8_f24_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 342 - "w8_f26_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 343 - "w8_f27_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 344 - "w8_f28_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 345 - "w8_f29_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 346 - "w8_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 347 - "w8_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 348 - "w8_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 349 - "w8_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 350 - "w8_f33_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 351 - "w8_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 352 - "w8_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 353 - "w8_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 354 - "w8_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 355 - "w8_f5_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 356 - "w8_f6_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 357 - "w8_f7_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 358 - "RESULTS: continuous-stream real benchmark untuk `ra_core_orbit`"
Cohesion: 0.17
Nodes (12): Closing note, Continuous-stream, sesi/mesin yang sama (hari ini), Deviasi dari rencana: sanity-check tambahan, Diskusi, Hasil, Kesimpulan, Konteks, Metodologi (+4 more)

### Community 359 - "w8_f9_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 360 - "BCFN/TMFn/FPF structural defect at small K"
Cohesion: 0.25
Nodes (9): BCFN/TMFn/FPF structural defect at small K, bench_ra_core.c (reinit-sweep microbench pattern), Kandidat A: widened o (2-tap to 8-tap XOR), Kandidat B: widened o + N discarded warm-up rounds, Standing rule: no re-seed retries on PractRand FAIL, RESULTS.md deliverable spec for this experiment, singleblock-k-threshold-characterization RESULTS, singleblock-o-width-fix experiment (prior) (+1 more)

### Community 361 - "HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup"
Cohesion: 0.22
Nodes (9): Cara lanjut di sesi berikutnya, Context, HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup, Ide fix dari user (2 tahap, verbatim diparafrase), Kandidat A — perlebar `o` ke 8-tap (isolasi 1 variabel), Kandidat B (fallback, hanya jalan jika Kandidat A gagal/belum cukup), Non-goals (eksplisit), Rencana eksekusi (belum dijalankan) (+1 more)

### Community 362 - "2026-9-3_dieharder-inject-crossing/candidates/w2_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 363 - "w8_f10_i1.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 364 - "w8_f10_i2.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 365 - "w8_f10_i3.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 366 - "w8_f10_i4.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 367 - "w8_f10_i5.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 368 - "w8_f10_i6.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 369 - "w8_f28_i1.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 370 - "w8_f28_i2.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 371 - "w8_f28_i3.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 372 - "w8_f28_i4.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 373 - "w8_f28_i5.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 374 - "w8_f28_i6.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 375 - "promotion_search_inject.py"
Cohesion: 0.44
Nodes (8): compile_candidate(), iter_checkpoints(), _log(), main(), Path, Promotion tier for the 12 inject-crossing candidates (w8_f10_i1..i6,…, run_candidate_all_k(), run_one()

### Community 376 - "tier1_gate_ofmix.py"
Cohesion: 0.42
Nodes (8): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the orbit-o-fmix-fix experiment, port…, run_dual_ordering_stage(), run_stage()

### Community 377 - "Hasil: diagnosis resonansi-konstanta Weyl-stepping vs `ra_init_state_orbit`"
Cohesion: 0.22
Nodes (9): 1. Konteks, 2. File yang dibuat, 3. Konstanta yang diuji, 4. Validasi §5 langkah 1: byte-exact (WAJIB lolos sebelum PractRand), 5. Tier 1: PractRand staged, K=1, base_key=`0x12345`, `C_alt1=0x2545F491`, 6. Verdict, 7. Yang TIDAK dilakukan sesi ini (di luar scope, per persetujuan awal), 8. Referensi (+1 more)

### Community 378 - "HANDOVER: karakterisasi ekshaustif fungsi round-1 K=1 (`ra_core_orbit`)"
Cohesion: 0.22
Nodes (9): 1. Derivasi persis `f(key)` (WAJIB diverifikasi byte-exact dulu), 2. Tiga analisis, urutan prioritas ini, 3. Larangan eksplisit / rambu untuk sesi baru, 4. Referensi, HANDOVER: karakterisasi ekshaustif fungsi round-1 K=1 (`ra_core_orbit`), Ide baru: `f(key)` bisa dikarakterisasi ekshaustif, bukan ditebak, Kenapa ini ada (jangan ulangi metode lama), Status (+1 more)

### Community 379 - "ra_init_state_orbit"
Cohesion: 0.31
Nodes (9): f(key) exhaustive-characterization function, fmix32, GUARD_M constant, ra_core_orbit, ra_init_state_orbit, ra_permutation_cycle_orbit, rot32, Combination point a=(d^o)^(cons+a) as suspected root-cause locus (+1 more)

### Community 380 - "tier1_gate_weylfinal.py"
Cohesion: 0.42
Nodes (8): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the weyl_fmix32 / weyl_triple32…, run_dual_ordering_stage(), run_stage()

### Community 381 - "Promosi Kandidat D ke `ra_core.c` kanonik — RESULTS"
Cohesion: 0.22
Nodes (9): 1. Diff formula yang diterapkan, 2. KAT checksum regenerasi, 3. Byte-identical merge-sanity-check, 4. Caller yang diupdate (13 file + `bench_release.c`), 5. Avalanche-effect curve, 3 skema — pasca-promosi, 6. Throughput 5-mode standar — pasca-promosi, delta jujur, 7. README, 8. Kesimpulan (+1 more)

### Community 382 - "ra_prng2_struct.c"
Cohesion: 0.44
Nodes (8): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), ra_step(), rot32()

### Community 383 - "ra_prng2_thread.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 384 - "Shuffling perf stat (ra_prng2 original)"
Cohesion: 0.43
Nodes (8): RNGing perf stat (ra_prng2 original), Shuffling perf stat (ra_prng2 original), RNGing perf stat (ra_prng3), Scrambling perf stat (ra_prng3), Linux perf stat Tool, Scrambler Tool, tokens.txt Input Corpus, scrambled.txt (scramble-design experiment output)

### Community 385 - "ra_prng2 API Reference"
Cohesion: 0.36
Nodes (8): CPython Binding Documentation, Orphaned CPython Shared Object Rationale, RaPrng2 ctypes Wrapper, ra_prng2 API Reference, ra_core PRNG Core Loop, ra_hash State Reduction Function, RA_PRNG C Struct API, rot32 Left Bitwise Rotation

### Community 386 - "gen_variants.py"
Cohesion: 0.43
Nodes (7): build(), gen_c(), main(), Path, Generate + compile the phase-1 (L-only) state-update-mechanism spectrum…, The "permute" mechanism must reproduce its target's real source exactly (it's…, verify_control_bit_identical()

### Community 387 - "gen_variants_m.py"
Cohesion: 0.39
Nodes (7): build(), gen_c(), main(), Path, Generate + compile the phase-2 (M-only) state-update-mechanism spectrum, per…, The M mechanism only affects the reseed fold (once per 255 steps) -- the first…, verify_first_cycle_matches_control()

### Community 388 - "State Update Mechanism Spectrum"
Cohesion: 0.25
Nodes (8): State Update Mechanism Research Handover, Direct Overwrite State Update Mechanism, Dual Periodicity Validation Methodology, Four-Axis Evaluation Methodology, Non-Hybrid Step Constraint, Permutation-Only State Update Mechanism, ra_prng3 Full Injection State Overwrite, State Update Mechanism Spectrum

### Community 389 - "tahap3_cross_correlation.py"
Cohesion: 0.32
Nodes (7): parallelization-research infra (referenced), Devil's-Advocate Session (challenged parallelization research value), Tahap 3: Statistical Validation, build_group_addr(), main(), Tahap 3, Q1 Method A (re-pointed): Pearson cross-correlation between pairs of…, run()

### Community 390 - "Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`"
Cohesion: 0.25
Nodes (8): 1. Trigger dan motivasi, 2. Analisis (dikonfirmasi sebelum menulis kode apa pun), 3. File & perannya, 4. Batas scope keras (WAJIB dibaca sebelum memakai `ra_core_singleblock`), 5. Hasil kunci (angka lengkap di `RESULTS.md` "## Tahap 6"), 6. Status & langkah selanjutnya, 7. Setelah mengerjakan follow-up di atas, Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`

### Community 391 - "diag_init_avalanche.c"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_avalanche_init(), ra_init_state_avalanche(), ra_permutation_cycle_orbit(), rot32()

### Community 392 - "diag_init_keyterm_avalanche.c"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_keyterm_add(), ra_init_state_keyterm_add(), ra_permutation_cycle_orbit(), rot32()

### Community 393 - "diag_init_keyterm_xor.c"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_keyterm_xor(), ra_init_state_keyterm_xor(), ra_permutation_cycle_orbit(), rot32()

### Community 394 - "tier1_gate_orbit.py"
Cohesion: 0.31
Nodes (10): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the orbit init x cycle combo search,…, Run both orderings for each combo; a combo survives only if BOTH are…, Yield one full checkpoint block (str) at a time from RNG_test's stdout, as… (+2 more)

### Community 395 - "c/ra_prng2.c"
Cohesion: 0.36
Nodes (9): Candidate: ra_prng2 original (paperorig), FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed() (+1 more)

### Community 396 - "2. Inject-crossing (action item 2)"
Cohesion: 0.25
Nodes (7): 1. Dieharder (action item 1), 2. Inject-crossing (action item 2), 3. Recommendation, Dieharder + inject-crossing validation for singleblock K-small-defect fix candidates, Methodology correction found this session, Promotion Tier (16GB x K in {2,4,8,16,32,64,96}, VPS), Staged screening at K=96 (adapted quick-reject, local)

### Community 397 - "staged_inject_screening.py"
Cohesion: 0.43
Nodes (7): _log(), main(), n_words_for_mb(), Path, Staged PractRand screening for the 12 inject-crossing candidates…, run_candidate(), run_practrand()

### Community 398 - "2026-9-6_release-benchmark-prep/avalanche_curve_ra_core.py"
Cohesion: 0.46
Nodes (7): compute_curve(), main(), plot_core(), ndarray, Path, Avalanche-effect curve for `ra_core.c`, replicating the CSAI2025/ICCS2026…, save_csv()

### Community 399 - "ent (Entropy Testing Tool)"
Cohesion: 0.29
Nodes (7): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result

### Community 400 - "pcg32.c"
Cohesion: 0.43
Nodes (6): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), Candidate: pcg32, Official PCG reference (O'Neill, pcg-random.org)

### Community 401 - "src/xoshiro256.c"
Cohesion: 0.43
Nodes (6): main(), rotl(), xoshiro256_init(), xoshiro256_next(), Candidate: xoshiro256**, Official xoshiro256** reference (Blackman & Vigna)

### Community 402 - "xoshiro256_amortized.c"
Cohesion: 0.52
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next()

### Community 403 - "ra_permutation_cycle_singleblock"
Cohesion: 0.43
Nodes (7): ORBIT_KAT_CHECKSUMS (known-answer-test golden checksums, new), Finding: singleblock K=1+hashed shares identical defect with orbit, ra_core_v2.c (working copy for orbit fix + rolling optimization), ra_permutation_cycle_orbit (cycle transform function), ra_permutation_cycle_singleblock, Rolling-register optimization for wide-o (algebraic identity over GF(2)), Orbit/Singleblock Shared Baseline Handover

### Community 404 - "Cross-check against ra_core_orbit"
Cohesion: 0.29
Nodes (7): ra_init_state_orbit, ra_init_state_singleblock, 2026-9-1 multikey-remix-search HANDOVER (full warm-up rejected), Cross-check against ra_core_orbit, ra_init_state_singleblock, ra_reseed, Decision: stop fixing via init/seed, search per-round op combos instead

### Community 405 - "philox_amortized.c"
Cohesion: 0.39
Nodes (8): main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32(), philox_refill_buffer(), rand_bounded(), Speed Benchmark Output (philox amortized)

### Community 406 - "scrambler.py"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 407 - "2026-8-25_periodicity-heuristic-validation/run_sweep.py"
Cohesion: 0.48
Nodes (6): log2_fact(), main(), predicted(), Drives the full periodicity-heuristic-validation sweep. For each measurable (n,…, run_c(), run_python()

### Community 408 - "2026-8-28_periodicity-heuristic-validation2/avalanche_check.py"
Cohesion: 0.38
Nodes (6): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for pruned_wired_toy_prng.py.…, One outer iteration, no rehash -- mirrors next_state's inner loop exactly (tap-…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check()

### Community 409 - "Temuan metodologi: benchmark reinit-K vs steady-state untuk orbit/singleblock"
Cohesion: 0.29
Nodes (7): Bukti kunci (perf stat, isolated per-fungsi), File yang dihasilkan sesi ini, Kronologi investigasi, Penjelasan gap (hipotesis kerja, belum full-proof), Rekomendasi untuk penulisan paper, Ringkasan eksekutif, Temuan metodologi: benchmark reinit-K vs steady-state untuk orbit/singleblock

### Community 410 - "RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`"
Cohesion: 0.29
Nodes (6): Context, Files produced this session, Interpretation, Method, Results, RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`

### Community 411 - "avalanche_heatmap_winners_k255.py"
Cohesion: 0.52
Nodes (6): capture_cycle(), compute_matrix(), main(), plot_single(), Path, Avalanche heatmap (K=255, full single-key cycle) for the 11 Promotion Tier…

### Community 412 - "Validasi `ra_core` dengan workload Monte Carlo nyata (untuk paper baru)"
Cohesion: 0.25
Nodes (7): Generator pembanding yang dipakai ulang (tidak reimplementasi), Kenapa dokumen ini ada, Urutan implementasi, Validasi `ra_core` dengan workload Monte Carlo nyata (untuk paper baru), Workload A — niche `singleblock`: bulk/reinit-per-batch, Workload B — niche `orbit`: continuous/addressed, sedikit alamat panjang, Yang TIDAK dilakukan (di luar scope dokumen ini)

### Community 413 - "avalanche_multiseed.py"
Cohesion: 0.48
Nodes (6): capture_cycle(), evaluate_candidate(), main(), per_seed_stats(), Path, Axis B (HANDOVER.md sect 3): multi-seed avalanche + normality test for the 11…

### Community 414 - "verify_orbit_kat.py"
Cohesion: 0.38
Nodes (6): last_word(), main(), Path, Byte-identical + KAT verification for orbit_bench.c's extern-linked…, # NOTE: for orbit, requesting rng=n does NOT guarantee exactly n words are, stream_bytes()

### Community 415 - "src/chacha20.c"
Cohesion: 0.38
Nodes (5): chacha20_block(), main(), Candidate: chacha20, chacha20.c RFC 8439 nonce byte-order bug, RFC 8439 (ChaCha20 spec)

### Community 416 - "orbit-d0-raw-cons-fix -- hasil"
Cohesion: 0.29
Nodes (7): 1. Byte-exact vs oracle Python (`combo_prng_d0raw.py`), 2. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2), 3. Tier 1: PractRand dual-ordering, K=1 dan K=255, staged 64MB->2GB, 4. Verdict CLEAN-DUAL, 5. Kesimpulan, orbit-d0-raw-cons-fix -- hasil, Ringkasan axis yang sudah dicoba untuk defect K=1+hashed (kumulatif, per sesi ini)

### Community 417 - "source/chacha20.c"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 418 - "chacha20_amortized.c"
Cohesion: 0.47
Nodes (4): chacha20_block(), main(), rand_bounded(), Speed Benchmark Output (chacha20 amortized)

### Community 419 - "pcg.c"
Cohesion: 0.67
Nodes (5): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded()

### Community 420 - "source/xoshiro256.c"
Cohesion: 0.60
Nodes (5): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 421 - "orbit-kmin-battery gate (multistream K=1/K=255 statistical re-validation)"
Cohesion: 0.33
Nodes (6): avalanche_orbit_kmin.py (per-bit Hamming-distance gate), collision_scan_orbit_k1.py (Poisson-z collision model, 50000 keys), multikey_stream_orbit_k1 / multikey_stream_orbit_k255 chaining drivers, Repo rule: no re-seed retry on PractRand FAIL, orbit-kmin-battery gate (multistream K=1/K=255 statistical re-validation), PractRand 16GB test (RNG_test stdin32 -tlmax 16GB -multithreaded)

### Community 422 - "Avalanche Heatmap: singleblock N=16 (K=15 full block)"
Cohesion: 0.33
Nodes (6): Avalanche Heatmap: orbit N=64 LOWONLY (K=63 full block), Avalanche Heatmap: singleblock N=16 (K=15 full block), Orbit core avalanche/diffusion quality (key-bit to output-word flip test), ra_core_param.c (parametrized PRNG core), singleblock N=16 K=1 PractRand escalating BCFN anomaly (FAIL), State-Size / Speed Pareto Frontier experiment

### Community 423 - "2026-9-1_dieharder-battery/run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): main(), Path, Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"- reliability…, run_one(), run_test()

### Community 424 - "run_dieharder_battery_multikey.py"
Cohesion: 0.60
Nodes (5): main(), Path, Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"- reliability…, run_one(), run_test()

### Community 425 - "verify_unification.py"
Cohesion: 0.60
Nodes (5): compile_new(), compile_old(), main(), run_stream_new(), run_stream_old()

### Community 426 - "2026-9-1_production-candidate-battery/run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): main(), Path, Step 5 (PRODUCTION_READINESS_HANDOVER.md) -- dieharder for the two multikey-…, run_one(), run_test()

### Community 427 - "scramble_w8_f28_i0_lookonly.py"
Cohesion: 0.53
Nodes (5): bulk_chain(), fisher_yates_k255(), gen(), pull_n(), key = base_key + i*GOLDEN for i=0..count-1, one word each -- matches the…

### Community 428 - "2026-9-3_dieharder-inject-crossing/run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): main(), Path, dieharder "Good"-reliability battery for the two singleblock K-small-defect…, run_one(), run_test()

### Community 429 - "orbit-ad-mul-combine-fix -- hasil"
Cohesion: 0.33
Nodes (6): 1. Byte-exact vs oracle Python (`combo_prng_admul.py`) -- WAJIB sebelum statistik, 2. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2, BASE_SEED=1), 3. Kenapa REJECT: konsekuensi struktural dari `d=0` di ronde pertama, 4. Kesimpulan, orbit-ad-mul-combine-fix -- hasil, Ringkasan axis yang sudah dicoba untuk defect K=1+hashed (kumulatif)

### Community 430 - "other_winners_gen.py"
Cohesion: 0.38
Nodes (6): gen_c(), main(), Path, Generate + compile hardcoded C binaries for the 12 non-selected winning wirings…, Compare the compiled binary's --stream output against wired_prng.py's stream()…, verify_bit_identical()

### Community 431 - "run_altconst_practrand.py"
Cohesion: 0.53
Nodes (5): iter_checkpoints(), parse_size(), Staged PractRand runner for the weyl-constant-resonance-diagnosis experiment.…, run_stage(), run_staged()

### Community 432 - "run_dieharder_battery_addr_hi.py"
Cohesion: 0.60
Nodes (5): main(), Path, experiments/2026-9-5_orbit-address-widen-64bit -- Tahap 6: dieharder untuk…, run_one(), run_test()

### Community 433 - "Hasil: K=1 orbit dengan key dari `/dev/urandom` (proxy TRNG)"
Cohesion: 0.33
Nodes (6): File, Hasil: K=1 orbit dengan key dari `/dev/urandom` (proxy TRNG), Hasil PractRand (`run_urandom_key_A.log`, 1 run sampai 2GB), Kesimpulan, Pertanyaan, Sanity check (sebelum PractRand)

### Community 434 - "Hasil: `o` sebagai XOR 8 word mentah (tanpa bit-shift per-tap)"
Cohesion: 0.33
Nodes (6): File, Hasil, Hasil: `o` sebagai XOR 8 word mentah (tanpa bit-shift per-tap), Hipotesis, Implementasi, Kesimpulan

### Community 435 - "Hasil Workload A — Monte Carlo π estimation (niche `singleblock`)"
Cohesion: 0.29
Nodes (6): Catatan metodologi (jejak audit sesi ini), File, Hasil, Hasil Workload A — Monte Carlo π estimation (niche `singleblock`), Implikasi untuk paper, Setup

### Community 436 - "HANDOVER: baseline dual-core (orbit+singleblock) siap-edit"
Cohesion: 0.33
Nodes (6): HANDOVER: baseline dual-core (orbit+singleblock) siap-edit, Kenapa folder ini ada, Kenapa ini feasible (dibuktikan dari ra_core.c, bukan asumsi), Status, Temuan penting yang melatarbelakangi baseline ini, Verifikasi yang sudah dijalankan (sebelum dipakai edit)

### Community 437 - "weyl_fmix32.c"
Cohesion: 0.53
Nodes (5): FILE, fmix32(), main(), splitmix32(), weyl_fmix32_core()

### Community 438 - "weyl_triple32.c"
Cohesion: 0.53
Nodes (5): FILE, main(), splitmix32(), triple32(), weyl_triple32_core()

### Community 439 - "Promosi Kandidat D (64-bit address widening) ke `ra_core.c` kanonik — HANDOVER"
Cohesion: 0.33
Nodes (6): Keputusan desain kunci, Promosi Kandidat D (64-bit address widening) ke `ra_core.c` kanonik — HANDOVER, Scope IN, Scope OUT (dicatat, tidak dikerjakan sesi ini), Status akhir sesi ini, Tujuan

### Community 440 - "test_ra_hash.py"
Cohesion: 0.33
Nodes (3): Pure-Python unit tests for ra_prng2.ra_hash / ra_core. These do not require the…, Regression test for the historical bug: ra_hash must be a pure function writing…, test_ra_hash_does_not_mutate_input()

### Community 441 - "auto_stop_power38.py"
Cohesion: 0.60
Nodes (4): find_pid(), get_status(), main(), Monitors .cycle_measure_ckpt_n8_w4_rows4_seed0.bin and stops cycle_measure when…

### Community 442 - "full_scale_sanity_check.py"
Cohesion: 0.50
Nodes (4): check_no_repeat(), main(), Path, Full-scale (real 256x32-bit) practical periodicity sanity check, per…

### Community 443 - "avalanche_orbit_singleblock_k255.py"
Cohesion: 0.60
Nodes (4): capture_cycle(), main(), production-candidate-battery, Step 1: avalanche gate for `orbit` and…, run_gate()

### Community 444 - "multikey_stream (K=255 gate binary)"
Cohesion: 0.40
Nodes (5): dieharder_K1_piped.txt (raw dieharder output, K=1, battery gate), dieharder_K255_piped.txt (raw dieharder output, K=255, battery gate), multikey_stream (K=255 gate binary), multikey_stream_singleblock_k1 (K=1 gate binary), run_dieharder_battery.py (production-candidate-battery adaptation)

### Community 445 - "score_shuffle.py"
Cohesion: 0.50
Nodes (4): main(), Path, Step 7 (PRODUCTION_READINESS_HANDOVER.md) -- score `scrambler_ra_core_…, score()

### Community 446 - "scrambler_ra_core_singleblock.c"
Cohesion: 0.70
Nodes (4): fisher_yates_k1(), main(), pull_n(), pull_one()

### Community 447 - "Avalanche heatmap w8_f8_i0 K=255 (full cycle)"
Cohesion: 0.40
Nodes (5): Avalanche heatmap w8_f8_i0 K=255 (full cycle), Avalanche heatmap w8_f9_i0 K=255 (full cycle), Avalanche test method: seed-bit-flip vs. output-word bit-difference heatmap, Cycle-op combo candidate w8_f8_i0 (tap width=8, finalizer f8, i0), Candidate w8_f9_i0 (tap width=8, finalizer c^=c>>16, cycle-op combo)

### Community 448 - "Avalanche Effect Analysis (ALL_OPS)"
Cohesion: 0.50
Nodes (5): Avalanche Effect Analysis (ALL_OPS), Bit Difference Distribution Across 32 Flipped Bits, Cycle 1 Avalanche Fraction (0.497576), Cycle 2 Avalanche Fraction (0.502141), Avalanche Effect Heatmap (Cycle 1 vs Cycle 2)

### Community 449 - "heatmap.py"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 450 - "ra_prng2.py"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 451 - "c/ra_prng3.c"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 452 - "Avalanche Effect Curve: ra_core_orbit (K=255)"
Cohesion: 0.50
Nodes (4): CSAI2025/ICCS2026 avalanche-effect analysis methodology, Avalanche Effect Curve: ra_core_orbit (K=255), Avalanche Effect Curve: ra_core singleblock, ra_core orbit PRNG core

### Community 453 - "Cycle 3 Avalanche Fraction (0.470588)"
Cohesion: 0.83
Nodes (4): Cycle 1 Avalanche Fraction (0.418459), Cycle 3 Avalanche Fraction (0.470588), Avalanche Heatmap: Cycle 1 vs Cycle 3 (pruned_winner), Reseed Avalanche Diffusion in Pruned Winner

### Community 454 - "108-Wiring Exhaustive Search"
Cohesion: 0.67
Nodes (4): 108-Wiring Exhaustive Search, PractRand Small Gate Vulnerability, Wiring Configuration v05 (winner_wired), Wiring Configuration v08 (winner_wired_v2)

### Community 455 - "Paper heuristic λ ≈ 0.7824·√|S|"
Cohesion: 0.50
Nodes (4): Paper heuristic λ ≈ 0.7824·√|S|, Research Question: does pruned/rewired algorithm shift the heuristic's λ-vs-λ_pred fit, Central Research Question (RESULTS.md), Verdict 2: heuristic behaves as conservative lower bound at large-scale configs only

### Community 456 - "Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))"
Cohesion: 0.50
Nodes (4): Tap shift-width collapse (e>=w yields identically-zero term), Tap-survivor generalization (top-2-by-count rule, G=8→(6,7)), Finding: non-vacuous tap pruning (G=4) gives closest fit to heuristic, Caveat: tap pruning is a no-op at G<=2 configs

### Community 457 - "dieharder_battery_param.py"
Cohesion: 0.60
Nodes (5): main(), Path, experiments/2026-9-11_state-size-pareto-frontier/dieharder_battery_param.py --…, run_one(), run_test()

### Community 459 - "recipes.py"
Cohesion: 0.15
Nodes (10): Cycle-operation combo DSL search space (756 combos), all_promotion_candidates(), _build_finalizer_recipes(), FinalizerStage, inject_describe(), DSL catalog for the cycle-operation combo search (HANDOVER.md). Search space…, 7 inject variants for a single (width, finalizer_idx) Tier-1 survivor., main() (+2 more)

### Community 460 - "Address Widening 64-bit RESULTS.md"
Cohesion: 0.32
Nodes (7): Orbit Widening Verification, main(), splitmix32_hi(), splitmix64_next(), Address Widening 64-bit RESULTS.md, 3-Way Avalanche Test (key_only / addr_hi_only / combined), Promotion of Candidate D to ra_core.c - Deferred (>=13 caller files affected)

### Community 461 - "Avalanche Effect Heatmap (Original Algorithm)"
Cohesion: 0.67
Nodes (4): Avalanche Effect, Avalanche Effect Heatmap (Original Algorithm), Hamming Distance Analysis, Original PRNG Algorithm

### Community 462 - "Graphify Knowledge Graph Rule"
Cohesion: 0.67
Nodes (3): Graphify Query & Navigation Protocol, Graphify Knowledge Graph Rule, Graphify Pipeline Workflow

### Community 464 - "Graphify-First Research Workflow Policy"
Cohesion: 0.67
Nodes (3): graphify-out/graph.json Knowledge Graph Artifact, graphify Skill, Graphify-First Research Workflow Policy

### Community 465 - "Two-mode decision: orbit + singleblock only, no third fixed-init mode"
Cohesion: 0.67
Nodes (3): tahap6_bench.c (benchmark harness, no-L fast path source), Two-mode decision: orbit + singleblock only, no third fixed-init mode, winner_wired_v2.c (old fixed-init, no-key stream mode)

### Community 466 - "Cycle 1 vs Cycle 5 Avalanche Fraction Convergence"
Cohesion: 1.00
Nodes (3): Cycle 1 vs Cycle 5 Avalanche Fraction Convergence, Avalanche Heatmap: Pruned Winner Cycle 1 vs Cycle 5, Pruned Winner Architecture (TAP6, TAP7, ROT_C, SHR13)

### Community 467 - "Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)"
Cohesion: 1.00
Nodes (3): Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13), pruned_winner Cycle 1 Avalanche Sensitivity and Dead Zones, pruned_winner 4-Op Candidate Configuration (TAP6, TAP7, ROT_C, SHR13)

### Community 468 - "Greedy Operation-Pruning Search Paradigm"
Cohesion: 1.00
Nodes (3): Greedy Operation-Pruning Search Paradigm, Operation-Pruning Research Handover Spec, Limits of Zero-Drift Micro-Optimizations on ra_core

### Community 469 - "Seed Bit 5/6 Avalanche-Dead Defect"
Cohesion: 0.67
Nodes (3): Seed Bit 5/6 Avalanche-Dead Defect, Per-Bit Avalanche Floor Gate, Short Feedback Loop Cancellation

### Community 470 - "avalanche_3way_addr_hi.py"
Cohesion: 0.53
Nodes (5): capture(), main(), Path, experiments/2026-9-5_orbit-address-widen-64bit -- Tahap 2: avalanche 3-arah…, run_direction()

### Community 471 - "collision_scan.py"
Cohesion: 0.67
Nodes (3): collision_scan.py, Q2: Cross-stream collision probability, ProcessPoolExecutor deadlock -> ThreadPoolExecutor+lock fix

### Community 472 - "Q3: Multi-process throughput scalability"
Cohesion: 0.67
Nodes (3): perf_scaling.sh, Q3: Multi-process throughput scalability, wall_time_scaling.py

### Community 473 - "pcg.c (PCG32 comparison binary)"
Cohesion: 0.67
Nodes (3): out_pcg.txt (output artifact of pcg speed-benchmark run), out_pcg_amortized.txt (output artifact of pcg amortized speed-benchmark run), pcg.c (PCG32 comparison binary)

### Community 474 - "xoshiro256.c (fread+tokenizer comparison binary)"
Cohesion: 0.67
Nodes (3): out_xoshiro256.txt (output artifact of xoshiro256 speed-benchmark run), out_xoshiro256_amortized.txt (output artifact of xoshiro256 amortized speed-benchmark run), xoshiro256.c (fread+tokenizer comparison binary)

### Community 479 - "Avalanche heatmap w8_f22_i0 K=255 (full cycle)"
Cohesion: 1.00
Nodes (3): Avalanche heatmap w8_f22_i0 K=255 (full cycle), Candidate w8_f22_i0 (tap width=8, finalizer f22, cycle-op combo search), Result: uniform avalanche (overall=0.5010, min_bit=0.4915), no visible weak bit/word structure

### Community 480 - "Validasi pola addr_hi-increment chaining untuk fitur shuffle pip-ra-core"
Cohesion: 0.33
Nodes (6): Dampak ke plan pip-ra-core, Hasil, Kesimpulan, Konteks, Metodologi, Validasi pola addr_hi-increment chaining untuk fitur shuffle pip-ra-core

### Community 483 - "tools/battery/"
Cohesion: 0.33
Nodes (6): Cara pakai untuk kandidat baru, Kontrak yang harus dipenuhi kandidat, Menguji core produksi (ra_core.c) tanpa mengubahnya, Mode dual-core: edit sekali, tes dua core sekaligus (direkomendasikan), tools/battery/, Yang TIDAK dicakup tool ini

### Community 484 - "sub-projects/ (downstream applications)"
Cohesion: 0.67
Nodes (3): fingerprinting_tools sub-project, image_encrypting sub-project, sub-projects/ (downstream applications)

### Community 497 - "kat_mt19937"
Cohesion: 0.60
Nodes (5): kat_mt19937(), mt19937_genrand_int32(), mt19937_init_genrand(), ra_core_mt19937(), mt19937_state_t

### Community 627 - "verify_fresh_build.py"
Cohesion: 0.60
Nodes (4): check_multikey_cross(), check_validate(), main(), production-candidate-battery, Step 0: passive correctness gate on the FRESH…

### Community 628 - "HANDOVER: Continuous-stream real benchmark untuk `ra_core_orbit`"
Cohesion: 0.40
Nodes (5): Deliverable, Deviasi dari rencana (didokumentasikan), HANDOVER: Continuous-stream real benchmark untuk `ra_core_orbit`, Konteks, Rencana implementasi (plan yang disetujui)

## Ambiguous Edges - Review These
- `ra_core_param.c` → `Orbit Multi-cycle Off-by-one Fix Handover`  [AMBIGUOUS]
  experiments/2026-9-12_orbit-multicycle-offbyone-fix/HANDOVER.md · relation: references
- `ra_hash()` → `64-bit Address Space Widening Design (key+param2)`  [AMBIGUOUS]
  experiments/2026-9-5_orbit-address-widen-64bit/HANDOVER.md · relation: references
- `ra_reseed()` → `64-bit Address Space Widening Design (key+param2)`  [AMBIGUOUS]
  experiments/2026-9-5_orbit-address-widen-64bit/HANDOVER.md · relation: references
- `Orbit Continuous-Bench Results` → `RESULTS_TAHAP2.md (orbit K=1 historical figure)`  [AMBIGUOUS]
  experiments/2026-9-3_orbit-continuous-bench/RESULTS.md · relation: references
- `State-Size/Speed Pareto Frontier HANDOVER Plan` → `Avalanche Heatmap: orbit N=32 MUTUAL (K=31 full block)`  [AMBIGUOUS]
  experiments/2026-9-11_state-size-pareto-frontier/heatmaps/heatmap_orbit_N32_MUTUAL.png · relation: conceptually_related_to
- `ra_prng2 family` → `ra_core_orbit multi-cycle off-by-one bug`  [AMBIGUOUS]
  experiments/2026-9-12_orbit-multicycle-offbyone-fix/HANDOVER.md · relation: references
- `ra_prng3 family` → `ra_core_orbit multi-cycle off-by-one bug`  [AMBIGUOUS]
  experiments/2026-9-12_orbit-multicycle-offbyone-fix/HANDOVER.md · relation: references
- `Hamas A. Rahman` → `hamzy hams (GitHub maintainer)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to
- `singleblock N=16 K=1 PractRand escalating BCFN anomaly (FAIL)` → `Avalanche Heatmap: singleblock N=16 (K=15 full block)`  [AMBIGUOUS]
  experiments/2026-9-11_state-size-pareto-frontier/heatmaps/heatmap_singleblock_N16.png · relation: conceptually_related_to

## Knowledge Gaps
- **981 isolated node(s):** `other_winners_perf.sh script`, `other_winners_practrand.sh script`, `perf_state_update.sh script`, `perf_state_update_m.sh script`, `toy_probe_phase3.sh script` (+976 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **139 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `ra_core_param.c` and `Orbit Multi-cycle Off-by-one Fix Handover`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `ra_hash()` and `64-bit Address Space Widening Design (key+param2)`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `ra_reseed()` and `64-bit Address Space Widening Design (key+param2)`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Orbit Continuous-Bench Results` and `RESULTS_TAHAP2.md (orbit K=1 historical figure)`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `State-Size/Speed Pareto Frontier HANDOVER Plan` and `Avalanche Heatmap: orbit N=32 MUTUAL (K=31 full block)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **What is the exact relationship between `ra_prng2 family` and `ra_core_orbit multi-cycle off-by-one bug`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `ra_prng3 family` and `ra_core_orbit multi-cycle off-by-one bug`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._