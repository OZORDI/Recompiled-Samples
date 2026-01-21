/**
 * Kernel Implementation Stubs for Table Tennis
 * Shared kernel exports for all RAGE games
 */

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <atomic>

#include "ppc_config.h"
#include "../ppc_context.h"
#include "../../rage_runtime/rage_runtime.h"

#define KERNEL_LOG(fmt, ...) printf("[KERNEL] " fmt "\n", ##__VA_ARGS__)
#define STUB_LOG(name) printf("[STUB] %s\n", name)

// Helper macros for guest memory access
#define GUEST_LOAD32(addr) RageRuntime::GuestLoad32(base, addr)
#define GUEST_STORE32(addr, val) RageRuntime::GuestStore32(base, addr, val)

// Thread tracking
static std::mutex g_thread_mutex;
static std::atomic<uint32_t> g_next_thread_id{0x1000};
static std::atomic<uint32_t> g_next_handle{0x80000000};

// ============================================================================
// Threading
// ============================================================================

PPC_FUNC(__imp__ExCreateThread) {
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t stack_size = ctx.r4.u32;
    uint32_t thread_id_ptr = ctx.r5.u32;
    uint32_t xapi_startup = ctx.r6.u32;
    uint32_t start_address = ctx.r7.u32;
    uint32_t start_context = ctx.r8.u32;
    uint32_t flags = ctx.r9.u32;
    
    uint32_t tid = g_next_thread_id++;
    KERNEL_LOG("ExCreateThread(start=0x%08X, ctx=0x%08X, flags=0x%X) -> tid %u",
               start_address, start_context, flags, tid);
    
    if (thread_id_ptr) GUEST_STORE32(thread_id_ptr, tid);
    if (handle_ptr) GUEST_STORE32(handle_ptr, tid);
    
    ctx.r3.u64 = 0;  // STATUS_SUCCESS
}

PPC_FUNC(__imp__ExTerminateThread) {
    uint32_t exit_code = ctx.r3.u32;
    KERNEL_LOG("ExTerminateThread(0x%X)", exit_code);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtResumeThread) {
    uint32_t handle = ctx.r3.u32;
    uint32_t suspend_count_ptr = ctx.r4.u32;
    KERNEL_LOG("NtResumeThread(0x%X)", handle);
    if (suspend_count_ptr) GUEST_STORE32(suspend_count_ptr, 1);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtSuspendThread) {
    STUB_LOG("NtSuspendThread");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeResumeThread) {
    STUB_LOG("KeResumeThread");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetBasePriorityThread) {
    STUB_LOG("KeSetBasePriorityThread");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetAffinityThread) {
    STUB_LOG("KeSetAffinityThread");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetDisableBoostThread) {
    STUB_LOG("KeSetDisableBoostThread");
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeGetCurrentProcessType) {
    ctx.r3.u64 = 1;  // USER process
}

// ============================================================================
// Synchronization
// ============================================================================

PPC_FUNC(__imp__NtCreateEvent) {
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t handle = g_next_handle++;
    if (handle_ptr) GUEST_STORE32(handle_ptr, handle);
    KERNEL_LOG("NtCreateEvent -> 0x%X", handle);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtCreateMutant) {
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t handle = g_next_handle++;
    if (handle_ptr) GUEST_STORE32(handle_ptr, handle);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtCreateSemaphore) {
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t handle = g_next_handle++;
    if (handle_ptr) GUEST_STORE32(handle_ptr, handle);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetEvent) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeResetEvent) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KePulseEvent) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtSetEvent) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtClearEvent) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtReleaseMutant) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtReleaseSemaphore) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeInitializeEvent) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeInitializeSemaphore) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeReleaseSemaphore) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeInitializeMutant) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeReleaseMutant) { ctx.r3.u64 = 0; }

// ============================================================================
// Wait Functions
// ============================================================================

PPC_FUNC(__imp__KeWaitForSingleObject) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeWaitForMultipleObjects) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtWaitForSingleObjectEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtWaitForMultipleObjectsEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtSignalAndWaitForSingleObjectEx) { ctx.r3.u64 = 0; }

PPC_FUNC(__imp__KeDelayExecutionThread) {
    uint32_t interval_ptr = ctx.r5.u32;
    if (interval_ptr) {
        int64_t interval = static_cast<int64_t>(
            RageRuntime::bswap64(*reinterpret_cast<uint64_t*>(base + interval_ptr)));
        if (interval < 0) {
            int64_t ms = (-interval) / 10000;
            if (ms > 0 && ms < 10000) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            }
        }
    }
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtYieldExecution) {
    std::this_thread::yield();
    ctx.r3.u64 = 0;
}

// ============================================================================
// Critical Sections
// ============================================================================

PPC_FUNC(__imp__RtlInitializeCriticalSection) {
    uint32_t cs_ptr = ctx.r3.u32;
    if (cs_ptr) {
        GUEST_STORE32(cs_ptr + 0x00, 0);
        GUEST_STORE32(cs_ptr + 0x04, static_cast<uint32_t>(-1));
        GUEST_STORE32(cs_ptr + 0x08, 0);
        GUEST_STORE32(cs_ptr + 0x0C, 0);
    }
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__RtlInitializeCriticalSectionAndSpinCount) {
    uint32_t cs_ptr = ctx.r3.u32;
    if (cs_ptr) {
        GUEST_STORE32(cs_ptr + 0x00, 0);
        GUEST_STORE32(cs_ptr + 0x04, static_cast<uint32_t>(-1));
    }
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__RtlEnterCriticalSection) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlLeaveCriticalSection) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlTryEnterCriticalSection) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__KeEnterCriticalRegion) {}
PPC_FUNC(__imp__KeLeaveCriticalRegion) {}

// ============================================================================
// Spinlocks & IRQL
// ============================================================================

PPC_FUNC(__imp__KfAcquireSpinLock) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfReleaseSpinLock) {}
PPC_FUNC(__imp__KeAcquireSpinLockAtRaisedIrql) {}
PPC_FUNC(__imp__KeReleaseSpinLockFromRaisedIrql) {}
PPC_FUNC(__imp__KeTryToAcquireSpinLockAtRaisedIrql) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__KeRaiseIrqlToDpcLevel) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfLowerIrql) {}
PPC_FUNC(__imp__KeLockL2) {}
PPC_FUNC(__imp__KeUnlockL2) {}

// ============================================================================
// Memory
// ============================================================================

static std::atomic<uint32_t> g_phys_alloc{0xA0000000};

PPC_FUNC(__imp__NtAllocateVirtualMemory) {
    uint32_t size_ptr = ctx.r5.u32;
    uint32_t size = size_ptr ? GUEST_LOAD32(size_ptr) : 0x10000;
    KERNEL_LOG("NtAllocateVirtualMemory(size=0x%X)", size);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtFreeVirtualMemory) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtQueryVirtualMemory) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtProtectVirtualMemory) { ctx.r3.u64 = 0; }

PPC_FUNC(__imp__MmAllocatePhysicalMemoryEx) {
    uint32_t size = ctx.r4.u32;
    uint32_t aligned = (size + 0xFFF) & ~0xFFF;
    uint32_t addr = g_phys_alloc.fetch_add(aligned);
    KERNEL_LOG("MmAllocatePhysicalMemoryEx(0x%X) -> 0x%08X", size, addr);
    ctx.r3.u64 = addr;
}

PPC_FUNC(__imp__MmFreePhysicalMemory) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__MmGetPhysicalAddress) { ctx.r3.u64 = ctx.r3.u32; }
PPC_FUNC(__imp__MmQueryAddressProtect) { ctx.r3.u64 = 0x04; }
PPC_FUNC(__imp__MmQueryAllocationSize) { ctx.r3.u64 = 0x10000; }
PPC_FUNC(__imp__MmQueryStatistics) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__MmSetAddressProtect) { ctx.r3.u64 = 0; }

// ============================================================================
// File I/O
// ============================================================================

PPC_FUNC(__imp__NtCreateFile) {
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t handle = g_next_handle++;
    if (handle_ptr) GUEST_STORE32(handle_ptr, handle);
    KERNEL_LOG("NtCreateFile -> 0x%X", handle);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtOpenFile) {
    uint32_t handle_ptr = ctx.r3.u32;
    uint32_t handle = g_next_handle++;
    if (handle_ptr) GUEST_STORE32(handle_ptr, handle);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtReadFile) { ctx.r3.u64 = 0xC0000011; }  // EOF
PPC_FUNC(__imp__NtWriteFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtClose) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtQueryInformationFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtSetInformationFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtQueryDirectoryFile) { ctx.r3.u64 = 0xC000000F; }
PPC_FUNC(__imp__NtQueryFullAttributesFile) { ctx.r3.u64 = 0xC000000F; }
PPC_FUNC(__imp__NtQueryVolumeInformationFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtFlushBuffersFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtDuplicateObject) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtReadFileScatter) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtDeviceIoControlFile) { ctx.r3.u64 = 0; }

// ============================================================================
// Object Manager
// ============================================================================

PPC_FUNC(__imp__ObReferenceObjectByHandle) {
    uint32_t handle = ctx.r3.u32;
    uint32_t obj_ptr_ptr = ctx.r5.u32;
    if (obj_ptr_ptr) GUEST_STORE32(obj_ptr_ptr, handle);
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__ObDereferenceObject) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObOpenObjectByPointer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObLookupThreadByThreadId) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObCreateSymbolicLink) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObDeleteSymbolicLink) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObIsTitleObject) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__ObReferenceObject) { ctx.r3.u64 = 0; }

// ============================================================================
// Time
// ============================================================================

PPC_FUNC(__imp__KeQueryPerformanceFrequency) {
    ctx.r3.u64 = 50000000;  // 50 MHz
}

PPC_FUNC(__imp__KeQuerySystemTime) {
    uint32_t time_ptr = ctx.r3.u32;
    if (time_ptr) {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        int64_t ft = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() / 100;
        ft += 116444736000000000LL;
        *reinterpret_cast<uint64_t*>(base + time_ptr) = RageRuntime::bswap64(ft);
    }
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeQueryInterruptTime) {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    ctx.r3.u64 = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() / 100;
}

// ============================================================================
// TLS
// ============================================================================

static std::atomic<uint32_t> g_next_tls{1};
static thread_local std::unordered_map<uint32_t, uint64_t> g_tls_values;

PPC_FUNC(__imp__KeTlsAlloc) { ctx.r3.u64 = g_next_tls++; }
PPC_FUNC(__imp__KeTlsFree) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__KeTlsGetValue) {
    uint32_t slot = ctx.r3.u32;
    auto it = g_tls_values.find(slot);
    ctx.r3.u64 = (it != g_tls_values.end()) ? it->second : 0;
}
PPC_FUNC(__imp__KeTlsSetValue) {
    g_tls_values[ctx.r3.u32] = ctx.r4.u64;
    ctx.r3.u64 = 1;
}

// ============================================================================
// Debug
// ============================================================================

PPC_FUNC(__imp__DbgPrint) {
    uint32_t fmt_ptr = ctx.r3.u32;
    if (fmt_ptr) printf("[GAME] %s", reinterpret_cast<char*>(base + fmt_ptr));
    ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__DbgBreakPoint) {}
PPC_FUNC(__imp__KeBugCheck) { KERNEL_LOG("BugCheck(0x%X)", ctx.r3.u32); }
PPC_FUNC(__imp__KeBugCheckEx) { KERNEL_LOG("BugCheckEx(0x%X)", ctx.r3.u32); }

// ============================================================================
// RTL
// ============================================================================

PPC_FUNC(__imp__RtlInitAnsiString) {
    uint32_t dest = ctx.r3.u32;
    uint32_t src = ctx.r4.u32;
    if (dest && src) {
        uint16_t len = static_cast<uint16_t>(strlen(reinterpret_cast<char*>(base + src)));
        *reinterpret_cast<uint16_t*>(base + dest) = RageRuntime::bswap16(len);
        *reinterpret_cast<uint16_t*>(base + dest + 2) = RageRuntime::bswap16(len + 1);
        GUEST_STORE32(dest + 4, src);
    }
}

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
PPC_FUNC(__imp__KiApcNormalRoutineNop) {}
PPC_FUNC(__imp__sprintf) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp___vsnprintf) { ctx.r3.u64 = 0; }

// ============================================================================
// XEX / Config
// ============================================================================

PPC_FUNC(__imp__XexCheckExecutablePrivilege) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__XexGetModuleHandle) { ctx.r3.u64 = 0x82000000; }
PPC_FUNC(__imp__XexGetProcedureAddress) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ExGetXConfigSetting) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ExRegisterTitleTerminateNotification) {}
PPC_FUNC(__imp__HalReturnToFirmware) {}
PPC_FUNC(__imp__ExAllocatePoolTypeWithTag) {
    uint32_t size = ctx.r4.u32;
    uint32_t addr = g_phys_alloc.fetch_add((size + 0xF) & ~0xF);
    ctx.r3.u64 = addr;
}
PPC_FUNC(__imp__ExFreePool) {}

// ============================================================================
// Video (stubs - rendering handled externally)
// ============================================================================

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
PPC_FUNC(__imp__XGetVideoMode) {}
PPC_FUNC(__imp__XGetAVPack) { ctx.r3.u64 = 0x00010000; }
PPC_FUNC(__imp__XGetGameRegion) { ctx.r3.u64 = 0xFF; }
PPC_FUNC(__imp__XGetLanguage) { ctx.r3.u64 = 1; }

// ============================================================================
// Audio (stubs)
// ============================================================================

PPC_FUNC(__imp__XAudioGetSpeakerConfig) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioGetVoiceCategoryVolume) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioGetVoiceCategoryVolumeChangeMask) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioRegisterRenderDriverClient) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioSubmitRenderDriverFrame) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XAudioUnregisterRenderDriverClient) {}
PPC_FUNC(__imp__XMACreateContext) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMAReleaseContext) {}

// ============================================================================
// XAM (stubs)
// ============================================================================

PPC_FUNC(__imp__XamAlloc) {
    uint32_t size = ctx.r4.u32;
    uint32_t addr = g_phys_alloc.fetch_add((size + 0xF) & ~0xF);
    uint32_t ptr_ptr = ctx.r5.u32;
    if (ptr_ptr) GUEST_STORE32(ptr_ptr, addr);
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
PPC_FUNC(__imp__XamLoaderLaunchTitle) {}
PPC_FUNC(__imp__XamLoaderTerminateTitle) {}
PPC_FUNC(__imp__XamNotifyCreateListener) { ctx.r3.u64 = g_next_handle++; }
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

// ============================================================================
// Networking (stubs)
// ============================================================================

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
PPC_FUNC(__imp__NetDll_socket) { ctx.r3.u64 = g_next_handle++; }

// ============================================================================
// Misc
// ============================================================================

PPC_FUNC(__imp__XeCryptSha) {}
PPC_FUNC(__imp__XeKeysConsolePrivateKeySign) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XeKeysConsoleSignatureVerification) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgCancelIORequest) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgInProcessCall) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgStartIORequest) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XMsgStartIORequestEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XNotifyGetNext) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XNotifyPositionUI) {}

// Additional stubs for Table Tennis
PPC_FUNC(__imp__NetDll_XNetInAddrToXnAddr) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__Refresh) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__VdGetGraphicsAsicID) { ctx.r3.u64 = 0x58800002; }  // Xenos GPU ID
PPC_FUNC(__imp__VdInitializeEDRAM) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentCreate) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamContentDelete) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowAchievementsUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamShowFriendsUI) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamWriteGamerTile) { ctx.r3.u64 = 0; }

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
    if (handle_ptr) GUEST_STORE32(handle_ptr, g_next_handle++);
    ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__NtSetTimerEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtCancelTimer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtSetInformationThread) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeQueryBasePriorityThread) { ctx.r3.u64 = 0; }
