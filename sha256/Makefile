default: execs
include ../Makefile.inc

# Force SHA256 instructions on Linux (enabled by default on macOS).
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8-a+crypto+sha2
endif

test: sha256_test
	./sha256_test
perf: sha256_perf
	./sha256_perf
