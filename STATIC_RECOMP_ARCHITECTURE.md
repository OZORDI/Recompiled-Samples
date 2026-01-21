# Static Recompilation Runtime Architecture

## Overview

This document describes how to adapt Xenia's kernel for static recompilation.
The goal is to run statically recompiled Xbox 360 code using Xenia's kernel
infrastructure, WITHOUT the JIT compiler or GPU emulation.

**Key Principle**: GPU/rendering is NOT implemented - that's for game developers
to implement themselves. This is a static recompiler runtime, not an emulator.

## Xenia's Architecture (from fresh clone study)

### Key Components

1. **KernelState** (`src/xenia/kernel/kernel_state.cc`)
   - Global singleton managing all kernel objects
   - Thread registration/unregistration
   - TLS bitmap management
   - DPC dispatch thread
   - Object table for handles

2. **XThread** (`src/xenia/kernel/xthread.cc`)
   - Xbox 360 thread abstraction
   - Creates native thread with `xe::threading::Thread::Create()`
   - `Execute()` method calls `processor()->Execute(thread_state, address, args)`
   - Manages PCR (Processor Control Region) and TLS

3. **XObject** (`src/xenia/kernel/xobject.cc`)
   - Base class for all kernel objects
   - Reference counting via `Retain()`/`Release()`
   - Handle management via ObjectTable
   - Types: Thread, Event, Mutant, Semaphore, File, etc.

4. **Processor** (`src/xenia/cpu/processor.cc`)
   - JIT compilation and execution
   - `Execute()` - main execution entry point
   - `LookupFunction()` - finds/compiles function at address

5. **PPCContext** (`src/xenia/cpu/ppc/ppc_context.h`)
   - PowerPC register state (r0-r31, f0-f31, v0-v127)
   - CR, XER, LR, CTR registers
   - `virtual_membase` - pointer to guest memory

### Execution Flow in Xenia

```
XThread::Execute()
    |
    v
kernel_state()->OnThreadExecute(this)  // DLL attach callbacks
    |
    v
processor()->Execute(thread_state, address, args)
    |
    v
[JIT compiles and executes PowerPC code]
    |
    v
XThread::Exit() or return
```

## Static Recompilation Adaptation

### What Changes

1. **Replace Processor::Execute()** with static function dispatch:
   ```cpp
   // Instead of:
   processor()->Execute(thread_state, address, args);
   
   // Use:
   StaticDispatch(address, context, memory_base);
   ```

2. **StaticDispatch** looks up pre-compiled function:
   ```cpp
   void StaticDispatch(uint32_t address, PPCContext* ctx, uint8_t* base) {
       auto func = g_function_table[address];
       if (func) {
           func(*ctx, base);
       } else {
           printf("Missing function: 0x%08X\n", address);
       }
   }
   ```

3. **XThread::Execute()** adapted for static recomp:
   ```cpp
   void XThread::Execute() {
       kernel_state()->OnThreadExecute(this);
       
       uint32_t address = creation_params_.xapi_thread_startup 
           ? creation_params_.xapi_thread_startup 
           : creation_params_.start_address;
       
       // Set up arguments in context
       context_->r[3] = creation_params_.start_context;
       if (creation_params_.xapi_thread_startup) {
           context_->r[4] = creation_params_.start_address;
       }
       
       // Call static function instead of JIT
       StaticDispatch(address, context_, memory_base_);
       
       Exit(static_cast<int>(context_->r[3]));
   }
   ```

### What Stays the Same

- **KernelState** - thread/object management unchanged
- **XObject hierarchy** - Event, Mutant, Semaphore work as-is
- **Synchronization primitives** - Wait, Signal, etc.
- **Memory management** - guest memory layout
- **Kernel function implementations** - ExCreateThread, KeWaitForSingleObject, etc.

### What's NOT Implemented (Developer Responsibility)

- **GPU/Graphics** - VdInitializeEngines, VdSwap, etc. are stubs
- **Audio** - XAudio stubs only
- **Input** - XamInputGetState returns disconnected
- **Network** - Socket stubs only

Developers implementing a port should:
1. Hook the Vd* functions to their rendering backend
2. Implement audio mixing/output
3. Connect input to their platform
4. Handle file I/O for their platform

## File Structure for Static Recomp Runtime

```
rage_runtime/
├── kernel/
│   ├── kernel_state.h/.cc    - Thread/object management
│   ├── xthread.h/.cc         - Thread implementation  
│   ├── xobject.h/.cc         - Base object class
│   ├── xevent.h/.cc          - Event synchronization
│   ├── xmutant.h/.cc         - Mutex synchronization
│   ├── xsemaphore.h/.cc      - Semaphore synchronization
│   └── util/
│       └── object_table.h/.cc - Handle management
├── ppc_context.h             - Register state
├── static_dispatch.h/.cc     - Function lookup/dispatch
└── memory.h/.cc              - Guest memory management

RAGE_GTA_IV/
├── main.cpp                  - Entry point
├── kernel_stubs.cpp          - Xbox kernel function implementations
├── ppc_recomp.*.cpp          - Statically recompiled game code
└── ppc_func_mapping.cpp      - Function address -> pointer table
```

## Key Interfaces

### Function Signature
```cpp
typedef void (*PPCFunc)(PPCContext& ctx, uint8_t* base);
```

### Function Lookup
```cpp
// Game provides this table
extern PPCFunc g_function_table[];
extern size_t g_function_count;

PPCFunc LookupFunction(uint32_t address) {
    // Binary search or direct index based on address layout
    return g_function_table[AddressToIndex(address)];
}
```

### Kernel Call Convention
```cpp
// Kernel functions use same signature
PPC_FUNC(ExCreateThread) {
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t stack_size = ctx.r4.u32;
    // ... implement kernel logic ...
    ctx.r3.u64 = X_STATUS_SUCCESS;  // Return value
}
```

## Building

```bash
# Build kernel library
cd rage_runtime/kernel && make

# Build game with static recomp
cd RAGE_GTA_IV && make real
```

## Testing with LLDB

```bash
lldb ./gta4_recomp
(lldb) breakpoint set -n "XThread::Execute"
(lldb) run
(lldb) bt  # backtrace when stopped
(lldb) p ctx.r3  # print register
```
