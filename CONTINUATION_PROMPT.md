# GTA IV Static Recompilation - Continuation Prompt

## Latest Session Summary (January 2026)

### What Was Fixed This Session

1. **Fixed Running Thread Counter** - The old `WaitForThreads()` was waiting for `thread_count()` which tracked **registered** threads (never decremented until destructor). Now properly tracks **running** threads via `running_thread_count_` atomic counter.

2. **Fixed XamAlloc** - Was returning 0 (failure). Now properly allocates memory from a dedicated region.

3. **Added Tracing** - Increased kernel call trace limit to 500, added tracing to spinlock/IRQL functions.

### Current State (VERIFIED WORKING)
```
[KERNEL] Thread started (running=1)
[XTHREAD] Thread 4096 starting at 0x829B08E0
[KERNEL] Thread started (running=2)  
[XTHREAD] Thread 4097 starting at 0x829B08E0
```

GTA IV now:
- ✅ Passes initial video mode check
- ✅ Creates 2 worker threads (IDs 4096, 4097)
- ✅ **Threads stay running** (process killed by timeout, not exit)
- ✅ Running thread count correctly shows 2
- ✅ Makes ~33 kernel calls during initialization
- ❌ Threads stuck in recompiled game code (no further kernel calls)

### THE CURRENT PROBLEM

**Threads Stuck in Game Code Loops:**

After ~33 kernel calls, all threads (main + 2 workers) are running but not making kernel calls. They're stuck in recompiled game code that doesn't call kernel functions.

Possible causes:
1. **Worker threads waiting for jobs** - 0x829B08E0 is likely a job queue worker that spins waiting for work
2. **Main thread in init loop** - May be waiting for GPU/video initialization
3. **Missing interrupt/callback mechanism** - Xbox 360 games use DPCs and interrupts

### Key Files Modified

**`rage_runtime/kernel/kernel_state.h`** - Added:
```cpp
void OnThreadStarted();
void OnThreadFinished();
size_t running_thread_count() const;
std::atomic<size_t> running_thread_count_{0};
```

**`rage_runtime/kernel/kernel_state.cc`** - Added running thread tracking:
```cpp
void KernelState::OnThreadStarted() {
    size_t count = running_thread_count_.fetch_add(1) + 1;
    printf("[KERNEL] Thread started (running=%zu)\n", count);
}

void KernelState::OnThreadFinished() {
    size_t count = running_thread_count_.fetch_sub(1) - 1;
    printf("[KERNEL] Thread finished (running=%zu)\n", count);
}
```

**`rage_runtime/kernel/xthread.cc`** - Calls OnThreadStarted/Finished in ExecuteEntry()

**`RAGE_GTA_IV/kernel_impl_xenia.cpp`** - Fixed XamAlloc, added more tracing

### Next Steps to Try

1. **Implement DPC/Timer mechanism** - Game may need timer callbacks to make progress
2. **Check VdSwap/VSync** - Game might be waiting for video frame completion
3. **Add job queue tracing** - Understand what worker threads at 0x829B08E0 are waiting for
4. **Implement XMsgStartIORequest** - Called 22 times, might need proper async I/O

### Build & Test Commands

```bash
cd /Users/Ozordi/Downloads/xenia/xenia/Recompiled\ Samples/RAGE_GTA_IV

# Rebuild everything
make -C ../rage_runtime/kernel clean && make -C ../rage_runtime/kernel -j4
rm -f main.o kernel_impl_xenia.o gta4_recomp && make real -j4

# Run with timeout (game should run until killed)
timeout 5 ./gta4_recomp 2>&1 | grep -E "(KCALL|Thread|KERNEL|EXECUTE)"
```

### Kernel Call Trace (First 33 Calls)
```
[KCALL 0-1] RtlEnterCriticalSection x2
[KCALL 2-3] ExCreateThread x2 (creates worker threads)
[KCALL 4-5] RtlEnterCriticalSection x2
[KCALL 6-7] KeTlsGetValue, KeTlsSetValue
[KCALL 8-13] RtlEnterCriticalSection + XGetVideoMode x2
[KCALL 14-27] RtlEnterCriticalSection, MmAllocatePhysicalMemoryEx x3, RtlInitializeCriticalSection
[KCALL 28-32] RtlEnterCriticalSection x5
--- No more kernel calls after this ---
```

### Repository
- GitHub: https://github.com/OZORDI/XenonRunner
- Branch: master

---

## Latest Update (January 2026 - Session 2)

### Fresh Xenia Study
Cloned fresh Xenia to `/tmp/xenia-fresh` and studied:
- `src/xenia/kernel/kernel_state.cc` - Thread/object management
- `src/xenia/kernel/xthread.cc` - Thread execution flow
- `src/xenia/cpu/processor.h` - Execute() interface
- `src/xenia/cpu/ppc/ppc_context.h` - Register state

Key insight: Xenia's `XThread::Execute()` calls `processor()->Execute(thread_state, address, args)` which runs JIT code. For static recomp, this becomes `StaticDispatch(address, context, memory_base)`.

### Created Documentation
- `STATIC_RECOMP_ARCHITECTURE.md` - Full architecture document for static recompilation runtime

### Fixes Applied
1. VdSwap frame limiter (16ms sleep to simulate vsync)
2. Running thread counter (properly tracks executing threads)
3. XamAlloc (now allocates memory instead of returning 0)

### Current Issue
Game stuck at ~32 kernel calls during initialization - never reaches main loop or VdSwap.

**Debug with lldb:**
```bash
cd /Users/Ozordi/Downloads/xenia/xenia/Recompiled\ Samples/RAGE_GTA_IV
lldb ./gta4_recomp
(lldb) breakpoint set -n "xe::kernel::XThread::ExecuteEntry"
(lldb) run
# When stopped, use 'bt' for backtrace, 'thread list' for all threads
```

### Key Principle (from user)
**GPU/rendering is NOT implemented** - that's for game developers to do themselves. This is a static recompiler runtime, not an emulator.
