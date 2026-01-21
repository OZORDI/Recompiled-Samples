# Build Xenia's actual xboxkrnl implementations
# These are the REAL implementations from src/xenia/kernel/xboxkrnl/

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -g -fPIC \
    -I../../../../src \
    -I../../../../third_party/fmt/include \
    -I../../.. \
    -DXENIA_STATIC_RECOMP

# Xenia kernel source files (the REAL implementations)
XBOXKRNL_SOURCES = \
    xboxkrnl_threading.cc \
    xboxkrnl_memory.cc \
    xboxkrnl_io.cc \
    xboxkrnl_io_info.cc \
    xboxkrnl_ob.cc \
    xboxkrnl_rtl.cc \
    xboxkrnl_strings.cc \
    xboxkrnl_error.cc \
    xboxkrnl_video.cc \
    xboxkrnl_audio.cc \
    xboxkrnl_module.cc \
    xboxkrnl_misc.cc

OBJECTS = $(XBOXKRNL_SOURCES:.cc=.o)

all: libxboxkrnl.a

libxboxkrnl.a: $(OBJECTS)
	ar rcs $@ $^

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) libxboxkrnl.a

.PHONY: all clean
