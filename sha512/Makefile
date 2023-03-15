default: execs
include ../Makefile.inc

# Force SHA512 instructions on Linux (enabled by default on macOS).
# Note that we need "armv8.2-a+sha3", omiting the ".2" does not work.
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8.2-a+sha3
endif

test: sha512_test
	./sha512_test
perf: sha512_perf
	./sha512_perf
