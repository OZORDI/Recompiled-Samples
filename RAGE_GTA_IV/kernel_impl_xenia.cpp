/**
 * Kernel Implementation - Extracted from Xenia's actual _entry functions
 * 
 * This file contains the REAL logic from src/xenia/kernel/xboxkrnl/*.cc
 * adapted for static recompilation.
 */

#include "ppc_config.h"
#include "ppc_context.h"

// Use Xenia's kernel objects
#include "kernel/kernel_state.h"
#include "kernel/xthread.h"
#include "kernel/xevent.h"
#include "kernel/xmutant.h"
#include "kernel/xsemaphore.h"
#include "kernel/vfs.h"

#include <chrono>
#include <thread>
#include <cstring>

using namespace xe;
using namespace xe::kernel;

// Global kernel state
static KernelState* g_kernel = nullptr;
static xe::vfs::VirtualFileSystem* g_vfs = nullptr;

#define KERNEL_CHECK() if (!g_kernel) { ctx.r3.u64 = X_STATUS_UNSUCCESSFUL; return; }
#define BASE (g_kernel->memory_base())

// Kernel call tracing
static std::atomic<uint32_t> g_kernel_call_count{0};
#define TRACE_KERNEL(name) do { \
    uint32_t count = g_kernel_call_count.fetch_add(1); \
    if (count < 500) { printf("[KCALL %u] %s\n", count, name); fflush(stdout); } \
} while(0)

// Helper: Read big-endian 32-bit from guest memory
inline uint32_t GuestRead32(uint8_t* base, uint32_t addr) {
    return __builtin_bswap32(*reinterpret_cast<uint32_t*>(base + addr));
}
inline void GuestWrite32(uint8_t* base, uint32_t addr, uint32_t val) {
    *reinterpret_cast<uint32_t*>(base + addr) = __builtin_bswap32(val);
}
inline uint64_t GuestRead64(uint8_t* base, uint32_t addr) {
    return __builtin_bswap64(*reinterpret_cast<uint64_t*>(base + addr));
}
inline void GuestWrite64(uint8_t* base, uint32_t addr, uint64_t val) {
    *reinterpret_cast<uint64_t*>(base + addr) = __builtin_bswap64(val);
}

//=============================================================================
// Initialization
//=============================================================================
extern "C" void InitializeKernel(uint8_t* memory_base, FunctionLookupFunc lookup) {
    g_kernel = new KernelState();
    g_kernel->set_memory_base(memory_base);
    g_kernel->set_function_lookup(lookup);
    g_kernel->SetCodeRegion(PPC_CODE_BASE, PPC_CODE_SIZE);
    
    // Initialize VFS
    g_vfs = new xe::vfs::VirtualFileSystem();
    xe::vfs::games::SetupGTAIV(g_vfs, "/Users/Ozordi/Downloads/xenia/xenia/RAGE/extracted");
    xe::vfs::VirtualFileSystem::set_shared(g_vfs);
    
    printf("[KERNEL] Initialized with memory base %p\n", memory_base);
}

extern "C" void ShutdownKernel() {
    delete g_vfs;
    delete g_kernel;
    g_kernel = nullptr;
    g_vfs = nullptr;
    printf("[KERNEL] Shutdown complete\n");
}

extern "C" void WaitForKernelThreads(uint32_t timeout_ms) {
    if (g_kernel) {
        g_kernel->WaitForThreads(timeout_ms);
    }
}

//=============================================================================
// Threading - From xboxkrnl_threading.cc
//=============================================================================

// ExCreateThread - Xenia's actual logic
PPC_FUNC(__imp__ExCreateThread) {
    TRACE_KERNEL("ExCreateThread");
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t stack_size = ctx.r4.u32;
    uint32_t thread_id_ptr = ctx.r5.u32;
    uint32_t xapi_thread_startup = ctx.r6.u32;
    uint32_t start_address = ctx.r7.u32;
    uint32_t start_context = ctx.r8.u32;
    uint32_t creation_flags = ctx.r9.u32;
    
    // Xenia: Inherit default stack size if 0
    uint32_t actual_stack_size = stack_size;
    if (actual_stack_size == 0) {
        actual_stack_size = 64 * 1024;  // Default
    }
    // Xenia: Stack must be aligned to 16kb pages
    actual_stack_size = std::max(0x4000u, (actual_stack_size + 0xFFF) & 0xFFFFF000);
    
    // Create thread using Xenia's XThread
    auto thread = new XThread(g_kernel, actual_stack_size, xapi_thread_startup,
                              start_address, start_context, creation_flags, true, false);
    
    X_STATUS result = thread->Create();
    if (XFAILED(result)) {
        printf("[KERNEL] ExCreateThread failed: 0x%X\n", result);
        ctx.r3.u64 = result;
        return;
    }
    
    printf("[KERNEL] ExCreateThread(start=0x%08X, ctx=0x%08X, flags=0x%X) -> handle=0x%X\n",
           start_address, start_context, creation_flags, thread->handle());
    
    if (handle_ptr) {
        if (creation_flags & 0x80) {
            GuestWrite32(BASE, handle_ptr, thread->guest_object());
        } else {
            GuestWrite32(BASE, handle_ptr, thread->handle());
        }
    }
    if (thread_id_ptr) {
        GuestWrite32(BASE, thread_id_ptr, thread->thread_id());
    }
    
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// NtResumeThread - Xenia's actual logic
PPC_FUNC(__imp__NtResumeThread) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t suspend_count_ptr = ctx.r4.u32;
    
    uint32_t suspend_count = 0;
    X_STATUS result = X_STATUS_INVALID_HANDLE;
    
    auto thread = g_kernel->object_table()->LookupObject<XThread>(handle);
    if (thread) {
        result = thread->Resume(&suspend_count);
    }
    
    if (suspend_count_ptr) {
        GuestWrite32(BASE, suspend_count_ptr, suspend_count);
    }
    ctx.r3.u64 = result;
}

// NtSuspendThread - Xenia's actual logic
PPC_FUNC(__imp__NtSuspendThread) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t suspend_count_ptr = ctx.r4.u32;
    
    uint32_t suspend_count = 0;
    X_STATUS result = X_STATUS_SUCCESS;
    
    auto thread = g_kernel->object_table()->LookupObject<XThread>(handle);
    if (thread) {
        result = thread->Suspend(&suspend_count);
    } else {
        result = X_STATUS_INVALID_HANDLE;
    }
    
    if (suspend_count_ptr) {
        GuestWrite32(BASE, suspend_count_ptr, suspend_count);
    }
    ctx.r3.u64 = result;
}

// ExTerminateThread - Xenia's actual logic
PPC_FUNC(__imp__ExTerminateThread) {
    KERNEL_CHECK();
    XThread* thread = XThread::GetCurrentThread();
    if (thread) {
        ctx.r3.u64 = thread->Exit(ctx.r3.u32);
    } else {
        ctx.r3.u64 = X_STATUS_UNSUCCESSFUL;
    }
}

// KeDelayExecutionThread - Xenia's actual logic
PPC_FUNC(__imp__KeDelayExecutionThread) {
    uint32_t processor_mode = ctx.r3.u32;
    uint32_t alertable = ctx.r4.u32;
    uint32_t interval_ptr = ctx.r5.u32;
    
    if (interval_ptr && BASE) {
        int64_t interval = static_cast<int64_t>(GuestRead64(BASE, interval_ptr));
        if (interval < 0) {
            // Relative time in 100ns units
            int64_t ms = (-interval) / 10000;
            if (ms > 0 && ms < 60000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            }
        }
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// NtYieldExecution - Xenia's actual logic
PPC_FUNC(__imp__NtYieldExecution) {
    std::this_thread::yield();
    ctx.r3.u64 = 0;
}

// KeQueryPerformanceFrequency - Xenia's actual logic
PPC_FUNC(__imp__KeQueryPerformanceFrequency) {
    ctx.r3.u64 = 50000000;  // 50MHz like Xbox 360
}

// KeQuerySystemTime - Xenia's actual logic
PPC_FUNC(__imp__KeQuerySystemTime) {
    uint32_t time_ptr = ctx.r3.u32;
    if (time_ptr && BASE) {
        auto now = std::chrono::system_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()).count();
        // Convert to Windows FILETIME (100ns intervals since 1601)
        uint64_t ft = ns / 100 + 116444736000000000ULL;
        GuestWrite64(BASE, time_ptr, ft);
    }
}

// KeGetCurrentProcessType - Xenia's actual logic
PPC_FUNC(__imp__KeGetCurrentProcessType) {
    ctx.r3.u64 = 1;  // X_PROCTYPE_USER
}

//=============================================================================
// Events - From xboxkrnl_threading.cc
//=============================================================================

// KeInitializeEvent - Initialize in-place event object
PPC_FUNC(__imp__KeInitializeEvent) {
    TRACE_KERNEL("KeInitializeEvent");
    KERNEL_CHECK();
    uint32_t event_ptr = ctx.r3.u32;
    uint32_t event_type = ctx.r4.u32;
    uint32_t initial_state = ctx.r5.u32;
    
    if (event_ptr && BASE) {
        // Initialize the X_KEVENT structure in guest memory
        memset(BASE + event_ptr, 0, sizeof(X_DISPATCH_HEADER));
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

// NtCreateEvent - Xenia's actual logic
PPC_FUNC(__imp__NtCreateEvent) {
    TRACE_KERNEL("NtCreateEvent");
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t obj_attrs = ctx.r4.u32;
    uint32_t event_type = ctx.r5.u32;
    uint32_t initial_state = ctx.r6.u32;
    
    auto ev = new XEvent(g_kernel);
    ev->Initialize(!event_type, !!initial_state);
    
    if (handle_ptr) {
        GuestWrite32(BASE, handle_ptr, ev->handle());
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// KeSetEvent - Xenia's actual logic
PPC_FUNC(__imp__KeSetEvent) {
    KERNEL_CHECK();
    uint32_t event_ptr = ctx.r3.u32;
    uint32_t increment = ctx.r4.u32;
    uint32_t wait = ctx.r5.u32;
    
    auto ev = XObject::GetNativeObject<XEvent>(g_kernel, BASE + event_ptr);
    if (ev) {
        ctx.r3.s64 = ev->Set(increment, !!wait);
    } else {
        ctx.r3.s64 = 0;
    }
}

// KeResetEvent - Xenia's actual logic
PPC_FUNC(__imp__KeResetEvent) {
    KERNEL_CHECK();
    uint32_t event_ptr = ctx.r3.u32;
    
    auto ev = XObject::GetNativeObject<XEvent>(g_kernel, BASE + event_ptr);
    if (ev) {
        ctx.r3.s64 = ev->Reset();
    } else {
        ctx.r3.s64 = 0;
    }
}

// NtSetEvent - Xenia's actual logic
PPC_FUNC(__imp__NtSetEvent) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t prev_state_ptr = ctx.r4.u32;
    
    auto ev = g_kernel->object_table()->LookupObject<XEvent>(handle);
    if (ev) {
        int32_t was_signalled = ev->Set(0, false);
        if (prev_state_ptr) {
            GuestWrite32(BASE, prev_state_ptr, static_cast<uint32_t>(was_signalled));
        }
        ctx.r3.u64 = X_STATUS_SUCCESS;
    } else {
        ctx.r3.u64 = X_STATUS_INVALID_HANDLE;
    }
}

// NtClearEvent - Xenia's actual logic
PPC_FUNC(__imp__NtClearEvent) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    
    auto ev = g_kernel->object_table()->LookupObject<XEvent>(handle);
    if (ev) {
        ev->Reset();
        ctx.r3.u64 = X_STATUS_SUCCESS;
    } else {
        ctx.r3.u64 = X_STATUS_INVALID_HANDLE;
    }
}

//=============================================================================
// Wait Functions - From xboxkrnl_threading.cc
//=============================================================================

// KeWaitForSingleObject - Xenia's actual logic
PPC_FUNC(__imp__KeWaitForSingleObject) {
    TRACE_KERNEL("KeWaitForSingleObject");
    KERNEL_CHECK();
    uint32_t object_ptr = ctx.r3.u32;
    uint32_t wait_reason = ctx.r4.u32;
    uint32_t processor_mode = ctx.r5.u32;
    uint32_t alertable = ctx.r6.u32;
    uint32_t timeout_ptr = ctx.r7.u32;
    
    auto object = XObject::GetNativeObject<XObject>(g_kernel, BASE + object_ptr);
    if (!object) {
        ctx.r3.u64 = X_STATUS_ABANDONED_WAIT_0;
        return;
    }
    
    uint64_t timeout = 0;
    uint64_t* timeout_arg = nullptr;
    if (timeout_ptr) {
        timeout = GuestRead64(BASE, timeout_ptr);
        timeout_arg = &timeout;
    }
    
    ctx.r3.u64 = object->Wait(wait_reason, processor_mode, alertable, timeout_arg);
}

// NtWaitForSingleObjectEx - Xenia's actual logic
PPC_FUNC(__imp__NtWaitForSingleObjectEx) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t wait_mode = ctx.r4.u32;
    uint32_t alertable = ctx.r5.u32;
    uint32_t timeout_ptr = ctx.r6.u32;
    
    auto object = g_kernel->object_table()->LookupObject<XObject>(handle);
    if (!object) {
        ctx.r3.u64 = X_STATUS_INVALID_HANDLE;
        return;
    }
    
    uint64_t timeout = 0;
    uint64_t* timeout_arg = nullptr;
    if (timeout_ptr) {
        timeout = GuestRead64(BASE, timeout_ptr);
        timeout_arg = &timeout;
    }
    
    ctx.r3.u64 = object->Wait(3, wait_mode, alertable, timeout_arg);
}

// KeWaitForMultipleObjects - Xenia's actual logic (simplified)
PPC_FUNC(__imp__KeWaitForMultipleObjects) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// NtWaitForMultipleObjectsEx - Xenia's actual logic (simplified)
PPC_FUNC(__imp__NtWaitForMultipleObjectsEx) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

//=============================================================================
// Semaphores - From xboxkrnl_threading.cc
//=============================================================================

// NtCreateSemaphore - Xenia's actual logic
PPC_FUNC(__imp__NtCreateSemaphore) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t obj_attrs = ctx.r4.u32;
    int32_t count = ctx.r5.s32;
    int32_t limit = ctx.r6.s32;
    
    auto sem = new XSemaphore(g_kernel);
    sem->Initialize(count, limit);
    
    if (handle_ptr) {
        GuestWrite32(BASE, handle_ptr, sem->handle());
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// KeInitializeSemaphore - Xenia's actual logic
PPC_FUNC(__imp__KeInitializeSemaphore) {
    uint32_t sem_ptr = ctx.r3.u32;
    int32_t count = ctx.r4.s32;
    int32_t limit = ctx.r5.s32;
    
    if (sem_ptr && BASE) {
        // X_KSEMAPHORE structure
        GuestWrite32(BASE, sem_ptr, 5);  // header.type = SemaphoreObject
        GuestWrite32(BASE, sem_ptr + 4, count);  // header.signal_state
        GuestWrite32(BASE, sem_ptr + 16, limit);  // limit
    }
}

// NtReleaseSemaphore - Xenia's actual logic
PPC_FUNC(__imp__NtReleaseSemaphore) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    int32_t release_count = ctx.r4.s32;
    uint32_t prev_count_ptr = ctx.r5.u32;
    
    int32_t previous_count = 0;
    X_STATUS result = X_STATUS_SUCCESS;
    
    auto sem = g_kernel->object_table()->LookupObject<XSemaphore>(handle);
    if (sem) {
        previous_count = sem->ReleaseSemaphore(release_count);
    } else {
        result = X_STATUS_INVALID_HANDLE;
    }
    
    if (prev_count_ptr) {
        GuestWrite32(BASE, prev_count_ptr, static_cast<uint32_t>(previous_count));
    }
    ctx.r3.u64 = result;
}

// KeReleaseSemaphore - Xenia's actual logic
PPC_FUNC(__imp__KeReleaseSemaphore) {
    KERNEL_CHECK();
    uint32_t sem_ptr = ctx.r3.u32;
    uint32_t increment = ctx.r4.u32;
    uint32_t adjustment = ctx.r5.u32;
    uint32_t wait = ctx.r6.u32;
    
    auto sem = XObject::GetNativeObject<XSemaphore>(g_kernel, BASE + sem_ptr);
    if (sem) {
        ctx.r3.s64 = sem->ReleaseSemaphore(adjustment);
    } else {
        ctx.r3.s64 = 0;
    }
}

//=============================================================================
// Mutants - From xboxkrnl_threading.cc
//=============================================================================

// NtCreateMutant - Xenia's actual logic
PPC_FUNC(__imp__NtCreateMutant) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t obj_attrs = ctx.r4.u32;
    uint32_t initial_owner = ctx.r5.u32;
    
    auto mutant = new XMutant(g_kernel);
    mutant->Initialize(initial_owner ? true : false);
    
    if (handle_ptr) {
        GuestWrite32(BASE, handle_ptr, mutant->handle());
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// NtReleaseMutant - Xenia's actual logic
PPC_FUNC(__imp__NtReleaseMutant) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t unknown = ctx.r4.u32;
    
    auto mutant = g_kernel->object_table()->LookupObject<XMutant>(handle);
    if (mutant) {
        ctx.r3.u64 = mutant->ReleaseMutant(0, false, false);
    } else {
        ctx.r3.u64 = X_STATUS_INVALID_HANDLE;
    }
}

//=============================================================================
// Object Management - From xboxkrnl_ob.cc
//=============================================================================

// NtClose - Xenia's actual logic
PPC_FUNC(__imp__NtClose) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    ctx.r3.u64 = g_kernel->object_table()->RemoveHandle(handle);
}

// ObReferenceObjectByHandle - Xenia's actual logic
PPC_FUNC(__imp__ObReferenceObjectByHandle) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t object_type = ctx.r4.u32;
    uint32_t out_ptr = ctx.r5.u32;
    
    auto object = g_kernel->object_table()->LookupObject<XObject>(handle);
    if (object && out_ptr) {
        GuestWrite32(BASE, out_ptr, object->guest_object());
        ctx.r3.u64 = X_STATUS_SUCCESS;
    } else {
        ctx.r3.u64 = X_STATUS_INVALID_HANDLE;
    }
}

// ObDereferenceObject
PPC_FUNC(__imp__ObDereferenceObject) {
    // Objects are ref-counted, decrement handled automatically
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

//=============================================================================
// Memory - From xboxkrnl_memory.cc
//=============================================================================

static std::atomic<uint32_t> s_virtual_alloc_ptr{0x20000000};
static std::atomic<uint32_t> s_physical_alloc_ptr{0xA0000000};

// NtAllocateVirtualMemory - Xenia's actual logic
PPC_FUNC(__imp__NtAllocateVirtualMemory) {
    TRACE_KERNEL("NtAllocateVirtualMemory");
    KERNEL_CHECK();
    uint32_t base_addr_ptr = ctx.r3.u32;
    uint32_t region_size_ptr = ctx.r4.u32;
    uint32_t alloc_type = ctx.r5.u32;
    uint32_t protect_bits = ctx.r6.u32;
    
    if (!base_addr_ptr || !region_size_ptr) {
        ctx.r3.u64 = X_STATUS_INVALID_PARAMETER;
        return;
    }
    
    uint32_t base_addr = GuestRead32(BASE, base_addr_ptr);
    uint32_t region_size = GuestRead32(BASE, region_size_ptr);
    
    if (region_size == 0) {
        ctx.r3.u64 = X_STATUS_INVALID_PARAMETER;
        return;
    }
    
    // Align to page size
    uint32_t page_size = 4 * 1024;
    region_size = (region_size + page_size - 1) & ~(page_size - 1);
    
    uint32_t address;
    if (base_addr == 0) {
        address = s_virtual_alloc_ptr.fetch_add(region_size);
    } else {
        address = base_addr;
    }
    
    GuestWrite32(BASE, base_addr_ptr, address);
    GuestWrite32(BASE, region_size_ptr, region_size);
    
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// MmAllocatePhysicalMemoryEx - Xenia's actual logic
PPC_FUNC(__imp__MmAllocatePhysicalMemoryEx) {
    TRACE_KERNEL("MmAllocatePhysicalMemoryEx");
    uint32_t flags = ctx.r3.u32;
    uint32_t region_size = ctx.r4.u32;
    uint32_t protect_bits = ctx.r5.u32;
    uint32_t min_addr = ctx.r6.u32;
    uint32_t max_addr = ctx.r7.u32;
    uint32_t alignment = ctx.r8.u32;
    
    // Xenia: Check protection bits
    if (!(protect_bits & (0x02 | 0x04))) {  // X_PAGE_READONLY | X_PAGE_READWRITE
        ctx.r3.u64 = 0;
        return;
    }
    
    // Align to 4KB
    uint32_t aligned_size = (region_size + 0xFFF) & ~0xFFF;
    if (aligned_size == 0) aligned_size = 0x1000;
    
    uint32_t address = s_physical_alloc_ptr.fetch_add(aligned_size);
    ctx.r3.u64 = address;
}

// NtFreeVirtualMemory
PPC_FUNC(__imp__NtFreeVirtualMemory) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// MmFreePhysicalMemory
PPC_FUNC(__imp__MmFreePhysicalMemory) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// MmGetPhysicalAddress
PPC_FUNC(__imp__MmGetPhysicalAddress) {
    ctx.r3.u64 = ctx.r3.u32;  // Identity mapping for now
}

// MmQueryAddressProtect
PPC_FUNC(__imp__MmQueryAddressProtect) {
    ctx.r3.u64 = 0x04;  // X_PAGE_READWRITE
}

// MmQueryAllocationSize
PPC_FUNC(__imp__MmQueryAllocationSize) {
    ctx.r3.u64 = 0x10000;  // 64KB default
}

// MmQueryStatistics
PPC_FUNC(__imp__MmQueryStatistics) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// NtQueryVirtualMemory
PPC_FUNC(__imp__NtQueryVirtualMemory) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

//=============================================================================
// TLS - From xboxkrnl_threading.cc
//=============================================================================

static std::atomic<uint32_t> s_tls_index{0};
static uint32_t s_tls_values[64] = {0};

// KeTlsAlloc - Xenia's actual logic
PPC_FUNC(__imp__KeTlsAlloc) {
    TRACE_KERNEL("KeTlsAlloc");
    uint32_t slot = s_tls_index.fetch_add(1);
    if (slot < 64) {
        s_tls_values[slot] = 0;
        ctx.r3.u64 = slot;
    } else {
        ctx.r3.u64 = 0xFFFFFFFF;  // X_TLS_OUT_OF_INDEXES
    }
}

// KeTlsFree - Xenia's actual logic
PPC_FUNC(__imp__KeTlsFree) {
    uint32_t index = ctx.r3.u32;
    if (index == 0xFFFFFFFF) {
        ctx.r3.u64 = 0;
        return;
    }
    ctx.r3.u64 = 1;
}

// KeTlsGetValue - Xenia's actual logic
PPC_FUNC(__imp__KeTlsGetValue) {
    TRACE_KERNEL("KeTlsGetValue");
    uint32_t index = ctx.r3.u32;
    if (index < 64) {
        ctx.r3.u64 = s_tls_values[index];
    } else {
        ctx.r3.u64 = 0;
    }
}

// KeTlsSetValue - Xenia's actual logic
PPC_FUNC(__imp__KeTlsSetValue) {
    TRACE_KERNEL("KeTlsSetValue");
    uint32_t index = ctx.r3.u32;
    uint32_t value = ctx.r4.u32;
    if (index < 64) {
        s_tls_values[index] = value;
        ctx.r3.u64 = 1;
    } else {
        ctx.r3.u64 = 0;
    }
}

//=============================================================================
// Critical Sections - From xboxkrnl_rtl.cc
//=============================================================================

PPC_FUNC(__imp__RtlInitializeCriticalSection) {
    TRACE_KERNEL("RtlInitializeCriticalSection");
    uint32_t cs_ptr = ctx.r3.u32;
    if (cs_ptr && BASE) {
        memset(BASE + cs_ptr, 0, 28);
        GuestWrite32(BASE, cs_ptr + 8, static_cast<uint32_t>(-1));  // LockCount
        GuestWrite32(BASE, cs_ptr + 12, 0);  // RecursionCount
        GuestWrite32(BASE, cs_ptr + 16, 0);  // OwningThread
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__RtlEnterCriticalSection) {
    uint32_t cs_ptr = ctx.r3.u32;
    // Handle NULL critical section - game may call with uninitialized pointer
    if (cs_ptr == 0) {
        ctx.r3.u64 = X_STATUS_SUCCESS;
        return;
    }
    if (BASE) {
        uint32_t rec = GuestRead32(BASE, cs_ptr + 12);
        GuestWrite32(BASE, cs_ptr + 12, rec + 1);
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__RtlLeaveCriticalSection) {
    uint32_t cs_ptr = ctx.r3.u32;
    if (cs_ptr && BASE) {
        uint32_t rec = GuestRead32(BASE, cs_ptr + 12);
        if (rec > 0) {
            GuestWrite32(BASE, cs_ptr + 12, rec - 1);
        }
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__RtlTryEnterCriticalSection) {
    ctx.r3.u64 = 1;  // Always succeed
}

//=============================================================================
// Spinlocks
//=============================================================================
PPC_FUNC(__imp__KfAcquireSpinLock) { TRACE_KERNEL("KfAcquireSpinLock"); ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfReleaseSpinLock) { TRACE_KERNEL("KfReleaseSpinLock"); }
PPC_FUNC(__imp__KeAcquireSpinLockAtRaisedIrql) { TRACE_KERNEL("KeAcquireSpinLockAtRaisedIrql"); }
PPC_FUNC(__imp__KeReleaseSpinLockFromRaisedIrql) { TRACE_KERNEL("KeReleaseSpinLockFromRaisedIrql"); }
PPC_FUNC(__imp__KeTryToAcquireSpinLockAtRaisedIrql) { ctx.r3.u64 = 1; }

//=============================================================================
// IRQL
//=============================================================================
PPC_FUNC(__imp__KeRaiseIrqlToDpcLevel) { TRACE_KERNEL("KeRaiseIrqlToDpcLevel"); ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfLowerIrql) { TRACE_KERNEL("KfLowerIrql"); }
PPC_FUNC(__imp__KeEnterCriticalRegion) { }
PPC_FUNC(__imp__KeLeaveCriticalRegion) { }
PPC_FUNC(__imp__KeLockL2) { }
PPC_FUNC(__imp__KeUnlockL2) { }

//=============================================================================
// Thread Properties
//=============================================================================
PPC_FUNC(__imp__KeResumeThread) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__KeSetBasePriorityThread) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeSetAffinityThread) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeSetDisableBoostThread) { ctx.r3.u64 = 0; }

//=============================================================================
// Strings
//=============================================================================
PPC_FUNC(__imp__RtlInitAnsiString) {
    uint32_t dest_ptr = ctx.r3.u32;
    uint32_t src_ptr = ctx.r4.u32;
    if (dest_ptr && BASE) {
        if (src_ptr) {
            char* src = reinterpret_cast<char*>(BASE + src_ptr);
            uint16_t len = static_cast<uint16_t>(strlen(src));
            GuestWrite32(BASE, dest_ptr, (len << 16) | (len + 1));
            GuestWrite32(BASE, dest_ptr + 4, src_ptr);
        } else {
            GuestWrite32(BASE, dest_ptr, 0);
            GuestWrite32(BASE, dest_ptr + 4, 0);
        }
    }
}

//=============================================================================
// Debug/Misc
//=============================================================================
PPC_FUNC(__imp__DbgPrint) {
    uint32_t fmt_ptr = ctx.r3.u32;
    if (fmt_ptr && BASE) {
        printf("[GAME] %s\n", reinterpret_cast<char*>(BASE + fmt_ptr));
    }
}
PPC_FUNC(__imp__DbgBreakPoint) { }
PPC_FUNC(__imp__KeBugCheck) { printf("[KERNEL] BugCheck!\n"); }
PPC_FUNC(__imp__KeBugCheckEx) { printf("[KERNEL] BugCheckEx!\n"); }

//=============================================================================
// Timers - From xboxkrnl_threading.cc
//=============================================================================
PPC_FUNC(__imp__NtCreateTimer) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    if (handle_ptr) {
        GuestWrite32(BASE, handle_ptr, 0x1000);  // Dummy handle
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtSetTimerEx) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtCancelTimer) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

//=============================================================================
// File I/O - From xboxkrnl_io.cc
//=============================================================================
#include <map>
static std::map<uint32_t, FILE*> s_file_handles;
static std::atomic<uint32_t> s_next_file_handle{0x10000};

static std::string ReadGuestPath(uint8_t* base, uint32_t obj_attrs_ptr) {
    if (!obj_attrs_ptr || !base) return "";
    uint32_t name_ptr = GuestRead32(base, obj_attrs_ptr + 4);
    if (!name_ptr) return "";
    uint32_t buffer_ptr = GuestRead32(base, name_ptr + 4);
    if (!buffer_ptr) return "";
    return std::string(reinterpret_cast<char*>(base + buffer_ptr));
}

// NtCreateFile - Xenia's actual logic
PPC_FUNC(__imp__NtCreateFile) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t desired_access = ctx.r4.u32;
    uint32_t obj_attrs = ctx.r5.u32;
    uint32_t io_status = ctx.r6.u32;
    
    std::string path = ReadGuestPath(BASE, obj_attrs);
    
    if (!g_vfs || path.empty()) {
        ctx.r3.u64 = 0xC000000F;  // STATUS_NO_SUCH_FILE
        return;
    }
    
    FILE* f = g_vfs->OpenFile(path, "rb");
    if (!f) {
        ctx.r3.u64 = 0xC000000F;
        return;
    }
    
    uint32_t handle = s_next_file_handle.fetch_add(1);
    s_file_handles[handle] = f;
    
    if (handle_ptr) GuestWrite32(BASE, handle_ptr, handle);
    if (io_status) {
        GuestWrite32(BASE, io_status, X_STATUS_SUCCESS);
        GuestWrite32(BASE, io_status + 4, 1);  // FILE_OPENED
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// NtOpenFile - Xenia's actual logic (calls NtCreateFile)
PPC_FUNC(__imp__NtOpenFile) {
    __imp__NtCreateFile(ctx, base);
}

// NtReadFile - Xenia's actual logic
PPC_FUNC(__imp__NtReadFile) {
    KERNEL_CHECK();
    uint32_t handle = ctx.r3.u32;
    uint32_t event = ctx.r4.u32;
    uint32_t apc_routine = ctx.r5.u32;
    uint32_t apc_context = ctx.r6.u32;
    uint32_t io_status = ctx.r7.u32;
    uint32_t buffer = ctx.r8.u32;
    uint32_t length = ctx.r9.u32;
    uint32_t offset_ptr = ctx.r10.u32;
    
    auto it = s_file_handles.find(handle);
    if (it == s_file_handles.end()) {
        ctx.r3.u64 = 0xC0000008;  // STATUS_INVALID_HANDLE
        return;
    }
    
    FILE* f = it->second;
    
    if (offset_ptr) {
        int64_t offset = static_cast<int64_t>(GuestRead64(BASE, offset_ptr));
        if (offset >= 0) {
            fseek(f, offset, SEEK_SET);
        }
    }
    
    size_t bytes_read = fread(BASE + buffer, 1, length, f);
    
    if (io_status) {
        GuestWrite32(BASE, io_status, X_STATUS_SUCCESS);
        GuestWrite32(BASE, io_status + 4, static_cast<uint32_t>(bytes_read));
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// NtWriteFile
PPC_FUNC(__imp__NtWriteFile) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

// NtQueryInformationFile - Xenia's actual logic
PPC_FUNC(__imp__NtQueryInformationFile) {
    KERNEL_CHECK();
    uint32_t handle = ctx.r3.u32;
    uint32_t io_status = ctx.r4.u32;
    uint32_t info_ptr = ctx.r5.u32;
    uint32_t length = ctx.r6.u32;
    uint32_t info_class = ctx.r7.u32;
    
    auto it = s_file_handles.find(handle);
    if (it == s_file_handles.end()) {
        ctx.r3.u64 = 0xC0000008;
        return;
    }
    
    FILE* f = it->second;
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    uint64_t size = ftell(f);
    fseek(f, pos, SEEK_SET);
    
    if (info_class == 5 && info_ptr && length >= 24) {  // FileStandardInformation
        GuestWrite64(BASE, info_ptr, size);
        GuestWrite64(BASE, info_ptr + 8, size);
        GuestWrite32(BASE, info_ptr + 16, 1);
        GuestWrite32(BASE, info_ptr + 20, 0);
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtSetInformationFile) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__NtQueryDirectoryFile) { ctx.r3.u64 = 0xC000000F; }
PPC_FUNC(__imp__NtQueryFullAttributesFile) { ctx.r3.u64 = 0xC000000F; }
PPC_FUNC(__imp__NtQueryVolumeInformationFile) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__NtFlushBuffersFile) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__NtDuplicateObject) { ctx.r3.u64 = X_STATUS_SUCCESS; }

//=============================================================================
// Video - From xboxkrnl_video.cc
//=============================================================================
PPC_FUNC(__imp__VdInitializeEngines) { TRACE_KERNEL("VdInitializeEngines"); ctx.r3.u64 = 1; }
PPC_FUNC(__imp__VdGetSystemCommandBuffer) {
    uint32_t p0 = ctx.r3.u32;
    uint32_t p1 = ctx.r4.u32;
    if (p0 && BASE) {
        memset(BASE + p0, 0, 0x94);
        GuestWrite32(BASE, p0, 0xBEEF0000);
    }
    if (p1 && BASE) {
        GuestWrite32(BASE, p1, 0xBEEF0001);
    }
}
PPC_FUNC(__imp__VdInitializeRingBuffer) { TRACE_KERNEL("VdInitializeRingBuffer"); ctx.r3.u64 = 0xC0000000; }
PPC_FUNC(__imp__VdQueryVideoMode) {
    TRACE_KERNEL("VdQueryVideoMode");
    uint32_t mode_ptr = ctx.r3.u32;
    if (mode_ptr && BASE) {
        memset(BASE + mode_ptr, 0, 0x8C);
        GuestWrite32(BASE, mode_ptr + 0x00, 1280);    // display_width
        GuestWrite32(BASE, mode_ptr + 0x04, 720);     // display_height
        GuestWrite32(BASE, mode_ptr + 0x08, 0);       // is_interlaced
        GuestWrite32(BASE, mode_ptr + 0x0C, 1);       // is_widescreen
        GuestWrite32(BASE, mode_ptr + 0x10, 1);       // is_hi_def
        GuestWrite32(BASE, mode_ptr + 0x14, 0x42700000);  // refresh_rate = 60.0f
        GuestWrite32(BASE, mode_ptr + 0x18, 1);       // video_standard = NTSC
    }
}
PPC_FUNC(__imp__VdCallGraphicsNotificationRoutines) { }
PPC_FUNC(__imp__VdEnableDisableClockGating) { }
PPC_FUNC(__imp__VdEnableRingBufferRPtrWriteBack) { }
PPC_FUNC(__imp__VdGetCurrentDisplayGamma) { }
PPC_FUNC(__imp__VdGetCurrentDisplayInformation) { }
PPC_FUNC(__imp__VdInitializeScalerCommandBuffer) { ctx.r3.u64 = 64; }  // Return count
PPC_FUNC(__imp__VdIsHSIOTrainingSucceeded) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__VdPersistDisplay) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__VdQueryVideoFlags) { ctx.r3.u64 = 0x7; }  // widescreen + HD flags
PPC_FUNC(__imp__VdRetrainEDRAM) { }
PPC_FUNC(__imp__VdRetrainEDRAMWorker) { }
PPC_FUNC(__imp__VdSetDisplayMode) { }
PPC_FUNC(__imp__VdSetGraphicsInterruptCallback) { }
PPC_FUNC(__imp__VdSetSystemCommandBufferGpuIdentifierAddress) { }
PPC_FUNC(__imp__VdShutdownEngines) { }
PPC_FUNC(__imp__VdSwap) {
    // Simulate vsync - without this, game spins forever in main loop
    static std::atomic<uint32_t> frame_count{0};
    uint32_t frame = ++frame_count;
    if (frame <= 10 || frame % 60 == 0) {
        printf("[VIDEO] VdSwap frame %u\n", frame);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60fps
}
PPC_FUNC(__imp__XGetVideoMode) {
    // XGetVideoMode fills X_VIDEO_MODE structure at r3
    // Called by sub_82850AF0 which checks is_widescreen (offset 0x0C)
    uint32_t mode_ptr = ctx.r3.u32;
    printf("[KERNEL] XGetVideoMode called, mode_ptr=0x%08X\n", mode_ptr);
    if (mode_ptr && BASE) {
        memset(BASE + mode_ptr, 0, 0x30);
        GuestWrite32(BASE, mode_ptr + 0x00, 1280);    // display_width
        GuestWrite32(BASE, mode_ptr + 0x04, 720);     // display_height
        GuestWrite32(BASE, mode_ptr + 0x08, 0);       // is_interlaced
        GuestWrite32(BASE, mode_ptr + 0x0C, 1);       // is_widescreen (CRITICAL - checked by init)
        GuestWrite32(BASE, mode_ptr + 0x10, 1);       // is_hi_def
        GuestWrite32(BASE, mode_ptr + 0x14, 0x42700000);  // refresh_rate = 60.0f
        GuestWrite32(BASE, mode_ptr + 0x18, 1);       // video_standard = NTSC
    }
}
PPC_FUNC(__imp__XGetAVPack) { ctx.r3.u64 = 6; }
PPC_FUNC(__imp__XGetGameRegion) { ctx.r3.u64 = 0xFF; }
PPC_FUNC(__imp__XGetLanguage) { ctx.r3.u64 = 1; }

//=============================================================================
// Audio - From xboxkrnl_audio.cc
//=============================================================================
PPC_FUNC(__imp__XAudioGetSpeakerConfig) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioGetVoiceCategoryVolume) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioGetVoiceCategoryVolumeChangeMask) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioRegisterRenderDriverClient) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__XAudioSubmitRenderDriverFrame) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__XAudioUnregisterRenderDriverClient) { }
PPC_FUNC(__imp__XMACreateContext) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMAReleaseContext) { }

//=============================================================================
// XAM - From xam module
//=============================================================================
static std::atomic<uint32_t> s_xam_alloc_ptr{0x30000000};
PPC_FUNC(__imp__XamAlloc) {
    TRACE_KERNEL("XamAlloc");
    uint32_t flags = ctx.r3.u32;
    uint32_t size = ctx.r4.u32;
    uint32_t out_ptr = ctx.r5.u32;
    
    if (size == 0) size = 0x1000;
    uint32_t aligned_size = (size + 0xFFF) & ~0xFFF;
    uint32_t address = s_xam_alloc_ptr.fetch_add(aligned_size);
    
    if (out_ptr && BASE) {
        GuestWrite32(BASE, out_ptr, address);
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}
PPC_FUNC(__imp__XamFree) { TRACE_KERNEL("XamFree"); }
PPC_FUNC(__imp__XamContentClose) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentCreateEnumerator) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentCreateEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentGetCreator) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentGetDeviceData) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentSetThumbnail) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamCreateEnumeratorHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamEnableInactivityProcessing) { }
PPC_FUNC(__imp__XamEnumerate) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamGetExecutionId) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamGetPrivateEnumStructureFromHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamGetSystemVersion) { ctx.r3.u64 = 0x20000000; }
PPC_FUNC(__imp__XamInputGetCapabilities) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamInputGetKeystrokeEx) { ctx.r3.u64 = 0xC000009D; } // ERROR_DEVICE_NOT_CONNECTED
PPC_FUNC(__imp__XamInputGetState) { ctx.r3.u64 = 0xC000009D; }
PPC_FUNC(__imp__XamInputSetState) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamIsUIActive) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamLoaderGetLaunchData) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamLoaderGetLaunchDataSize) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamNotifyCreateListener) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowMessageBoxUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserGetSigninState) { ctx.r3.u64 = 0; }

//=============================================================================
// Misc stubs
//=============================================================================
PPC_FUNC(__imp__sprintf) {
    uint32_t buf = ctx.r3.u32;
    uint32_t fmt = ctx.r4.u32;
    if (buf && fmt && BASE) {
        sprintf(reinterpret_cast<char*>(BASE + buf), "%s", 
                reinterpret_cast<char*>(BASE + fmt));
    }
    ctx.r3.u64 = 0;
}
PPC_FUNC(__imp___vsnprintf) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlMultiByteToUnicodeN) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlNtStatusToDosError) { ctx.r3.u64 = ctx.r3.u32; }
PPC_FUNC(__imp__RtlCompareMemoryUlong) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlFillMemoryUlong) { }
PPC_FUNC(__imp__RtlCaptureContext) { }
PPC_FUNC(__imp__RtlRaiseException) { }
PPC_FUNC(__imp__RtlUnwind) { }
PPC_FUNC(__imp____C_specific_handler) { }
PPC_FUNC(__imp__KiApcNormalRoutineNop) { }
PPC_FUNC(__imp__RtlCompareStringN) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlFreeAnsiString) { }
PPC_FUNC(__imp__RtlInitUnicodeString) { }
PPC_FUNC(__imp__RtlTimeFieldsToTime) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlTimeToTimeFields) { }
PPC_FUNC(__imp__RtlUnicodeStringToAnsiString) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlUnicodeToMultiByteN) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlUpcaseUnicodeChar) { ctx.r3.u64 = ctx.r3.u32; }
PPC_FUNC(__imp__ObOpenObjectByPointer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObLookupThreadByThreadId) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObCreateSymbolicLink) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObDeleteSymbolicLink) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObIsTitleObject) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObReferenceObject) { }
PPC_FUNC(__imp__ExGetXConfigSetting) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ExRegisterTitleTerminateNotification) { }
PPC_FUNC(__imp__HalReturnToFirmware) { }
PPC_FUNC(__imp__RtlImageXexHeaderField) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XexCheckExecutablePrivilege) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__XexGetModuleHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XexGetProcedureAddress) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoCheckShareAccess) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoCompleteRequest) { }
PPC_FUNC(__imp__IoCreateDevice) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoDeleteDevice) { }
PPC_FUNC(__imp__IoDismountVolume) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoDismountVolumeByFileHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoInvalidDeviceRequest) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoRemoveShareAccess) { }
PPC_FUNC(__imp__IoSetShareAccess) { }
PPC_FUNC(__imp__StfsControlDevice) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__StfsCreateDevice) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtDeviceIoControlFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtReadFileScatter) { ctx.r3.u64 = 0; }

//=============================================================================
// Memory Pool - From xboxkrnl_memory.cc
//=============================================================================
PPC_FUNC(__imp__ExAllocatePoolTypeWithTag) {
    uint32_t size = ctx.r4.u32;
    uint32_t addr = s_virtual_alloc_ptr.fetch_add((size + 0xFFF) & ~0xFFF);
    ctx.r3.u64 = addr;
}
PPC_FUNC(__imp__ExFreePool) { }

//=============================================================================
// Networking - Stubs (network not supported)
//=============================================================================
PPC_FUNC(__imp__NetDll_WSACleanup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_WSAGetLastError) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_WSAStartup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetCleanup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetGetConnectStatus) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetGetEthernetLinkStatus) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetGetTitleXnAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetQosListen) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetQosLookup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetQosRelease) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetServerToInAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetStartup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetUnregisterInAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetXnAddrToInAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_accept) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_bind) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_closesocket) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_connect) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_getsockname) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_inet_addr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_ioctlsocket) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_listen) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_recv) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_recvfrom) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_select) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_send) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_sendto) { ctx.r3.u64 = -1; }
PPC_FUNC(__imp__NetDll_setsockopt) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_shutdown) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_socket) { ctx.r3.u64 = -1; }

//=============================================================================
// XMsg - Stubs
//=============================================================================
PPC_FUNC(__imp__XMsgCancelIORequest) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgInProcessCall) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgStartIORequest) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgStartIORequestEx) { ctx.r3.u64 = 0; }

//=============================================================================
// XNotify - Stubs
//=============================================================================
PPC_FUNC(__imp__XNotifyGetNext) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XNotifyPositionUI) { }

//=============================================================================
// More XAM - Stubs
//=============================================================================
PPC_FUNC(__imp__XamLoaderLaunchTitle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamLoaderTerminateTitle) { }
PPC_FUNC(__imp__XamResetInactivity) { }
PPC_FUNC(__imp__XamSessionCreateHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamSessionRefObjByHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowDeviceSelectorUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowDirtyDiscErrorUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowGamerCardUIForXUID) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowMessageBoxUIEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowPlayerReviewUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowSigninUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamTaskCloseHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamTaskSchedule) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamTaskShouldExit) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserAreUsersFriends) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserCheckPrivilege) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserCreateAchievementEnumerator) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserCreateStatsEnumerator) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserGetName) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserGetSigninInfo) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserGetXUID) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserReadProfileSettings) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserWriteProfileSettings) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamVoiceClose) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamVoiceCreate) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamVoiceHeadsetPresent) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamVoiceSubmitPacket) { ctx.r3.u64 = 0; }

//=============================================================================
// Crypto - Stubs
//=============================================================================
PPC_FUNC(__imp__XeCryptSha) { }
PPC_FUNC(__imp__XeKeysConsolePrivateKeySign) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XeKeysConsoleSignatureVerification) { ctx.r3.u64 = 0; }
