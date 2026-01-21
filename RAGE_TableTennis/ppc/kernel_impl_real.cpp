/**
 * Real Kernel Implementation for Table Tennis
 * Uses Xenia-derived kernel objects for actual functionality
 */

// Include game's context FIRST (defines PPC_FUNC macro)
#include "ppc_config.h"
#include "../ppc_context.h"

// Then kernel headers
#include "kernel/kernel_state.h"
#include "kernel/kernel_exports.h"
#include "kernel/xthread.h"
#include "kernel/xevent.h"
#include "kernel/xmutant.h"
#include "kernel/xsemaphore.h"

#include <chrono>
#include <thread>
#include <cstdio>

// Shared runtime
#include "../../rage_runtime/rage_runtime.h"

using namespace xe::kernel;
namespace exports = xe::kernel::exports;

// Global kernel state
static KernelState* g_kernel = nullptr;

#define KERNEL_CHECK() \
    if (!g_kernel) { \
        printf("[KERNEL] ERROR: Kernel not initialized!\n"); \
        ctx.r3.u64 = 0xC0000001; \
        return; \
    }

#define BASE RageRuntime::GetMemoryBase()

// Helper for guest memory
inline uint32_t GuestLoad32(uint8_t* base, uint32_t addr) {
    return RageRuntime::GuestLoad32(base, addr);
}
inline void GuestStore32(uint8_t* base, uint32_t addr, uint32_t val) {
    RageRuntime::GuestStore32(base, addr, val);
}

//=============================================================================
// Kernel Initialization
//=============================================================================
extern "C" void InitializeKernel(uint8_t* memory_base, xe::kernel::FunctionLookupFunc lookup) {
    g_kernel = new KernelState();
    g_kernel->set_memory_base(memory_base);
    g_kernel->set_function_lookup(lookup);
    KernelState::set_shared(g_kernel);
    printf("[KERNEL] Initialized with memory base %p\n", (void*)memory_base);
}

extern "C" void ShutdownKernel() {
    delete g_kernel;
    g_kernel = nullptr;
    printf("[KERNEL] Shutdown complete\n");
}

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
    
    ctx.r3.u64 = exports::ExCreateThread(
        g_kernel, BASE, handle_ptr, stack_size, thread_id_ptr,
        xapi_startup, start_address, start_context, flags);
}

PPC_FUNC(__imp__ExTerminateThread) {
    KERNEL_CHECK();
    ctx.r3.u64 = exports::ExTerminateThread(g_kernel, ctx.r3.u32);
}

PPC_FUNC(__imp__NtResumeThread) {
    KERNEL_CHECK();
    uint32_t prev = 0;
    ctx.r3.u64 = exports::NtResumeThread(g_kernel, ctx.r3.u32, &prev);
    if (ctx.r4.u32) GuestStore32(BASE, ctx.r4.u32, prev);
}

PPC_FUNC(__imp__NtSuspendThread) {
    KERNEL_CHECK();
    uint32_t prev = 0;
    ctx.r3.u64 = exports::NtSuspendThread(g_kernel, ctx.r3.u32, &prev);
    if (ctx.r4.u32) GuestStore32(BASE, ctx.r4.u32, prev);
}

PPC_FUNC(__imp__KeResumeThread) {
    printf("[STUB] KeResumeThread\n");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetBasePriorityThread) {
    printf("[STUB] KeSetBasePriorityThread\n");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetAffinityThread) {
    printf("[STUB] KeSetAffinityThread\n");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetDisableBoostThread) {
    printf("[STUB] KeSetDisableBoostThread\n");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeGetCurrentProcessType) {
    ctx.r3.u64 = 1;  // USER process
}

//=============================================================================
// Events
//=============================================================================
PPC_FUNC(__imp__NtCreateEvent) {
    KERNEL_CHECK();
    ctx.r3.u64 = exports::NtCreateEvent(g_kernel, BASE, ctx.r3.u32, ctx.r4.u32,
                                         ctx.r5.u32, ctx.r6.u32);
}

PPC_FUNC(__imp__NtSetEvent) {
    KERNEL_CHECK();
    auto event = g_kernel->object_table()->LookupObject<XEvent>(ctx.r3.u32);
    if (event) {
        event->Set(0, false);
        ctx.r3.u64 = 0;
    } else {
        ctx.r3.u64 = 0xC0000008;  // STATUS_INVALID_HANDLE
    }
}

PPC_FUNC(__imp__NtClearEvent) {
    KERNEL_CHECK();
    auto event = g_kernel->object_table()->LookupObject<XEvent>(ctx.r3.u32);
    if (event) {
        event->Reset();
        ctx.r3.u64 = 0;
    } else {
        ctx.r3.u64 = 0xC0000008;
    }
}

PPC_FUNC(__imp__KeSetEvent) {
    KERNEL_CHECK();
    ctx.r3.u64 = exports::KeSetEvent(g_kernel, BASE, ctx.r3.u32, ctx.r4.u32, ctx.r5.u32);
}

PPC_FUNC(__imp__KeResetEvent) {
    KERNEL_CHECK();
    ctx.r3.u64 = exports::KeResetEvent(g_kernel, BASE, ctx.r3.u32);
}

PPC_FUNC(__imp__KePulseEvent) {
    printf("[STUB] KePulseEvent\n");
    ctx.r3.u64 = 0;
}

//=============================================================================
// Mutexes
//=============================================================================
PPC_FUNC(__imp__NtCreateMutant) {
    KERNEL_CHECK();
    ctx.r3.u64 = exports::NtCreateMutant(g_kernel, BASE, ctx.r3.u32, ctx.r4.u32, ctx.r5.u32);
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
    ctx.r3.u64 = exports::NtCreateSemaphore(g_kernel, BASE, ctx.r3.u32, ctx.r4.u32,
                                             ctx.r5.s32, ctx.r6.s32);
}

PPC_FUNC(__imp__NtReleaseSemaphore) {
    KERNEL_CHECK();
    int32_t prev = 0;
    ctx.r3.u64 = exports::NtReleaseSemaphore(g_kernel, ctx.r3.u32, ctx.r4.s32, &prev);
    if (ctx.r5.u32) GuestStore32(BASE, ctx.r5.u32, static_cast<uint32_t>(prev));
}

//=============================================================================
// Wait Functions
//=============================================================================
PPC_FUNC(__imp__NtWaitForSingleObjectEx) {
    KERNEL_CHECK();
    uint64_t timeout = ctx.r6.u64;
    ctx.r3.u64 = exports::NtWaitForSingleObjectEx(g_kernel, ctx.r3.u32,
                                                   ctx.r4.u32, ctx.r5.u32, &timeout);
}

PPC_FUNC(__imp__KeWaitForSingleObject) {
    KERNEL_CHECK();
    uint64_t timeout = ctx.r7.u64;
    ctx.r3.u64 = exports::KeWaitForSingleObject(g_kernel, BASE, ctx.r3.u32,
                                                 ctx.r4.u32, ctx.r5.u32, ctx.r6.u32, &timeout);
}

PPC_FUNC(__imp__NtWaitForMultipleObjectsEx) {
    printf("[STUB] NtWaitForMultipleObjectsEx\n");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtSignalAndWaitForSingleObjectEx) {
    printf("[STUB] NtSignalAndWaitForSingleObjectEx\n");
    ctx.r3.u64 = 0;
}

//=============================================================================
// TLS
//=============================================================================
static std::atomic<uint32_t> g_tls_index{0};

PPC_FUNC(__imp__KeTlsAlloc) {
    ctx.r3.u64 = g_tls_index++;
}

PPC_FUNC(__imp__KeTlsFree) {
    ctx.r3.u64 = 1;  // TRUE
}

PPC_FUNC(__imp__KeTlsGetValue) {
    uint32_t pcr = ctx.r13.u32;
    uint32_t index = ctx.r3.u32;
    uint32_t tls_ptr = GuestLoad32(BASE, pcr);
    ctx.r3.u64 = GuestLoad32(BASE, tls_ptr + index * 4);
}

PPC_FUNC(__imp__KeTlsSetValue) {
    uint32_t pcr = ctx.r13.u32;
    uint32_t index = ctx.r3.u32;
    uint32_t value = ctx.r4.u32;
    uint32_t tls_ptr = GuestLoad32(BASE, pcr);
    GuestStore32(BASE, tls_ptr + index * 4, value);
    ctx.r3.u64 = 1;  // TRUE
}

//=============================================================================
// Time
//=============================================================================
PPC_FUNC(__imp__KeQuerySystemTime) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto filetime = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() * 10
                    + 116444736000000000ULL;
    
    uint32_t time_ptr = ctx.r3.u32;
    GuestStore32(BASE, time_ptr, static_cast<uint32_t>(filetime >> 32));
    GuestStore32(BASE, time_ptr + 4, static_cast<uint32_t>(filetime));
}

PPC_FUNC(__imp__KeQueryPerformanceFrequency) {
    ctx.r3.u64 = 50000000;  // 50MHz
}

PPC_FUNC(__imp__KeDelayExecutionThread) {
    int64_t delay = static_cast<int64_t>(ctx.r5.u64);
    if (delay < 0) {
        delay = -delay / 10000;  // 100ns units to ms
    }
    if (delay > 0 && delay < 1000000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtYieldExecution) {
    std::this_thread::yield();
    ctx.r3.u64 = 0;
}

//=============================================================================
// Critical Sections
//=============================================================================
PPC_FUNC(__imp__RtlInitializeCriticalSection) {
    uint32_t cs_ptr = ctx.r3.u32;
    GuestStore32(BASE, cs_ptr + 0, 0);   // lock count
    GuestStore32(BASE, cs_ptr + 4, 0);   // recursion count
    GuestStore32(BASE, cs_ptr + 8, 0);   // owning thread
}

PPC_FUNC(__imp__RtlEnterCriticalSection) {
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__RtlLeaveCriticalSection) {
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__RtlTryEnterCriticalSection) {
    ctx.r3.u64 = 1;  // Success
}

//=============================================================================
// Memory
//=============================================================================
PPC_FUNC(__imp__NtAllocateVirtualMemory) {
    uint32_t base_ptr = ctx.r3.u32;
    uint32_t size_ptr = ctx.r5.u32;
    
    uint32_t req_base = GuestLoad32(BASE, base_ptr);
    uint32_t req_size = GuestLoad32(BASE, size_ptr);
    
    static uint32_t heap_ptr = 0x20000000;
    uint32_t alloc_base = req_base ? req_base : heap_ptr;
    heap_ptr += (req_size + 0xFFF) & ~0xFFF;
    
    GuestStore32(BASE, base_ptr, alloc_base);
    GuestStore32(BASE, size_ptr, req_size);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtFreeVirtualMemory) {
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__MmAllocatePhysicalMemoryEx) {
    static uint32_t phys_ptr = 0x10000000;
    uint32_t size = ctx.r4.u32;
    uint32_t result = phys_ptr;
    phys_ptr += (size + 0xFFF) & ~0xFFF;
    ctx.r3.u64 = result;
}

PPC_FUNC(__imp__MmFreePhysicalMemory) {
}

PPC_FUNC(__imp__MmGetPhysicalAddress) {
    ctx.r3.u64 = ctx.r3.u32;  // Identity mapping
}

PPC_FUNC(__imp__MmQueryAddressProtect) {
    ctx.r3.u64 = 0x04;  // PAGE_READWRITE
}

//=============================================================================
// Handles
//=============================================================================
PPC_FUNC(__imp__ObReferenceObjectByHandle) {
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__ObDereferenceObject) {
}

PPC_FUNC(__imp__NtClose) {
    ctx.r3.u64 = 0;
}

//=============================================================================
// Debug/Output
//=============================================================================
PPC_FUNC(__imp__DbgPrint) {
    uint32_t fmt_addr = ctx.r3.u32;
    char* fmt = reinterpret_cast<char*>(BASE + fmt_addr);
    printf("[GAME] %s", fmt);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__RtlInitAnsiString) {
    uint32_t dest = ctx.r3.u32;
    uint32_t src = ctx.r4.u32;
    
    if (src) {
        char* str = reinterpret_cast<char*>(BASE + src);
        uint16_t len = static_cast<uint16_t>(strlen(str));
        GuestStore32(BASE, dest, (len << 16) | (len + 1));
        GuestStore32(BASE, dest + 4, src);
    }
}

//=============================================================================
// String Functions
//=============================================================================
PPC_FUNC(__imp__sprintf) {
    uint32_t buf_addr = ctx.r3.u32;
    uint32_t fmt_addr = ctx.r4.u32;
    char* buf = reinterpret_cast<char*>(BASE + buf_addr);
    char* fmt = reinterpret_cast<char*>(BASE + fmt_addr);
    ctx.r3.u64 = sprintf(buf, "%s", fmt);
}

PPC_FUNC(__imp___vsnprintf) {
    uint32_t buf_addr = ctx.r3.u32;
    uint32_t count = ctx.r4.u32;
    uint32_t fmt_addr = ctx.r5.u32;
    char* buf = reinterpret_cast<char*>(BASE + buf_addr);
    char* fmt = reinterpret_cast<char*>(BASE + fmt_addr);
    ctx.r3.u64 = snprintf(buf, count, "%s", fmt);
}

//=============================================================================
// File I/O (stubs)
//=============================================================================
PPC_FUNC(__imp__NtCreateFile) {
    printf("[STUB] NtCreateFile\n");
    ctx.r3.u64 = 0xC0000034;  // STATUS_OBJECT_NAME_NOT_FOUND
}

PPC_FUNC(__imp__NtOpenFile) {
    printf("[STUB] NtOpenFile\n");
    ctx.r3.u64 = 0xC0000034;
}

PPC_FUNC(__imp__NtReadFile) {
    printf("[STUB] NtReadFile\n");
    ctx.r3.u64 = 0xC0000034;
}

PPC_FUNC(__imp__NtWriteFile) {
    printf("[STUB] NtWriteFile\n");
    ctx.r3.u64 = 0xC0000034;
}

PPC_FUNC(__imp__NtQueryInformationFile) {
    printf("[STUB] NtQueryInformationFile\n");
    ctx.r3.u64 = 0xC0000034;
}

//=============================================================================
// XAM Stubs
//=============================================================================
PPC_FUNC(__imp__XamLoaderLaunchTitle) {
    printf("[STUB] XamLoaderLaunchTitle\n");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XamShowMessageBoxUI) {
    printf("[STUB] XamShowMessageBoxUI\n");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XGetVideoMode) {
    uint32_t mode_ptr = ctx.r3.u32;
    GuestStore32(BASE, mode_ptr + 0, 1280);
    GuestStore32(BASE, mode_ptr + 4, 720);
    GuestStore32(BASE, mode_ptr + 8, 1);
    ctx.r3.u64 = 1;
}

//=============================================================================
// Additional Stubs (from kernel_impl.cpp)
//=============================================================================

// Spinlocks
PPC_FUNC(__imp__KfAcquireSpinLock) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfReleaseSpinLock) {}
PPC_FUNC(__imp__KeAcquireSpinLockAtRaisedIrql) {}
PPC_FUNC(__imp__KeReleaseSpinLockFromRaisedIrql) {}
PPC_FUNC(__imp__KeTryToAcquireSpinLockAtRaisedIrql) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__KeRaiseIrqlToDpcLevel) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfLowerIrql) {}
PPC_FUNC(__imp__KeLockL2) {}
PPC_FUNC(__imp__KeUnlockL2) {}
PPC_FUNC(__imp__KeEnterCriticalRegion) {}
PPC_FUNC(__imp__KeLeaveCriticalRegion) {}

// Memory extras
PPC_FUNC(__imp__NtQueryVirtualMemory) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtProtectVirtualMemory) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__MmQueryAllocationSize) { ctx.r3.u64 = 0x10000; }
PPC_FUNC(__imp__MmQueryStatistics) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__MmSetAddressProtect) { ctx.r3.u64 = 0; }

// File I/O extras
PPC_FUNC(__imp__NtSetInformationFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtQueryDirectoryFile) { ctx.r3.u64 = 0xC000000F; }
PPC_FUNC(__imp__NtQueryFullAttributesFile) { ctx.r3.u64 = 0xC000000F; }
PPC_FUNC(__imp__NtQueryVolumeInformationFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtFlushBuffersFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtDuplicateObject) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtReadFileScatter) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtDeviceIoControlFile) { ctx.r3.u64 = 0; }

// Object Manager extras
PPC_FUNC(__imp__ObOpenObjectByPointer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObLookupThreadByThreadId) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObCreateSymbolicLink) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObDeleteSymbolicLink) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObIsTitleObject) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__ObReferenceObject) { ctx.r3.u64 = 0; }

// Time extras
PPC_FUNC(__imp__KeQueryInterruptTime) {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    ctx.r3.u64 = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() / 100;
}

// Debug
PPC_FUNC(__imp__DbgBreakPoint) {}
PPC_FUNC(__imp__KeBugCheck) { printf("[KERNEL] BugCheck(0x%X)\n", ctx.r3.u32); }
PPC_FUNC(__imp__KeBugCheckEx) { printf("[KERNEL] BugCheckEx(0x%X)\n", ctx.r3.u32); }
PPC_FUNC(__imp__KiApcNormalRoutineNop) {}

// RTL extras
PPC_FUNC(__imp__RtlFreeAnsiString) {}
PPC_FUNC(__imp__RtlInitUnicodeString) {}
PPC_FUNC(__imp__RtlUnicodeStringToAnsiString) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlMultiByteToUnicodeN) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlUnicodeToMultiByteN) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlUpcaseUnicodeChar) { ctx.r3.u64 = ctx.r3.u32; }
PPC_FUNC(__imp__RtlCompareStringN) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlCompareMemoryUlong) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlFillMemoryUlong) {}
PPC_FUNC(__imp__RtlNtStatusToDosError) { ctx.r3.u64 = ctx.r3.u32 & 0xFFFF; }
PPC_FUNC(__imp__RtlTimeFieldsToTime) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__RtlTimeToTimeFields) {}
PPC_FUNC(__imp__RtlCaptureContext) {}
PPC_FUNC(__imp__RtlRaiseException) {}
PPC_FUNC(__imp__RtlUnwind) {}
PPC_FUNC(__imp__RtlImageXexHeaderField) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp____C_specific_handler) { ctx.r3.u64 = 0; }

// XEX / Config
PPC_FUNC(__imp__XexCheckExecutablePrivilege) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__XexGetModuleHandle) { ctx.r3.u64 = 0x82000000; }
PPC_FUNC(__imp__XexGetProcedureAddress) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ExGetXConfigSetting) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ExRegisterTitleTerminateNotification) {}
PPC_FUNC(__imp__HalReturnToFirmware) {}

static std::atomic<uint32_t> g_pool_alloc{0xB0000000};
PPC_FUNC(__imp__ExAllocatePoolTypeWithTag) {
    uint32_t size = ctx.r4.u32;
    uint32_t addr = g_pool_alloc.fetch_add((size + 0xF) & ~0xF);
    ctx.r3.u64 = addr;
}
PPC_FUNC(__imp__ExFreePool) {}

// Video stubs
PPC_FUNC(__imp__VdCallGraphicsNotificationRoutines) {}
PPC_FUNC(__imp__VdEnableDisableClockGating) {}
PPC_FUNC(__imp__VdEnableRingBufferRPtrWriteBack) {}
PPC_FUNC(__imp__VdGetCurrentDisplayGamma) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdGetCurrentDisplayInformation) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdGetSystemCommandBuffer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdInitializeEngines) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdInitializeRingBuffer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdInitializeScalerCommandBuffer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdIsHSIOTrainingSucceeded) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__VdPersistDisplay) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdQueryVideoFlags) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdQueryVideoMode) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdRetrainEDRAM) {}
PPC_FUNC(__imp__VdRetrainEDRAMWorker) {}
PPC_FUNC(__imp__VdSetDisplayMode) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdSetGraphicsInterruptCallback) {}
PPC_FUNC(__imp__VdSetSystemCommandBufferGpuIdentifierAddress) {}
PPC_FUNC(__imp__VdShutdownEngines) {}
PPC_FUNC(__imp__VdSwap) {}
PPC_FUNC(__imp__XGetAVPack) { ctx.r3.u64 = 0x00010000; }
PPC_FUNC(__imp__XGetGameRegion) { ctx.r3.u64 = 0xFF; }
PPC_FUNC(__imp__XGetLanguage) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__VdGetGraphicsAsicID) { ctx.r3.u64 = 0x58800002; }
PPC_FUNC(__imp__VdInitializeEDRAM) { ctx.r3.u64 = 0; }

// Audio stubs
PPC_FUNC(__imp__XAudioGetSpeakerConfig) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioGetVoiceCategoryVolume) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioGetVoiceCategoryVolumeChangeMask) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioRegisterRenderDriverClient) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioSubmitRenderDriverFrame) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioUnregisterRenderDriverClient) {}
PPC_FUNC(__imp__XMACreateContext) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMAReleaseContext) {}

// XAM stubs
static std::atomic<uint32_t> g_xam_handle{0xC0000000};
PPC_FUNC(__imp__XamAlloc) {
    uint32_t size = ctx.r4.u32;
    uint32_t addr = g_pool_alloc.fetch_add((size + 0xF) & ~0xF);
    uint32_t ptr_ptr = ctx.r5.u32;
    if (ptr_ptr) GuestStore32(BASE, ptr_ptr, addr);
    ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__XamFree) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentClose) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentCreateEnumerator) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentCreateEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentGetCreator) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentGetDeviceData) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentSetThumbnail) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamCreateEnumeratorHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamEnableInactivityProcessing) {}
PPC_FUNC(__imp__XamEnumerate) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamGetExecutionId) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamGetPrivateEnumStructureFromHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamGetSystemVersion) { ctx.r3.u64 = 0x20000000; }
PPC_FUNC(__imp__XamInputGetCapabilities) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamInputGetKeystrokeEx) { ctx.r3.u64 = 0x80000015; }
PPC_FUNC(__imp__XamInputGetState) { ctx.r3.u64 = 0x80000015; }
PPC_FUNC(__imp__XamInputSetState) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamLoaderTerminateTitle) {}
PPC_FUNC(__imp__XamNotifyCreateListener) { ctx.r3.u64 = g_xam_handle++; }
PPC_FUNC(__imp__XamResetInactivity) {}
PPC_FUNC(__imp__XamSessionCreateHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamSessionRefObjByHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowDeviceSelectorUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowDirtyDiscErrorUI) {}
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
PPC_FUNC(__imp__XamUserGetSigninState) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__XamUserGetXUID) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserReadProfileSettings) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserWriteProfileSettings) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamVoiceClose) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamVoiceCreate) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamVoiceHeadsetPresent) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamVoiceSubmitPacket) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentCreate) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentDelete) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowAchievementsUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowFriendsUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamWriteGamerTile) { ctx.r3.u64 = 0; }

// Networking stubs
PPC_FUNC(__imp__NetDll_WSACleanup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_WSAGetLastError) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_WSAStartup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetCleanup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetGetConnectStatus) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetGetEthernetLinkStatus) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__NetDll_XNetGetTitleXnAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetQosListen) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetQosLookup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetQosRelease) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetServerToInAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetStartup) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetUnregisterInAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetXnAddrToInAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_XNetInAddrToXnAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_accept) { ctx.r3.u64 = static_cast<uint32_t>(-1); }
PPC_FUNC(__imp__NetDll_bind) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_closesocket) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_connect) { ctx.r3.u64 = static_cast<uint32_t>(-1); }
PPC_FUNC(__imp__NetDll_getsockname) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_inet_addr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_ioctlsocket) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_listen) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_recv) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_recvfrom) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_select) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_send) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_sendto) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_setsockopt) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_shutdown) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NetDll_socket) { ctx.r3.u64 = g_xam_handle++; }

// Misc
PPC_FUNC(__imp__XeCryptSha) {}
PPC_FUNC(__imp__XeKeysConsolePrivateKeySign) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XeKeysConsoleSignatureVerification) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgCancelIORequest) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgInProcessCall) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgStartIORequest) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgStartIORequestEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XNotifyGetNext) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XNotifyPositionUI) {}
PPC_FUNC(__imp__Refresh) { ctx.r3.u64 = 0; }

// I/O stubs
PPC_FUNC(__imp__IoCheckShareAccess) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoCompleteRequest) {}
PPC_FUNC(__imp__IoCreateDevice) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoDeleteDevice) {}
PPC_FUNC(__imp__IoDismountVolume) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoDismountVolumeByFileHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoInvalidDeviceRequest) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoRemoveShareAccess) {}
PPC_FUNC(__imp__IoSetShareAccess) {}
PPC_FUNC(__imp__StfsControlDevice) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__StfsCreateDevice) { ctx.r3.u64 = 0; }

// Timer
PPC_FUNC(__imp__NtCreateTimer) {
    uint32_t handle_ptr = ctx.r3.u32;
    if (handle_ptr) GuestStore32(BASE, handle_ptr, g_xam_handle++);
    ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__NtSetTimerEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtCancelTimer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtSetInformationThread) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeQueryBasePriorityThread) { ctx.r3.u64 = 0; }
