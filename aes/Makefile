default: execs
include ../Makefile.inc

# Force AES instructions on Linux (enabled by default on macOS).
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8-a+crypto
endif

test: aes_test
	./aes_test
perf: aes_perf
	./aes_perf
