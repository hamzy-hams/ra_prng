CC       := gcc
# -std=gnu17 (not c11): the original sources rely on GNU/POSIX extensions
# (clock_gettime) that strict ISO C hides. -include stdalign.h supplies the
# `alignas` macro the sources use without requiring C23 or editing the sources.
CFLAGS   := -O3 -march=native -std=gnu17 -include stdalign.h
BUILD    := build
BIN      := $(BUILD)/bin
LIB      := $(BUILD)/lib

.PHONY: all build-c build-ext install dev-install test bench clean

all: build-c build-ext

build-c: $(BIN)/ra_prng2 $(BIN)/ra_prng2_thread $(BIN)/ra_prng3

$(BIN)/ra_prng2: src/ra_prng2/c/ra_prng2.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/ra_prng2_thread: src/ra_prng2/c/ra_prng2_thread.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -fopenmp $< -o $@

$(BIN)/ra_prng3: src/ra_prng3/c/ra_prng3.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) $< -o $@

# Shared library exposing ra_prng_init/next/advance (see src/ra_prng2/c/ra_prng2_struct.c)
# consumed by the ctypes binding in bindings/cpython/_ra_prng2.py.
build-ext: $(LIB)/libra_prng2.so

$(LIB)/libra_prng2.so: src/ra_prng2/c/ra_prng2_struct.c
	mkdir -p $(LIB)
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

# Editable install of the pure-Python ra_prng2 reference implementation (pyproject.toml).
install: build-ext
	pip install -e .

dev-install: build-ext
	pip install -e '.[dev]'

# Tolerant while tests/ is still an empty scaffold (exit code 5 = "no tests collected").
test:
	pytest tests/ -q; code=$$?; \
	if [ $$code -eq 5 ]; then echo "No tests collected yet - OK"; exit 0; fi; \
	exit $$code

# No automated benchmark harness exists yet (benchmarks/results/* are precomputed
# outputs from manual runs of Dieharder/NIST STS/TestU01/PractRand). See
# benchmarks/comparisons/source/README.md for how those were produced.
bench:
	@echo "No automated bench runner yet - see benchmarks/comparisons/source/ and benchmarks/results/"

clean:
	rm -rf $(BUILD)
