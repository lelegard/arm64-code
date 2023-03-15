default: execs
include ../Makefile.inc

# Impossible to compile SHA512 intrinsics with GCC. See:
# https://stackoverflow.com/questions/75745202/arm64-sha512-intrinsics-fail-to-compile-with-gcc-target-specific-option-mismat
ifeq ($(SYSTEM),Linux)
    CXX = clang
endif

# Force SHA512 instructions on Linux (enabled by default on macOS).
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8-a+sha3
endif

test: sha512_test
	./sha512_test
perf: sha512_perf
	./sha512_perf
