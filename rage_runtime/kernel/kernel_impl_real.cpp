/**
 * Real Kernel Implementation - Replaces stub kernel_impl.cpp
 * Uses Xenia-derived kernel objects for actual functionality
 * 
 * Include this file instead of kernel_impl.cpp to use real kernel
 */

#include <cstring>
#include <chrono>
#include <thread>
#include <atomic>
#include <unordered_map>

#include "kernel_state.h"
#include "kernel_exports.h"
#include "xthread.h"
#include "xevent.h"
#include "xmutant.h"
#include "xsemaphore.h"

// Include the game's PPC context
#include "ppc_context.h"

using namespace xe;
using namespace xe::kernel;
using namespace xe::kernel::exports;

// Global kernel state
static KernelState* g_kernel = nullptr;

//=============================================================================
// Kernel Initialization (call from main before game execution)
//=============================================================================
extern "C" void InitializeKernel(uint8_t* memory_base, 
                                  KernelState::FunctionLookupFunc lookup) {
    g_kernel = new KernelState();
    g_kernel->set_memory_base(memory_base);
    g_kernel->set_function_lookup(lookup);
    printf("[KERNEL] Initialized with memory base %p\n", memory_base);
}

extern "C" void ShutdownKernel() {
    delete g_kernel;
    g_kernel = nullptr;
}

extern "C" KernelState* GetKernelState() {
    return g_kernel;
}

//=============================================================================
// Helper Macros
//=============================================================================
#define KERNEL_CHECK() if (!g_kernel) { ctx.r3.u64 = X_STATUS_UNSUCCESSFUL; return; }
#define BASE (g_kernel->memory_base())

//=============================================================================
// Threading
//=============================================================================
PPC_FUNC(__imp__ExCreateThread) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t stack_size = ctx.r4.u32;
    uint32_t thread_id_ptr = ctx.r5.u32;
    uint32_t xapi_startup = ctx.r6.u32;
    uint32_t start_address = ctx.r7.u32;
    uint32_t start_context = ctx.r8.u32;
    uint32_t flags = ctx.r9.u32;
    
    printf("[KERNEL] ExCreateThread(start=0x%08X, ctx=0x%08X, flags=0x%X)\n",
           start_address, start_context, flags);
    
    ctx.r3.u64 = exports::ExCreateThread(g_kernel, BASE, handle_ptr, stack_size,
                                          thread_id_ptr, xapi_startup,
                                          start_address, start_context, flags);
}

PPC_FUNC(__imp__NtResumeThread) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t count_ptr = ctx.r4.u32;
    
    uint32_t count = 0;
    X_STATUS result = exports::NtResumeThread(g_kernel, handle, &count);
    
    if (count_ptr) {
        GuestStore32(BASE, count_ptr, count);
    }
    ctx.r3.u64 = result;
}

PPC_FUNC(__imp__NtSuspendThread) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t count_ptr = ctx.r4.u32;
    
    uint32_t count = 0;
    X_STATUS result = exports::NtSuspendThread(g_kernel, handle, &count);
    
    if (count_ptr) {
        GuestStore32(BASE, count_ptr, count);
    }
    ctx.r3.u64 = result;
}

PPC_FUNC(__imp__ExTerminateThread) {
    KERNEL_CHECK();
    ctx.r3.u64 = exports::ExTerminateThread(g_kernel, ctx.r3.u32);
}

PPC_FUNC(__imp__KeResumeThread) {
    KERNEL_CHECK();
    // KeResumeThread takes thread object pointer, not handle
    // For now, treat as NtResumeThread
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__KeSetBasePriorityThread) {
    ctx.r3.u64 = 0;  // Return previous priority
}

PPC_FUNC(__imp__KeSetAffinityThread) {
    ctx.r3.u64 = 0;  // Return previous affinity
}

PPC_FUNC(__imp__KeSetDisableBoostThread) {
    ctx.r3.u64 = 0;
}

//=============================================================================
// Events
//=============================================================================
PPC_FUNC(__imp__KeInitializeEvent) {
    KERNEL_CHECK();
    uint32_t event_ptr = ctx.r3.u32;
    uint32_t event_type = ctx.r4.u32;
    uint32_t initial_state = ctx.r5.u32;
    
    if (event_ptr && BASE) {
        // Initialize the X_KEVENT structure in guest memory
        // Zero out the dispatch header first
        memset(BASE + event_ptr, 0, sizeof(X_DISPATCH_HEADER));
        
        // Set type and signal state
        auto header = reinterpret_cast<X_DISPATCH_HEADER*>(BASE + event_ptr);
        header->type = event_type;  // 0 = NotificationEvent, 1 = SynchronizationEvent
        header->signal_state = initial_state;
        
        // Create the backing XEvent object
        auto ev = XObject::GetNativeObject<XEvent>(g_kernel, BASE + event_ptr, event_type);
        if (!ev) {
            printf("[KERNEL] KeInitializeEvent: Failed to create event at 0x%08X\n", event_ptr);
        }
    }
}

PPC_FUNC(__imp__NtCreateEvent) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t obj_attrs = ctx.r4.u32;
    uint32_t event_type = ctx.r5.u32;
    uint32_t initial_state = ctx.r6.u32;
    
    ctx.r3.u64 = exports::NtCreateEvent(g_kernel, BASE, handle_ptr, obj_attrs,
                                         event_type, initial_state);
}

PPC_FUNC(__imp__KeSetEvent) {
    KERNEL_CHECK();
    uint32_t event_ptr = ctx.r3.u32;
    uint32_t increment = ctx.r4.u32;
    uint32_t wait = ctx.r5.u32;
    
    ctx.r3.s64 = exports::KeSetEvent(g_kernel, BASE, event_ptr, increment, wait);
}

PPC_FUNC(__imp__KeResetEvent) {
    KERNEL_CHECK();
    ctx.r3.s64 = exports::KeResetEvent(g_kernel, BASE, ctx.r3.u32);
}

PPC_FUNC(__imp__NtSetEvent) {
    KERNEL_CHECK();
    auto ev = g_kernel->object_table()->LookupObject<XEvent>(ctx.r3.u32);
    if (ev) {
        int32_t prev = ev->Set(0, false);
        if (ctx.r4.u32) GuestStore32(BASE, ctx.r4.u32, prev);
        ctx.r3.u64 = X_STATUS_SUCCESS;
    } else {
        ctx.r3.u64 = X_STATUS_INVALID_HANDLE;
    }
}

PPC_FUNC(__imp__NtClearEvent) {
    KERNEL_CHECK();
    auto ev = g_kernel->object_table()->LookupObject<XEvent>(ctx.r3.u32);
    ctx.r3.u64 = ev ? (ev->Reset(), X_STATUS_SUCCESS) : X_STATUS_INVALID_HANDLE;
}

//=============================================================================
// Mutants
//=============================================================================
PPC_FUNC(__imp__NtCreateMutant) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t obj_attrs = ctx.r4.u32;
    uint32_t initial_owner = ctx.r5.u32;
    
    ctx.r3.u64 = exports::NtCreateMutant(g_kernel, BASE, handle_ptr, obj_attrs,
                                          initial_owner);
}

PPC_FUNC(__imp__NtReleaseMutant) {
    KERNEL_CHECK();
    int32_t prev = 0;
    ctx.r3.u64 = exports::NtReleaseMutant(g_kernel, ctx.r3.u32, &prev);
    if (ctx.r4.u32) GuestStore32(BASE, ctx.r4.u32, prev);
}

//=============================================================================
// Semaphores
//=============================================================================
PPC_FUNC(__imp__NtCreateSemaphore) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t obj_attrs = ctx.r4.u32;
    int32_t initial = ctx.r5.s32;
    int32_t maximum = ctx.r6.s32;
    
    ctx.r3.u64 = exports::NtCreateSemaphore(g_kernel, BASE, handle_ptr, obj_attrs,
                                             initial, maximum);
}

PPC_FUNC(__imp__NtReleaseSemaphore) {
    KERNEL_CHECK();
    int32_t prev = 0;
    ctx.r3.u64 = exports::NtReleaseSemaphore(g_kernel, ctx.r3.u32, 
                                              ctx.r4.s32, &prev);
    if (ctx.r5.u32) GuestStore32(BASE, ctx.r5.u32, prev);
}

PPC_FUNC(__imp__KeInitializeSemaphore) {
    KERNEL_CHECK();
    // Initialize in-place semaphore
    uint32_t sem_ptr = ctx.r3.u32;
    int32_t count = ctx.r4.s32;
    int32_t limit = ctx.r5.s32;
    
    if (sem_ptr) {
        auto header = reinterpret_cast<X_DISPATCH_HEADER*>(BASE + sem_ptr);
        header->type = 5;
        header->signal_state = count;
        // Store limit after header
        GuestStore32(BASE, sem_ptr + sizeof(X_DISPATCH_HEADER), limit);
    }
}

PPC_FUNC(__imp__KeReleaseSemaphore) {
    KERNEL_CHECK();
    uint32_t sem_ptr = ctx.r3.u32;
    auto sem = XObject::GetNativeObject<XSemaphore>(g_kernel, BASE + sem_ptr);
    if (sem) {
        ctx.r3.s64 = sem->ReleaseSemaphore(ctx.r4.s32);
    } else {
        ctx.r3.s64 = 0;
    }
}

//=============================================================================
// Wait Functions
//=============================================================================
PPC_FUNC(__imp__KeWaitForSingleObject) {
    KERNEL_CHECK();
    uint32_t obj_ptr = ctx.r3.u32;
    uint32_t reason = ctx.r4.u32;
    uint32_t mode = ctx.r5.u32;
    uint32_t alertable = ctx.r6.u32;
    uint32_t timeout_ptr = ctx.r7.u32;
    
    uint64_t* timeout = timeout_ptr ? reinterpret_cast<uint64_t*>(BASE + timeout_ptr) : nullptr;
    ctx.r3.u64 = exports::KeWaitForSingleObject(g_kernel, BASE, obj_ptr,
                                                 reason, mode, alertable, timeout);
}

PPC_FUNC(__imp__KeWaitForMultipleObjects) {
    KERNEL_CHECK();
    // Simplified - just return success for now
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtWaitForSingleObjectEx) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t mode = ctx.r4.u32;
    uint32_t alertable = ctx.r5.u32;
    uint32_t timeout_ptr = ctx.r6.u32;
    
    uint64_t* timeout = timeout_ptr ? reinterpret_cast<uint64_t*>(BASE + timeout_ptr) : nullptr;
    ctx.r3.u64 = exports::NtWaitForSingleObjectEx(g_kernel, handle, mode,
                                                   alertable, timeout);
}

PPC_FUNC(__imp__NtWaitForMultipleObjectsEx) {
    KERNEL_CHECK();
    uint32_t count = ctx.r3.u32;
    uint32_t handles_ptr = ctx.r4.u32;
    uint32_t wait_type = ctx.r5.u32;
    uint32_t mode = ctx.r6.u32;
    uint32_t alertable = ctx.r7.u32;
    uint32_t timeout_ptr = ctx.r8.u32;
    
    uint64_t* timeout = timeout_ptr ? reinterpret_cast<uint64_t*>(BASE + timeout_ptr) : nullptr;
    ctx.r3.u64 = exports::NtWaitForMultipleObjectsEx(g_kernel, BASE, count,
                                                      handles_ptr, wait_type,
                                                      mode, alertable, timeout);
}

//=============================================================================
// Critical Sections (host-based)
//=============================================================================
PPC_FUNC(__imp__RtlInitializeCriticalSection) {
    uint32_t cs = ctx.r3.u32;
    if (cs && BASE) {
        GuestStore32(BASE, cs + 0x04, 0xFFFFFFFF);  // LockCount = -1
        GuestStore32(BASE, cs + 0x08, 0);            // RecursionCount = 0
        GuestStore32(BASE, cs + 0x0C, 0);            // OwningThread = 0
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__RtlEnterCriticalSection) {
    // Simplified - just succeed (proper impl would need per-CS mutex)
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__RtlLeaveCriticalSection) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__RtlTryEnterCriticalSection) {
    ctx.r3.u64 = 1;  // Always succeed
}

PPC_FUNC(__imp__KeEnterCriticalRegion) {
    // No-op for now
}

PPC_FUNC(__imp__KeLeaveCriticalRegion) {
    // No-op for now
}

//=============================================================================
// Object Management
//=============================================================================
PPC_FUNC(__imp__NtClose) {
    KERNEL_CHECK();
    ctx.r3.u64 = exports::NtClose(g_kernel, ctx.r3.u32);
}

PPC_FUNC(__imp__ObReferenceObjectByHandle) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t obj_type = ctx.r4.u32;
    uint32_t obj_ptr_ptr = ctx.r5.u32;
    
    ctx.r3.u64 = exports::ObReferenceObjectByHandle(g_kernel, BASE, handle,
                                                     obj_type, obj_ptr_ptr);
}

PPC_FUNC(__imp__ObDereferenceObject) {
    // No-op - ref counting handled internally
}

//=============================================================================
// Process/System Info
//=============================================================================
PPC_FUNC(__imp__KeGetCurrentProcessType) {
    ctx.r3.u64 = g_kernel ? exports::KeGetCurrentProcessType(g_kernel) : 1;
}

//=============================================================================
// Memory - Using Xenia's physical memory layout
//=============================================================================
// Physical memory regions (matching Xenia):
// 0xA0000000-0xBFFFFFFF: Physical 64KB pages
// 0xC0000000-0xDFFFFFFF: Physical 16MB pages  
// 0xE0000000-0xFFFFFFFF: Physical 4KB pages
static std::atomic<uint32_t> phys_alloc_64k{0xA0000000};
static std::atomic<uint32_t> phys_alloc_4k{0xE0000000};
static std::atomic<uint32_t> virt_alloc_ptr{0x40000000};

PPC_FUNC(__imp__NtAllocateVirtualMemory) {
    KERNEL_CHECK();
    uint32_t base_ptr = ctx.r3.u32;
    uint32_t size_ptr = ctx.r4.u32;
    uint32_t alloc_type = ctx.r5.u32;
    uint32_t protect = ctx.r6.u32;
    
    uint32_t requested_base = base_ptr ? GuestLoad32(BASE, base_ptr) : 0;
    uint32_t size = size_ptr ? GuestLoad32(BASE, size_ptr) : 0x10000;
    uint32_t aligned_size = (size + 0xFFF) & ~0xFFF;
    
    uint32_t result_addr;
    if (requested_base != 0) {
        // Fixed allocation - just return the requested address
        result_addr = requested_base;
    } else {
        // Allocate new virtual memory
        result_addr = virt_alloc_ptr.fetch_add(aligned_size);
    }
    
    printf("[MEM] NtAllocateVirtualMemory(base=0x%08X, size=0x%X) -> 0x%08X\n", 
           requested_base, size, result_addr);
    
    if (base_ptr) GuestStore32(BASE, base_ptr, result_addr);
    if (size_ptr) GuestStore32(BASE, size_ptr, aligned_size);
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__MmAllocatePhysicalMemoryEx) {
    // r3 = flags, r4 = size, r5 = protect, r6 = min, r7 = max, r8 = alignment
    uint32_t flags = ctx.r3.u32;
    uint32_t size = ctx.r4.u32;
    uint32_t protect = ctx.r5.u32;
    uint32_t min_addr = ctx.r6.u32;
    uint32_t max_addr = ctx.r7.u32;
    uint32_t alignment = ctx.r8.u32;
    
    // Determine page size from protect bits
    uint32_t page_size = 4 * 1024;  // Default 4KB
    if (protect & 0x20000000) {      // X_MEM_LARGE_PAGES
        page_size = 64 * 1024;
    } else if (protect & 0x40000000) { // X_MEM_16MB_PAGES
        page_size = 16 * 1024 * 1024;
    }
    
    uint32_t aligned_size = (size + page_size - 1) & ~(page_size - 1);
    uint32_t align = (alignment > page_size) ? alignment : page_size;
    
    // Allocate from appropriate pool
    uint32_t result;
    if (page_size >= 64 * 1024) {
        result = phys_alloc_64k.fetch_add(aligned_size);
        // Align to requested alignment
        result = (result + align - 1) & ~(align - 1);
        phys_alloc_64k.store(result + aligned_size);
    } else {
        result = phys_alloc_4k.fetch_add(aligned_size);
        result = (result + align - 1) & ~(align - 1);
        phys_alloc_4k.store(result + aligned_size);
    }
    
    printf("[MEM] MmAllocatePhysicalMemoryEx(size=0x%X, align=0x%X) -> 0x%08X\n", 
           size, align, result);
    ctx.r3.u64 = result;
}

PPC_FUNC(__imp__MmAllocatePhysicalMemory) {
    // Simple version: r3 = flags, r4 = size, r5 = protect
    uint32_t size = ctx.r4.u32;
    uint32_t aligned = (size + 0xFFF) & ~0xFFF;
    uint32_t result = phys_alloc_4k.fetch_add(aligned);
    printf("[MEM] MmAllocatePhysicalMemory(size=0x%X) -> 0x%08X\n", size, result);
    ctx.r3.u64 = result;
}

PPC_FUNC(__imp__MmCreateKernelStack) {
    uint32_t stack_size = ctx.r3.u32;
    uint32_t aligned = (stack_size + 0xFFF) & ~0xFFF;
    if (aligned < 0x4000) aligned = 0x4000;  // Min 16KB
    
    // Allocate from 0x70000000 range like Xenia
    static std::atomic<uint32_t> stack_alloc{0x70000000};
    uint32_t stack_base = stack_alloc.fetch_add(aligned);
    
    // Return stack TOP (base + size)
    ctx.r3.u64 = stack_base + aligned;
    printf("[MEM] MmCreateKernelStack(size=0x%X) -> 0x%08X\n", stack_size, stack_base + aligned);
}

PPC_FUNC(__imp__MmDeleteKernelStack) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__NtFreeVirtualMemory) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__MmFreePhysicalMemory) { }

PPC_FUNC(__imp__MmGetPhysicalAddress) {
    // Virtual to physical - in our case, identity mapping for physical range
    uint32_t virt = ctx.r3.u32;
    // For addresses in physical range, return as-is
    // For virtual addresses, offset from physical base
    uint32_t phys = virt;
    if (virt >= 0x40000000 && virt < 0x70000000) {
        phys = 0xE0000000 + (virt - 0x40000000);
    }
    ctx.r3.u64 = phys;
}

PPC_FUNC(__imp__MmQueryAddressProtect) { ctx.r3.u64 = 0x04; }  // PAGE_READWRITE
PPC_FUNC(__imp__MmQueryAllocationSize) { ctx.r3.u64 = 0x10000; }

PPC_FUNC(__imp__MmQueryStatistics) {
    uint32_t stats_ptr = ctx.r3.u32;
    if (stats_ptr && BASE) {
        // Fill with reasonable values (512MB system)
        GuestStore32(BASE, stats_ptr + 0x04, 0x20000);  // total_physical_pages (512MB/4KB)
        GuestStore32(BASE, stats_ptr + 0x08, 0x300);    // kernel_pages
        // Title section at offset 0x0C
        GuestStore32(BASE, stats_ptr + 0x0C, 0x1F000);  // available_pages
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtQueryVirtualMemory) { ctx.r3.u64 = X_STATUS_SUCCESS; }

//=============================================================================
// Time
//=============================================================================
PPC_FUNC(__imp__KeQueryPerformanceFrequency) {
    ctx.r3.u64 = 50000000;  // 50MHz
}

PPC_FUNC(__imp__KeQuerySystemTime) {
    uint32_t time_ptr = ctx.r3.u32;
    if (time_ptr && BASE) {
        auto now = std::chrono::system_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()).count();
        uint64_t ft = ns / 100 + 116444736000000000ULL;
        GuestStore64(BASE, time_ptr, ft);
    }
}

PPC_FUNC(__imp__KeDelayExecutionThread) {
    uint32_t interval_ptr = ctx.r5.u32;
    if (interval_ptr && BASE) {
        int64_t interval = static_cast<int64_t>(GuestLoad64(BASE, interval_ptr));
        if (interval < 0) {
            int64_t ms = (-interval) / 10000;
            if (ms > 0 && ms < 10000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            }
        }
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtYieldExecution) {
    std::this_thread::yield();
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

//=============================================================================
// Spinlocks (simplified)
//=============================================================================
PPC_FUNC(__imp__KfAcquireSpinLock) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfReleaseSpinLock) { }
PPC_FUNC(__imp__KeAcquireSpinLockAtRaisedIrql) { }
PPC_FUNC(__imp__KeReleaseSpinLockFromRaisedIrql) { }
PPC_FUNC(__imp__KeTryToAcquireSpinLockAtRaisedIrql) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__KeRaiseIrqlToDpcLevel) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfLowerIrql) { }
PPC_FUNC(__imp__KeLockL2) { }
PPC_FUNC(__imp__KeUnlockL2) { }

//=============================================================================
// TLS
//=============================================================================
PPC_FUNC(__imp__KeTlsAlloc) {
    KERNEL_CHECK();
    ctx.r3.u64 = g_kernel->AllocateTLS();
}

PPC_FUNC(__imp__KeTlsFree) {
    KERNEL_CHECK();
    g_kernel->FreeTLS(ctx.r3.u32);
    ctx.r3.u64 = 1;
}

// TLS values stored per-thread - simplified using static map for now
static thread_local std::unordered_map<uint32_t, uint64_t> tls_values;

PPC_FUNC(__imp__KeTlsGetValue) {
    uint32_t slot = ctx.r3.u32;
    auto it = tls_values.find(slot);
    ctx.r3.u64 = (it != tls_values.end()) ? it->second : 0;
}

PPC_FUNC(__imp__KeTlsSetValue) {
    tls_values[ctx.r3.u32] = ctx.r4.u64;
    ctx.r3.u64 = 1;
}

//=============================================================================
// Timers
//=============================================================================
PPC_FUNC(__imp__NtCreateTimer) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    static std::atomic<uint32_t> timer_handle_counter{0};
    uint32_t handle = 0x80000000 + (timer_handle_counter.fetch_add(1) & 0xFFFF);
    if (handle_ptr) GuestStore32(BASE, handle_ptr, handle);
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtSetTimerEx) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__NtCancelTimer) { ctx.r3.u64 = X_STATUS_SUCCESS; }

//=============================================================================
// Debug
//=============================================================================
PPC_FUNC(__imp__DbgPrint) {
    uint32_t fmt_ptr = ctx.r3.u32;
    if (fmt_ptr && BASE) {
        printf("[GAME] %s", reinterpret_cast<char*>(BASE + fmt_ptr));
    }
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__DbgBreakPoint) { }
PPC_FUNC(__imp__KeBugCheck) { printf("[KERNEL] BugCheck!\n"); std::abort(); }
PPC_FUNC(__imp__KeBugCheckEx) { printf("[KERNEL] BugCheckEx!\n"); std::abort(); }

//=============================================================================
// RTL Strings (keep existing implementations)
//=============================================================================
PPC_FUNC(__imp__RtlInitAnsiString) {
    uint32_t dest = ctx.r3.u32;
    uint32_t src = ctx.r4.u32;
    if (dest && src && BASE) {
        const char* s = reinterpret_cast<const char*>(BASE + src);
        uint16_t len = static_cast<uint16_t>(strlen(s));
        *reinterpret_cast<uint16_t*>(BASE + dest) = __builtin_bswap16(len);
        *reinterpret_cast<uint16_t*>(BASE + dest + 2) = __builtin_bswap16(len + 1);
        GuestStore32(BASE, dest + 4, src);
    }
}

// Include remaining stubs for completeness...
#define STUB(name) PPC_FUNC(__imp__##name) { printf("[STUB] " #name "\n"); ctx.r3.u64 = 0; }
#define STUB_RET(name, ret) PPC_FUNC(__imp__##name) { ctx.r3.u64 = (ret); }

// RTL
STUB(RtlMultiByteToUnicodeN)
STUB(RtlNtStatusToDosError)
STUB(RtlCompareMemoryUlong)
STUB(RtlFillMemoryUlong)
STUB(RtlCaptureContext)
STUB(RtlRaiseException)
STUB(RtlUnwind)
STUB(__C_specific_handler)
STUB(KiApcNormalRoutineNop)
STUB(RtlCompareStringN)
STUB(RtlFreeAnsiString)
STUB(RtlInitUnicodeString)
STUB(RtlTimeFieldsToTime)
STUB(RtlTimeToTimeFields)
STUB(RtlUnicodeStringToAnsiString)
STUB(RtlUnicodeToMultiByteN)
STUB(RtlUpcaseUnicodeChar)

// File I/O (stubs - would connect to VFS)
STUB(NtCreateFile)
STUB(NtOpenFile)
STUB(NtReadFile)
STUB(NtWriteFile)
STUB(NtQueryInformationFile)
STUB(NtSetInformationFile)
STUB_RET(NtQueryDirectoryFile, 0xC000000F)
STUB_RET(NtQueryFullAttributesFile, 0xC000000F)
STUB(NtQueryVolumeInformationFile)
STUB(NtFlushBuffersFile)
STUB(NtDuplicateObject)

// Object manager extras
STUB(ObOpenObjectByPointer)
STUB(ObLookupThreadByThreadId)
STUB(ObCreateSymbolicLink)
STUB(ObDeleteSymbolicLink)
STUB(ObIsTitleObject)
STUB(ObReferenceObject)

// Config
STUB(ExGetXConfigSetting)
STUB(ExRegisterTitleTerminateNotification)
STUB(HalReturnToFirmware)
STUB(RtlImageXexHeaderField)
STUB_RET(XexCheckExecutablePrivilege, 1)
STUB(XexGetModuleHandle)
STUB(XexGetProcedureAddress)

// I/O
STUB(IoCheckShareAccess)
STUB(IoCompleteRequest)
STUB(IoCreateDevice)
STUB(IoDeleteDevice)
STUB(IoDismountVolume)
STUB(IoDismountVolumeByFileHandle)
STUB(IoInvalidDeviceRequest)
STUB(IoRemoveShareAccess)
STUB(IoSetShareAccess)
STUB(StfsControlDevice)
STUB(StfsCreateDevice)
STUB(NtDeviceIoControlFile)
STUB(NtReadFileScatter)

// Video - Using Xenia's actual return values
PPC_FUNC(__imp__VdInitializeEngines) {
    // r3 = 0x4F810000, r4 = callback, r5 = arg, r6 = PFP, r7 = ME
    printf("[VIDEO] VdInitializeEngines()\n");
    ctx.r3.u64 = 1;  // Success - must return 1
}

PPC_FUNC(__imp__VdGetSystemCommandBuffer) {
    uint32_t p0_ptr = ctx.r3.u32;
    uint32_t p1_ptr = ctx.r4.u32;
    printf("[VIDEO] VdGetSystemCommandBuffer(p0=%08X, p1=%08X)\n", p0_ptr, p1_ptr);
    if (p0_ptr && BASE) {
        memset(BASE + p0_ptr, 0, 0x94);
        GuestStore32(BASE, p0_ptr, 0xBEEF0000);
    }
    if (p1_ptr && BASE) {
        GuestStore32(BASE, p1_ptr, 0xBEEF0001);
    }
}

PPC_FUNC(__imp__VdQueryVideoMode) {
    uint32_t mode_ptr = ctx.r3.u32;
    printf("[VIDEO] VdQueryVideoMode(%08X)\n", mode_ptr);
    if (mode_ptr && BASE) {
        memset(BASE + mode_ptr, 0, 0x8C);
        GuestStore32(BASE, mode_ptr + 0x00, 1280);     // display_width
        GuestStore32(BASE, mode_ptr + 0x04, 720);      // display_height
        GuestStore32(BASE, mode_ptr + 0x08, 0);        // is_interlaced
        GuestStore32(BASE, mode_ptr + 0x0C, 1);        // is_widescreen
        GuestStore32(BASE, mode_ptr + 0x10, 1);        // is_hi_def
        // refresh_rate at 0x14 (float 60.0)
        uint32_t refresh = 0x42700000;  // 60.0f in IEEE 754
        GuestStore32(BASE, mode_ptr + 0x14, refresh);
        GuestStore32(BASE, mode_ptr + 0x18, 1);        // video_standard = NTSC
    }
}

PPC_FUNC(__imp__VdQueryVideoFlags) {
    // Return widescreen + HD flags
    ctx.r3.u64 = 0x7;  // widescreen(1) | 1024+(2) | 1920+(4)
}

PPC_FUNC(__imp__VdGetGraphicsAsicID) {
    // Games compare for < 0x10 and do VdInitializeEDRAM, else other
    ctx.r3.u64 = 0x11;
}

PPC_FUNC(__imp__VdIsHSIOTrainingSucceeded) {
    ctx.r3.u64 = 1;  // Must return TRUE
}

PPC_FUNC(__imp__VdSetGraphicsInterruptCallback) {
    printf("[VIDEO] VdSetGraphicsInterruptCallback(cb=%08X, data=%08X)\n",
           ctx.r3.u32, ctx.r4.u32);
    // Store callback for later use if needed
}

PPC_FUNC(__imp__VdInitializeRingBuffer) {
    printf("[VIDEO] VdInitializeRingBuffer(ptr=%08X, log2=%d)\n",
           ctx.r3.u32, ctx.r4.s32);
}

PPC_FUNC(__imp__VdEnableRingBufferRPtrWriteBack) {
    printf("[VIDEO] VdEnableRingBufferRPtrWriteBack(ptr=%08X, log2=%d)\n",
           ctx.r3.u32, ctx.r4.s32);
}

PPC_FUNC(__imp__VdInitializeScalerCommandBuffer) {
    uint32_t dest_ptr = ctx.r3.u32 + 40;  // Approximate offset
    uint32_t dest_count = ctx.r4.u32;
    printf("[VIDEO] VdInitializeScalerCommandBuffer()\n");
    // Fill with NOP packets (0x80000000)
    if (dest_ptr && dest_count && BASE) {
        for (uint32_t i = 0; i < dest_count && i < 64; i++) {
            GuestStore32(BASE, dest_ptr + i * 4, 0x80000000);
        }
    }
    ctx.r3.u64 = dest_count > 0 ? dest_count : 64;
}

PPC_FUNC(__imp__VdPersistDisplay) {
    uint32_t unk1_ptr = ctx.r4.u32;
    if (unk1_ptr && BASE) {
        // Allocate small block for cleanup
        uint32_t addr = phys_alloc_4k.fetch_add(64);
        GuestStore32(BASE, unk1_ptr, addr);
    }
    ctx.r3.u64 = 1;
}

PPC_FUNC(__imp__VdGetCurrentDisplayGamma) {
    uint32_t type_ptr = ctx.r3.u32;
    uint32_t power_ptr = ctx.r4.u32;
    if (type_ptr && BASE) GuestStore32(BASE, type_ptr, 2);  // BT.709
    if (power_ptr && BASE) {
        uint32_t gamma = 0x400E38E4;  // ~2.22222f
        GuestStore32(BASE, power_ptr, gamma);
    }
}

PPC_FUNC(__imp__VdGetCurrentDisplayInformation) {
    uint32_t info_ptr = ctx.r3.u32;
    if (info_ptr && BASE) {
        memset(BASE + info_ptr, 0, 0x58);
        // front_buffer dimensions
        GuestStore16(BASE, info_ptr + 0x00, 1280);
        GuestStore16(BASE, info_ptr + 0x02, 720);
        // display dimensions
        GuestStore16(BASE, info_ptr + 0x48, 1280);
        GuestStore16(BASE, info_ptr + 0x4A, 720);
    }
}

STUB(VdCallGraphicsNotificationRoutines)
STUB(VdEnableDisableClockGating)
STUB(VdRetrainEDRAM)
STUB(VdRetrainEDRAMWorker)
STUB(VdSetDisplayMode)
STUB(VdSetSystemCommandBufferGpuIdentifierAddress)
STUB(VdShutdownEngines)
STUB(VdSwap)
STUB(XGetVideoMode)
STUB_RET(XGetAVPack, 6)
STUB_RET(XGetGameRegion, 0xFF)
STUB_RET(XGetLanguage, 1)

// Audio
STUB(XAudioGetSpeakerConfig)
STUB(XAudioGetVoiceCategoryVolume)
STUB(XAudioGetVoiceCategoryVolumeChangeMask)
STUB(XAudioRegisterRenderDriverClient)
STUB(XAudioSubmitRenderDriverFrame)
STUB(XAudioUnregisterRenderDriverClient)
STUB(XMACreateContext)
STUB(XMAReleaseContext)

// XAM
STUB(XamAlloc)
STUB(XamFree)
STUB(XamContentClose)
STUB(XamContentCreateEnumerator)
STUB(XamContentCreateEx)
STUB(XamContentGetCreator)
STUB(XamContentGetDeviceData)
STUB(XamContentSetThumbnail)
STUB(XamCreateEnumeratorHandle)
STUB(XamEnableInactivityProcessing)
STUB(XamEnumerate)
STUB(XamGetExecutionId)
STUB(XamGetPrivateEnumStructureFromHandle)
STUB(XamGetSystemVersion)
STUB(XamInputGetCapabilities)
STUB(XamInputGetKeystrokeEx)
STUB(XamInputGetState)
STUB(XamInputSetState)
STUB(XamLoaderLaunchTitle)
STUB(XamLoaderTerminateTitle)
STUB(XamNotifyCreateListener)
STUB(XamResetInactivity)
STUB(XamSessionCreateHandle)
STUB(XamSessionRefObjByHandle)
STUB(XamShowDeviceSelectorUI)
STUB(XamShowDirtyDiscErrorUI)
STUB(XamShowGamerCardUIForXUID)
STUB(XamShowMessageBoxUIEx)
STUB(XamShowPlayerReviewUI)
STUB(XamShowSigninUI)
STUB(XamTaskCloseHandle)
STUB(XamTaskSchedule)
STUB(XamTaskShouldExit)
STUB(XamUserAreUsersFriends)
STUB(XamUserCheckPrivilege)
STUB(XamUserCreateAchievementEnumerator)
STUB(XamUserCreateStatsEnumerator)
STUB(XamUserGetName)
STUB(XamUserGetSigninInfo)
STUB_RET(XamUserGetSigninState, 1)
STUB(XamUserGetXUID)
STUB(XamUserReadProfileSettings)
STUB(XamUserWriteProfileSettings)
STUB(XamVoiceClose)
STUB(XamVoiceCreate)
STUB(XamVoiceHeadsetPresent)
STUB(XamVoiceSubmitPacket)

// Networking
STUB(NetDll_WSACleanup)
STUB(NetDll_WSAGetLastError)
STUB(NetDll_WSAStartup)
STUB(NetDll_XNetCleanup)
STUB(NetDll_XNetGetConnectStatus)
STUB(NetDll_XNetGetEthernetLinkStatus)
STUB(NetDll_XNetGetTitleXnAddr)
STUB(NetDll_XNetQosListen)
STUB(NetDll_XNetQosLookup)
STUB(NetDll_XNetQosRelease)
STUB(NetDll_XNetServerToInAddr)
STUB(NetDll_XNetStartup)
STUB(NetDll_XNetUnregisterInAddr)
STUB(NetDll_XNetXnAddrToInAddr)
STUB(NetDll_accept)
STUB(NetDll_bind)
STUB(NetDll_closesocket)
STUB(NetDll_connect)
STUB(NetDll_getsockname)
STUB(NetDll_inet_addr)
STUB(NetDll_ioctlsocket)
STUB(NetDll_listen)
STUB(NetDll_recv)
STUB(NetDll_recvfrom)
STUB(NetDll_select)
STUB(NetDll_send)
STUB(NetDll_sendto)
STUB(NetDll_setsockopt)
STUB(NetDll_shutdown)
STUB(NetDll_socket)

// Crypto
STUB(XeCryptSha)
STUB(XeKeysConsolePrivateKeySign)
STUB(XeKeysConsoleSignatureVerification)

// Msg/Notify
STUB(XMsgCancelIORequest)
STUB(XMsgInProcessCall)
STUB(XMsgStartIORequest)
STUB(XMsgStartIORequestEx)
STUB(XNotifyGetNext)
STUB(XNotifyPositionUI)

// Memory pools
STUB(ExAllocatePoolTypeWithTag)
STUB(ExFreePool)

// sprintf (simplified)
PPC_FUNC(__imp__sprintf) {
    uint32_t buf = ctx.r3.u32;
    uint32_t fmt = ctx.r4.u32;
    if (buf && fmt && BASE) {
        strcpy(reinterpret_cast<char*>(BASE + buf), 
               reinterpret_cast<char*>(BASE + fmt));
        ctx.r3.u64 = strlen(reinterpret_cast<char*>(BASE + fmt));
    } else {
        ctx.r3.u64 = 0;
    }
}

PPC_FUNC(__imp___vsnprintf) {
    uint32_t buf = ctx.r3.u32;
    uint32_t size = ctx.r4.u32;
    uint32_t fmt = ctx.r5.u32;
    if (buf && fmt && size > 0 && BASE) {
        strncpy(reinterpret_cast<char*>(BASE + buf),
                reinterpret_cast<char*>(BASE + fmt), size - 1);
        BASE[buf + size - 1] = 0;
        ctx.r3.u64 = strlen(reinterpret_cast<char*>(BASE + buf));
    } else {
        ctx.r3.u64 = 0;
    }
}
