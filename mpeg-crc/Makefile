SYSTEM   := $(shell uname -s)
CXXFLAGS += -std=c++11 -O2
LDLIBS   += -lstdc++
ARFLAGS   = rc

# Force CRC32 instructions on Linux (enabled by default on macOS).
ifeq ($(SYSTEM),Linux)
    CXXFLAGS += -march=armv8-a+crc
endif

SOURCES   := $(wildcard *.cpp)
HEADERS   := $(wildcard *.h)
EXECS     := $(filter-out $(basename $(HEADERS)),$(basename $(SOURCES)))
ALL_OBJS  := $(patsubst %.cpp,%.o,$(SOURCES))
EXEC_OBJS := $(addsuffix .o,$(EXECS))
LIB_OBJS  := $(addsuffix .o,$(filter $(basename $(HEADERS)),$(basename $(SOURCES))))
LIB_FILE  := libcrc.a

default: $(EXECS)

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
