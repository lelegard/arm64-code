#----------------------------------------------------------------------------
#
# Arm64 CPU system registers tools
# Copyright (c) 2023, Thierry Lelegard
# BSD-2-Clause license, see the LICENSE file.
#
#----------------------------------------------------------------------------

MAKEFLAGS += --no-print-directory
SUBDIRS := $(sort $(filter-out .git%,$(notdir $(shell find . -mindepth 1 -maxdepth 1 -type d))))

default test perf clean:
	for d in $(SUBDIRS); do $(MAKE) $@ -C $$d; done
