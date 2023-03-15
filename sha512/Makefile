default: execs
include ../Makefile.inc

# Force SHA512 instructions on Linux (enabled by default on macOS).
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8-a+sha2
endif

test: sha512_test
	./sha512_test
perf: sha512_perf
	./sha512_perf
