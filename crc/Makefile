default: execs
include ../Makefile.inc

# Force CRC32 instructions on Linux (enabled by default on macOS).
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8-a+crc
endif

test: crc_test
	./crc_test
perf: crc_perf
	./crc_perf
