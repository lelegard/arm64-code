default: execs
include ../Makefile.inc

# Force SHA1 instructions on Linux (enabled by default on macOS).
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8-a+crypto
endif
