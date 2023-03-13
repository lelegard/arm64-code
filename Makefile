#----------------------------------------------------------------------------
#
# Arm64 CPU system registers tools
# Copyright (c) 2023, Thierry Lelegard
# BSD-2-Clause license, see the LICENSE file.
#
#----------------------------------------------------------------------------

MAKEFLAGS += --no-print-directory

default test perf clean:
	$(MAKE) $@ -C mpeg-crc
	$(MAKE) $@ -C sha1
	$(MAKE) $@ -C aes
