# Kernel Library Makefile
# Builds the Xenia-derived kernel as a static library

CXX := clang++
CXXFLAGS := -std=c++17 -O2 -fPIC -Wall -Wno-unused-function
CXXFLAGS += -I.. -I.

# Source files
SOURCES := \
	threading.cc \
	xobject.cc \
	xevent.cc \
	xmutant.cc \
	xsemaphore.cc \
	xthread.cc \
	kernel_state.cc \
	vfs.cc \
	util/object_table.cc

OBJECTS := $(SOURCES:.cc=.o)

# Output
LIBNAME := libxekernel.a

.PHONY: all clean

all: $(LIBNAME)

$(LIBNAME): $(OBJECTS)
	ar rcs $@ $^
	@echo "Built $(LIBNAME)"

%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIBNAME)

# Dependencies
threading.o: threading.h
xobject.o: xobject.h xbox_types.h kernel_state.h
xevent.o: xevent.h xobject.h kernel_state.h
xmutant.o: xmutant.h xobject.h kernel_state.h
xsemaphore.o: xsemaphore.h xobject.h kernel_state.h
xthread.o: xthread.h xobject.h kernel_state.h threading.h
kernel_state.o: kernel_state.h xobject.h util/object_table.h
util/object_table.o: util/object_table.h xobject.h
