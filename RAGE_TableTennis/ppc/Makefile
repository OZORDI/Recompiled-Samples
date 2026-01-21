# Rockstar Table Tennis - Static Recompilation Build

CXX = clang++
CXXFLAGS = -std=c++17 -O2 -g -Wall -Wno-unused-variable -Wno-unused-function

# Paths
XENIA_ROOT = ../../..
SIMDE_PATH = $(XENIA_ROOT)/third_party/simde/simde
KERNEL_PATH = ../../rage_runtime/kernel
CXXFLAGS += -I$(SIMDE_PATH) -I.. -I../../rage_runtime -I$(KERNEL_PATH)

# Platform
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -lpthread
    CXXFLAGS += -mmacosx-version-min=10.15
else ifeq ($(UNAME_S),Linux)
    LDFLAGS = -lpthread -lrt
else
    LDFLAGS = -lws2_32
endif

# Real kernel support: make USE_REAL_KERNEL=1
ifdef USE_REAL_KERNEL
    KERNEL_SRC = kernel_impl_real.cpp
    CXXFLAGS += -DUSE_REAL_KERNEL
    LDFLAGS += -L$(KERNEL_PATH) -lxekernel
else
    KERNEL_SRC = kernel_impl.cpp
endif

# Sources
SOURCES = main.cpp $(KERNEL_SRC) ppc_func_mapping.cpp
RECOMP_SOURCES := $(wildcard ppc_recomp.*.cpp)
SOURCES += $(RECOMP_SOURCES)

OBJECTS = $(SOURCES:.cpp=.o)
TARGET = tabletennis

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built $(TARGET)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Build with real kernel
real: kernel
	$(MAKE) USE_REAL_KERNEL=1

kernel:
	$(MAKE) -C $(KERNEL_PATH)

.PHONY: all clean run real kernel
