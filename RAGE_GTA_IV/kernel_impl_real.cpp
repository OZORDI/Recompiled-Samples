/**
 * Real Kernel Implementation for GTA IV
 * Uses Xenia-derived kernel objects for actual functionality
 * 
 * This replaces kernel_impl.cpp with real kernel support
 */

// Include game's context FIRST (defines PPC_FUNC macro)
#include "ppc_config.h"
#include "ppc_context.h"

// Then kernel headers
#include "kernel/kernel_state.h"
#include "kernel/kernel_exports.h"
#include "kernel/xthread.h"
#include "kernel/xevent.h"
#include "kernel/xmutant.h"
#include "kernel/xsemaphore.h"

#include <chrono>
#include <thread>
#include <unordered_map>
#include <map>
#include "kernel/vfs.h"

using namespace xe;
using namespace xe::kernel;
using namespace xe::kernel::exports;

// Global kernel state
static KernelState* g_kernel = nullptr;

// File handle tracking for VFS
static std::map<uint32_t, FILE*> g_file_handles;
static std::map<uint32_t, uint64_t> g_file_positions;
static std::atomic<uint32_t> g_next_file_handle{0x10000};
static xe::vfs::VirtualFileSystem* g_vfs = nullptr;

//=============================================================================
// Kernel Initialization (call from main before game execution)
//=============================================================================
extern "C" void InitializeKernel(uint8_t* memory_base, 
                                  FunctionLookupFunc lookup) {
    g_kernel = new KernelState();
    g_kernel->set_memory_base(memory_base);
    g_kernel->set_function_lookup(lookup);
    g_kernel->SetCodeRegion(PPC_CODE_BASE, PPC_CODE_SIZE);
    
    // Initialize VFS for GTA IV
    g_vfs = new xe::vfs::VirtualFileSystem();
    xe::vfs::games::SetupGTAIV(g_vfs, "/Users/Ozordi/Downloads/xenia/xenia/RAGE/extracted");
    xe::vfs::VirtualFileSystem::set_shared(g_vfs);
    
    printf("[KERNEL] Initialized with memory base %p\n", memory_base);
    printf("[KERNEL] VFS initialized with asset root: %s\n", g_vfs->asset_root().c_str());
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
    
    printf("[KERNEL] NtResumeThread(handle=0x%X)\n", handle);
    
    uint32_t count = 0;
    X_STATUS result = exports::NtResumeThread(g_kernel, handle, &count);
    
    printf("[KERNEL] NtResumeThread result=0x%X, count=%u\n", result, count);
    
    if (count_ptr) GuestStore32(BASE, count_ptr, count);
    ctx.r3.u64 = result;
}

PPC_FUNC(__imp__NtSuspendThread) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t count_ptr = ctx.r4.u32;
    
    uint32_t count = 0;
    X_STATUS result = exports::NtSuspendThread(g_kernel, handle, &count);
    
    if (count_ptr) GuestStore32(BASE, count_ptr, count);
    ctx.r3.u64 = result;
}

PPC_FUNC(__imp__ExTerminateThread) {
    KERNEL_CHECK();
    ctx.r3.u64 = exports::ExTerminateThread(g_kernel, ctx.r3.u32);
}

PPC_FUNC(__imp__KeResumeThread) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__KeSetBasePriorityThread) {
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetAffinityThread) {
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetDisableBoostThread) {
    ctx.r3.u64 = 0;
}

//=============================================================================
// Events
//=============================================================================
PPC_FUNC(__imp__NtCreateEvent) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t obj_attrs = ctx.r4.u32;
    uint32_t event_type = ctx.r5.u32;
    uint32_t initial_state = ctx.r6.u32;
    
    printf("[KERNEL] NtCreateEvent(type=%u, initial=%u)\n", event_type, initial_state);
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
    if (ctx.r4.u32) GuestStore32(BASE, ctx.r4.u32, static_cast<uint32_t>(prev));
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
    uint32_t sem_ptr = ctx.r3.u32;
    int32_t count = ctx.r4.s32;
    int32_t limit = ctx.r5.s32;
    
    if (sem_ptr && BASE) {
        auto header = reinterpret_cast<X_DISPATCH_HEADER*>(BASE + sem_ptr);
        header->type = 5;
        header->signal_state = count;
        GuestStore32(BASE, sem_ptr + sizeof(X_DISPATCH_HEADER), limit);
    }
}

PPC_FUNC(__imp__KeReleaseSemaphore) {
    KERNEL_CHECK();
    uint32_t sem_ptr = ctx.r3.u32;
    auto sem = XObject::GetNativeObject<XSemaphore>(g_kernel, BASE + sem_ptr);
    ctx.r3.s64 = sem ? sem->ReleaseSemaphore(ctx.r4.s32) : 0;
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
    
    printf("[KERNEL] KeWaitForSingleObject(obj=0x%08X, timeout_ptr=0x%08X)\n", obj_ptr, timeout_ptr);
    
    uint64_t* timeout = timeout_ptr ? reinterpret_cast<uint64_t*>(BASE + timeout_ptr) : nullptr;
    ctx.r3.u64 = exports::KeWaitForSingleObject(g_kernel, BASE, obj_ptr,
                                                 reason, mode, alertable, timeout);
    printf("[KERNEL] KeWaitForSingleObject returned 0x%X\n", (uint32_t)ctx.r3.u64);
}

PPC_FUNC(__imp__KeWaitForMultipleObjects) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtWaitForSingleObjectEx) {
    KERNEL_CHECK();
    X_HANDLE handle = ctx.r3.u32;
    uint32_t mode = ctx.r4.u32;
    uint32_t alertable = ctx.r5.u32;
    uint32_t timeout_ptr = ctx.r6.u32;
    
    uint64_t* timeout = timeout_ptr ? reinterpret_cast<uint64_t*>(BASE + timeout_ptr) : nullptr;
    ctx.r3.u64 = exports::NtWaitForSingleObjectEx(g_kernel, handle, mode, alertable, timeout);
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
// Critical Sections
//=============================================================================
PPC_FUNC(__imp__RtlInitializeCriticalSection) {
    uint32_t cs = ctx.r3.u32;
    if (cs && BASE) {
        GuestStore32(BASE, cs + 0x04, 0xFFFFFFFF);
        GuestStore32(BASE, cs + 0x08, 0);
        GuestStore32(BASE, cs + 0x0C, 0);
    }
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__RtlEnterCriticalSection) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__RtlLeaveCriticalSection) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__RtlTryEnterCriticalSection) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__KeEnterCriticalRegion) { }
PPC_FUNC(__imp__KeLeaveCriticalRegion) { }

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

PPC_FUNC(__imp__ObDereferenceObject) { }

//=============================================================================
// Process Info
//=============================================================================
PPC_FUNC(__imp__KeGetCurrentProcessType) {
    ctx.r3.u64 = g_kernel ? exports::KeGetCurrentProcessType(g_kernel) : 1;
}

//=============================================================================
// Memory
//=============================================================================
static std::atomic<uint32_t> phys_alloc_ptr{0xA0000000};

PPC_FUNC(__imp__NtAllocateVirtualMemory) {
    uint32_t size_ptr = ctx.r5.u32;
    uint32_t size = (size_ptr && BASE) ? GuestLoad32(BASE, size_ptr) : 0x10000;
    printf("[KERNEL] NtAllocateVirtualMemory(size=0x%X)\n", size);
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__MmAllocatePhysicalMemoryEx) {
    uint32_t size = ctx.r4.u32;
    uint32_t aligned = (size + 0xFFF) & ~0xFFF;
    uint32_t result = phys_alloc_ptr.fetch_add(aligned);
    printf("[KERNEL] MmAllocatePhysicalMemoryEx(size=0x%X) -> 0x%08X\n", size, result);
    ctx.r3.u64 = result;
}

PPC_FUNC(__imp__NtFreeVirtualMemory) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__MmFreePhysicalMemory) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__MmGetPhysicalAddress) { ctx.r3.u64 = ctx.r3.u32; }
PPC_FUNC(__imp__MmQueryAddressProtect) { ctx.r3.u64 = 0x04; }
PPC_FUNC(__imp__MmQueryAllocationSize) { ctx.r3.u64 = 0x10000; }
PPC_FUNC(__imp__MmQueryStatistics) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__NtQueryVirtualMemory) { ctx.r3.u64 = X_STATUS_SUCCESS; }

//=============================================================================
// Time
//=============================================================================
PPC_FUNC(__imp__KeQueryPerformanceFrequency) { ctx.r3.u64 = 50000000; }

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

static std::atomic<int> delay_count{0};
PPC_FUNC(__imp__KeDelayExecutionThread) {
    if (++delay_count <= 5) printf("[KERNEL] KeDelayExecutionThread called\n");
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

static std::atomic<int> yield_count{0};
PPC_FUNC(__imp__NtYieldExecution) {
    if (++yield_count <= 5) printf("[KERNEL] NtYieldExecution called\n");
    std::this_thread::yield();
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

//=============================================================================
// Spinlocks
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
    ctx.r3.u64 = g_kernel ? g_kernel->AllocateTLS() : 0;
}

PPC_FUNC(__imp__KeTlsFree) {
    if (g_kernel) g_kernel->FreeTLS(ctx.r3.u32);
    ctx.r3.u64 = 1;
}

static thread_local std::unordered_map<uint32_t, uint64_t> tls_values;

PPC_FUNC(__imp__KeTlsGetValue) {
    auto it = tls_values.find(ctx.r3.u32);
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
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t handle = 0x80000000 + (phys_alloc_ptr.fetch_add(1) & 0xFFFF);
    if (handle_ptr && BASE) GuestStore32(BASE, handle_ptr, handle);
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtSetTimerEx) { ctx.r3.u64 = X_STATUS_SUCCESS; }
PPC_FUNC(__imp__NtCancelTimer) { ctx.r3.u64 = X_STATUS_SUCCESS; }

//=============================================================================
// Debug
//=============================================================================
PPC_FUNC(__imp__DbgPrint) {
    uint32_t fmt_ptr = ctx.r3.u32;
    if (fmt_ptr && BASE) printf("[GAME] %s", reinterpret_cast<char*>(BASE + fmt_ptr));
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__DbgBreakPoint) { }
PPC_FUNC(__imp__KeBugCheck) { printf("[KERNEL] BugCheck!\n"); std::abort(); }
PPC_FUNC(__imp__KeBugCheckEx) { printf("[KERNEL] BugCheckEx!\n"); std::abort(); }

//=============================================================================
// RTL Strings
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

//=============================================================================
// Remaining stubs (keep for completeness)
//=============================================================================
#define STUB(name) PPC_FUNC(__imp__##name) { ctx.r3.u64 = 0; }
#define STUB_RET(name, ret) PPC_FUNC(__imp__##name) { ctx.r3.u64 = (ret); }

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

//=============================================================================
// File I/O - Real VFS Implementation
//=============================================================================
static std::string ReadGuestString(uint8_t* base, uint32_t addr) {
    if (!addr || !base) return "";
    char* ptr = reinterpret_cast<char*>(base + addr);
    return std::string(ptr);
}

static std::string ReadXboxPath(uint8_t* base, uint32_t obj_attrs_ptr) {
    if (!obj_attrs_ptr || !base) return "";
    // X_OBJECT_ATTRIBUTES: root_directory (4), name_ptr (4), attributes (4)
    uint32_t name_ptr = GuestLoad32(base, obj_attrs_ptr + 4);
    if (!name_ptr) return "";
    // X_ANSI_STRING: length (2), max_length (2), buffer_ptr (4)
    uint32_t buffer_ptr = GuestLoad32(base, name_ptr + 4);
    return ReadGuestString(base, buffer_ptr);
}

PPC_FUNC(__imp__NtCreateFile) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t desired_access = ctx.r4.u32;
    uint32_t obj_attrs = ctx.r5.u32;
    
    std::string path = ReadXboxPath(BASE, obj_attrs);
    printf("[KERNEL] NtCreateFile('%s')\n", path.c_str());
    
    if (!g_vfs || path.empty()) {
        ctx.r3.u64 = 0xC000000F; // STATUS_NO_SUCH_FILE
        return;
    }
    
    FILE* f = g_vfs->OpenFile(path, "rb");
    if (!f) {
        printf("[KERNEL] NtCreateFile: File not found: %s\n", path.c_str());
        ctx.r3.u64 = 0xC000000F; // STATUS_NO_SUCH_FILE
        return;
    }
    
    uint32_t handle = g_next_file_handle.fetch_add(1);
    g_file_handles[handle] = f;
    g_file_positions[handle] = 0;
    
    if (handle_ptr) GuestStore32(BASE, handle_ptr, handle);
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtOpenFile) {
    KERNEL_CHECK();
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t desired_access = ctx.r4.u32;
    uint32_t obj_attrs = ctx.r5.u32;
    
    std::string path = ReadXboxPath(BASE, obj_attrs);
    printf("[KERNEL] NtOpenFile('%s')\n", path.c_str());
    
    if (!g_vfs || path.empty()) {
        ctx.r3.u64 = 0xC000000F;
        return;
    }
    
    FILE* f = g_vfs->OpenFile(path, "rb");
    if (!f) {
        printf("[KERNEL] NtOpenFile: File not found: %s\n", path.c_str());
        ctx.r3.u64 = 0xC000000F;
        return;
    }
    
    uint32_t handle = g_next_file_handle.fetch_add(1);
    g_file_handles[handle] = f;
    g_file_positions[handle] = 0;
    
    if (handle_ptr) GuestStore32(BASE, handle_ptr, handle);
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtReadFile) {
    KERNEL_CHECK();
    uint32_t handle = ctx.r3.u32;
    uint32_t event = ctx.r4.u32;
    uint32_t apc_routine = ctx.r5.u32;
    uint32_t apc_context = ctx.r6.u32;
    uint32_t io_status_ptr = ctx.r7.u32;
    uint32_t buffer = ctx.r8.u32;
    uint32_t length = ctx.r9.u32;
    uint32_t byte_offset_ptr = ctx.r10.u32;
    
    auto it = g_file_handles.find(handle);
    if (it == g_file_handles.end()) {
        ctx.r3.u64 = 0xC0000008; // STATUS_INVALID_HANDLE
        return;
    }
    
    FILE* f = it->second;
    
    // Handle byte offset
    if (byte_offset_ptr) {
        int64_t offset = static_cast<int64_t>(GuestLoad64(BASE, byte_offset_ptr));
        if (offset >= 0) {
            fseek(f, offset, SEEK_SET);
        }
    }
    
    size_t bytes_read = fread(BASE + buffer, 1, length, f);
    
    if (io_status_ptr) {
        GuestStore32(BASE, io_status_ptr, X_STATUS_SUCCESS);
        GuestStore32(BASE, io_status_ptr + 4, static_cast<uint32_t>(bytes_read));
    }
    
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtWriteFile) {
    ctx.r3.u64 = X_STATUS_SUCCESS; // Writes are no-op for now
}

PPC_FUNC(__imp__NtQueryInformationFile) {
    KERNEL_CHECK();
    uint32_t handle = ctx.r3.u32;
    uint32_t io_status_ptr = ctx.r4.u32;
    uint32_t info_ptr = ctx.r5.u32;
    uint32_t length = ctx.r6.u32;
    uint32_t info_class = ctx.r7.u32;
    
    auto it = g_file_handles.find(handle);
    if (it == g_file_handles.end()) {
        ctx.r3.u64 = 0xC0000008;
        return;
    }
    
    FILE* f = it->second;
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    uint64_t size = ftell(f);
    fseek(f, pos, SEEK_SET);
    
    // FileStandardInformation (5) or FileNetworkOpenInformation (34)
    if (info_class == 5 && info_ptr && length >= 24) {
        GuestStore64(BASE, info_ptr, size);      // AllocationSize
        GuestStore64(BASE, info_ptr + 8, size);  // EndOfFile
        GuestStore32(BASE, info_ptr + 16, 1);    // NumberOfLinks
        GuestStore32(BASE, info_ptr + 20, 0);    // DeletePending/Directory
    }
    
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

PPC_FUNC(__imp__NtSetInformationFile) {
    ctx.r3.u64 = X_STATUS_SUCCESS;
}

STUB_RET(NtQueryDirectoryFile, 0xC000000F)
STUB_RET(NtQueryFullAttributesFile, 0xC000000F)
STUB(NtQueryVolumeInformationFile)
STUB(NtFlushBuffersFile)
STUB(NtDuplicateObject)

STUB(ObOpenObjectByPointer)
STUB(ObLookupThreadByThreadId)
STUB(ObCreateSymbolicLink)
STUB(ObDeleteSymbolicLink)
STUB(ObIsTitleObject)
STUB(ObReferenceObject)

STUB(ExGetXConfigSetting)
STUB(ExRegisterTitleTerminateNotification)
STUB(HalReturnToFirmware)
STUB(RtlImageXexHeaderField)
STUB_RET(XexCheckExecutablePrivilege, 1)
STUB(XexGetModuleHandle)
STUB(XexGetProcedureAddress)

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

//=============================================================================
// GPU/Video - Match Xenia's implementations
//=============================================================================
PPC_FUNC(__imp__VdInitializeEngines) {
    // Xenia returns 1 for success
    ctx.r3.u64 = 1;
}

PPC_FUNC(__imp__VdGetSystemCommandBuffer) {
    // Xenia writes specific values to output pointers
    uint32_t p0_ptr = ctx.r3.u32;
    uint32_t p1_ptr = ctx.r4.u32;
    if (p0_ptr && BASE) {
        memset(BASE + p0_ptr, 0, 0x94);
        GuestStore32(BASE, p0_ptr, 0xBEEF0000);
    }
    if (p1_ptr && BASE) {
        GuestStore32(BASE, p1_ptr, 0xBEEF0001);
    }
}

PPC_FUNC(__imp__VdInitializeRingBuffer) {
    // Return ring buffer base address
    ctx.r3.u64 = 0xC0000000; // Typical GPU ring buffer address
}

PPC_FUNC(__imp__VdQueryVideoMode) {
    // Write video mode info to output pointer
    uint32_t mode_ptr = ctx.r3.u32;
    if (mode_ptr && BASE) {
        GuestStore32(BASE, mode_ptr + 0x00, 1280);  // width
        GuestStore32(BASE, mode_ptr + 0x04, 720);   // height
        GuestStore32(BASE, mode_ptr + 0x08, 0);     // interlaced
        GuestStore32(BASE, mode_ptr + 0x0C, 60);    // refresh rate
    }
}

STUB(VdCallGraphicsNotificationRoutines)
STUB(VdEnableDisableClockGating)
STUB(VdEnableRingBufferRPtrWriteBack)
STUB(VdGetCurrentDisplayGamma)
STUB(VdGetCurrentDisplayInformation)
STUB(VdInitializeScalerCommandBuffer)
STUB_RET(VdIsHSIOTrainingSucceeded, 1)
STUB(VdPersistDisplay)
STUB(VdQueryVideoFlags)
STUB(VdRetrainEDRAM)
STUB(VdRetrainEDRAMWorker)
STUB(VdSetDisplayMode)
STUB(VdSetGraphicsInterruptCallback)
STUB(VdSetSystemCommandBufferGpuIdentifierAddress)
STUB(VdShutdownEngines)
STUB(VdSwap)
STUB(XGetVideoMode)
STUB_RET(XGetAVPack, 6)
STUB_RET(XGetGameRegion, 0xFF)
STUB_RET(XGetLanguage, 1)

STUB(XAudioGetSpeakerConfig)
STUB(XAudioGetVoiceCategoryVolume)
STUB(XAudioGetVoiceCategoryVolumeChangeMask)
STUB(XAudioRegisterRenderDriverClient)
STUB(XAudioSubmitRenderDriverFrame)
STUB(XAudioUnregisterRenderDriverClient)
STUB(XMACreateContext)
STUB(XMAReleaseContext)

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

STUB(XeCryptSha)
STUB(XeKeysConsolePrivateKeySign)
STUB(XeKeysConsoleSignatureVerification)

STUB(XMsgCancelIORequest)
STUB(XMsgInProcessCall)
STUB(XMsgStartIORequest)
STUB(XMsgStartIORequestEx)
STUB(XNotifyGetNext)
STUB(XNotifyPositionUI)

STUB(ExAllocatePoolTypeWithTag)
STUB(ExFreePool)

PPC_FUNC(__imp__sprintf) {
    uint32_t buf = ctx.r3.u32;
    uint32_t fmt = ctx.r4.u32;
    if (buf && fmt && BASE) {
        strcpy(reinterpret_cast<char*>(BASE + buf), reinterpret_cast<char*>(BASE + fmt));
        ctx.r3.u64 = strlen(reinterpret_cast<char*>(BASE + fmt));
    } else ctx.r3.u64 = 0;
}

PPC_FUNC(__imp___vsnprintf) {
    uint32_t buf = ctx.r3.u32;
    uint32_t size = ctx.r4.u32;
    uint32_t fmt = ctx.r5.u32;
    if (buf && fmt && size > 0 && BASE) {
        strncpy(reinterpret_cast<char*>(BASE + buf), reinterpret_cast<char*>(BASE + fmt), size - 1);
        BASE[buf + size - 1] = 0;
        ctx.r3.u64 = strlen(reinterpret_cast<char*>(BASE + buf));
    } else ctx.r3.u64 = 0;
}
