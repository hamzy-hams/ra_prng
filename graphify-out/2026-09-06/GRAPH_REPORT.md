# Graph Report - ra_prng  (2026-09-06)

## Corpus Check
- 628 files · ~23,561,223 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 5518 nodes · 9211 edges · 579 communities (462 shown, 117 thin omitted)
- Extraction: 98% EXTRACTED · 2% INFERRED · 0% AMBIGUOUS · INFERRED: 185 edges (avg confidence: 0.86)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `305e60e0`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- winners_bench.c
- benchmark_all.c
- tahap4_bench.c
- ra_core.c
- Production Readiness Handover (ra_core_orbit/ra_core_singleblock)
- Completed Tasks
- tahap0_prototype.py
- Handover: uji 2 kandidat fix tertarget (seed `d0` di fase init) untuk defect K=1+hashed pada `ra_core_orbit`
- ra_core_orbit
- Singleblock Cycle-Operation Combo Search Handover
- Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)
- ra_init_state_orbit
- combo_prng_warmup.py
- practrand_gate.py
- ra_core_singleblock Cycle-Operation Combo Search — RESULTS
- diag_wideo_warmup_singleblock.c
- diag_init_candidates.c
- 2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md
- ra_prng2 (32-bit generator, CSAI2025)
- ra_prng2 (Array-Based PRNG)
- RESULTS: Priority 1 — Init-loop speed optimization
- common.py (TIERS_Q2)
- Langkah 3: Interleaved 1TB for winner_wired_addressable (shared core-loop test)
- Pareto Selection Results (Combo Winners)
- exhaustive_analysis.c
- tahap6_bench.c
- Random
- cycle_analysis_weylfinal.py
- Dieharder Test Suite
- README.md
- winner_wired_v2.c
- combo_gen.py
- Wiring
- combo_gen_orbit.py
- ra_core_v3.c
- bit_bias_analysis.py
- HANDOVER — Refinement akhir `ra_core_singleblock`: difusi-di-akhir, optimasi, eksperimen kecil
- State-Update Mechanism Research Results (Phase 1-3)
- pruned_winner 4-Operation Fast Variant
- other/avalanche_heatmap_original.py
- cross_correlation.py
- bench_release.c
- 64-bit Address Widening Handover
- driver_main_template.c
- 2026-8-25_periodicity-heuristic-validation/cycle_measure.c
- avalanche_heatmap_pruned_winner.py
- Candidate
- 2026-8-28_periodicity-heuristic-validation2/cycle_measure.c
- stream_values
- c/ra_prng2.c
- ICCS 2026 Research Paper: Array-Native Randomness
- ra_core_singleblock
- avalanche_stats
- Handover: PRNG family benchmark (ra_prng semua varian vs PRNG terkenal)
- ra_core_v2.c
- candidate_a.c
- HANDOVER — release-benchmark-prep (2026-09-06)
- avalanche_heatmap_winner_v2.py
- Operand Position Search Experiment Results
- ra_core (unified addressable-init successor)
- winner_wired_addressable.c
- ra_core_singleblock
- combo_prng.py
- ra_core_baseline.c
- combo_prng_ofmix.py
- candidate_c.c
- avalanche_effect_analysis.py
- others/avalanche_heatmap_original.py
- Params
- RESULTS: continuous-stream real benchmark untuk `ra_core_orbit`
- scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)
- interleave_practrand.py
- RESULTS: production-candidate battery for `ra_core.c`
- combo_prng_orbit.py
- combo_prng_d0raw.py
- RaPrng2
- Parallelization Research Handover
- toy_prng.py
- next_state
- stream_driven_shuffle.c
- benchmark_all_run.py
- diag_keyzero_guard.c
- ADDENDUM_POST_FIX_STATUS.md — Step 4-8, post `w8_f10_i0` promotion
- quality_gate.py (avalanche_stats)
- combo_prng_admul.py
- candidate_e.c
- run_verify_all
- 2026-9-4_orbit-kmin-battery/RESULTS.md
- recipes.py
- Promosi Kandidat D ke `ra_core.c` kanonik — RESULTS
- Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`
- candidate_b.c
- candidate_d.c
- Singleblock cycle-op combo search experiment (2026-09-02)
- RESULTS: Pareto selection over the 11 combo-search winners
- avalanche_heatmap_winner.py
- Handover: Tahap 5 — Optimasi Biaya Init Addressable
- io_only_isolation_test.c
- Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect
- RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)
- 2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py
- Orbit K=1 PractRand Defect
- random_seeds
- avalanche_ra_core.py
- diag_wideo_singleblock.c
- diag_prereseed.c
- Orbit Fix and Wide-o Rolling Optimization Handover
- Orbit Continuous-Stream Measurement (376.2 MB/s)
- orbit-d0-raw-cons-fix -- hasil
- position_split_orbit.c
- Avalanche Effect (Bit-Flip Sensitivity)
- Design decisions (read before touching `pruned_wired_toy_prng.py`)
- pareto_select.py
- control.c
- d0_hash.c
- d0_realround.c
- o_fmix_every_round.c
- o_fmix_init_only.c
- d0_raw_cons.c
- Hamas A. Rahman (author)
- BuildPyWithCEngine
- 2025-10-5_scramble-design/scc_test.py
- pruned_prng.c
- Cycle 1 Initial State Avalanche Fraction (0.487783)
- Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)
- tahap5_benchmark.py
- tahap6_benchmark.py
- RESULTS: key=0 degenerate-state guard-XOR fix
- HANDOVER: Init-loop speed optimization untuk `ra_core_orbit` / `ra_core_singleblock`
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
- tier1_gate_orbit.py
- tier1_gate_warmup.py
- multikey_stream_orbit_altconst.c
- bench_addr_hi.c
- tier1_gate_d0raw.py
- Handover: redesign arsitektur "Weyl-add + finalizer 32-bit murni" untuk defect K-small `ra_core_orbit`
- w8_f25_i0.c
- 2026-8-28_periodicity-heuristic-validation2/run_sweep.py
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
- pcg_amortized.c
- source/ra_prng2.c
- source/ra_prng3.c
- scrambler.c
- 2026-8-25_periodicity-heuristic-validation/avalanche_check.py
- Tahap 2: Folder/Battery Validasi Produksi
- pruned_winner.c
- pruned_winner_refactored.c
- 2026-8-27_operand-position-search/baseline.c
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
- Promosi Kandidat D (64-bit address widening) ke `ra_core.c` kanonik — HANDOVER
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
- winner_wired_addressable_v2.c
- Handover: Addressable-Init Data Shuffling & Speed Investigation
- Hasil validasi
- scrambler_addressable.py
- scrambler_wired_addressable.py
- no_reseed_isolation_test.c
- main
- PRNG Family Benchmark Results
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
- Axis B - Avalanche Multi-seed + Normality
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
- Hasil: K=1 orbit dengan key dari `/dev/urandom` (proxy TRNG)
- w8_f9_i0.c
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
- tier1_gate_weylfinal.py
- ra_prng2_struct.c
- Fisher–Yates Shuffle
- ra_prng2_thread.c
- ra_prng2 API Reference
- gen_variants.py
- gen_variants_m.py
- tahap3_cross_correlation.py
- diag_init_avalanche.c
- diag_init_keyterm_avalanche.c
- diag_init_keyterm_xor.c
- collision_scan_singleblock_k1.py
- Hasil: `o` sebagai XOR 8 word mentah (tanpa bit-shift per-tap)
- 2. Inject-crossing (action item 2)
- staged_inject_screening.py
- orbit-round1-collision-position-isolation -- hasil (diagnosis only)
- collision_scan_orbit_k1.py
- Address Widening 64-bit RESULTS.md
- ent (Entropy Testing Tool)
- pcg32.c
- src/xoshiro256.c
- xoshiro256_amortized.c
- scrambler.py
- 2026-8-25_periodicity-heuristic-validation/run_sweep.py
- 2026-8-28_periodicity-heuristic-validation2/avalanche_check.py
- simd_prototype.c
- RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`
- Family Productionization Handover
- K=1 Exhaustive Function Analysis Handover
- `ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi
- avalanche_heatmap_winners_k255.py
- verify_orbit_kat.py
- kat_mt19937
- PRNG Periodicity Analysis
- source/chacha20.c
- chacha20_amortized.c
- pcg.c
- source/xoshiro256.c
- verify_fresh_build.py
- 2026-9-1_dieharder-battery/run_dieharder_battery.py
- run_dieharder_battery_multikey.py
- verify_unification.py
- 2026-9-1_production-candidate-battery/run_dieharder_battery.py
- scramble_w8_f28_i0_lookonly.py
- 2026-9-3_dieharder-inject-crossing/run_dieharder_battery.py
- orbit-ad-mul-combine-fix -- hasil
- 2026-9-4_orbit-kmin-battery/run_dieharder_battery.py
- STATUS (2026-09-04) — BLOCKED sebelum run utama, oleh temuan tak terduga
- run_altconst_practrand.py
- run_dieharder_battery_addr_hi.py
- weyl_fmix32.c
- weyl_triple32.c
- test_ra_hash.py
- tools/battery/
- verify_init_candidates.py
- auto_stop_power38.py
- full_scale_sanity_check.py
- wall_time_scaling.py
- multikey_stream (K=255 gate binary)
- score_shuffle.py
- scrambler_ra_core_singleblock.c
- Avalanche heatmap w8_f8_i0 K=255 (full cycle)
- Avalanche Effect Analysis (ALL_OPS)
- heatmap.py
- ra_prng2.py
- c/ra_prng3.c
- Avalanche Effect Curve: ra_core_orbit (K=255)
- ra_prng3 (64-bit experimental successor)
- Cycle 3 Avalanche Fraction (0.470588)
- Paper heuristic λ ≈ 0.7824·√|S|
- Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))
- Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche
- JAX
- robust_reinit.py
- pick_guard_constants.py
- Avalanche Effect Heatmap (Original Algorithm)
- Graphify Knowledge Graph Rule
- dev_urandom.c
- Graphify-First Research Workflow Policy
- ra_reseed/ra_hash orbit pre-reseed BCFN defect (still open, separate track)
- Two-mode decision: orbit + singleblock only, no third fixed-init mode
- Cycle 1 vs Cycle 5 Avalanche Fraction Convergence
- Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)
- Greedy Operation-Pruning Search Paradigm
- collision_scan.py
- Q3: Multi-process throughput scalability
- pcg.c (PCG32 comparison binary)
- xoshiro256.c (fread+tokenizer comparison binary)
- run_speed_benchmark.sh
- 2. Blocker utama: singleblock K-small structural defect
- Avalanche heatmap w8_f22_i0 K=255 (full cycle)
- HANDOVER: Continuous-stream real benchmark untuk `ra_core_orbit`
- CycleStage
- pick_guard_constant_d.py
- bench_init_only.c
- Periodicity Heuristic Validation Status Report
- sub-projects/ (downstream applications)
- Bit-bias marginal (single-bit) diagnostic test
- collision_scan_ra_core_singleblock.py (K=255 collision-scan script)
- combo_gen_warmup.py (C codegen)
- combo_prng.py (Python oracle for combo-search candidates)
- verify_unification.py (bit-identical cross-check script)
- End-diffusion idea: add new diffusion stage near output instead of only at round start
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
- CI Build and Test Job
- conftest.py
- Addressable-Init Research (Tahap 0-5)
- /dev/urandom RNG Speed Benchmark Result
- PCG32 RNG Speed Benchmark Result
- Philox4x32 RNG Speed Benchmark Result
- xoshiro256** RNG Speed Benchmark Result
- benchmarks/comparisons/source/README.md (naming convention)
- Project Root Changelog
- checksum_key() (ra_core.c KAT-checksum capture function)
- 'Orbit Addressing' naming policy reversal (docs-only -> code identifier)
- Orbit/singleblock benchmark standard (5-number report: multistream K=1/K=255/--stream, singleblock K=1/K=255)
- 2025-10-5_scramble-design/scrambler.c (predecessor)
- rows=2 Configuration Log
- Design decision: op/wiring mapping ported to toy scale
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
- w8_f33_i0 (ADD->ROT, no xorshift)
- w8_f9_i0 (XORSHIFT(16))
- Orbit Addressing (f(key) state-init mechanism)
- ra-prng
- production-candidate-battery gate (Step 0-8)
- benchmarks/results (precomputed statistical output)
- ra_prng2 Package Changelog
- ra_prng3 Package Changelog
- Image Encrypting Documentation
- ZepFold (token-shuffling CLI approach)

## God Nodes (most connected - your core abstractions)
1. `ra_core_singleblock Cycle-Operation Combo Search — RESULTS` - 51 edges
2. `Production Readiness Handover (ra_core_orbit/ra_core_singleblock)` - 37 edges
3. `Params` - 33 edges
4. `ra_prng2 (Array-Based PRNG)` - 29 edges
5. `ra_core_orbit()` - 28 edges
6. `ra_prng2 (32-bit generator, CSAI2025)` - 27 edges
7. `ra_permutation_cycle_singleblock()` - 24 edges
8. `ra_core_singleblock()` - 24 edges
9. `Orbit Fix and Wide-o Rolling Optimization Handover` - 24 edges
10. `State-Update Mechanism Research Results (Phase 1-3)` - 23 edges

## Surprising Connections (you probably didn't know these)
- `hamzy hams (GitHub maintainer)` --conceptually_related_to--> `Hamas A. Rahman (author)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf → README.md
- `Candidate` --semantically_similar_to--> `Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13)`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/pruned_prng.py → experiments/2026-8-26_operation-pruning-research/avalanche_heatmap_pruned_winner_cycle1_vs_cycle2.png
- `winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039)` --conceptually_related_to--> `Avalanche Effect (Bit-Flip Sensitivity)`  [INFERRED]
  experiments/2026-8-27_operand-position-search/avalanche_heatmap_winner_v2.png → research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf
- `pruned_winner 4-Operation Fast Variant` --semantically_similar_to--> `ra_prng2 (32-bit generator, CSAI2025)`  [INFERRED] [semantically similar]
  experiments/2026-8-26_operation-pruning-research/RESULTS.md → README.md
- `Addressable / Orbit-style Initialization` --semantically_similar_to--> `singleblock vs orbit speed crossover (K≈20-21, singleblock faster above this)`  [INFERRED] [semantically similar]
  README.md → experiments/2026-9-2_singleblock-k-threshold-characterization/RESULTS.md

## Import Cycles
- None detected.

## Hyperedges (group relationships)
- **PRNG Speed and Shuffling Benchmark Comparison** — research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng2, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_ra_prng3, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_pcg32, research_csai2025_ra_prng_ra_prng2_and_beyond__an_array_based_prng_architecture_for_efficient_random_generation_pdfca_162_final_chacha20 [EXTRACTED 0.85]
- **Shared dual-core (orbit+singleblock) battery testing infrastructure** — tools_battery_readme, experiments_2026_9_5_orbit_singleblock_shared_baseline_handover, experiments_2026_9_1_family_productionization_production_readiness_handover, concept_tools_battery_tool [EXTRACTED 0.90]
- **Statistical battery tests applied together to validate orbit K-small fix** — concept_dieharder_battery, concept_practrand_16gb, concept_collision_scan_orbit_k1, concept_avalanche_orbit_kmin, concept_multikey_stream_orbit_drivers [EXTRACTED 0.90]
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
- **Cycle-transform DSL finalizer candidates (curated set of 4)** — experiments_2026_9_4_orbit_init_cycle_combo_search_handover_w8_f9_i0, experiments_2026_9_4_orbit_init_cycle_combo_search_handover_w8_f33_i0 [EXTRACTED 1.00]
- **pruned_winner Cycle 1 vs Cycle 90 Avalanche Analysis** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_reseed_diffusion_result, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle90_avalanche_fraction [EXTRACTED 1.00]
- **winner_wired Avalanche Diffusion Convergence Across Cycles** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_cycle1_vs_cycle4_cycle4_avalanche [EXTRACTED 1.00]
- **Winner Wired PRNG Avalanche Effect Analysis** — experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_figure, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_winner_wired_config, experiments_2026_8_27_operand_position_search_avalanche_heatmap_winner_avalanche_performance [EXTRACTED 1.00]
- **Avalanche Effect Cycle Comparison** — experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_avalanche_effect_analysis, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_1_avalanche, experiments_others_avalanche_effect_heatmap_cycle1_vs_cycle2_cycle_2_avalanche [EXTRACTED 1.00]
- **Family Use-Case to Algorithm Mapping** — experiments_2026_9_1_family_productionization_handover_usecase_mapping, experiments_2026_9_1_family_productionization_handover_winner_wired_unified, experiments_2026_8_30_addressable_init_research_tahap6_bench_ra_core_singleblock, experiments_2026_9_1_family_productionization_handover_ra_prng_family [EXTRACTED 1.00]
- **fmemopen buffer-size bug: prior discovery and required avoidance in new drivers** — experiments_2026_9_1_production_candidate_battery_addendum_post_fix_status_doc, experiments_2026_9_4_orbit_reseed_postboundary_bcfn_handover_fmemopen_glibc_bug, experiments_2026_9_4_orbit_reseed_postboundary_bcfn_multikey_stream_orbit_postreseed_k1, experiments_2026_9_4_orbit_reseed_postboundary_bcfn_multikey_stream_orbit_postreseed_k255 [EXTRACTED 1.00]
- **Documented gate-size-trap incidents across search experiments** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_operation_pruning_research_files, experiments_2026_9_2_singleblock_cycle_combo_search_handover_operand_position_search_files, experiments_2026_9_2_singleblock_cycle_combo_search_handover_singleblock_o_width_fix_experiment, experiments_2026_9_2_singleblock_cycle_combo_search_handover_gate_size_trap [EXTRACTED 1.00]
- **12-candidate PRNG family benchmark roster (ra_prng family vs literature PRNGs)** — src_ra_prng2_c_ra_prng2, experiments_2026_8_27_operand_position_search_winner_wired_v2, experiments_2026_8_30_addressable_init_research_tahap6_bench [EXTRACTED 1.00]
- **1TB interleaved-PractRand anomaly diagnostic investigation** — experiments_2026_8_29_parallelization_research_handover_1tb_followup, experiments_2026_8_29_parallelization_research_results, experiments_2026_8_30_addressable_init_research_results [EXTRACTED 1.00]
- **Addressable-init research pipeline Tahap 0 through Tahap 6** — experiments_2026_8_30_addressable_init_research_results_kandidat5 [EXTRACTED 1.00]
- **Guard-XOR Fix Implementation & Verification Chain** — experiments_2026_9_1_keyzero_guard_fix_results_guard_xor_fix, experiments_2026_9_1_keyzero_guard_fix_diag_keyzero_guard, experiments_2026_9_1_keyzero_guard_fix_pick_guard_constants [EXTRACTED 1.00]
- **Production-Candidate Battery Verification Axes (Step 0-3)** — experiments_2026_9_1_production_candidate_battery_results, experiments_2026_9_1_production_candidate_battery_quality_gate, experiments_2026_8_29_parallelization_research_cross_correlation_ra_core_singleblock, experiments_2026_8_29_parallelization_research_collision_scan_ra_core_singleblock, experiments_2026_9_1_production_candidate_battery_results_key_zero_defect [EXTRACTED 1.00]
- **(a)/(a')/(b)/(c) hypothesis decision matrix synthesized in Langkah 4** — experiments_2026_8_29_parallelization_research_handover_1tb_followup_langkah4_sintesis, experiments_2026_8_29_parallelization_research_handover_1tb_followup_hypothesis_a, experiments_2026_8_29_parallelization_research_handover_1tb_followup_hypothesis_a_prime, experiments_2026_8_29_parallelization_research_handover_1tb_followup_hypothesis_b, experiments_2026_8_29_parallelization_research_handover_1tb_followup_hypothesis_c [EXTRACTED 1.00]
- **K=255 Throughput Benchmark: ra_core vs Philox** — experiments_2026_9_1_production_candidate_battery_bench_ra_core_results_philox_baseline, experiments_2026_9_1_production_candidate_battery_bench_ra_core_results_log [EXTRACTED 1.00]
- **KAT checksum cross-verification of rolling optimization correctness** — experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_singleblock_kat_checksums, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_orbit_kat_checksums, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_ra_core_v2_c, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_validate_command [EXTRACTED 1.00]
- **L Mechanism Spectrum Tested in Phase 1** — experiments_2026_8_28_state_update_mechanism_research_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_overwrite_mechanism [EXTRACTED 1.00]
- **M Mechanism Spectrum Tested in Phase 2** — experiments_2026_8_28_state_update_mechanism_research_xor_fold_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_permute_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_inject_mechanism, experiments_2026_8_28_state_update_mechanism_research_m_overwrite_mechanism [EXTRACTED 1.00]
- **64-bit Init Mixing-Formula Candidates (A/B/C) Bake-off** — experiments_2026_9_5_orbit_address_widen_64bit_candidate_a, experiments_2026_9_5_orbit_address_widen_64bit_candidate_b, experiments_2026_9_5_orbit_address_widen_64bit_candidate_c [EXTRACTED 1.00]
- **Four-Analysis Investigation Flow Resolving K=1 Orbit Defect** — experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_bijectivity_analysis, experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_fpf_histogram_analysis, experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_weyl_cancellation_refuted, experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_key_duplication_hypothesis [EXTRACTED 1.00]
- **Orbit K=1+hashed-key defect root-cause investigation chain** — experiments_2026_9_4_orbit_kmin_battery_addendum_hashed_key_gap, experiments_2026_9_4_orbit_init_zerotap_fix_results_zero_tap_hypothesis, experiments_2026_9_4_orbit_init_cycle_combo_search_handover_init_formula_axis, experiments_2026_9_4_orbit_init_cycle_combo_search_handover_cycle_transform_axis [EXTRACTED 1.00]
- **Orbit K-small defect identified and fixed via w8_f10_i0 on ra_core_orbit/singleblock identity** — experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_orbit_k_small_defect, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_ra_core_orbit, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_ra_core_singleblock, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_w8_f10_i0_fix [EXTRACTED 1.00]
- **Original Algorithm Avalanche Cycle Evolution (Cycle 1 vs Cycle 3)** — experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_heatmap, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_27_operand_position_search_other_avalanche_heatmap_original_cycle1_vs_cycle3_avalanche_diffusion_comparison [EXTRACTED 1.00]
- **Original pre-reseed BCFN defect: diagnosis, fix, and validation chain** — experiments_2026_9_1_multikey_remix_search_handover_doc, experiments_2026_9_4_orbit_reseed_postboundary_bcfn_handover_w8_f10_i0_fix, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_orbit, experiments_2026_9_4_orbit_kmin_battery_results_doc [EXTRACTED 1.00]
- **Pareto-selection axes (Speed A + Avalanche B feed the frontier)** — experiments_2026_9_3_combo_winner_pareto_selection_handover_axis_a_speed, experiments_2026_9_3_combo_winner_pareto_selection_handover_axis_b_avalanche_normality, experiments_2026_9_3_combo_winner_pareto_selection_handover_pareto_frontier [EXTRACTED 1.00]
- **Three enrichment slots forming the bounded per-round combo search space** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_search_space_dsl, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot1_o_tap_width, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot2_c_finalizer, experiments_2026_9_2_singleblock_cycle_combo_search_handover_slot3_extra_inject [EXTRACTED 1.00]
- **Post-reseed BCFN test design (isolate first cycle after one ra_reseed)** — experiments_2026_9_4_orbit_reseed_postboundary_bcfn_handover_doc, experiments_2026_9_1_family_productionization_ra_core_ra_core_orbit, experiments_2026_9_1_family_productionization_ra_core_ra_reseed, experiments_2026_9_1_family_productionization_ra_core_ra_hash, experiments_2026_9_4_orbit_reseed_postboundary_bcfn_multikey_stream_orbit_postreseed_k1, experiments_2026_9_4_orbit_reseed_postboundary_bcfn_multikey_stream_orbit_postreseed_k255 [EXTRACTED 1.00]
- **12-candidate PRNG family benchmark roster** — experiments_2026_8_31_prng_family_benchmark_handover_candidate_paperorig, experiments_2026_8_31_prng_family_benchmark_handover_candidate_wiredv2, experiments_2026_8_31_prng_family_benchmark_handover_candidate_addrcont, experiments_2026_8_31_prng_family_benchmark_handover_candidate_singleblock_k1, experiments_2026_8_31_prng_family_benchmark_handover_candidate_singleblock_k255, experiments_2026_8_31_prng_family_benchmark_handover_candidate_philox, experiments_2026_8_31_prng_family_benchmark_handover_candidate_xoshiro256, experiments_2026_8_31_prng_family_benchmark_handover_candidate_pcg32, experiments_2026_8_31_prng_family_benchmark_handover_candidate_chacha20, experiments_2026_8_31_prng_family_benchmark_handover_candidate_dev_urandom, experiments_2026_8_31_prng_family_benchmark_handover_candidate_mt19937, experiments_2026_8_31_prng_family_benchmark_handover_candidate_splitmix64 [EXTRACTED 1.00]
- **Cross-family PRNG speed comparison table (ra_prng orbit/singleblock vs other cores)** — experiments_2026_9_1_family_productionization_ra_core_ra_core_singleblock, experiments_2026_9_1_family_productionization_ra_core_ra_core_orbit, experiments_2026_8_31_prng_family_benchmark_benchmark_all_philox, experiments_2026_8_31_prng_family_benchmark_benchmark_all_splitmix64, experiments_2026_8_31_prng_family_benchmark_benchmark_all_pcg32, experiments_2026_8_31_prng_family_benchmark_benchmark_all_paperorig, experiments_2026_8_31_prng_family_benchmark_benchmark_all_xoshiro256, experiments_2026_8_31_prng_family_benchmark_benchmark_all_wiredv2, experiments_2026_8_31_prng_family_benchmark_benchmark_all_addrcont, experiments_2026_8_31_prng_family_benchmark_benchmark_all_chacha20, experiments_2026_8_31_prng_family_benchmark_benchmark_all_mt19937, experiments_2026_8_31_prng_family_benchmark_benchmark_all_dev_urandom [EXTRACTED 1.00]
- **11 Promotion-Tier FULLY CLEAN winners (K in {1,2,4,8,16,32,64,96} @16GB)** — experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f8_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f9_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f10_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f22_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f24_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f25_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f26_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f27_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f28_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f29_i0, experiments_2026_9_2_singleblock_cycle_combo_search_results_w8_f33_i0 [EXTRACTED 1.00]
- **pruned_winner Avalanche Analysis and Metric Evaluation** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_figure, experiments_2026_8_26_operation_pruning_research_pruned_winner_avalanche_behavior, experiments_2026_8_26_operation_pruning_research_pruned_winner_candidate_configuration [EXTRACTED 1.00]
- **ra_core throughput comparison vs Philox4x32/Xoshiro256**** — experiments_2026_9_1_family_productionization_ra_core, readme_philox4x32, readme_xoshiro256starstar [EXTRACTED 1.00]
- **Statistical test batteries used to validate ra_prng2** — readme_dieharder, readme_practrand, readme_nist_sts, readme_testu01_bigcrush [EXTRACTED 1.00]
- **Performance Benchmark Comparison Group** — readme_ra_prng2, readme_ra_prng3, readme_chacha20, readme_philox4x32, readme_dev_urandom [EXTRACTED 1.00]
- **Repository Structure Mapping (docs/STRUCTURE.md)** — readme_docs_structure, readme_ra_prng2, readme_ra_prng3, readme_benchmarks_comparisons_source [EXTRACTED 1.00]
- **Statistical Validation of ra_prng2** — readme_ra_prng2, readme_dieharder, readme_nist_sts, readme_testu01_bigcrush, readme_practrand [EXTRACTED 1.00]
- **Release Benchmark Prep Gate Components for ra_core.c Promotion** — experiments_2026_9_6_release_benchmark_prep_avalanche_curve_task, experiments_2026_9_6_release_benchmark_prep_throughput_reconfirmation, experiments_2026_9_6_release_benchmark_prep_production_promotion_decision [EXTRACTED 1.00]
- **Rolling-register wide-o optimization applied to both cycle functions in ra_core_v2.c** — experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_ra_core_v2_c, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_ra_permutation_cycle_orbit, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_ra_permutation_cycle_singleblock, experiments_2026_9_4_orbit_fix_and_wideo_rolling_optimization_handover_rolling_register_optimization [EXTRACTED 1.00]
- **Prior experiments that ruled out the init/seed fix direction** — experiments_2026_9_2_singleblock_cycle_combo_search_handover_singleblock_prereseed_experiment, experiments_2026_9_2_singleblock_cycle_combo_search_handover_ra_core_orbit_crosscheck, experiments_2026_9_2_singleblock_cycle_combo_search_handover_kandidat_a_wideo, experiments_2026_9_2_singleblock_cycle_combo_search_handover_kandidat_b_wideo_warmup [EXTRACTED 1.00]
- **winner_wired_v2 and winner_wired_addressable both implement the same shared core generation loop** — experiments_2026_8_29_parallelization_research_handover_1tb_followup_winner_wired_v2, experiments_2026_8_29_parallelization_research_handover_1tb_followup_winner_wired_addressable, experiments_2026_8_29_parallelization_research_handover_1tb_followup_core_generation_loop [EXTRACTED 1.00]
- **Prior Research Folders That Tested Only Single-Stream Axes** — concept_periodicity_heuristic_validation_research, concept_operation_pruning_research, concept_operand_position_search_research, concept_state_update_mechanism_research, concept_periodicity_heuristic_validation2_research [EXTRACTED 1.00]
- **Tahap 0 candidate init formulas (Kandidat 1-5) evaluated together** — experiments_2026_8_30_addressable_init_research_handover_kandidat1, experiments_2026_8_30_addressable_init_research_handover_kandidat2, experiments_2026_8_30_addressable_init_research_handover_kandidat3_control, experiments_2026_8_30_addressable_init_research_handover_kandidat4, experiments_2026_8_30_addressable_init_research_handover_kandidat5 [EXTRACTED 1.00]
- **Tahap 1 Bit-Identical Unification** — experiments_2026_9_1_family_productionization_handover_tahap1_unification, experiments_2026_8_27_operand_position_search_winner_wired_v2_winner_wired_v2, experiments_2026_8_30_addressable_init_research_winner_wired_addressable_winner_wired_addressable [EXTRACTED 1.00]
- **Tahap 2 Production Validation Battery** — experiments_2026_9_1_family_productionization_handover_tahap2_validation_battery, experiments_2026_9_1_family_productionization_handover_winner_wired_unified, experiments_2026_8_30_addressable_init_research_tahap6_bench_ra_core_singleblock, quality_gate_avalanche_gate_min_bit [EXTRACTED 1.00]
- **Two-Stage Fix Strategy (Wide-o + Warm-up Fallback)** — experiments_2026_9_2_singleblock_o_width_fix_handover_wide_o_candidate, experiments_2026_9_2_singleblock_o_width_fix_handover_warmup_fallback_candidate, project_multikey_remix_search [EXTRACTED 1.00]
- **Watchdog process-matching bug, its fix, and the sweep changes it required** — experiments_2026_8_28_periodicity_heuristic_validation2_handover_watchdog_pgrep_bug, experiments_2026_8_28_periodicity_heuristic_validation2_handover_auto_stop_power38, experiments_2026_8_28_periodicity_heuristic_validation2_handover_cmd_pattern_fix, experiments_2026_8_28_periodicity_heuristic_validation2_handover_run_sweep, experiments_2026_8_28_periodicity_heuristic_validation2_handover_manual_only_exclusion [EXTRACTED 1.00]
- **10 of 11 Promotion Tier winners share an XORSHIFT-based finalizer stage as the dominant recurring pattern** — w8_f8_i0, w8_f9_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f10_i0, w8_f22_i0, w8_f24_i0, w8_f25_i0, w8_f26_i0, w8_f27_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f28_i0, w8_f29_i0 [EXTRACTED 1.00]
- **D vs E Final Decision: Identical Statistical Gates, Decided by GPU Portability Tie-break** — experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_d, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_e, experiments_2026_9_5_orbit_address_widen_64bit_results_gpu_portability_tiebreak, experiments_2026_9_5_orbit_address_widen_64bit_results_avalanche_3way_test, experiments_2026_9_5_orbit_address_widen_64bit_results_practrand_gap16_test, experiments_2026_9_5_orbit_address_widen_64bit_results_fpf_collision_scan_test, experiments_2026_9_5_orbit_address_widen_64bit_results_dieharder_battery [INFERRED 0.75]
- **Array-Native Data Permutation and Fingerprinting Workflow** — research_iccs2026_ra_prng_avalanche_sensitivity, sub_projects_fingerprinting_tools_concept, benchmarks_comparisons_source_zepfold_cli [INFERRED 0.75]
- **PractRand anomaly signals observed across both candidates** — experiments_2026_9_2_singleblock_o_width_fix_results_bcfn, experiments_2026_9_2_singleblock_o_width_fix_results_tmfn, experiments_2026_9_2_singleblock_o_width_fix_results_fpf14 [INFERRED 0.75]
- **Cross-session convergence on the a=(d^o)^(cons+a) combination point as root-cause locus** — experiments_2026_9_5_orbit_o_noshift_fix_results_combination_point_concept, experiments_2026_9_5_orbit_o_noshift_fix_results_staggering_shift_hypothesis, experiments_2026_9_5_orbit_k1_exhaustive_function_analysis_handover_k1_orbit_defect, experiments_2026_9_5_orbit_o_noshift_fix_results_o_noshift_candidate [INFERRED 0.80]
- **K-kecil Defect Discovery Chain** — experiments_2026_9_2_singleblock_o_width_fix_handover_k_kecil_defect, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_singleblock, experiments_2026_9_2_singleblock_k_threshold_characterization_results, experiments_2026_9_1_production_candidate_battery [INFERRED 0.80]
- **Multi-tier statistical validation pipeline (avalanche gate, PractRand, dieharder) used across all candidate fixes** — concept_practrand, concept_dieharder, concept_production_readiness_gate, concept_tools_battery_tool [INFERRED 0.80]
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
- **Nine independent fix/diagnosis axes tried against orbit K=1+hashed defect (all failed or refuted)** — concept_zero_tap_hypothesis, concept_init_cycle_combo_search, concept_d0_hash_candidate, concept_d0_realround_candidate, concept_weyl_constant_resonance_hypothesis, concept_o_fmix_init_only_candidate, concept_o_fmix_every_round_candidate, concept_ad_mul_combine_candidate, concept_round1_collision_hypothesis, concept_d0_raw_cons_candidate, concept_weyl_splitmix_finalizer_redesign [INFERRED 0.85]
- **5 Candidates Evaluated Through the Same Statistical Gate Battery (avalanche, Gap-16, FPF, dieharder)** — experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_a, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_b, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_c, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_d, experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_e, experiments_2026_9_5_orbit_address_widen_64bit_results_avalanche_3way_test, experiments_2026_9_5_orbit_address_widen_64bit_results_practrand_gap16_test, experiments_2026_9_5_orbit_address_widen_64bit_results_fpf_collision_scan_test, experiments_2026_9_5_orbit_address_widen_64bit_results_dieharder_battery [INFERRED 0.85]
- **Q1-Q4 Parallelization Research Questions** — concept_stream_independence_testing, concept_seed_collision_probability, concept_throughput_scalability, concept_simd_vectorization_feasibility [INFERRED 0.85]
- **Pareto Frontier Selection over Combo-Winner Candidates** — experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f10_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f28_i0, experiments_2026_9_2_singleblock_cycle_combo_search_candidates_w8_f25_i0, experiments_2026_9_3_combo_winner_pareto_selection_pareto_select [INFERRED 0.85]
- **Q1 cross-stream independence investigation (Method A + Method B)** — experiments_2026_8_29_parallelization_research_results_q1_independence, experiments_2026_8_29_parallelization_research_results_method_a_cross_correlation, experiments_2026_8_29_parallelization_research_results_method_b_interleaved_practrand, experiments_2026_8_29_parallelization_research_results_cross_correlation_py, experiments_2026_8_29_parallelization_research_results_interleave_practrand_py [INFERRED 0.85]
- **Candidate D Implements Modified ra_core.c Init/Signature Functions, 255-Round Engine Unchanged** — experiments_2026_9_5_orbit_address_widen_64bit_candidates_candidate_d, experiments_2026_9_1_family_productionization_ra_core_ra_init_state_orbit, experiments_2026_9_1_family_productionization_ra_core_ra_init_state_singleblock, experiments_2026_9_1_family_productionization_ra_core_ra_core_orbit, experiments_2026_9_1_family_productionization_ra_core_ra_core_singleblock, experiments_2026_9_5_orbit_address_widen_64bit_results_ra_permutation_cycle_verbatim [INFERRED 0.85]
- **RNG Statistical Test Suites Used for Empirical Validation** — concept_testu01_bigcrush, concept_dieharder, concept_nist_sts, concept_practrand, concept_ent [INFERRED 0.85]
- **K-small BCFN defect fix search (Kandidat A/B -> no-recommendation)** — experiments_2026_9_2_singleblock_o_width_fix_results_kandidat_a, experiments_2026_9_2_singleblock_o_width_fix_results_kandidat_b, experiments_2026_9_2_singleblock_o_width_fix_results_rekomendasi, experiments_2026_9_2_singleblock_k_threshold_characterization_results [INFERRED 0.85]
- **Rantai investigasi defect K kecil ra_core_singleblock** — experiments_2026_9_1_keyzero_guard_fix_results_k1_singleblock_defect, experiments_2026_9_2_singleblock_prereseed_experiment_results, experiments_2026_9_2_singleblock_k_threshold_characterization_results_safe_k_threshold [INFERRED 0.90]
- **Pruned Winner Cycle 1 vs Cycle 3 Avalanche Comparison** — experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_figure, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle1_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_cycle3_avalanche_fraction, experiments_2026_8_26_operation_pruning_research_avalanche_heatmap_pruned_winner_cycle1_vs_cycle3_reseed_avalanche_diffusion [INFERRED 0.95]
- **ra_core.c static functions verbatim-copied into diag_init_candidates.c** — experiments_2026_9_3_init_loop_optimization_diag_init_candidates, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_orbit, experiments_2026_9_1_family_productionization_ra_core_ra_permutation_cycle_singleblock, experiments_2026_9_1_family_productionization_ra_core_ra_hash, experiments_2026_9_1_family_productionization_ra_core_ra_reseed, experiments_2026_9_1_family_productionization_ra_core_rot32, experiments_2026_9_1_family_productionization_ra_core_fmix32, experiments_2026_9_1_family_productionization_ra_core_ra_init_state_orbit [INFERRED 0.95]
- **Avalanche Proxy vs Full Empirical Quality Disconnect** — experiments_2026_8_26_operation_pruning_research_results_shift_repair_failure, experiments_2026_8_26_operation_pruning_research_status_gate_size_traps, experiments_2026_8_27_operand_position_search_handover_dead_seed_bits, experiments_2026_8_27_operand_position_search_handover_avalanche_heatmap_baseline [INFERRED]
- **Pruned Winner Multi-battery Statistical Validation** — experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_26_operation_pruning_research_practrand_pruned_winner_1tb_practrand_1tb_log, experiments_2026_8_26_operation_pruning_research_dieharder_pruned_winner_piped_piped_results, experiments_2026_8_26_operation_pruning_research_dieharder_gcd_fixed_gcd_test_log, experiments_2026_8_26_operation_pruning_research_dieharder_rgb_minimum_distance_fixed_rgb_min_dist_log [INFERRED]
- **ra_prng Algorithmic Evolution and Research Pipeline** — readme_ra_prng2, experiments_2026_8_26_operation_pruning_research_results_pruned_winner, experiments_2026_8_27_operand_position_search_handover_handover_spec [INFERRED]

## Communities (579 total, 117 thin omitted)

### Community 0 - "winners_bench.c"
Cohesion: 0.08
Nodes (57): core_entry_t, find_core(), main(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10(), philox4x32_round() (+49 more)

### Community 1 - "benchmark_all.c"
Cohesion: 0.15
Nodes (28): FILE, kat_philox(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), ra_core_baseline(), ra_core_p1(), ra_core_p2() (+20 more)

### Community 2 - "tahap4_bench.c"
Cohesion: 0.06
Nodes (60): main(), philox4x32_10(), philox4x32_round(), main(), philox4x32_10(), philox4x32_round(), philox_ctr_increment(), philox_rand32() (+52 more)

### Community 3 - "ra_core.c"
Cohesion: 0.08
Nodes (37): operation-pruning-research experiment (gate-trap example), core_entry_t, core_fn_t, checksum_key_core(), find_core(), fmix32(), main(), ra_init_state_orbit() (+29 more)

### Community 4 - "Production Readiness Handover (ra_core_orbit/ra_core_singleblock)"
Cohesion: 0.10
Nodes (44): a=(d*o)^(cons+a) MUL-combine candidate, d0_hash candidate (key-dependent seed d via fmix32), d0_raw_cons candidate (d = cons & 0xFF, no hash), d0_realround candidate (warm-up round i=255 recomputed), DC6-9x1Bytes-1 PractRand test (new K=255 regression signature), fmix32 finalizer (MurmurHash3-style), FPF-14+6/16 PractRand test family (defect signature), Init x cycle-transform 8x4 combo search (0/32 survivors) (+36 more)

### Community 5 - "Completed Tasks"
Cohesion: 0.07
Nodes (41): 2026-08-29 correction: (8,4,rows=4) watchdog bug + stale headline number, auto_stop_power38.py (watchdog script), avalanche_check.py, .cycle_measure_ckpt_n8_w4_rows4_seed0.bin (CkptHeader checkpoint), Fix: CMD_PATTERN uses exact argv instead of broad pgrep prefix, Corrected λ > 3.045×10^11 for (8,4,rows=4), cycle_measure.py / cycle_measure.c (Brent's algorithm), enumerate_n2w4.py (+33 more)

### Community 6 - "tahap0_prototype.py"
Cohesion: 0.09
Nodes (40): blake2b8(), init_kandidat1(), init_kandidat2(), init_kandidat3_control(), init_kandidat4(), init_kandidat5(), init_original(), main() (+32 more)

### Community 7 - "Handover: uji 2 kandidat fix tertarget (seed `d0` di fase init) untuk defect K=1+hashed pada `ra_core_orbit`"
Cohesion: 0.06
Nodes (38): singleblock-prereseed-experiment, 0. WAJIB: checkpoint ke user sebelum implementasi, 10. Referensi cepat, 1. Kenapa search ini perlu (ringkasan, baca sumber untuk detail lengkap), 2. Scope, 3. Kandidat A — `d0_hash` (sintetis, murah), 4. Kandidat B — `d0_realround` (satu ronde asli i=255, dibuang, dihitung di fase init), 5. Perubahan kode (minimal, hanya signature — lihat §0.5) (+30 more)

### Community 8 - "ra_core_orbit"
Cohesion: 0.07
Nodes (40): main(), splitmix32(), main(), PRODUCTION_READINESS_HANDOVER.md, FILE, ra_core_orbit(), ra_hash(), ra_permutation_cycle_orbit() (+32 more)

### Community 9 - "Singleblock Cycle-Operation Combo Search Handover"
Cohesion: 0.10
Nodes (32): ra_core_singleblock, Singleblock Cycle-Operation Combo Search Handover, avalanche_gate_min_bit (vs scalar-average avalanche_gate), BCFN/TMFn/FPF structural defect at small K, bench_ra_core.c (reinit-sweep microbench pattern), CLAUDE.md workflow rule: run /graphify --update before closing task, diag_wideo_singleblock.c / diag_wideo_warmup_singleblock.c (multikey --stream driver pattern), fmix32 (+24 more)

### Community 10 - "Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey)"
Cohesion: 0.22
Nodes (9): Candidates tried so far, Context: where this came from, Handover: mekanisme mixing murah untuk pola aggressive-reinit (multikey), Non-goals, Non-regression verification results (2026-09-01) -- verification bar item 2 DONE, Process note (per explicit user feedback this session), Root cause (conclusively diagnosed this session — do not re-litigate), The task: find something cheaper than a full/partial warm-up cycle (+1 more)

### Community 11 - "ra_init_state_orbit"
Cohesion: 0.31
Nodes (9): f(key) exhaustive-characterization function, fmix32, GUARD_M constant, ra_core_orbit, ra_init_state_orbit, ra_permutation_cycle_orbit, rot32, Combination point a=(d^o)^(cons+a) as suspected root-cause locus (+1 more)

### Community 12 - "combo_prng_warmup.py"
Cohesion: 0.11
Nodes (28): generate_all(), Path, Per-variant C codegen for the orbit init-warmup fix experiment (HANDOVER.md),…, Compare compiled binary's --single output against combo_prng_warmup.py's…, The mandatory hand-trace: control (production behavior byte-for-byte) must…, render_c(), sanity_check(), verify_control_matches_production() (+20 more)

### Community 13 - "practrand_gate.py"
Cohesion: 0.11
Nodes (26): avalanche_gate_min_bit(), avalanche_stats(), capture_via_binary(), hamming(), main(), Path, Run `<binary> [<core>] --single <seed> <cycle_words>` and parse raw uint32 LE…, Flip each of the 32 seed bits, capture one cycle via `capture_fn(seed) ->… (+18 more)

### Community 14 - "ra_core_singleblock Cycle-Operation Combo Search — RESULTS"
Cohesion: 0.12
Nodes (33): operand_search.py (operation-pruning-research), pruned_winner — dead-bit defect candidate from operand-position-search: seed bits 5/6 nearly non-avalanching across 253/255 output positions, mean per-bit fraction 0.0078, passed old scalar gate but caught by full K=255 heatmap, combo_search_log.jsonl, promotion_log.jsonl / promotion_logs/*.log, ra_core_singleblock Cycle-Operation Combo Search — RESULTS, Promotion Tier (29 candidates x 8 K-values x 16GB, VPS), Tier 0 — avalanche gate (min-per-bit, K=1), w8_f10_i0 (width=8, finalizer XORSHIFT(17)) (+25 more)

### Community 15 - "diag_wideo_warmup_singleblock.c"
Cohesion: 0.10
Nodes (28): FILE, fmix32(), main(), narrow_o(), now_seconds(), pick_use_wide(), ra_core_singleblock_warmup(), ra_init_state_singleblock() (+20 more)

### Community 16 - "diag_init_candidates.c"
Cohesion: 0.13
Nodes (34): Accum candidate (ring-arithmetic strength reduction), Backend-bound execution stalls (K=255 singleblock_accum regression), core_entry_t, FILE, find_core(), fmix32(), main(), now_seconds() (+26 more)

### Community 17 - "2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md"
Cohesion: 0.06
Nodes (34): candidates/init_linear_shared__w8_f10_i0 (kontrol/produksi binary), linear_p1 IDX_XFORM candidate (index i+1), Retracted claim: ra_hash/ra_reseed pointed as most likely root cause, init_linear_p1_shared x w8_f10/f9_i0 FAIL even under Weyl ordering (secondary finding), Corrected insight: K=1 tests quality of orbit_round1 as hash function key->1 word, Zero-Tap Init Fix Results, Hypothesis (see conversation, 2026-09-04), Patch tested (+26 more)

### Community 18 - "ra_prng2 (32-bit generator, CSAI2025)"
Cohesion: 0.11
Nodes (26): ra_prng2 NIST STS Test Result, ra_prng2 PractRand Test Result (up to 128 GB, no anomalies), ra_prng README, Hamas A. Rahman (author, @hamzy-hams), benchmarks/comparisons (throughput/entropy comparisons), benchmarks/comparisons file-shuffling CLI tool, benchmarks/comparisons/source (file-shuffling CLI tool), bindings/cpython (pip package) (+18 more)

### Community 19 - "ra_prng2 (Array-Based PRNG)"
Cohesion: 0.09
Nodes (24): PractRand, BigCrush (TestU01), Bit Shifting, Rationale: Branchless Design for CPU Pipeline Efficiency, ChaCha20, Claude E. Shannon, Daniel Lemire, ent Entropy Tool (+16 more)

### Community 20 - "RESULTS: Priority 1 — Init-loop speed optimization"
Cohesion: 0.17
Nodes (12): Against the HANDOVER talking-point thresholds, Assembly diff (`gcc -O3 -march=native -S`, extracted per-function), Benchmark (K=1, `--bench <core> 50000000 1`, 3 runs each, min/best-of-3 internal to each run), Benchmark (K=255, `--bench <core> 50000000 255`, 3 runs each) — the candidate is NOT a strict win across all K, Benchmark (orbit `--stream`, single continuous call, no forced reinit), Candidate implemented, Open question for the user, `ra_hash`/`ra_reseed` (+4 more)

### Community 21 - "common.py (TIERS_Q2)"
Cohesion: 0.10
Nodes (19): collision_scan.py, common.py (TIERS_Q2), main(), Non-regression check for `ra_init_orbit` (the multikey mix-init fix candidate,…, run_sequential(), Path, Langkah 2 of HANDOVER_1TB_FOLLOWUP.md: re-run the interleaved 1TB Method B test…, run_live() (+11 more)

### Community 22 - "Langkah 3: Interleaved 1TB for winner_wired_addressable (shared core-loop test)"
Cohesion: 0.13
Nodes (26): winner_wired_addressable Method A+B validated clean up to 128GB (xlarge tier), re-verified raw stdout this session, interleave_practrand_1tb_confirm.py (planned; reuse run_interleave_practrand with alt seeds), Shared core generation loop: ra_permutation_cycle / ra_reseed / ra_core (identical between winner_wired_v2.c and winner_wired_addressable.c; only ra_init_state[_addressable] differs), 1TB Anomaly Diagnostic Plan (Langkah 1-4), GATED_TIERS explicit-invocation gate (1tb/xlarge require explicit arg, unlike smoke/medium/full), HANDOVER_TAHAP5.md §6 convention: full Tahap 0-3 addressable-init repeat required if init formula changes, Harness bug: JSON 'passed' field mismatches lowercase PractRand 'suspicious'/'very suspicious' tags (case-sensitivity), not fixed in-place, (a) Cross-stream correlation specific to winner_wired_v2 init formula (+18 more)

### Community 23 - "Pareto Selection Results (Combo Winners)"
Cohesion: 0.18
Nodes (18): Sanity-check: orbit's implausible 2.4x speed vs old singleblock was a cross-session machine-speed artifact, Deviation: baseline copied from ra_core.c not benchmark_all.c (stale init formula), addrcont core, chacha20 core, dev_urandom baseline, mt19937 core, paperorig core, pcg32 core (+10 more)

### Community 24 - "exhaustive_analysis.c"
Cohesion: 0.17
Nodes (23): alloc_all(), chi2_independence(), count_low_zeroes32(), counter1_bump(), counter1_report(), ct_bump(), f_key(), fmix32() (+15 more)

### Community 25 - "tahap6_bench.c"
Cohesion: 0.19
Nodes (24): core_entry_t, FILE, find_core(), main(), mode_init_cost(), mode_reinit_sweep(), mode_throughput(), now_seconds() (+16 more)

### Community 26 - "Random"
Cohesion: 0.12
Nodes (19): chi_square_uniform(), entropy(), multi_key_distinctness_check(), Shuffle a fixed-size sample of input_tokens with n_keys different random keys;…, read_tokens(), runs_test(), serial_correlation(), multi_key_distinctness_check() (+11 more)

### Community 27 - "cycle_analysis_weylfinal.py"
Cohesion: 0.16
Nodes (21): emit(), fmix32(), main(), make_finalizer(), Matches multikey_stream_hashed.c splitmix32(uint64_t*) exactly -- 64-bit…, splitmix32_stream(), triple32(), weyl_core() (+13 more)

### Community 28 - "Dieharder Test Suite"
Cohesion: 0.12
Nodes (23): Dieharder Test Results (ra_prng2 original), RNGing perf stat (ra_prng2 original), Shuffling perf stat (ra_prng2 original), TestU01 BigCrush Results (ra_prng2 original), Dieharder Test Results (ra_prng3), ent Entropy Test Results (ra_prng3), NIST STS Test Results (ra_prng3), RNGing perf stat (ra_prng3) (+15 more)

### Community 29 - "README.md"
Cohesion: 0.11
Nodes (21): avalanche_orbit_kmin.py (per-bit Hamming-distance gate), collision_scan_orbit_k1.py (Poisson-z collision model, 50000 keys), multikey_stream_orbit_k1 / multikey_stream_orbit_k255 chaining drivers, Repo rule: no re-seed retry on PractRand FAIL, ORBIT_KAT_CHECKSUMS (known-answer-test golden checksums, new), orbit-kmin-battery gate (multistream K=1/K=255 statistical re-validation), PractRand 16GB test (RNG_test stdin32 -tlmax 16GB -multithreaded), ra_core.c (canonical production file) (+13 more)

### Community 30 - "winner_wired_v2.c"
Cohesion: 0.18
Nodes (18): Four-Axis Validation: periodicity, PractRand, avalanche, speed, permute + xor_fold State-Update Mechanism (kept unchanged), FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle() (+10 more)

### Community 31 - "combo_gen.py"
Cohesion: 0.18
Nodes (21): CORES[] function-pointer dispatch table (ra_core.c), generate_all_tier01(), Path, Compare compiled binary's --single output (multi-round, rng=37) against…, The ONE true hand-trace: width=2, finalizer=empty(idx0), inject=off(idx0) must…, render_c(), sanity_check(), verify_control_matches_production() (+13 more)

### Community 32 - "Wiring"
Cohesion: 0.12
Nodes (31): _capture_first_cycle(), _log(), main(), Driver for the operand-position rewiring search over wiring.py's 108 candidate…, tier0_avalanche(), tier1_practrand_small(), capture(), main() (+23 more)

### Community 33 - "combo_gen_orbit.py"
Cohesion: 0.19
Nodes (20): generate_all(), Path, Per-candidate C codegen for the orbit init-formula x cycle-transform combo…, Compare compiled binary's --single output (multi-round, rng=37) against…, The mandatory hand-trace: (init_linear_shared, w8_f10_i0) must reproduce…, render_c(), sanity_check(), verify_control_matches_production() (+12 more)

### Community 34 - "ra_core_v3.c"
Cohesion: 0.18
Nodes (21): main(), splitmix32(), core_entry_t, core_fn_t, FILE, checksum_key_core(), find_core(), fmix32() (+13 more)

### Community 35 - "bit_bias_analysis.py"
Cohesion: 0.18
Nodes (18): bit_frequencies(), bit_frequencies_fast(), iter_words(), main(), Path, Part B: bit/position bias localization for the K=1+hashed-key defect. Reads raw…, Return (n_words, counts[32]) where counts[b] = number of words with bit b set., Vectorized-ish version: unpack in bulk, tally via bit masks per chunk. (+10 more)

### Community 36 - "HANDOVER — Refinement akhir `ra_core_singleblock`: difusi-di-akhir, optimasi, eksperimen kecil"
Cohesion: 0.10
Nodes (21): Benchmark 5-angka standard (orbit K1/K255/--stream + singleblock K1/K255), Candidate w8_f10_i3 (add_a offset 64, FAIL at K=4), combo_gen.write_and_compile (candidate binary generator), shuffled_k1.txt (raw shuffle output data, K=1), shuffled_k255.txt (raw shuffle output data, K=255), 1. Baseline: `ra_core_baseline.c` (REFERENSI BEKU, jangan diedit), 2. Struktur round saat ini — di mana "difusi" terjadi, 3. Arah kerja (dari user, DICATAT SEBAGAI ARAH — bukan spek final) (+13 more)

### Community 37 - "State-Update Mechanism Research Results (Phase 1-3)"
Cohesion: 0.18
Nodes (20): cycle_measure.c (toy model), load_means(), main(), avalanche_gate_min_bit metric (per-bit floor 0.2, band [0.3,0.7]), State-Update Mechanism Research Handover (original spec), Handover Phase 3: L/M Mechanism Follow-up, inject (L) - one-directional overwrite mechanism, Instruction count (perf stat) as primary speed axis (+12 more)

### Community 38 - "pruned_winner 4-Operation Fast Variant"
Cohesion: 0.14
Nodes (18): Dieharder DNA Test Log (Suspect/EOF Excluded), Dieharder Marsaglia-Tsang GCD Test Results, Dieharder 27 Piped Test Results, Dieharder Rewind Artifact Failure Log, Dieharder RGB Minimum Distance Test Results (ntup=2..5), PractRand 128GB Test Output (0 Anomalies), PractRand 16GB Test Output (0 Anomalies), PractRand 1TB Test Output (0 Anomalies, 304 Checks) (+10 more)

### Community 39 - "other/avalanche_heatmap_original.py"
Cohesion: 0.14
Nodes (21): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), Original Algorithm Avalanche Diffusion Comparison (Cycle 1 vs Cycle 3), Original Algorithm Cycle 1 Avalanche Fraction (0.497576), Original Algorithm Cycle 3 Avalanche Fraction (0.499740), Avalanche Heatmap: Original Algorithm (ALL_OPS) -- Cycle 1 vs. Cycle 3, main() (+13 more)

### Community 40 - "cross_correlation.py"
Cohesion: 0.15
Nodes (19): analyze_group(), build_group(), main(), build_group(), main(), Non-regression check for `ra_init_orbit` (the multikey mix-init fix candidate,…, run(), ndarray (+11 more)

### Community 41 - "bench_release.c"
Cohesion: 0.20
Nodes (23): bench_init_orbit(), bench_init_philox(), bench_init_singleblock(), bench_init_xoshiro(), bench_real_reinit(), bench_real_reinit4(), bench_real_stream(), bench_real_stream4() (+15 more)

### Community 42 - "64-bit Address Widening Handover"
Cohesion: 0.13
Nodes (22): 64-bit Widening Test Battery Plan (avalanche/collision/PractRand/dieharder/bench), Candidate A: Dual-Keyterm Multiplicative Combine, Candidate B: SplitMix64-style 64-bit Pre-mix, Candidate C: param2-in-Rotation-Only, Execution Guardrails / Prohibitions for 64-bit Widening, 64-bit Address Widening Handover, Second-Parameter Naming Debate (counter vs salt/addr_hi), Section 5: 64-bit Address Space Recommendation (quantitative projection) (+14 more)

### Community 43 - "driver_main_template.c"
Cohesion: 0.13
Nodes (16): FILE, fmix32(), my_init(), my_transform(), rot32(), FILE, fmix32(), my_init() (+8 more)

### Community 44 - "2026-8-25_periodicity-heuristic-validation/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 45 - "avalanche_heatmap_pruned_winner.py"
Cohesion: 0.14
Nodes (20): analyze_cycle(), compute_avalanche_matrix(), Reseed-Driven Avalanche Strengthening Effect, Initial State Avalanche Fraction (Cycle 1: 0.418459), Post-Reseed Avalanche Fraction (Cycle 2: 0.468409), Avalanche Effect Comparison Heatmap (pruned_winner Cycle 1 vs Cycle 2), Pruned Winner Operation Set (TAP6, TAP7, ROT_C, SHR13), main() (+12 more)

### Community 46 - "Candidate"
Cohesion: 0.16
Nodes (21): Candidate, final_cons(), _build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32() (+13 more)

### Community 47 - "2026-8-28_periodicity-heuristic-validation2/cycle_measure.c"
Cohesion: 0.21
Nodes (19): brent_resumable(), FILE, State, init_state(), load_checkpoint(), main(), next_state(), params_init() (+11 more)

### Community 48 - "stream_values"
Cohesion: 0.14
Nodes (17): ensure_ra_prng2_cli(), ndarray, Path, Shared helpers for the parallelization-research harness. Never modifies…, Bounded capture: run `binary --stream seed n`, return n uint32 values. Suitable…, Streaming capture: caller reads/closes proc.stdout incrementally. Use for large…, Compile the optional paper-exact comparator into this folder, if not already…, stream_popen() (+9 more)

### Community 49 - "c/ra_prng2.c"
Cohesion: 0.24
Nodes (12): Read-only constraint on src/ra_prng2, src/ra_prng3, winner_wired_v2.c, Parallelization Research Results, ra_prng2.c vs winner_wired_v2.c independence comparison, Candidate: ra_prng2 original (paperorig), FILE, main(), ra_core(), ra_hash() (+4 more)

### Community 50 - "ICCS 2026 Research Paper: Array-Native Randomness"
Cohesion: 0.12
Nodes (19): Benchmark Source Comparisons Documentation, ZepFold Shuffling CLI Tool, Toy PRNG Avalanche Sanity Verification Gate, Brent Cycle Detection Methodology, Periodicity Validation Experiment Handover, Periodicity Heuristic Validation Results Report, Periodicity Heuristic Scalability & Non-Monotonicity Verdict, Initial Sweep Periodicity Measurement Summary (+11 more)

### Community 51 - "ra_core_singleblock"
Cohesion: 0.22
Nodes (13): winner_wired_v2.c, HANDOVER_TAHAP6.md, Tahap 6 hard scope limit (rng<=255, hard-abort guard), ra_core_singleblock(), winner_wired_addressable.c, Candidate: addressable agresif K=255 (singleblock), prng-family-benchmark experiment (12 candidates), ra_prng-family (unified codebase concept) (+5 more)

### Community 52 - "avalanche_stats"
Cohesion: 0.10
Nodes (30): _log(), main(), phase_a_hash_access_check(), phase_b_greedy_ablation(), Driver for the operation-pruning greedy ablation search. Two phases: Phase A:…, all_ops_baseline(), describe(), ops_to_bitmask() (+22 more)

### Community 53 - "Handover: PRNG family benchmark (ra_prng semua varian vs PRNG terkenal)"
Cohesion: 0.10
Nodes (18): Addressable-Init Research Results, Hasil (elapsed time, detik, `perf stat -r 30`), Interpretasi, Konteks tabel lama, Metodologi, Pertanyaan, Reproduksi, Speed Benchmark: `scrambler_addressable` vs Algoritma Lain (+10 more)

### Community 54 - "ra_core_v2.c"
Cohesion: 0.23
Nodes (18): core_entry_t, core_fn_t, FILE, checksum_key_core(), find_core(), fmix32(), main(), ra_core_orbit() (+10 more)

### Community 55 - "candidate_a.c"
Cohesion: 0.22
Nodes (18): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+10 more)

### Community 56 - "HANDOVER — release-benchmark-prep (2026-09-06)"
Cohesion: 0.09
Nodes (23): Avalanche test: per-bit avalanche fraction heatmap (flipped seed bit x output word index), Avalanche heatmap K=255 (full cycle): w8_f24_i0, Candidate w8_f24_i0 (tap width=8, finalizer variant f24), Avalanche Curve Results: 6/6 Combos PASS Band [0.3,0.7], Avalanche-Effect Curve Benchmark Task (3-scheme, paper methodology), bench_release Rerun Raw Output 2026-09-06, Release Benchmark Prep Handover, File yang dihasilkan (+15 more)

### Community 57 - "avalanche_heatmap_winner_v2.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for `winner_wired_v2.c` (wiring "v08" in RESULTS.md's…, Mirrors winner_wired_v2.c's ra_reseed()/ra_hash() exactly (identical to… (+9 more)

### Community 58 - "Operand Position Search Experiment Results"
Cohesion: 0.07
Nodes (33): baseline_local PractRand 16GB Test Log, v01 PractRand 16GB Test Log (FAIL), v02 PractRand 16GB Test Log (FAIL), v03 PractRand 16GB Test Log (Pass), v04 PractRand 16GB Test Log (Pass), v06 PractRand 16GB Test Log (Pass), v07 PractRand 16GB Test Log (Pass), v08 PractRand 16GB Test Log (Pass) (+25 more)

### Community 59 - "ra_core (unified addressable-init successor)"
Cohesion: 0.16
Nodes (18): Orbit (deterministic trajectory selected by key), Point Addressing (Philox f(counter,key)), Masked-wrap v1 attempt (regressed at large K), Release benchmark (bench_release.c, 2026-09-04), Split-loop fix for variable-length M[] init, Variable-length M[] init optimization (singleblock), Addressable / Orbit-style Initialization Research, 2026-09-04 Release Benchmark Table (+10 more)

### Community 60 - "winner_wired_addressable.c"
Cohesion: 0.17
Nodes (17): Orbit Engine, L[256] array (dead-code discovery), Baseline decision: winner_wired_addressable.c (portable), not _v2.c, Tahap 2: C Implementation, FILE, main(), Orbit Addressing formula, ra_core() (+9 more)

### Community 61 - "ra_core_singleblock"
Cohesion: 0.05
Nodes (45): pruned_prng.c, build_o() (original 8-tap formula), main(), multikey_stream.c Weyl-increment multikey pattern (base pattern), ra_core_singleblock(), ra_permutation_cycle_singleblock(), main(), Tahap 1: Unifikasi ra_core.c (orbit + singleblock) (+37 more)

### Community 62 - "combo_prng.py"
Cohesion: 0.22
Nodes (16): apply_finalizer(), fmix32(), ra_init_state_singleblock(), Pure-Python reference oracle for the cycle-operation combo DSL. Used by Tier 0…, Single round1 output word: matches ra_core_singleblock(key, rng=1) with…, Full multi-round stream for one ra_core_singleblock_combo(key, rng) call, rng…, rot32(), round1() (+8 more)

### Community 63 - "ra_core_baseline.c"
Cohesion: 0.25
Nodes (17): core_entry_t, FILE, checksum_key(), find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock() (+9 more)

### Community 64 - "combo_prng_ofmix.py"
Cohesion: 0.22
Nodes (16): fmix32(), orbit_round1(), orbit_stream_full(), _production_round1(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_ofmix(), ra_reseed() (+8 more)

### Community 65 - "candidate_c.c"
Cohesion: 0.22
Nodes (17): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+9 more)

### Community 66 - "avalanche_effect_analysis.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_reseed(), plot_comparison(), plot_single(), Path (+9 more)

### Community 67 - "others/avalanche_heatmap_original.py"
Cohesion: 0.18
Nodes (17): analyze_cycle(), compute_avalanche_matrix(), cross_check_final_cons(), main(), original_capture(), original_reseed(), plot_comparison(), plot_single() (+9 more)

### Community 68 - "Params"
Cohesion: 0.15
Nodes (10): group_size(), Params, Rescale the four fixed 32-bit plain-shift amounts to width w. s_w = round(s * w…, Top-w-bits truncation of a 32-bit golden-ratio-derived constant. Trap fix:…, G(n, rows): generalization of ra_hash's fixed 8/32 structural constants (256 =…, rescale_shifts(), truncate_const(), enumerate_state_space() (+2 more)

### Community 69 - "RESULTS: continuous-stream real benchmark untuk `ra_core_orbit`"
Cohesion: 0.17
Nodes (12): Closing note, Continuous-stream, sesi/mesin yang sama (hari ini), Deviasi dari rencana: sanity-check tambahan, Diskusi, Hasil, Kesimpulan, Konteks, Metodologi (+4 more)

### Community 70 - "scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI)"
Cohesion: 0.14
Nodes (17): 7. Follow-up: cycle `winner_wired_v2` + addressable init — SELESAI (2026-08-31), Double-swap hypothesis (disproven), Follow-up: varian cycle `winner_wired_v2` + addressable init (2026-08-31), scc_test_wired.py (statistical & distinctness validation, wired variant), scrambler_addressable.c (CLI), ra_init_state_addressable(L, M, key), scrambler_wired_addressable.c (winner_wired_v2 cycle + addressable-init shuffle CLI), scrambler_wired_addressable.py (Python reference port, wired variant) (+9 more)

### Community 71 - "interleave_practrand.py"
Cohesion: 0.20
Nodes (10): 1TB Interleaved PractRand Anomaly (3/304 tests flagged, unconfirmed), common.py (shared harness config, TIERS_Q1B), cross_correlation.py, cross_correlation_ra_prng2.py, interleave_practrand.py, interleave_practrand_ra_prng2.py, Method A: Cross-correlation analysis (lag-0 Pearson, Bonferroni), Method B: Interleaved cross-stream PractRand test (+2 more)

### Community 72 - "RESULTS: production-candidate battery for `ra_core.c`"
Cohesion: 0.12
Nodes (17): clean, but a SEPARATE, more severe defect (K=1 pattern) now blocks, CRITICAL DEFECT: key=0 produces a permanent all-zero stream (both modes), further progress — gate STILL NOT PASSED, history, superseded by the rerun above for Steps 0 and 3 specifically, Original Step 0-3 run (2026-09-01, before the key=0 fix) — kept for, Provenance (Step 0), Recommendation (K=1 structural defect — NEW, open, needs a decision), Recommendation (key=0 — RESOLVED, kept for history) (+9 more)

### Community 73 - "combo_prng_orbit.py"
Cohesion: 0.21
Nodes (16): apply_cycle(), _cycle_variant_lookup(), fmix32(), _init_variant_lookup(), orbit_stream_full(), ra_hash(), ra_init_state_orbit_variant(), ra_permutation_cycle_orbit_variant() (+8 more)

### Community 74 - "combo_prng_d0raw.py"
Cohesion: 0.22
Nodes (15): fmix32(), orbit_round1(), orbit_stream_full(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_warmup(), ra_reseed(), Pure-Python reference oracle for orbit-d0-raw-cons-fix (d0_raw_cons candidate).… (+7 more)

### Community 75 - "RaPrng2"
Cohesion: 0.18
Nodes (11): _find_lib_path(), _lib_handle(), _load_library(), Path, RaPrng2, ctypes binding for the ra_prng2 C engine. This is glue only - it marshals calls…, Thin wrapper around one RA_PRNG instance., CDLL (+3 more)

### Community 76 - "Parallelization Research Handover"
Cohesion: 0.12
Nodes (16): Embarrassingly Parallel Architecture (per-instance state, no locks), Operand-Position Search Research (2026-8-27), Operation Pruning Research (2026-8-26), Periodicity Heuristic Validation 2 (2026-8-28), Periodicity Heuristic Validation Research (2026-8-25), Read-Only Source Constraint (never modify ra_prng2/ra_prng3/winner_wired_v2.c in place), Seed/Cycle Collision Probability (Q2), SIMD Vectorization Feasibility (Q4) (+8 more)

### Community 77 - "toy_prng.py"
Cohesion: 0.26
Nodes (13): brent(), Brent's cycle-detection algorithm applied to toy_prng.next_state. Measures the…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, enumerate_state_space(), main(), Full state-space enumeration for n=2,w=4 (|S|=131072, default rows=2). Directly…, init_state(), next_state() (+5 more)

### Community 78 - "next_state"
Cohesion: 0.20
Nodes (13): brent(), Brent's cycle-detection algorithm applied to pruned_wired_toy_prng.next_state.…, Standard Brent's cycle detection. Returns (lam, mu): lam = cycle length…, next_state(), State, ra_hash_gen_sequential(), Toy-scale (n, w, rows)-parameterized port of winner_wired_v2.c's permutation…, HASH_ACCESS=sequential port of winner_wired_v2.c's ra_hash: each output word is… (+5 more)

### Community 79 - "stream_driven_shuffle.c"
Cohesion: 0.23
Nodes (15): Speed Benchmark Output (stream_driven_shuffle), FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash() (+7 more)

### Community 80 - "benchmark_all_run.py"
Cohesion: 0.27
Nodes (15): build_bench_all(), build_crosscheck_originals(), main(), ols_fit(), paperorig/wiredv2 originals: `bin --stream key n`. addrcont original…, Like run(), but keeps stdout as raw bytes -- for --stream binary output., run(), run_crosscheck() (+7 more)

### Community 81 - "diag_keyzero_guard.c"
Cohesion: 0.33
Nodes (15): FILE, fmix32(), is_all_zero(), main(), multikey_chain(), ra_core_orbit_stream(), ra_core_singleblock_block(), ra_hash() (+7 more)

### Community 82 - "ADDENDUM_POST_FIX_STATUS.md — Step 4-8, post `w8_f10_i0` promotion"
Cohesion: 0.12
Nodes (14): ADDENDUM_POST_FIX_STATUS.md — Step 4-8, post `w8_f10_i0` promotion, Bug found and fixed: glibc `fmemopen` exact-buffer-size clobber, Overall gate verdict: PASS, Post-fix scoring, Step 0 (rebuild) — PASS, Step 4 — collision-scan K=1: PASS, Step 5 — dieharder, K=255 & K=1: PASS, Step 6 — PractRand, K=255 & K=1, 16GB (VPS): PASS (+6 more)

### Community 83 - "quality_gate.py (avalanche_stats)"
Cohesion: 0.11
Nodes (22): avalanche_gate_min_bit_binary(), capture_first_cycle(), main(), practrand_prefix_gate_binary(), Path, Avalanche + PractRand measurement for the phase-1 (L-only) state-update-…, `CYCLES` outer cycles' worth of `c` values -- see the CAPTURE_LEN comment above…, # NOTE: unlike pruned_prng.c's CLI (quality_gate.py's convention, where (+14 more)

### Community 84 - "combo_prng_admul.py"
Cohesion: 0.23
Nodes (14): fmix32(), orbit_round1(), orbit_stream_full(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_admul(), ra_reseed(), Pure-Python reference oracle for the orbit-ad-mul-combine-fix experiment… (+6 more)

### Community 85 - "candidate_e.c"
Cohesion: 0.26
Nodes (15): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+7 more)

### Community 86 - "run_verify_all"
Cohesion: 0.24
Nodes (10): chacha20_block(), kat_chacha20(), kat_pcg32(), kat_splitmix64(), kat_xoshiro256(), ra_core_chacha20(), ra_core_xoshiro256(), run_verify_all() (+2 more)

### Community 87 - "2026-9-4_orbit-kmin-battery/RESULTS.md"
Cohesion: 0.15
Nodes (13): dieharder battery (27 Good-reliability tests, run_dieharder_battery.py), BUILD.sh script, dieharder orbit K=1 raw output log, dieharder orbit K=255 raw output log, avalanche test (avalanche_orbit_kmin.py, per-bit Hamming-distance gate), collision-scan (collision_scan_orbit_k1.py, 2^32-space Poisson-z model, 50,000 keys), Methodology, PractRand (RNG_test stdin32 -tlmax 16GB -multithreaded) (+5 more)

### Community 88 - "recipes.py"
Cohesion: 0.15
Nodes (10): Cycle-operation combo DSL search space (756 combos), all_promotion_candidates(), _build_finalizer_recipes(), FinalizerStage, inject_describe(), DSL catalog for the cycle-operation combo search (HANDOVER.md). Search space…, 7 inject variants for a single (width, finalizer_idx) Tier-1 survivor., main() (+2 more)

### Community 89 - "Promosi Kandidat D ke `ra_core.c` kanonik — RESULTS"
Cohesion: 0.20
Nodes (9): 1. Diff formula yang diterapkan, 2. KAT checksum regenerasi, 3. Byte-identical merge-sanity-check, 4. Caller yang diupdate (13 file + `bench_release.c`), 5. Avalanche-effect curve, 3 skema — pasca-promosi, 6. Throughput 5-mode standar — pasca-promosi, delta jujur, 7. README, 8. Kesimpulan (+1 more)

### Community 90 - "Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`"
Cohesion: 0.25
Nodes (8): 1. Trigger dan motivasi, 2. Analisis (dikonfirmasi sebelum menulis kode apa pun), 3. File & perannya, 4. Batas scope keras (WAJIB dibaca sebelum memakai `ra_core_singleblock`), 5. Hasil kunci (angka lengkap di `RESULTS.md` "## Tahap 6"), 6. Status & langkah selanjutnya, 7. Setelah mengerjakan follow-up di atas, Handover: Tahap 6 -- fast path addressable tanpa `L[]` untuk `rng <= 255`

### Community 91 - "candidate_b.c"
Cohesion: 0.29
Nodes (14): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+6 more)

### Community 92 - "candidate_d.c"
Cohesion: 0.29
Nodes (14): core_entry_t, FILE, find_core(), fmix32(), main(), ra_core_orbit(), ra_core_singleblock(), ra_hash() (+6 more)

### Community 93 - "Singleblock cycle-op combo search experiment (2026-09-02)"
Cohesion: 0.16
Nodes (14): Avalanche effect diagnostic (seed-bit-flip vs output-bit-difference), Avalanche heatmap test method (seed-bit-flip vs output-word bit-diff), Orbit K-small multistream defect (2026-09-04 discovery), Singleblock K-small structural defect (fixed via cycle-op combo search), w8_f10_i0 cycle-op combo candidate (tap width=8, finalizer variant 10, i0), PRNG cycle-op candidate w8_f33_i0 (tap width=8, finalizer #33, i0), Singleblock cycle-op combo search experiment (2026-09-02), Avalanche Heatmap w8_f10_i0 (K=255) (+6 more)

### Community 94 - "RESULTS: Pareto selection over the 11 combo-search winners"
Cohesion: 0.17
Nodes (12): Axis A — Speed, Closing note, Deviations from HANDOVER.md (documented, both investigated before deviating), K=1 (frequent reinit) vs largest-K (steady state), Konteks, MB/s sanity check, Pareto frontier, Perbandingan vs keluarga PRNG lain (+4 more)

### Community 95 - "avalanche_heatmap_winner.py"
Cohesion: 0.25
Nodes (13): analyze_cycle(), compute_avalanche_matrix(), main(), plot_comparison(), plot_single(), Path, Avalanche-effect heatmap for the winning operand-position rewiring found by…, Mirrors winner_wired.c's ra_reseed()/ra_hash() exactly (identical to… (+5 more)

### Community 96 - "Handover: Tahap 5 — Optimasi Biaya Init Addressable"
Cohesion: 0.10
Nodes (19): 1. Status & tujuan, 2. Formula & fakta struktural (sudah final, tidak perlu digali ulang), 3. Temuan disassembly (sudah dijalankan read-only, sesi persiapan 2026-08-30), 4. Kandidat optimasi, berperingkat, 5. Rencana file & eksperimen, 6. Gerbang validasi statistik (aturan mekanis, bukan penilaian bebas), 7. Batasan read-only (berlaku juga untuk Tahap 5), 8. Kondisi berhenti / pelaporan eksplisit (+11 more)

### Community 97 - "io_only_isolation_test.c"
Cohesion: 0.26
Nodes (13): FILE, Options, load_tokens_from_file(), main(), parse_args(), ra_core(), ra_hash(), ra_init_state_addressable() (+5 more)

### Community 98 - "Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect"
Cohesion: 0.19
Nodes (17): BCFN structural defect (K small, ra_core_singleblock) — FAILs up to K≈72, clean from K≈96, avalanche_heatmap_winners_k255.png — heatmap visualization (32x255 cells per candidate), uniform noise pattern for all 11 winners, no dark horizontal lines (no dead bits), Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect, Dead-bit defect class — near-zero avalanche fraction for specific seed bits across the full output cycle, invisible to single-word (K=1) scalar avalanche gates, detectable only via full-cycle (K=255) heatmap, Promotion Tier — 29 candidates x 8 K-values x 16GB PractRand on VPS, 11/29 fully clean, ra_core_singleblock (algorithm), Tier 0 — avalanche gate (min-per-bit, K=1, 108/108 passed), Tier 1 — multikey PractRand staged (K=1, 256MB->2GB, 30/108 survivors) (+9 more)

### Community 99 - "RESULTS: Tahap 1 — Unifikasi `ra_core.c` (orbit + singleblock)"
Cohesion: 0.15
Nodes (12): ra_init_state_orbit, ra_init_state_singleblock, Split-loop fix for variable-length M[] init (avoids masked-wrap vectorization regression), Variable-length M[] init for ra_core_singleblock (2026-09-04), Catatan scope eksplisit, File yang dihasilkan sesi ini, Keputusan penamaan & scope (dikonfirmasi user, sesi ini), Non-goals (tidak dikerjakan sesi ini, sesuai HANDOVER §3) (+4 more)

### Community 100 - "2026-8-25_periodicity-heuristic-validation/enumerate_n4w4.py"
Cohesion: 0.23
Nodes (12): decode_batch(), main(), pack_key(), Full state-space enumeration for n=4,w=4 (default rows=4 -> G=1), |S| = 4! *…, Cross-check vec_next_state against the scalar reference on random states drawn…, Pack (L, M, cons, it) arrays into a single uint64 key array, w bits per field,…, Mixed-radix decompose a flat index array into (lperm, m_0..m_{n-1}, it)…, Vectorized rotw: rotate low w bits of x left by r bits (mod w). x, r are numpy… (+4 more)

### Community 101 - "Orbit K=1 PractRand Defect"
Cohesion: 0.36
Nodes (8): PractRand FPF-14+6/16 test, Hashed key (SplitMix32-chain), Orbit K=1 PractRand Defect, /dev/urandom key, Weyl-ordering key (key=base+idx*0x9E3779B9), Orbit o-Noshift Fix Results, baseline.c (shared dual-core baseline), o_noshift.c candidate (raw XOR, no per-tap shift)

### Community 102 - "random_seeds"
Cohesion: 0.27
Nodes (11): digest_collision_prob(), main(), main(), Tahap 2 (family-productionization) follow-up, Q2 re-pointed at…, scan_fullblock(), Q2: probability of cross-stream collision/overlap between different seeds'…, Birthday-bound estimate for pure hash-digest collision probability across…, scan_blocksweep() (+3 more)

### Community 103 - "avalanche_ra_core.py"
Cohesion: 0.16
Nodes (18): capture_cycle(), main(), Tahap 2 (family-productionization): avalanche gate for ra_core.c's two…, run_gate(), compute_curve(), main(), plot_core(), ndarray (+10 more)

### Community 104 - "diag_wideo_singleblock.c"
Cohesion: 0.32
Nodes (12): core_fn_t, FILE, fmix32(), main(), now_seconds(), pick_variant(), ra_core_singleblock_narrow(), ra_core_singleblock_wideo() (+4 more)

### Community 105 - "diag_prereseed.c"
Cohesion: 0.31
Nodes (12): core_fn_t, FILE, fmix32(), main(), pick_variant(), ra_core_singleblock_baseline(), ra_core_singleblock_prereseed(), ra_hash() (+4 more)

### Community 106 - "Orbit Fix and Wide-o Rolling Optimization Handover"
Cohesion: 0.08
Nodes (36): Orbit Fix and Wide-o Rolling Optimization Handover, 1. Temuan awal: `ra_core_orbit` rentan defek K-kecil, 2. Keputusan user (AskUserQuestion + pesan lanjutan), 3. Insight user: rolling-register untuk wide-`o`, 4. Perubahan di `ra_core_v2.c`, 5. Verifikasi yang sudah dijalankan, 6. Yang BELUM dikerjakan / sengaja di-skip, BCFN defect (below K=96, narrow 2-tap o, no finalizer) (+28 more)

### Community 107 - "Orbit Continuous-Stream Measurement (376.2 MB/s)"
Cohesion: 0.33
Nodes (6): Axis A - Speed Benchmark Methodology, Deviation: K=1 noise fix via 7x min-of-repeats reinit-sweep, Orbit Continuous-Stream Measurement (376.2 MB/s), main(), Continuous-stream (real, syscall-inclusive) MB/s for ra_core_orbit via…, run_continuous_stream()

### Community 108 - "orbit-d0-raw-cons-fix -- hasil"
Cohesion: 0.29
Nodes (7): 1. Byte-exact vs oracle Python (`combo_prng_d0raw.py`), 2. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2), 3. Tier 1: PractRand dual-ordering, K=1 dan K=255, staged 64MB->2GB, 4. Verdict CLEAN-DUAL, 5. Kesimpulan, orbit-d0-raw-cons-fix -- hasil, Ringkasan axis yang sudah dicoba untuk defect K=1+hashed (kumulatif, per sesi ini)

### Community 109 - "position_split_orbit.c"
Cohesion: 0.37
Nodes (12): FILE, fmix32(), main(), ra_core_orbit(), ra_core_orbit_split(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit() (+4 more)

### Community 110 - "Avalanche Effect (Bit-Flip Sensitivity)"
Cohesion: 0.17
Nodes (12): pruned_winner Avalanche Fraction Metric (Cycle 1 vs Cycle 90), Avalanche Effect Heatmap: pruned_winner Cycle 1 vs Cycle 90, Multi-Cycle Reseed Avalanche Diffusion Improvement (0.418 to 0.468), winner_wired_v2 Avalanche Metrics (overall=0.487385, min_bit=0.473039), Avalanche Effect Heatmap (winner_wired_v2), winner_wired_v2 Wiring Parameters (a_xor=d, c_shift=a, rotc_amount=b, rotc_xor=a), Avalanche Effect (Bit-Flip Sensitivity), Joan Daemen and Vincent Rijmen (+4 more)

### Community 111 - "Design decisions (read before touching `pruned_wired_toy_prng.py`)"
Cohesion: 0.17
Nodes (11): 1. Op/wiring mapping ported to toy scale, 2. Tap-survivor generalization: top-2-by-count, and why, 3. Tap shift-width collapse (caught during design, not present in either parent), 4. `ra_hash_gen` divergence from ground truth (found while reading source, not assumed), 5. Self-check (passed) -- MUST stay passing, CRITICAL BUG found and fixed this session -- read before writing more scripts, Design decisions (read before touching `pruned_wired_toy_prng.py`), HANDOVER.md (periodicity-heuristic-validation2) (+3 more)

### Community 112 - "pareto_select.py"
Cohesion: 0.23
Nodes (10): dominates(), load_jsonl(), main(), Path, Pareto frontier selection (HANDOVER.md sect 4) over the 11 combo-search…, True if candidate a dominates candidate b: a is >= b on all 3 objectives (in…, main(), Axis A orchestrator (HANDOVER.md sect 2): K=1 / K=255 reinit-sweep and… (+2 more)

### Community 113 - "control.c"
Cohesion: 0.33
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_init_warmup_orbit(), ra_permutation_cycle_orbit_combo() (+3 more)

### Community 114 - "d0_hash.c"
Cohesion: 0.35
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_init_warmup_orbit(), ra_permutation_cycle_orbit_combo() (+3 more)

### Community 115 - "d0_realround.c"
Cohesion: 0.35
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_init_warmup_orbit(), ra_permutation_cycle_orbit_combo() (+3 more)

### Community 116 - "o_fmix_every_round.c"
Cohesion: 0.35
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_ofmix(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_ofmix(), ra_reseed() (+3 more)

### Community 117 - "o_fmix_init_only.c"
Cohesion: 0.35
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_ofmix(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_ofmix(), ra_reseed() (+3 more)

### Community 118 - "d0_raw_cons.c"
Cohesion: 0.33
Nodes (11): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit(), ra_init_warmup_orbit(), ra_permutation_cycle_orbit_combo() (+3 more)

### Community 119 - "Hamas A. Rahman (author)"
Cohesion: 0.25
Nodes (8): Hamas A. Rahman (author), ICCS2026 paper: Array-Native Randomness, research/ (papers), ACM (Association for Computing Machinery), CSAI 2025 (9th Int'l Conf. on Computer Science and AI), EI Compendex, ra_prng2 and Beyond (Paper CA-162), Scopus

### Community 120 - "BuildPyWithCEngine"
Cohesion: 0.20
Nodes (7): _build_py, Distribution, BinaryDistribution, BuildPyWithCEngine, Imperative build customization pyproject.toml's static [tool.setuptools] table…, After the normal .py copy step, compiles the ra_prng2 C engine and drops…, Declares this distribution as containing compiled, platform-specific code…

### Community 121 - "2025-10-5_scramble-design/scc_test.py"
Cohesion: 0.18
Nodes (10): chi_square_uniform(), entropy(), Calculate the Serial Correlation Coefficient (linear correlation) between…, Reads a text file containing tokens (integers) separated by spaces/newlines,…, Calculates the Shannon Entropy (bit) of the distribution of values in the…, Calculate the Chi-Square Statistic to test whether the frequency of each value…, Calculates Runs Test (Wald-Wolfowitz) for consecutive ascending/descending…, read_tokens() (+2 more)

### Community 122 - "pruned_prng.c"
Cohesion: 0.38
Nodes (10): build_o(), init_state(), permutation_cycle(), ra_hash(), reseed(), rot32(), FILE, has() (+2 more)

### Community 123 - "Cycle 1 Initial State Avalanche Fraction (0.487783)"
Cohesion: 0.29
Nodes (11): Avalanche Metrics (overall=0.487783, min_bit=0.472304), Warmup Cycle Avalanche Effect and Bit Diffusion, Cycle 1 Initial State Avalanche Fraction (0.487783), Cycle 3 Converged Avalanche Fraction (0.500751), Winner Wired Avalanche Heatmap Cycle 1 vs Cycle 3, Winner Wired Wiring Configuration (a_xor=d, c_shift=b, rotc_amount=b, rotc_xor=a), Cycle 4 Steady State Avalanche (fraction 0.499713), Avalanche Effect Heatmap Comparison: Cycle 1 vs Cycle 4 (+3 more)

### Community 124 - "Results: Periodicity Heuristic Validation 2 (Pruned & Rewired `winner_wired_v2`)"
Cohesion: 0.18
Nodes (10): 1. Summary Comparison: Unpruned Original vs `winner_wired_v2`, 2. Sebaran Siklus Utama (*Distinct $\lambda$ Distribution*), 3. Enumerasi Penuh & Uji Chi-Square vs Poisson(1), 4. Analisis Temuan Kunci (*Key Findings*), 5. Catatan Metodologi & Caveats (Sesuai `HANDOVER.md`), 6. Kesimpulan (*Verdict*), A. Skala $n=2, w=4$ ($|S| = 131.072$) — [`enumerate_n2w4_result.json`](./enumerate_n2w4_result.json), B. Skala $n=4, w=4$ ($|S| = 402.653.184$) — [`enumerate_n4w4_result.json`](./enumerate_n4w4_result.json) (+2 more)

### Community 125 - "tahap5_benchmark.py"
Cohesion: 0.36
Nodes (10): find_crossover(), main(), _parse_rows(), Path, Tahap 5: orchestrator for tahap5_bench / tahap5_bench_zmm512 /…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep() (+2 more)

### Community 126 - "tahap6_benchmark.py"
Cohesion: 0.33
Nodes (10): find_crossover(), main(), _parse_rows(), Tahap 6: orchestrator for tahap6_bench (C harness does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word.…, run_init_cost(), run_reinit_sweep(), run_throughput() (+2 more)

### Community 127 - "RESULTS: key=0 degenerate-state guard-XOR fix"
Cohesion: 0.18
Nodes (11): 1. Diagnostic candidate — `diag_keyzero_guard.c`, 2. Edge-case check — PASS, 3. BCFN non-regression — K=255 multikey pattern: PASS, 32GB clean, 4. Non-regression — `ra_core validate` after applying to `ra_core.c`, Applied to `ra_core.c`, CRITICAL — separate, pre-existing defect found: K=1 singleblock chaining catastrophically FAILs PractRand, Guard constants, RESULTS: key=0 degenerate-state guard-XOR fix (+3 more)

### Community 128 - "HANDOVER: Init-loop speed optimization untuk `ra_core_orbit` / `ra_core_singleblock`"
Cohesion: 0.18
Nodes (11): Critical files to reference, Deliverables, Goal -- three-tier strategy, in priority order, HANDOVER: Init-loop speed optimization untuk `ra_core_orbit` / `ra_core_singleblock`, Non-goals / hard constraints, Priority 1 (try first): find redundant/reducible operations with ZERO output change, Priority 2 (if P1 fails to help significantly): search for a new init formula (output allowed to change), Priority 3 (if both P1 and P2 fail): re-evaluate the tradeoff (+3 more)

### Community 129 - "ad_mul_combine.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_admul(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit_admul(), ra_reseed() (+2 more)

### Community 130 - "init_hashed_per_index__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 131 - "init_hashed_per_index__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 132 - "init_hashed_per_index__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 133 - "init_hashed_per_index__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 134 - "init_hashed_shared__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 135 - "init_hashed_shared__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 136 - "init_hashed_shared__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 137 - "init_hashed_shared__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 138 - "init_linear_p1_per_index__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 139 - "init_linear_p1_per_index__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 140 - "init_linear_p1_per_index__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 141 - "init_linear_p1_per_index__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 142 - "init_linear_p1_shared__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 143 - "init_linear_p1_shared__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 144 - "init_linear_p1_shared__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 145 - "init_linear_p1_shared__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 146 - "init_linear_per_index__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 147 - "init_linear_per_index__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 148 - "init_linear_per_index__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 149 - "init_linear_per_index__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 150 - "init_linear_shared__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 151 - "init_linear_shared__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 152 - "init_linear_shared__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 153 - "init_linear_shared__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 154 - "init_odd_per_index__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 155 - "init_odd_per_index__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 156 - "init_odd_per_index__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 157 - "init_odd_per_index__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 158 - "init_odd_shared__w8_f10_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 159 - "init_odd_shared__w8_f28_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 160 - "init_odd_shared__w8_f33_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 161 - "init_odd_shared__w8_f9_i0.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit_combo(), ra_hash(), ra_init_state_orbit_combo(), ra_permutation_cycle_orbit_combo(), ra_reseed() (+2 more)

### Community 162 - "Hasil: search init-formula × cycle-transform untuk `ra_core_orbit` (dual key-ordering)"
Cohesion: 0.18
Nodes (11): 1. Konteks, 2. Validasi infra (langkah 1-2 pipeline), 3. Tier 0 — avalanche gate (K=1), 4. Tier 1 — PractRand dual-ordering lokal, 5. Analisis pola kegagalan, 6. Kesimpulan dan langkah berikut, 7. File terkait, Hasil: search init-formula × cycle-transform untuk `ra_core_orbit` (dual key-ordering) (+3 more)

### Community 163 - "tier1_gate_orbit.py"
Cohesion: 0.31
Nodes (10): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the orbit init x cycle combo search,…, Run both orderings for each combo; a combo survives only if BOTH are…, Yield one full checkpoint block (str) at a time from RNG_test's stdout, as… (+2 more)

### Community 164 - "tier1_gate_warmup.py"
Cohesion: 0.31
Nodes (10): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the orbit init-warmup fix experiment,…, Run both orderings for each (variant, K) pair; a pair survives only if BOTH are…, Yield one full checkpoint block (str) at a time from RNG_test's stdout, as… (+2 more)

### Community 165 - "multikey_stream_orbit_altconst.c"
Cohesion: 0.36
Nodes (10): FILE, fmix32(), main(), ra_core_orbit(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed() (+2 more)

### Community 166 - "bench_addr_hi.c"
Cohesion: 0.42
Nodes (10): core_entry_t, find_core(), main(), mode_reinit_sweep(), mode_throughput(), now_seconds(), philox4x32_10(), philox4x32_round() (+2 more)

### Community 167 - "tier1_gate_d0raw.py"
Cohesion: 0.31
Nodes (10): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the orbit init-warmup fix experiment,…, Run both orderings for each (variant, K) pair; a pair survives only if BOTH are…, Yield one full checkpoint block (str) at a time from RNG_test's stdout, as… (+2 more)

### Community 168 - "Handover: redesign arsitektur "Weyl-add + finalizer 32-bit murni" untuk defect K-small `ra_core_orbit`"
Cohesion: 0.18
Nodes (11): 0. WAJIB: checkpoint ke user sebelum implementasi, 1. Kenapa handover ini ditulis (ringkasan, baca sumber untuk detail lengkap — JANGAN re-derive dari nol), 2. Insight arsitektural (dari diskusi sesi ini) dan scope, 3. Kandidat A — `weyl_fmix32` (finalizer termurah, precedent sudah ada di repo), 4. Kandidat B — `weyl_triple32` (finalizer lebih kuat, fallback kalau A gagal Tier 1), 5. Struktur kode yang harus dibuat, 6. Pipeline validasi (urutan wajib, disiplin sama seperti semua sesi K-small sebelumnya), 7. Kriteria hasil (+3 more)

### Community 169 - "w8_f25_i0.c"
Cohesion: 0.38
Nodes (9): Pareto Frontier Selection (3 objectives), FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo() (+1 more)

### Community 170 - "2026-8-28_periodicity-heuristic-validation2/run_sweep.py"
Cohesion: 0.33
Nodes (9): cycle_measure Binary Exit-42 Crash at n=8,w=4,rows=4, log2_fact(), main(), predicted(), Drives the pruned/wired periodicity-heuristic sweep. Same lambda_pred formula…, result_csv_name(), run_c(), run_python() (+1 more)

### Community 171 - "tahap4_benchmark.py"
Cohesion: 0.36
Nodes (9): find_crossover(), main(), _parse_rows(), Tahap 4: orchestrator for tahap4_bench (C harness, does the actual timed work…, Linear-interpolate the N/K where addr_ns_per_word == philox_ns_per_word., run_init_cost(), run_reinit_sweep(), run_throughput() (+1 more)

### Community 172 - "scrambler_addressable.c"
Cohesion: 0.38
Nodes (9): main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 173 - "scrambler_wired_addressable.c"
Cohesion: 0.38
Nodes (9): main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Options, load_tokens_from_file(), parse_args() (+1 more)

### Community 174 - "single_mread_isolation_test.c"
Cohesion: 0.38
Nodes (9): Options, load_tokens_from_file(), main(), parse_args(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32() (+1 more)

### Community 175 - "diag_hashed_init.c"
Cohesion: 0.40
Nodes (9): FILE, main(), ra_core_diag(), ra_hash(), ra_init_state_hashed_key(), ra_permutation_cycle(), ra_reseed(), rot32() (+1 more)

### Community 176 - "diag_orbit_regression.c"
Cohesion: 0.40
Nodes (9): FILE, fmix32(), main(), ra_core_orbit_stream(), ra_hash(), ra_init_orbit(), ra_permutation_cycle_orbit(), ra_reseed() (+1 more)

### Community 177 - "promotion_search.py"
Cohesion: 0.18
Nodes (17): BCFN structural defect (K-small), compile_candidate(), iter_checkpoints(), _log(), main(), Path, Promotion tier: runs ON THE VPS (not locally -- 8 K-values x up to 29…, Yield one full checkpoint block (str) at a time from RNG_test's stdout, as… (+9 more)

### Community 178 - "Handover: eliminasi 11 pemenang combo-search via Pareto (speed x avalanche), lalu posisikan vs keluarga PRNG lain"
Cohesion: 0.20
Nodes (10): 1. Konteks & tujuan, 2. Axis A — Speed: K=1, K=255, continuous-stream, 3. Axis B — Avalanche multi-seed + uji normalitas, 4. Pareto frontier selection, 5. Perbandingan vs keluarga PRNG lain — REUSE tabel lama, JANGAN tes ulang, 6. File yang TIDAK BOLEH diubah (read-only precedent), 7. Struktur file baru (folder ini), 8. Non-goals (+2 more)

### Community 179 - "Hasil: kandidat fix `fmix32(o)` untuk defect `ra_core_orbit` K=1+hashed"
Cohesion: 0.20
Nodes (10): 1. Konteks dan motivasi, 2. Kandidat yang diuji, 3. Byte-exact C vs oracle (WAJIB lolos sebelum PractRand), 4. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2), 5. Tier 1: PractRand dual-ordering staged, K=1 dan K=255, skala 2GB, 6. Overhead (ringan, timing tunggal K=255 stream 20M word), 7. Verdict dan insight penting, 8. Yang TIDAK dilakukan sesi ini (di luar scope, per persetujuan awal) (+2 more)

### Community 180 - "fpf_scan_addr_hi.c"
Cohesion: 0.38
Nodes (9): count_low_zeroes32(), fpf_bump(), fpf_process_word(), fpf_report(), gen_urandom_word(), main(), now_sec(), real_orbit_word() (+1 more)

### Community 181 - "RESULTS: Weyl-add + fmix32/triple32 finalizer redesign — both candidates FAIL"
Cohesion: 0.20
Nodes (10): 0. Scope executed vs handover, 1. Core code (both candidates, full generator — no L[]/M[]/255-round loop), 2.1 Byte-exact oracle vs C (`combo_prng_weylfinal.py`), 2.2 Tier 0 avalanche (K=1, band 0.3-0.7, min-bit floor 0.2), 2.3 Tier 1 PractRand, 8GB, dual-ordering, staged (A/B/C), 2.4 Benchmark, 2. Pipeline results, 3. Analysis (+2 more)

### Community 182 - "pcg_amortized.c"
Cohesion: 0.44
Nodes (8): pcg32_random_t, main(), pcg32_buffered_init(), pcg32_buffered_next(), pcg32_random_r(), pcg32_srandom(), rand_bounded(), pcg32_buffered_t

### Community 183 - "source/ra_prng2.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot32(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 184 - "source/ra_prng3.c"
Cohesion: 0.39
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), rot64(), save_tokens_to_file(), ZepFold(), ZepXORhash()

### Community 185 - "scrambler.c"
Cohesion: 0.39
Nodes (8): main(), ra_core(), ra_hash(), rot32(), Options, load_tokens_from_file(), parse_args(), save_tokens_to_file()

### Community 186 - "2026-8-25_periodicity-heuristic-validation/avalanche_check.py"
Cohesion: 0.31
Nodes (8): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for toy_prng.py. Port of…, One outer iteration, no rehash - mirrors zepfold_capture() in…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check(), Rotate the low w bits of x left by r bits (mod w)., rotw()

### Community 187 - "Tahap 2: Folder/Battery Validasi Produksi"
Cohesion: 0.22
Nodes (9): dieharder_pruned_winner_piped.txt, parallelization-research experiment (2026-8-29), scc_test.py, verify_parity.py, Min-of-Trials Benchmarking Pattern, Tahap 2: Folder/Battery Validasi Produksi, avalanche_gate_min_bit(), quality_gate.py (+1 more)

### Community 188 - "pruned_winner.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 189 - "pruned_winner_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 190 - "2026-8-27_operand-position-search/baseline.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 191 - "baseline_refactored.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 192 - "v01.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 193 - "v02.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 194 - "v03.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 195 - "v04.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 196 - "v06.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 197 - "v07.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 198 - "v08.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 199 - "v09.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 200 - "v10.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 201 - "v11.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 202 - "v12.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 203 - "v13.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 204 - "winner_wired.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 205 - "2026-8-28_periodicity-heuristic-validation2/enumerate_n4w4.py"
Cohesion: 0.39
Nodes (8): decode_batch(), main(), pack_key(), Full state-space enumeration for pruned/wired PRNG at n=4,w=4 (default rows=4…, Cross-check vec_next_state against the scalar reference on random states drawn…, rotw_vec(), validate(), vec_next_state()

### Community 206 - "Promosi Kandidat D (64-bit address widening) ke `ra_core.c` kanonik — HANDOVER"
Cohesion: 0.29
Nodes (6): Keputusan desain kunci, Promosi Kandidat D (64-bit address widening) ke `ra_core.c` kanonik — HANDOVER, Scope IN, Scope OUT (dicatat, tidak dikerjakan sesi ini), Status akhir sesi ini, Tujuan

### Community 207 - "original_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 208 - "original_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 209 - "original_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 210 - "original_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 211 - "original_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 212 - "original_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 213 - "winner_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 214 - "winner_m_inject.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 215 - "winner_m_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 216 - "winner_m_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 217 - "winner_overwrite.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 218 - "winner_permute.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 219 - "Handover: 1TB interleaved-PractRand anomaly follow-up"
Cohesion: 0.22
Nodes (8): 1. Status & apa yang sudah terjadi (2026-08-31), 2. Pertanyaan yang belum terjawab, 3. Rencana diagnostik (belum dikerjakan, urutan disarankan), 4. Setelah selesai, Handover: 1TB interleaved-PractRand anomaly follow-up, Langkah 1 — Single-stream `winner_wired_v2` ke 1TB (tanpa interleave), Langkah 2 — Re-run interleaved 1TB dengan seed set berbeda, Langkah 3 — Sintesis & keputusan

### Community 220 - "winner_wired_addressable_v2.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state_addressable(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 221 - "Handover: Addressable-Init Data Shuffling & Speed Investigation"
Cohesion: 0.22
Nodes (8): 1. Ringkasan alur & tujuan, 2. File & perannya (peta folder `experiments/2026-8-30_addressable-shuffle/`), 3. Hasil kunci (sudah final, jangan diulang), 4. Reproduksi cepat, 5. Constraint yang diwarisi, 6. Follow-up: SELESAI (2026-08-30, sesi lanjutan), 7. Setelah mengerjakan follow-up di atas, Handover: Addressable-Init Data Shuffling & Speed Investigation

### Community 222 - "Hasil validasi"
Cohesion: 0.22
Nodes (9): Addressable-Init Data Shuffling Results, 1. Smoke test, 2. Parity check (`verify_parity.py`), 3. Statistik kualitas shuffle (`scc_test.py`, key=1, n=100.000), 4. Multi-key distinctness spot-check (`scc_test.py`), Apa ini, Build & run, Follow-up (belum dilakukan sekarang) (+1 more)

### Community 223 - "scrambler_addressable.py"
Cohesion: 0.42
Nodes (8): load_token_ids_from_file(), main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Byte-for-byte port of winner_wired_addressable.c's function of the same name…, save_token_ids_to_file()

### Community 224 - "scrambler_wired_addressable.py"
Cohesion: 0.42
Nodes (8): load_token_ids_from_file(), main(), ra_hash(), ra_init_state_addressable(), ra_shuffle(), rot32(), Byte-for-byte port of winner_wired_addressable.c's function of the same name…, save_token_ids_to_file()

### Community 225 - "no_reseed_isolation_test.c"
Cohesion: 0.42
Nodes (8): Options, load_tokens_from_file(), main(), parse_args(), ra_init_state_addressable(), ra_shuffle(), rot32(), save_tokens_to_file()

### Community 226 - "main"
Cohesion: 0.47
Nodes (6): core_entry_t, find_core(), main(), mode_reinit_sweep(), mode_urandom_flat(), now_seconds()

### Community 227 - "PRNG Family Benchmark Results"
Cohesion: 0.14
Nodes (14): chacha20_block(), main(), Candidate: chacha20, PRNG Family Benchmark Results, chacha20.c RFC 8439 nonce byte-order bug, Closing note, K=1 (frequent reinit) vs largest-K (steady state), sorted by K=1 cost ascending, MB/s sanity check, sorted descending (+6 more)

### Community 228 - "bench_init_avalanche.c"
Cohesion: 0.50
Nodes (8): core_avalanche(), core_baseline(), fmix32(), main(), ra_cycle(), ra_init_state_avalanche(), ra_init_state_orbit(), rot32()

### Community 229 - "bench_init_keyterm_mul.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_keyterm_mul(), fmix32(), main(), ra_cycle(), ra_init_state_keyterm_mul(), ra_init_state_orbit(), rot32()

### Community 230 - "bench_warmup_cost.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_warmup(), main(), ra_cycle(), ra_hash(), ra_init_state_orbit(), ra_reseed(), rot32()

### Community 231 - "bench_warmup_cost2.c"
Cohesion: 0.50
Nodes (8): core_baseline(), core_warmup(), main(), ra_cycle(), ra_hash(), ra_init_state_orbit(), ra_reseed(), rot32()

### Community 232 - "diag_init_keyterm_mul.c"
Cohesion: 0.47
Nodes (8): FILE, fmix32(), main(), ra_core_orbit(), ra_init_orbit(), ra_init_singleblock(), ra_permutation_cycle_orbit(), rot32()

### Community 233 - "diag_paperlike_cycle.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_diag(), ra_hash(), ra_init_state_addressable(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 234 - "diag_warmup_depth.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_warmup(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed(), rot32()

### Community 235 - "diag_warmup_discard.c"
Cohesion: 0.44
Nodes (8): FILE, main(), ra_core_warmup(), ra_hash(), ra_init_state_orbit(), ra_permutation_cycle_orbit(), ra_reseed(), rot32()

### Community 236 - "2026-9-2_singleblock-cycle-combo-search/candidates/w2_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 237 - "w2_f10_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 238 - "w2_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 239 - "w2_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 240 - "w2_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 241 - "w2_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 242 - "w2_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 243 - "w2_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 244 - "w2_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 245 - "w2_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 246 - "w2_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 247 - "w2_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 248 - "w2_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 249 - "w2_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 250 - "w2_f22_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 251 - "w2_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 252 - "w2_f24_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 253 - "w2_f25_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 254 - "w2_f26_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 255 - "w2_f27_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 256 - "w2_f28_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 257 - "w2_f29_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 258 - "w2_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 259 - "w2_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 260 - "w2_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 261 - "w2_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 262 - "w2_f33_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 263 - "w2_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 264 - "w2_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 265 - "w2_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 266 - "w2_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 267 - "w2_f5_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 268 - "w2_f6_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 269 - "w2_f7_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 270 - "w2_f8_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 271 - "w2_f9_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 272 - "w4_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 273 - "w4_f10_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 274 - "w4_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 275 - "w4_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 276 - "w4_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 277 - "w4_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 278 - "w4_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 279 - "w4_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 280 - "w4_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 281 - "w4_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 282 - "w4_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 283 - "w4_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 284 - "w4_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 285 - "w4_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 286 - "w4_f22_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 287 - "w4_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 288 - "w4_f24_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 289 - "w4_f25_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 290 - "w4_f26_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 291 - "w4_f27_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 292 - "w4_f28_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 293 - "w4_f29_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 294 - "w4_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 295 - "w4_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 296 - "w4_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 297 - "w4_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 298 - "w4_f33_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 299 - "w4_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 300 - "w4_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 301 - "w4_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 302 - "w4_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 303 - "w4_f5_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 304 - "w4_f6_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 305 - "w4_f7_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 306 - "w4_f8_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 307 - "w4_f9_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 308 - "w8_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 309 - "w8_f10_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 310 - "w8_f11_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 311 - "w8_f12_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 312 - "w8_f13_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 313 - "w8_f14_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 314 - "w8_f15_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 315 - "w8_f16_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 316 - "w8_f17_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 317 - "w8_f18_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 318 - "w8_f19_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 319 - "w8_f1_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 320 - "w8_f20_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 321 - "w8_f21_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 322 - "Axis B - Avalanche Multi-seed + Normality"
Cohesion: 0.33
Nodes (10): Axis B - Avalanche Multi-seed + Normality, Methodology caveat: Shapiro-Wilk on quantized per_bit_fraction may be a discreteness artifact, FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock() (+2 more)

### Community 323 - "w8_f23_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 324 - "w8_f24_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 325 - "w8_f26_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 326 - "w8_f27_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 327 - "w8_f28_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 328 - "w8_f29_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 329 - "w8_f2_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 330 - "w8_f30_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 331 - "w8_f31_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 332 - "w8_f32_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 333 - "w8_f33_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 334 - "w8_f34_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 335 - "w8_f35_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 336 - "w8_f3_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 337 - "w8_f4_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 338 - "w8_f5_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 339 - "w8_f6_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 340 - "w8_f7_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 341 - "Hasil: K=1 orbit dengan key dari `/dev/urandom` (proxy TRNG)"
Cohesion: 0.33
Nodes (6): File, Hasil: K=1 orbit dengan key dari `/dev/urandom` (proxy TRNG), Hasil PractRand (`run_urandom_key_A.log`, 1 run sampai 2GB), Kesimpulan, Pertanyaan, Sanity check (sebelum PractRand)

### Community 342 - "w8_f9_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 343 - "HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup"
Cohesion: 0.22
Nodes (9): Cara lanjut di sesi berikutnya, Context, HANDOVER: fix defect K-kecil `ra_core_singleblock` — perlebar `o` ke 8-tap, fallback internal-warmup, Ide fix dari user (2 tahap, verbatim diparafrase), Kandidat A — perlebar `o` ke 8-tap (isolasi 1 variabel), Kandidat B (fallback, hanya jalan jika Kandidat A gagal/belum cukup), Non-goals (eksplisit), Rencana eksekusi (belum dijalankan) (+1 more)

### Community 344 - "2026-9-3_dieharder-inject-crossing/candidates/w2_f0_i0.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 345 - "w8_f10_i1.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 346 - "w8_f10_i2.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 347 - "w8_f10_i3.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 348 - "w8_f10_i4.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 349 - "w8_f10_i5.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 350 - "w8_f10_i6.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 351 - "w8_f28_i1.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 352 - "w8_f28_i2.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 353 - "w8_f28_i3.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 354 - "w8_f28_i4.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 355 - "w8_f28_i5.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 356 - "w8_f28_i6.c"
Cohesion: 0.44
Nodes (8): FILE, fmix32(), main(), now_seconds(), ra_core_singleblock_combo(), ra_init_state_singleblock(), ra_permutation_cycle_singleblock_combo(), rot32()

### Community 357 - "promotion_search_inject.py"
Cohesion: 0.44
Nodes (8): compile_candidate(), iter_checkpoints(), _log(), main(), Path, Promotion tier for the 12 inject-crossing candidates (w8_f10_i1..i6,…, run_candidate_all_k(), run_one()

### Community 358 - "tier1_gate_ofmix.py"
Cohesion: 0.42
Nodes (8): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the orbit-o-fmix-fix experiment, port…, run_dual_ordering_stage(), run_stage()

### Community 359 - "Hasil: diagnosis resonansi-konstanta Weyl-stepping vs `ra_init_state_orbit`"
Cohesion: 0.22
Nodes (9): 1. Konteks, 2. File yang dibuat, 3. Konstanta yang diuji, 4. Validasi §5 langkah 1: byte-exact (WAJIB lolos sebelum PractRand), 5. Tier 1: PractRand staged, K=1, base_key=`0x12345`, `C_alt1=0x2545F491`, 6. Verdict, 7. Yang TIDAK dilakukan sesi ini (di luar scope, per persetujuan awal), 8. Referensi (+1 more)

### Community 360 - "HANDOVER: karakterisasi ekshaustif fungsi round-1 K=1 (`ra_core_orbit`)"
Cohesion: 0.22
Nodes (9): 1. Derivasi persis `f(key)` (WAJIB diverifikasi byte-exact dulu), 2. Tiga analisis, urutan prioritas ini, 3. Larangan eksplisit / rambu untuk sesi baru, 4. Referensi, HANDOVER: karakterisasi ekshaustif fungsi round-1 K=1 (`ra_core_orbit`), Ide baru: `f(key)` bisa dikarakterisasi ekshaustif, bukan ditebak, Kenapa ini ada (jangan ulangi metode lama), Status (+1 more)

### Community 361 - "tier1_gate_weylfinal.py"
Cohesion: 0.42
Nodes (8): iter_checkpoints(), _log(), main(), parse_size(), Path, Tier 1: PractRand dual-ordering gate for the weyl_fmix32 / weyl_triple32…, run_dual_ordering_stage(), run_stage()

### Community 362 - "ra_prng2_struct.c"
Cohesion: 0.44
Nodes (8): RA_PRNG, main(), ra_hash(), ra_prng_advance(), ra_prng_init(), ra_prng_next(), ra_step(), rot32()

### Community 363 - "Fisher–Yates Shuffle"
Cohesion: 0.15
Nodes (13): Array Index Shuffling, Array L (256x32-bit permutation state), Array M (256x32-bit entropy reservoir), Bitwise Rotation (rotl32), R. Durstenfeld, Fisher–Yates Shuffle, Mutate Operator, Rationale: Non-Invertible Hash Reseed Trade-off (Reduces Period, Aids Diffusion) (+5 more)

### Community 364 - "ra_prng2_thread.c"
Cohesion: 0.42
Nodes (8): FILE, main(), ra_core(), ra_hash(), ra_init_state(), ra_permutation_cycle(), ra_reseed(), rot32()

### Community 365 - "ra_prng2 API Reference"
Cohesion: 0.36
Nodes (8): CPython Binding Documentation, Orphaned CPython Shared Object Rationale, RaPrng2 ctypes Wrapper, ra_prng2 API Reference, ra_core PRNG Core Loop, ra_hash State Reduction Function, RA_PRNG C Struct API, rot32 Left Bitwise Rotation

### Community 366 - "gen_variants.py"
Cohesion: 0.43
Nodes (7): build(), gen_c(), main(), Path, Generate + compile the phase-1 (L-only) state-update-mechanism spectrum…, The "permute" mechanism must reproduce its target's real source exactly (it's…, verify_control_bit_identical()

### Community 367 - "gen_variants_m.py"
Cohesion: 0.39
Nodes (7): build(), gen_c(), main(), Path, Generate + compile the phase-2 (M-only) state-update-mechanism spectrum, per…, The M mechanism only affects the reseed fold (once per 255 steps) -- the first…, verify_first_cycle_matches_control()

### Community 368 - "tahap3_cross_correlation.py"
Cohesion: 0.32
Nodes (7): parallelization-research infra (referenced), Devil's-Advocate Session (challenged parallelization research value), Tahap 3: Statistical Validation, build_group_addr(), main(), Tahap 3, Q1 Method A (re-pointed): Pearson cross-correlation between pairs of…, run()

### Community 369 - "diag_init_avalanche.c"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_avalanche_init(), ra_init_state_avalanche(), ra_permutation_cycle_orbit(), rot32()

### Community 370 - "diag_init_keyterm_avalanche.c"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_keyterm_add(), ra_init_state_keyterm_add(), ra_permutation_cycle_orbit(), rot32()

### Community 371 - "diag_init_keyterm_xor.c"
Cohesion: 0.50
Nodes (7): FILE, fmix32(), main(), ra_core_keyterm_xor(), ra_init_state_keyterm_xor(), ra_permutation_cycle_orbit(), rot32()

### Community 372 - "collision_scan_singleblock_k1.py"
Cohesion: 0.36
Nodes (7): main(), poisson_z(), Step 4 (PRODUCTION_READINESS_HANDOVER.md) -- collision-scan for…, Birthday-bound estimate for raw 32-bit-word collision across n_items…, Normal-approximation z-score for a Poisson(expected) count -- same spirit as…, raw_collision_prob(), scan_k1()

### Community 373 - "Hasil: `o` sebagai XOR 8 word mentah (tanpa bit-shift per-tap)"
Cohesion: 0.33
Nodes (6): File, Hasil, Hasil: `o` sebagai XOR 8 word mentah (tanpa bit-shift per-tap), Hipotesis, Implementasi, Kesimpulan

### Community 374 - "2. Inject-crossing (action item 2)"
Cohesion: 0.25
Nodes (7): 1. Dieharder (action item 1), 2. Inject-crossing (action item 2), 3. Recommendation, Dieharder + inject-crossing validation for singleblock K-small-defect fix candidates, Methodology correction found this session, Promotion Tier (16GB x K in {2,4,8,16,32,64,96}, VPS), Staged screening at K=96 (adapted quick-reject, local)

### Community 375 - "staged_inject_screening.py"
Cohesion: 0.43
Nodes (7): _log(), main(), n_words_for_mb(), Path, Staged PractRand screening for the 12 inject-crossing candidates…, run_candidate(), run_practrand()

### Community 376 - "orbit-round1-collision-position-isolation -- hasil (diagnosis only)"
Cohesion: 0.33
Nodes (6): 1. Driver `position_split_orbit.c` -- verifikasi wajib, 2. Stage 1: K=2, hashed, base_key=0x12345, staged 64MB->1GB per substream, 3. Interpretasi: hipotesis collision REFUTED sebagai penjelasan tunggal, 4. Status Stage 2 (K=255 asli), 5. `ra_core.c`, orbit-round1-collision-position-isolation -- hasil (diagnosis only)

### Community 377 - "collision_scan_orbit_k1.py"
Cohesion: 0.36
Nodes (7): main(), poisson_z(), experiments/2026-9-4_orbit-kmin-battery -- collision-scan for `ra_core_orbit`…, Birthday-bound estimate for raw 32-bit-word collision across n_items…, Normal-approximation z-score for a Poisson(expected) count., raw_collision_prob(), scan_k1()

### Community 378 - "Address Widening 64-bit RESULTS.md"
Cohesion: 0.32
Nodes (7): Orbit Widening Verification, main(), splitmix32_hi(), splitmix64_next(), Address Widening 64-bit RESULTS.md, 3-Way Avalanche Test (key_only / addr_hi_only / combined), Promotion of Candidate D to ra_core.c - Deferred (>=13 caller files affected)

### Community 379 - "ent (Entropy Testing Tool)"
Cohesion: 0.29
Nodes (7): ChaCha20 Entropy/Dieharder Test Result, /dev/urandom Entropy/Dieharder Test Result, ent (Entropy Testing Tool), PCG32 Entropy/Dieharder Test Result, Philox4x32 Entropy/Dieharder Test Result (1 subtest FAILED: rgb_lagged_sum ntup=31), xoshiro256** Entropy/Dieharder Test Result, ChaCha20 RNG Speed Benchmark Result

### Community 380 - "pcg32.c"
Cohesion: 0.43
Nodes (6): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), Candidate: pcg32, Official PCG reference (O'Neill, pcg-random.org)

### Community 381 - "src/xoshiro256.c"
Cohesion: 0.43
Nodes (6): main(), rotl(), xoshiro256_init(), xoshiro256_next(), Candidate: xoshiro256**, Official xoshiro256** reference (Blackman & Vigna)

### Community 382 - "xoshiro256_amortized.c"
Cohesion: 0.52
Nodes (6): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next(), xoshiro_buffered_next()

### Community 383 - "scrambler.py"
Cohesion: 0.52
Nodes (6): load_token_ids_from_file(), main(), ra_core(), ra_hash(), rot32(), save_token_ids_to_file()

### Community 384 - "2026-8-25_periodicity-heuristic-validation/run_sweep.py"
Cohesion: 0.48
Nodes (6): log2_fact(), main(), predicted(), Drives the full periodicity-heuristic-validation sweep. For each measurable (n,…, run_c(), run_python()

### Community 385 - "2026-8-28_periodicity-heuristic-validation2/avalanche_check.py"
Cohesion: 0.38
Nodes (6): capture_inner_outputs(), hamming(), Scaled-down avalanche / bit-flip sanity check for pruned_wired_toy_prng.py.…, One outer iteration, no rehash -- mirrors next_state's inner loop exactly (tap-…, Flip each bit of base_seed, compare captured c-sequences against the unflipped…, run_avalanche_check()

### Community 386 - "simd_prototype.c"
Cohesion: 0.48
Nodes (6): main(), rot32_avx2(), rot32_scalar(), run_scalar(), run_simd(), __m256i

### Community 387 - "RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`"
Cohesion: 0.29
Nodes (6): Context, Files produced this session, Interpretation, Method, Results, RESULTS: Dieharder "Good" Battery for `winner_wired_v2` and `winner_wired_addressable`

### Community 388 - "Family Productionization Handover"
Cohesion: 0.14
Nodes (14): CLAUDE.md (project instructions), docs/ERRATA.md, Family Productionization Handover, 0. Latar belakang (supaya sesi eksekusi tidak perlu riset ulang), 1. Tahap 1 (WAJIB PERTAMA): Unifikasi `winner_wired_v2.c` + `winner_wired_addressable.c`, 2. Tahap 2 (SETELAH Tahap 1 selesai & dikonfirmasi user): Folder/battery validasi produksi, 3. Non-goals eksplisit (supaya sesi eksekusi tidak overscope), 4. Open decisions untuk sesi eksekusi (BELUM diputuskan user — jangan diasumsikan, tanya dulu) (+6 more)

### Community 389 - "K=1 Exhaustive Function Analysis Handover"
Cohesion: 0.40
Nodes (5): /dev/urandom key-source diagnosis (defect is intrinsic, not SplitMix32-specific), K=1 Exhaustive Function Analysis Handover, 10-11 exhausted guess-and-check fix axes, Orbit K=1 Urandom-Key Diagnosis Results, Tier0/Tier1 battery tooling

### Community 390 - "`ra_core_singleblock`: pekerjaan baru, "aggressive reinit" skala produksi"
Cohesion: 0.15
Nodes (13): 1. Avalanche (`avalanche_ra_core.py`, reuse `quality_gate.py`'s `avalanche_gate_min_bit()`), 2. Speed (`bench_ra_core.c`, pola `tahap6_bench.c`), 3. Cross-correlation, multi-key (`cross_correlation_ra_core_singleblock.py`, tier "full": K=512, n=255), 4. Collision-scan, multi-key (`collision_scan_ra_core_singleblock.py`, tier "full": M=50.000, satu blok penuh 255-kata per key), 5. Dieharder "Good" battery, stream multi-key (`multikey_stream` + `run_dieharder_battery.py` reused), 6. PractRand bertahap, stream multi-key (`multikey_stream` -> `RNG_test stdin32`), Belum dikerjakan / perlu keputusan user, File yang dihasilkan sesi ini (+5 more)

### Community 391 - "avalanche_heatmap_winners_k255.py"
Cohesion: 0.52
Nodes (6): capture_cycle(), compute_matrix(), main(), plot_single(), Path, Avalanche heatmap (K=255, full single-key cycle) for the 11 Promotion Tier…

### Community 392 - "verify_orbit_kat.py"
Cohesion: 0.38
Nodes (6): last_word(), main(), Path, Byte-identical + KAT verification for orbit_bench.c's extern-linked…, # NOTE: for orbit, requesting rng=n does NOT guarantee exactly n words are, stream_bytes()

### Community 393 - "kat_mt19937"
Cohesion: 0.60
Nodes (5): kat_mt19937(), mt19937_genrand_int32(), mt19937_init_genrand(), ra_core_mt19937(), mt19937_state_t

### Community 394 - "PRNG Periodicity Analysis"
Cohesion: 0.29
Nodes (7): B. Harris, Donald E. Knuth, Makoto Matsumoto and Takuji Nishimura, Mersenne Twister (MT19937), PRNG Periodicity Analysis, Pierre L'Ecuyer, Random Mapping Theorem

### Community 395 - "source/chacha20.c"
Cohesion: 0.53
Nodes (4): chacha20_block(), chacha20_next(), main(), rand_bounded()

### Community 396 - "chacha20_amortized.c"
Cohesion: 0.47
Nodes (4): chacha20_block(), main(), rand_bounded(), Speed Benchmark Output (chacha20 amortized)

### Community 397 - "pcg.c"
Cohesion: 0.67
Nodes (5): pcg32_random_t, main(), pcg32_random_r(), pcg32_srandom(), rand_bounded()

### Community 398 - "source/xoshiro256.c"
Cohesion: 0.60
Nodes (5): main(), rand_bounded(), rotl(), xoshiro256_init(), xoshiro256_next()

### Community 399 - "verify_fresh_build.py"
Cohesion: 0.60
Nodes (4): check_multikey_cross(), check_validate(), main(), production-candidate-battery, Step 0: passive correctness gate on the FRESH…

### Community 400 - "2026-9-1_dieharder-battery/run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): main(), Path, Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"- reliability…, run_one(), run_test()

### Community 401 - "run_dieharder_battery_multikey.py"
Cohesion: 0.60
Nodes (5): main(), Path, Track A of the 2026-09-01 idle-VPS-capacity plan: dieharder "Good"- reliability…, run_one(), run_test()

### Community 402 - "verify_unification.py"
Cohesion: 0.60
Nodes (5): compile_new(), compile_old(), main(), run_stream_new(), run_stream_old()

### Community 403 - "2026-9-1_production-candidate-battery/run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): main(), Path, Step 5 (PRODUCTION_READINESS_HANDOVER.md) -- dieharder for the two multikey-…, run_one(), run_test()

### Community 404 - "scramble_w8_f28_i0_lookonly.py"
Cohesion: 0.53
Nodes (5): bulk_chain(), fisher_yates_k255(), gen(), pull_n(), key = base_key + i*GOLDEN for i=0..count-1, one word each -- matches the…

### Community 405 - "2026-9-3_dieharder-inject-crossing/run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): main(), Path, dieharder "Good"-reliability battery for the two singleblock K-small-defect…, run_one(), run_test()

### Community 406 - "orbit-ad-mul-combine-fix -- hasil"
Cohesion: 0.33
Nodes (6): 1. Byte-exact vs oracle Python (`combo_prng_admul.py`) -- WAJIB sebelum statistik, 2. Tier 0: avalanche gate (K=1, band LOW=0.3/HIGH=0.7/MIN_BIT_FLOOR=0.2, BASE_SEED=1), 3. Kenapa REJECT: konsekuensi struktural dari `d=0` di ronde pertama, 4. Kesimpulan, orbit-ad-mul-combine-fix -- hasil, Ringkasan axis yang sudah dicoba untuk defect K=1+hashed (kumulatif)

### Community 407 - "2026-9-4_orbit-kmin-battery/run_dieharder_battery.py"
Cohesion: 0.60
Nodes (5): main(), Path, experiments/2026-9-4_orbit-kmin-battery -- dieharder for the two multikey-…, run_one(), run_test()

### Community 408 - "STATUS (2026-09-04) — BLOCKED sebelum run utama, oleh temuan tak terduga"
Cohesion: 0.33
Nodes (5): Apa yang terjadi, File di folder ini, Kenapa ini memblokir test reseed, Keputusan (2026-09-04, user), STATUS (2026-09-04) — BLOCKED sebelum run utama, oleh temuan tak terduga

### Community 409 - "run_altconst_practrand.py"
Cohesion: 0.53
Nodes (5): iter_checkpoints(), parse_size(), Staged PractRand runner for the weyl-constant-resonance-diagnosis experiment.…, run_stage(), run_staged()

### Community 410 - "run_dieharder_battery_addr_hi.py"
Cohesion: 0.60
Nodes (5): main(), Path, experiments/2026-9-5_orbit-address-widen-64bit -- Tahap 6: dieharder untuk…, run_one(), run_test()

### Community 411 - "weyl_fmix32.c"
Cohesion: 0.53
Nodes (5): FILE, fmix32(), main(), splitmix32(), weyl_fmix32_core()

### Community 412 - "weyl_triple32.c"
Cohesion: 0.53
Nodes (5): FILE, main(), splitmix32(), triple32(), weyl_triple32_core()

### Community 413 - "test_ra_hash.py"
Cohesion: 0.33
Nodes (3): Pure-Python unit tests for ra_prng2.ra_hash / ra_core. These do not require the…, Regression test for the historical bug: ra_hash must be a pure function writing…, test_ra_hash_does_not_mutate_input()

### Community 414 - "tools/battery/"
Cohesion: 0.33
Nodes (6): Cara pakai untuk kandidat baru, Kontrak yang harus dipenuhi kandidat, Menguji core produksi (ra_core.c) tanpa mengubahnya, Mode dual-core: edit sekali, tes dua core sekaligus (direkomendasikan), tools/battery/, Yang TIDAK dicakup tool ini

### Community 415 - "verify_init_candidates.py"
Cohesion: 0.50
Nodes (4): main(), Path, Byte-identical verification for diag_init_candidates.c's four cores…, stream_bytes()

### Community 416 - "auto_stop_power38.py"
Cohesion: 0.60
Nodes (4): find_pid(), get_status(), main(), Monitors .cycle_measure_ckpt_n8_w4_rows4_seed0.bin and stops cycle_measure when…

### Community 417 - "full_scale_sanity_check.py"
Cohesion: 0.50
Nodes (4): check_no_repeat(), main(), Path, Full-scale (real 256x32-bit) practical periodicity sanity check, per…

### Community 418 - "wall_time_scaling.py"
Cohesion: 0.50
Nodes (4): main(), Q3(a): wall-clock speedup/efficiency when running N independent…, Launch n concurrent --stream processes (seeds 0..n-1), each emitting n_per_proc…, run_n_concurrent()

### Community 419 - "multikey_stream (K=255 gate binary)"
Cohesion: 0.40
Nodes (5): dieharder_K1_piped.txt (raw dieharder output, K=1, battery gate), dieharder_K255_piped.txt (raw dieharder output, K=255, battery gate), multikey_stream (K=255 gate binary), multikey_stream_singleblock_k1 (K=1 gate binary), run_dieharder_battery.py (production-candidate-battery adaptation)

### Community 420 - "score_shuffle.py"
Cohesion: 0.50
Nodes (4): main(), Path, Step 7 (PRODUCTION_READINESS_HANDOVER.md) -- score `scrambler_ra_core_…, score()

### Community 421 - "scrambler_ra_core_singleblock.c"
Cohesion: 0.70
Nodes (4): fisher_yates_k1(), main(), pull_n(), pull_one()

### Community 422 - "Avalanche heatmap w8_f8_i0 K=255 (full cycle)"
Cohesion: 0.40
Nodes (5): Avalanche heatmap w8_f8_i0 K=255 (full cycle), Avalanche heatmap w8_f9_i0 K=255 (full cycle), Avalanche test method: seed-bit-flip vs. output-word bit-difference heatmap, Cycle-op combo candidate w8_f8_i0 (tap width=8, finalizer f8, i0), Candidate w8_f9_i0 (tap width=8, finalizer c^=c>>16, cycle-op combo)

### Community 423 - "Avalanche Effect Analysis (ALL_OPS)"
Cohesion: 0.50
Nodes (5): Avalanche Effect Analysis (ALL_OPS), Bit Difference Distribution Across 32 Flipped Bits, Cycle 1 Avalanche Fraction (0.497576), Cycle 2 Avalanche Fraction (0.502141), Avalanche Effect Heatmap (Cycle 1 vs Cycle 2)

### Community 424 - "heatmap.py"
Cohesion: 0.70
Nodes (4): main(), rot32(), ZepFold(), ZepXORhash()

### Community 425 - "ra_prng2.py"
Cohesion: 0.70
Nodes (4): main(), ra_core(), ra_hash(), rot32()

### Community 426 - "c/ra_prng3.c"
Cohesion: 0.70
Nodes (4): main(), rot64(), ZepFold(), ZepXORhash()

### Community 427 - "Avalanche Effect Curve: ra_core_orbit (K=255)"
Cohesion: 0.50
Nodes (4): CSAI2025/ICCS2026 avalanche-effect analysis methodology, Avalanche Effect Curve: ra_core_orbit (K=255), Avalanche Effect Curve: ra_core singleblock, ra_core orbit PRNG core

### Community 428 - "ra_prng3 (64-bit experimental successor)"
Cohesion: 0.20
Nodes (10): Scrambling Speed Benchmark (perf stat, per-call vs amortized, 30 runs), ra_prng Build and Test Workflow, Contributing Guide, ra_prng Implementation Variant Roles, Repository Structure Specification, Array-Index Permutation as Randomness Source, CC BY-NC-SA 4.0 License, ra_prng (Array-Based PRNG Architecture) (+2 more)

### Community 429 - "Cycle 3 Avalanche Fraction (0.470588)"
Cohesion: 0.83
Nodes (4): Cycle 1 Avalanche Fraction (0.418459), Cycle 3 Avalanche Fraction (0.470588), Avalanche Heatmap: Cycle 1 vs Cycle 3 (pruned_winner), Reseed Avalanche Diffusion in Pruned Winner

### Community 430 - "Paper heuristic λ ≈ 0.7824·√|S|"
Cohesion: 0.50
Nodes (4): Paper heuristic λ ≈ 0.7824·√|S|, Research Question: does pruned/rewired algorithm shift the heuristic's λ-vs-λ_pred fit, Central Research Question (RESULTS.md), Verdict 2: heuristic behaves as conservative lower bound at large-scale configs only

### Community 431 - "Tap-survivor generalization (top-2-by-count rule, G=8→(6,7))"
Cohesion: 0.50
Nodes (4): Tap shift-width collapse (e>=w yields identically-zero term), Tap-survivor generalization (top-2-by-count rule, G=8→(6,7)), Finding: non-vacuous tap pruning (G=4) gives closest fit to heuristic, Caveat: tap pruning is a no-op at G<=2 configs

### Community 432 - "Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche"
Cohesion: 0.67
Nodes (4): Core Hypothesis: invariant L-multiset carries address fingerprint, M/cons carries avalanche, Kandidat 3 (negative control: L rotated only, multiset unchanged), Kandidat 5 (user's rot32-based L/M formula, winning candidate), L_MIX_CONST (frozen to 0x9e3779b7)

### Community 433 - "JAX"
Cohesion: 0.50
Nodes (4): JAX, Philox (counter-mode PRNG), Tahap 4: speed break-even benchmark vs Philox, Threefry2x32

### Community 435 - "pick_guard_constants.py"
Cohesion: 0.67
Nodes (3): candidates(), pick(), Guard Constant Derivation (popcount==16, SHA-256 scan)

### Community 436 - "Avalanche Effect Heatmap (Original Algorithm)"
Cohesion: 0.67
Nodes (4): Avalanche Effect, Avalanche Effect Heatmap (Original Algorithm), Hamming Distance Analysis, Original PRNG Algorithm

### Community 437 - "Graphify Knowledge Graph Rule"
Cohesion: 0.67
Nodes (3): Graphify Query & Navigation Protocol, Graphify Knowledge Graph Rule, Graphify Pipeline Workflow

### Community 439 - "Graphify-First Research Workflow Policy"
Cohesion: 0.67
Nodes (3): graphify-out/graph.json Knowledge Graph Artifact, graphify Skill, Graphify-First Research Workflow Policy

### Community 440 - "ra_reseed/ra_hash orbit pre-reseed BCFN defect (still open, separate track)"
Cohesion: 0.67
Nodes (3): ra_hash (shared helper), ra_reseed (shared helper), ra_reseed/ra_hash orbit pre-reseed BCFN defect (still open, separate track)

### Community 441 - "Two-mode decision: orbit + singleblock only, no third fixed-init mode"
Cohesion: 0.67
Nodes (3): tahap6_bench.c (benchmark harness, no-L fast path source), Two-mode decision: orbit + singleblock only, no third fixed-init mode, winner_wired_v2.c (old fixed-init, no-key stream mode)

### Community 442 - "Cycle 1 vs Cycle 5 Avalanche Fraction Convergence"
Cohesion: 1.00
Nodes (3): Cycle 1 vs Cycle 5 Avalanche Fraction Convergence, Avalanche Heatmap: Pruned Winner Cycle 1 vs Cycle 5, Pruned Winner Architecture (TAP6, TAP7, ROT_C, SHR13)

### Community 443 - "Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13)"
Cohesion: 1.00
Nodes (3): Avalanche Effect Heatmap: pruned_winner (TAP6, TAP7, ROT_C, SHR13), pruned_winner Cycle 1 Avalanche Sensitivity and Dead Zones, pruned_winner 4-Op Candidate Configuration (TAP6, TAP7, ROT_C, SHR13)

### Community 444 - "Greedy Operation-Pruning Search Paradigm"
Cohesion: 1.00
Nodes (3): Greedy Operation-Pruning Search Paradigm, Operation-Pruning Research Handover Spec, Limits of Zero-Drift Micro-Optimizations on ra_core

### Community 445 - "collision_scan.py"
Cohesion: 0.67
Nodes (3): collision_scan.py, Q2: Cross-stream collision probability, ProcessPoolExecutor deadlock -> ThreadPoolExecutor+lock fix

### Community 446 - "Q3: Multi-process throughput scalability"
Cohesion: 0.67
Nodes (3): perf_scaling.sh, Q3: Multi-process throughput scalability, wall_time_scaling.py

### Community 447 - "pcg.c (PCG32 comparison binary)"
Cohesion: 0.67
Nodes (3): out_pcg.txt (output artifact of pcg speed-benchmark run), out_pcg_amortized.txt (output artifact of pcg amortized speed-benchmark run), pcg.c (PCG32 comparison binary)

### Community 448 - "xoshiro256.c (fread+tokenizer comparison binary)"
Cohesion: 0.67
Nodes (3): out_xoshiro256.txt (output artifact of xoshiro256 speed-benchmark run), out_xoshiro256_amortized.txt (output artifact of xoshiro256 amortized speed-benchmark run), xoshiro256.c (fread+tokenizer comparison binary)

### Community 450 - "2. Blocker utama: singleblock K-small structural defect"
Cohesion: 0.67
Nodes (3): 2. Blocker utama: singleblock K-small structural defect, 2 kandidat fix co-frontier (commit `d2f1675`, hari ini), Safe-K floor formula SAAT INI (belum di-fix)

### Community 451 - "Avalanche heatmap w8_f22_i0 K=255 (full cycle)"
Cohesion: 1.00
Nodes (3): Avalanche heatmap w8_f22_i0 K=255 (full cycle), Candidate w8_f22_i0 (tap width=8, finalizer f22, cycle-op combo search), Result: uniform avalanche (overall=0.5010, min_bit=0.4915), no visible weak bit/word structure

### Community 452 - "HANDOVER: Continuous-stream real benchmark untuk `ra_core_orbit`"
Cohesion: 0.40
Nodes (5): Deliverable, Deviasi dari rencana (didokumentasikan), HANDOVER: Continuous-stream real benchmark untuk `ra_core_orbit`, Konteks, Rencana implementasi (plan yang disetujui)

### Community 456 - "Periodicity Heuristic Validation Status Report"
Cohesion: 0.50
Nodes (4): Brent Cycle Measure Checkpoint/Resume Architecture, Hash Depth Domination on Cycle Length, Exact State-Space In-Degree Poisson(1) Rejection, Periodicity Heuristic Validation Status Report

### Community 457 - "sub-projects/ (downstream applications)"
Cohesion: 0.67
Nodes (3): fingerprinting_tools sub-project, image_encrypting sub-project, sub-projects/ (downstream applications)

## Ambiguous Edges - Review These
- `ra_hash()` → `64-bit Address Space Widening Design (key+param2)`  [AMBIGUOUS]
  experiments/2026-9-5_orbit-address-widen-64bit/HANDOVER.md · relation: references
- `ra_reseed()` → `64-bit Address Space Widening Design (key+param2)`  [AMBIGUOUS]
  experiments/2026-9-5_orbit-address-widen-64bit/HANDOVER.md · relation: references
- `Orbit Continuous-Bench Results` → `RESULTS_TAHAP2.md (orbit K=1 historical figure)`  [AMBIGUOUS]
  experiments/2026-9-3_orbit-continuous-bench/RESULTS.md · relation: references
- `Hamas A. Rahman (author)` → `hamzy hams (GitHub maintainer)`  [AMBIGUOUS]
  research/csai2025_ra_prng/ra_prng2 and Beyond: An Array-Based PRNG Architecture for Efficient Random Generation.pdfCA-162-Final.pdf · relation: conceptually_related_to

## Knowledge Gaps
- **814 isolated node(s):** `other_winners_perf.sh script`, `other_winners_practrand.sh script`, `perf_state_update.sh script`, `perf_state_update_m.sh script`, `toy_probe_phase3.sh script` (+809 more)
  These have ≤1 connection - possible missing edges or undocumented components.
- **117 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `ra_hash()` and `64-bit Address Space Widening Design (key+param2)`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `ra_reseed()` and `64-bit Address Space Widening Design (key+param2)`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Orbit Continuous-Bench Results` and `RESULTS_TAHAP2.md (orbit K=1 historical figure)`?**
  _Edge tagged AMBIGUOUS (relation: references) - confidence is low._
- **What is the exact relationship between `Hamas A. Rahman (author)` and `hamzy hams (GitHub maintainer)`?**
  _Edge tagged AMBIGUOUS (relation: conceptually_related_to) - confidence is low._
- **Why does `ra_core_singleblock Cycle-Operation Combo Search — RESULTS` connect `ra_core_singleblock Cycle-Operation Combo Search — RESULTS` to `Avalanche heatmap K=255 (full cycle) re-check of 11 Promotion Tier winners for hidden dead-bit defect`, `ra_core.c`, `Handover: uji 2 kandidat fix tertarget (seed `d0` di fase init) untuk defect K=1+hashed pada `ra_core_orbit``, `ra_core_orbit`, `Singleblock Cycle-Operation Combo Search Handover`, `Orbit Fix and Wide-o Rolling Optimization Handover`, `promotion_search.py`, `avalanche_stats`, `Pareto Selection Results (Combo Winners)`, `recipes.py`, `HANDOVER — release-benchmark-prep (2026-09-06)`, `ra_core_singleblock`, `combo_prng.py`, `combo_gen.py`?**
  _High betweenness centrality (0.049) - this node is a cross-community bridge._
- **Why does `ra_core_orbit()` connect `ra_core_orbit` to `winners_bench.c`, `ra_core.c`, `64-bit Address Widening Handover`, `Orbit Continuous-Stream Measurement (376.2 MB/s)`, `candidate_d.c`, `2026-9-4_orbit-key-avalanche-diagnosis/RESULTS.md`, `HANDOVER — release-benchmark-prep (2026-09-06)`, `ra_core (unified addressable-init successor)`, `winner_wired_addressable.c`, `ra_core_singleblock`?**
  _High betweenness centrality (0.027) - this node is a cross-community bridge._
- **Why does `Axis B - Avalanche Multi-seed + Normality` connect `Axis B - Avalanche Multi-seed + Normality` to `winners_bench.c`, `w8_f24_i0.c`, `w8_f26_i0.c`, `w8_f27_i0.c`, `w8_f28_i0.c`, `w8_f29_i0.c`, `w8_f25_i0.c`, `w8_f33_i0.c`, `w8_f10_i0.c`, `w8_f9_i0.c`, `RESULTS: Pareto selection over the 11 combo-search winners`?**
  _High betweenness centrality (0.021) - this node is a cross-community bridge._