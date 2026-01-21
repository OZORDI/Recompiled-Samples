# GTA IV Static Recompilation - Makefile
# Builds the recompiled game with Xenia's REAL kernel runtime

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -g -Wall -Wno-unused-variable -Wno-unused-function

# SIMDE (SIMD Everywhere) for x86 intrinsics
XENIA_ROOT = ../..
SIMDE_PATH = thirdparty/simde
KERNEL_PATH = ../rage_runtime/kernel
CXXFLAGS += -I$(SIMDE_PATH) -I../rage_runtime -I$(KERNEL_PATH)

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS
    LDFLAGS = -lpthread
    CXXFLAGS += -mmacosx-version-min=10.15
else ifeq ($(UNAME_S),Linux)
    # Linux
    LDFLAGS = -lpthread -lrt
else
    # Windows (MinGW)
    LDFLAGS = -lws2_32
endif

# Kernel library
KERNEL_LIB = $(KERNEL_PATH)/libxekernel.a

# Source files - use kernel_impl.cpp (stubs) or kernel_impl_xenia.cpp (Xenia's logic)
# To use real kernel: make real
ifdef USE_REAL_KERNEL
    KERNEL_SRC = kernel_impl_xenia.cpp
    CXXFLAGS += -DUSE_REAL_KERNEL
    LDFLAGS += -L$(KERNEL_PATH) -lxekernel
else
    KERNEL_SRC = kernel_impl.cpp
endif

SOURCES = \
    main.cpp \
    $(KERNEL_SRC) \
    ppc_func_mapping.cpp

# Add all ppc_recomp.*.cpp files
RECOMP_SOURCES := $(wildcard ppc_recomp.*.cpp)
SOURCES += $(RECOMP_SOURCES)

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Output
TARGET = gta4_recomp

# Default target
all: $(TARGET)

# Build with real kernel
real: kernel
	$(MAKE) USE_REAL_KERNEL=1

# Build kernel library
kernel:
	$(MAKE) -C $(KERNEL_PATH)

$(TARGET): $(OBJECTS)
ifdef USE_REAL_KERNEL
	$(CXX) $(CXXFLAGS) -o $@ $^ $(KERNEL_LIB) $(LDFLAGS)
	@echo "Built $(TARGET) with REAL kernel"
else
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built $(TARGET) with stub kernel"
endif
	@echo "Run with: ./$(TARGET)"

%.o: %.cpp ppc_context.h ppc_config.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Count functions
count:
	@echo "Recompiled source files:"
	@ls -la ppc_recomp.*.cpp 2>/dev/null | wc -l
	@echo "Approximate function count:"
	@grep -c "^PPC_FUNC" ppc_recomp.*.cpp 2>/dev/null | awk -F: '{sum += $$2} END {print sum}'

# Show kernel stubs that need implementation
stubs:
	@echo "Kernel functions called by the game:"
	@grep -oh "__imp__[A-Za-z0-9_]*" ppc_recomp.*.cpp 2>/dev/null | sort | uniq -c | sort -rn

.PHONY: all real kernel clean run count stubs
