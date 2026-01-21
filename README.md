# Xenia Static Recompilation Runtime

This directory contains samples of statically recompiled Xbox 360 games that use Xenia's kernel as a runtime library.

## Overview

The Xenia Static Recompilation Runtime allows running ahead-of-time (AOT) compiled Xbox 360 games on native hardware. Instead of JIT-compiling PowerPC code at runtime, games are pre-compiled to native x86-64 code by XenonRecomp, then linked against Xenia's kernel implementation.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Recompiled Game                          │
│  (ppc_recomp.*.cpp - native x86-64 code from XenonRecomp)  │
└─────────────────────────────────────────────────────────────┘
                              │
                              │ calls __imp__* functions
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Kernel Exports                           │
│  (kernel_impl.cpp / kernel_exports.cc)                      │
│  __imp__ExCreateThread, __imp__NtCreateFile, etc.          │
└─────────────────────────────────────────────────────────────┘
                              │
                              │ bridges to
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Xenia Kernel                             │
│  (xboxkrnl.exe, xam.xex implementations)                    │
│  Threading, Sync, Memory, File I/O, Input, etc.            │
└─────────────────────────────────────────────────────────────┘
```

## Files Structure

Each recompiled game directory contains:

- `ppc_config.h` - Memory layout constants (IMAGE_BASE, CODE_BASE, etc.)
- `ppc_context.h` - PPCContext structure and helper macros
- `ppc_func_mapping.cpp` - Function address to native pointer mappings
- `ppc_recomp.*.cpp` - Actual recompiled game code
- `kernel_impl.cpp` - Kernel API implementations/stubs
- `main.cpp` - Entry point and initialization

## Key Concepts

### PPCContext

The `PPCContext` structure holds the emulated PowerPC CPU state:
- `r0-r31` - General purpose registers
- `f0-f31` - Floating point registers  
- `v0-v127` - VMX128 vector registers
- `lr`, `ctr` - Link and count registers
- `cr0-cr7` - Condition registers

### Function Signature

All recompiled functions use this signature:
```cpp
void func(PPCContext& ctx, uint8_t* base);
```

Where:
- `ctx` - CPU register state
- `base` - Guest memory base pointer

### Calling Convention

Arguments are passed in registers r3-r10:
- `r3` - First argument / return value
- `r4-r10` - Additional arguments

### Memory Access

Guest memory is accessed via the `base` pointer:
```cpp
// Load 32-bit value (big-endian)
uint32_t val = __builtin_bswap32(*(uint32_t*)(base + addr));

// Store 32-bit value (big-endian)
*(uint32_t*)(base + addr) = __builtin_bswap32(val);
```

### Indirect Calls

The `PPC_LOOKUP_FUNC` macro performs indirect function calls:
```cpp
PPCFunc* func = PPC_LOOKUP_FUNC(base, target_addr);
if (func) func(ctx, base);
```

This uses a perfect hash lookup table populated during initialization.

## Building

### Standalone (no Xenia linking)

```bash
cd RAGE_GTA_IV
make
./gta4_recomp
```

### With Xenia Runtime

1. Build Xenia as a static library:
```bash
cd /path/to/xenia
# Build xenia_runtime target
```

2. Link your game against Xenia:
```bash
clang++ -o game main.cpp ppc_*.cpp -lxenia_runtime -lpthread
```

## Kernel API Status

### Implemented
- Threading: ExCreateThread, NtResumeThread, NtSuspendThread
- Sync: NtCreateEvent, KeSetEvent, KeWaitForSingleObject
- Memory: NtAllocateVirtualMemory, MmAllocatePhysicalMemoryEx
- File I/O: NtCreateFile, NtReadFile, NtWriteFile, NtClose
- Time: KeQueryPerformanceFrequency, KeDelayExecutionThread
- Debug: DbgPrint

### Stubbed (returns success, minimal functionality)
- Most XAM functions (UI, achievements, multiplayer)
- Networking (NetDll_*)
- Video (Vd*)
- Audio (XAudio*, XMA*)

### Not Implemented
- GPU rendering (handled externally)
- Audio output (handled externally)

## Adding New Kernel APIs

1. Add declaration to `kernel_exports.h`:
```cpp
KERNEL_EXPORT(NewFunction);
```

2. Implement in `kernel_exports.cc` or `kernel_exports_xenia.cc`:
```cpp
BRIDGE_FUNC(NewFunction) {
    uint32_t arg1 = ARG_u32(3);
    // Implementation...
    RETURN(result);
}
```

3. If using Xenia's kernel, call the actual implementation:
```cpp
void __imp__NewFunction(PPCContext& ctx, uint8_t* base) {
    // Extract args
    // Call xenia kernel function
    // Set return value
}
```

## Debugging

### Missing Functions

If you see "Missing function at 0xXXXXXXXX":
1. Check if the address is in the recompiled code range
2. Verify ppc_func_mapping.cpp contains the address
3. May need to rerun XenonRecomp with that function

### Kernel Stubs

Run `make stubs` to see which kernel functions are called:
```bash
make stubs
```

This helps identify which stubs need real implementations.

## License

This project is part of the Xenia Xbox 360 Emulator project.
See the main Xenia LICENSE file for details.
