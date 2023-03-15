default: execs
include ../Makefile.inc

# Impossible to compile SHA256 intrinsics with GCC. See:
# https://stackoverflow.com/questions/75745202/arm64-sha512-intrinsics-fail-to-compile-with-gcc-target-specific-option-mismat
ifeq ($(SYSTEM),Linux)
    CXX = clang
endif

# Force SHA256 instructions on Linux (enabled by default on macOS).
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8-a+sha2
endif

test: sha256_test
	./sha256_test
perf: sha256_perf
	./sha256_perf
