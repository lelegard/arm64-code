#----------------------------------------------------------------------------
#
# Arm64 CPU system registers tools
# Copyright (c) 2023, Thierry Lelegard
# BSD-2-Clause license, see the LICENSE file.
#
# Common definitions for makefiles.
#
#----------------------------------------------------------------------------

SYSTEM   := $(shell uname -s)
CXXFLAGS += -std=c++11 -O2
LDLIBS   += -lstdc++
ARFLAGS   = rc

# All .cpp files with a corresponding .h are compiled into libtest.a.
# All .cpp files without corresponding .h are individual executables and use libtest.a.

SOURCES   := $(wildcard *.cpp)
HEADERS   := $(wildcard *.h)
EXECS     := $(filter-out $(basename $(HEADERS)),$(basename $(SOURCES)))
ALL_OBJS  := $(patsubst %.cpp,%.o,$(SOURCES))
EXEC_OBJS := $(addsuffix .o,$(EXECS))
LIB_OBJS  := $(addsuffix .o,$(filter $(basename $(HEADERS)),$(basename $(SOURCES))))
LIB_FILE  := libtest.a

execs: $(EXECS)

$(EXECS): $(LIB_FILE)

$(LIB_FILE): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^
clean:
	rm -f *.o *.a *.d $(EXECS)

# Regenerate implicit dependencies.
ifneq ($(if $(MAKECMDGOALS),$(filter-out clean,$(MAKECMDGOALS)),true),)
    -include $(patsubst %.cpp,%.d,$(SOURCES))
endif
%.d: %.cpp
	$(CXX) -MM $(CPPFLAGS) -MT $*.o -MT $@ $< >$@ || rm -f $@
