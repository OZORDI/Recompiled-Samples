/**
 * Kernel Bridge for XeniaRT
 * 
 * This file provides __imp__ functions using the local PPCContext type
 * and calls into xeniart's kernel API via C-linkage functions.
 * 
 * The local PPCContext (from ppc_context.h) has registers laid out differently
 * from Xenia's PPCContext, so we need to copy values between them.
 */

#include "ppc_config.h"
#include "ppc_context.h"

#include <cstdio>
#include <cstring>

// C-linkage function to call Xenia kernel export
// Takes register values as individual parameters and returns result in r3/r4
extern "C" void xeniart_call_kernel_ex(const char* name,
                                       uint64_t r3, uint64_t r4, uint64_t r5,
                                       uint64_t r6, uint64_t r7, uint64_t r8,
                                       uint64_t r9, uint64_t r10,
                                       uint64_t* out_r3, uint64_t* out_r4);

// Debug: dump all argument registers before kernel call
static void dump_ctx_regs(const char* name, PPCContext& ctx) {
  static int dump_count = 0;
  if (dump_count++ < 5) {
    printf("[KERNEL_BRIDGE] %s ctx regs:\n", name);
    printf("  r3=%016llx r4=%016llx r5=%016llx r6=%016llx\n",
           ctx.r3.u64, ctx.r4.u64, ctx.r5.u64, ctx.r6.u64);
    printf("  r7=%016llx r8=%016llx r9=%016llx r10=%016llx\n",
           ctx.r7.u64, ctx.r8.u64, ctx.r9.u64, ctx.r10.u64);
    printf("  r1(sp)=%016llx r13=%016llx\n", ctx.r1.u64, ctx.r13.u64);
  }
}

// Macro to define kernel exports that forward to xeniart
#define KERNEL_EXPORT(name)                                              \
  PPC_FUNC(__imp__##name) {                                              \
    dump_ctx_regs(#name, ctx);                                           \
    uint64_t out_r3 = 0, out_r4 = 0;                                     \
    xeniart_call_kernel_ex(#name,                                        \
                           ctx.r3.u64, ctx.r4.u64, ctx.r5.u64,           \
                           ctx.r6.u64, ctx.r7.u64, ctx.r8.u64,           \
                           ctx.r9.u64, ctx.r10.u64,                      \
                           &out_r3, &out_r4);                            \
    ctx.r3.u64 = out_r3;                                                 \
    ctx.r4.u64 = out_r4;                                                 \
  }

// Threading
KERNEL_EXPORT(ExCreateThread)
KERNEL_EXPORT(ExTerminateThread)
KERNEL_EXPORT(NtResumeThread)
KERNEL_EXPORT(NtSuspendThread)
KERNEL_EXPORT(KeSetBasePriorityThread)
KERNEL_EXPORT(KeSetAffinityThread)
KERNEL_EXPORT(KeGetCurrentProcessType)
KERNEL_EXPORT(KeDelayExecutionThread)
KERNEL_EXPORT(NtYieldExecution)
KERNEL_EXPORT(KeQuerySystemTime)
KERNEL_EXPORT(KeQueryPerformanceFrequency)
KERNEL_EXPORT(KeResumeThread)
KERNEL_EXPORT(KeSetDisableBoostThread)

// TLS
KERNEL_EXPORT(KeTlsAlloc)
KERNEL_EXPORT(KeTlsFree)
KERNEL_EXPORT(KeTlsGetValue)
KERNEL_EXPORT(KeTlsSetValue)

// Synchronization - Events
KERNEL_EXPORT(KeInitializeEvent)
KERNEL_EXPORT(KeSetEvent)
KERNEL_EXPORT(KeResetEvent)
KERNEL_EXPORT(NtCreateEvent)
KERNEL_EXPORT(NtSetEvent)
KERNEL_EXPORT(NtClearEvent)
KERNEL_EXPORT(KeWaitForSingleObject)
KERNEL_EXPORT(KeWaitForMultipleObjects)
KERNEL_EXPORT(NtWaitForSingleObjectEx)
KERNEL_EXPORT(NtWaitForMultipleObjectsEx)

// Synchronization - Critical Sections
KERNEL_EXPORT(RtlInitializeCriticalSection)
KERNEL_EXPORT(RtlInitializeCriticalSectionAndSpinCount)
KERNEL_EXPORT(RtlEnterCriticalSection)
KERNEL_EXPORT(RtlLeaveCriticalSection)
KERNEL_EXPORT(RtlTryEnterCriticalSection)
KERNEL_EXPORT(KeEnterCriticalRegion)
KERNEL_EXPORT(KeLeaveCriticalRegion)

// Synchronization - Semaphores
KERNEL_EXPORT(KeInitializeSemaphore)
KERNEL_EXPORT(NtCreateSemaphore)
KERNEL_EXPORT(NtReleaseSemaphore)
KERNEL_EXPORT(KeReleaseSemaphore)

// Synchronization - Mutants
KERNEL_EXPORT(NtCreateMutant)
KERNEL_EXPORT(NtReleaseMutant)

// Synchronization - Spinlocks
KERNEL_EXPORT(KeAcquireSpinLockAtRaisedIrql)
KERNEL_EXPORT(KeReleaseSpinLockFromRaisedIrql)
KERNEL_EXPORT(KeTryToAcquireSpinLockAtRaisedIrql)
KERNEL_EXPORT(KeRaiseIrqlToDpcLevel)
KERNEL_EXPORT(KfAcquireSpinLock)
KERNEL_EXPORT(KfReleaseSpinLock)
KERNEL_EXPORT(KfLowerIrql)
KERNEL_EXPORT(KeLockL2)
KERNEL_EXPORT(KeUnlockL2)

// Memory
KERNEL_EXPORT(NtAllocateVirtualMemory)
KERNEL_EXPORT(NtFreeVirtualMemory)
KERNEL_EXPORT(NtQueryVirtualMemory)
KERNEL_EXPORT(NtProtectVirtualMemory)
KERNEL_EXPORT(MmAllocatePhysicalMemoryEx)
KERNEL_EXPORT(MmFreePhysicalMemory)
KERNEL_EXPORT(MmQueryAddressProtect)
KERNEL_EXPORT(MmGetPhysicalAddress)
KERNEL_EXPORT(MmQueryAllocationSize)
KERNEL_EXPORT(ExAllocatePoolTypeWithTag)
KERNEL_EXPORT(ExFreePool)

// File I/O
KERNEL_EXPORT(NtCreateFile)
KERNEL_EXPORT(NtOpenFile)
KERNEL_EXPORT(NtReadFile)
KERNEL_EXPORT(NtWriteFile)
KERNEL_EXPORT(NtClose)
KERNEL_EXPORT(NtQueryInformationFile)
KERNEL_EXPORT(NtSetInformationFile)
KERNEL_EXPORT(NtQueryVolumeInformationFile)
KERNEL_EXPORT(NtQueryDirectoryFile)
KERNEL_EXPORT(NtQueryFullAttributesFile)
KERNEL_EXPORT(NtDeviceIoControlFile)
KERNEL_EXPORT(NtFlushBuffersFile)
KERNEL_EXPORT(NtReadFileScatter)

// Timers
KERNEL_EXPORT(NtCancelTimer)
KERNEL_EXPORT(NtCreateTimer)
KERNEL_EXPORT(NtSetTimerEx)

// Object Management
KERNEL_EXPORT(ObReferenceObjectByHandle)
KERNEL_EXPORT(ObDereferenceObject)
KERNEL_EXPORT(NtDuplicateObject)
KERNEL_EXPORT(KeGetCurrentThread)
KERNEL_EXPORT(ObCreateSymbolicLink)
KERNEL_EXPORT(ObDeleteSymbolicLink)
KERNEL_EXPORT(ObIsTitleObject)
KERNEL_EXPORT(ObReferenceObject)

// Debug
KERNEL_EXPORT(DbgPrint)
KERNEL_EXPORT(KeBugCheck)
KERNEL_EXPORT(KeBugCheckEx)
KERNEL_EXPORT(RtlRaiseException)

// XConfig
KERNEL_EXPORT(ExGetXConfigSetting)
KERNEL_EXPORT(ExRegisterTitleTerminateNotification)

// Xex
KERNEL_EXPORT(XexGetModuleHandle)
KERNEL_EXPORT(XexGetProcedureAddress)
KERNEL_EXPORT(XexCheckExecutablePrivilege)

// HAL
KERNEL_EXPORT(HalReturnToFirmware)

// I/O Manager
KERNEL_EXPORT(IoCheckShareAccess)
KERNEL_EXPORT(IoCompleteRequest)
KERNEL_EXPORT(IoCreateDevice)
KERNEL_EXPORT(IoDeleteDevice)
KERNEL_EXPORT(IoDismountVolume)
KERNEL_EXPORT(IoDismountVolumeByFileHandle)
KERNEL_EXPORT(IoInvalidDeviceRequest)
KERNEL_EXPORT(IoRemoveShareAccess)
KERNEL_EXPORT(IoSetShareAccess)

// RTL
KERNEL_EXPORT(RtlCaptureContext)
KERNEL_EXPORT(RtlCompareMemoryUlong)
KERNEL_EXPORT(RtlCompareStringN)
KERNEL_EXPORT(RtlFillMemoryUlong)
KERNEL_EXPORT(RtlFreeAnsiString)
KERNEL_EXPORT(RtlImageXexHeaderField)
KERNEL_EXPORT(RtlInitAnsiString)
KERNEL_EXPORT(RtlInitUnicodeString)
KERNEL_EXPORT(RtlMultiByteToUnicodeN)
KERNEL_EXPORT(RtlNtStatusToDosError)
KERNEL_EXPORT(RtlTimeFieldsToTime)
KERNEL_EXPORT(RtlTimeToTimeFields)
KERNEL_EXPORT(RtlUnicodeStringToAnsiString)
KERNEL_EXPORT(RtlUnicodeToMultiByteN)
KERNEL_EXPORT(RtlUnwind)
KERNEL_EXPORT(RtlUpcaseUnicodeChar)

// STFS
KERNEL_EXPORT(StfsControlDevice)
KERNEL_EXPORT(StfsCreateDevice)

// X* functions
KERNEL_EXPORT(XGetAVPack)
KERNEL_EXPORT(XGetGameRegion)
KERNEL_EXPORT(XGetLanguage)
KERNEL_EXPORT(XGetVideoMode)
KERNEL_EXPORT(XMsgCancelIORequest)
KERNEL_EXPORT(XMsgInProcessCall)
KERNEL_EXPORT(XMsgStartIORequest)
KERNEL_EXPORT(XMsgStartIORequestEx)
KERNEL_EXPORT(XNotifyGetNext)
KERNEL_EXPORT(XNotifyPositionUI)

// Crypto
KERNEL_EXPORT(XeCryptSha)
KERNEL_EXPORT(XeKeysConsolePrivateKeySign)
KERNEL_EXPORT(XeKeysConsoleSignatureVerification)

// C++ exception handling
KERNEL_EXPORT(__C_specific_handler)
KERNEL_EXPORT(KiApcNormalRoutineNop)

// XAM
KERNEL_EXPORT(XamAlloc)
KERNEL_EXPORT(XamFree)
KERNEL_EXPORT(XamLoaderGetLaunchDataSize)
KERNEL_EXPORT(XamLoaderGetLaunchData)
KERNEL_EXPORT(XamShowMessageBoxUI)
KERNEL_EXPORT(XamShowKeyboardUI)
KERNEL_EXPORT(XamUserGetSigninState)
KERNEL_EXPORT(XamUserGetName)
KERNEL_EXPORT(XamUserGetXUID)
KERNEL_EXPORT(XamContentCreateEnumerator)
KERNEL_EXPORT(XamContentCreate)
KERNEL_EXPORT(XamContentClose)
KERNEL_EXPORT(XamContentCreateEx)
KERNEL_EXPORT(XamContentGetCreator)
KERNEL_EXPORT(XamContentGetDeviceData)
KERNEL_EXPORT(XamContentSetThumbnail)
KERNEL_EXPORT(XamCreateEnumeratorHandle)
KERNEL_EXPORT(XamEnableInactivityProcessing)
KERNEL_EXPORT(XamEnumerate)
KERNEL_EXPORT(XamGetExecutionId)
KERNEL_EXPORT(XamGetPrivateEnumStructureFromHandle)
KERNEL_EXPORT(XamGetSystemVersion)
KERNEL_EXPORT(XamLoaderLaunchTitle)
KERNEL_EXPORT(XamLoaderTerminateTitle)
KERNEL_EXPORT(XamNotifyCreateListener)
KERNEL_EXPORT(XamResetInactivity)
KERNEL_EXPORT(XamSessionCreateHandle)
KERNEL_EXPORT(XamSessionRefObjByHandle)
KERNEL_EXPORT(XamShowDeviceSelectorUI)
KERNEL_EXPORT(XamShowDirtyDiscErrorUI)
KERNEL_EXPORT(XamShowGamerCardUIForXUID)
KERNEL_EXPORT(XamShowMessageBoxUIEx)
KERNEL_EXPORT(XamShowPlayerReviewUI)
KERNEL_EXPORT(XamShowSigninUI)
KERNEL_EXPORT(XamTaskCloseHandle)
KERNEL_EXPORT(XamTaskSchedule)
KERNEL_EXPORT(XamTaskShouldExit)
KERNEL_EXPORT(XamUserAreUsersFriends)
KERNEL_EXPORT(XamUserCheckPrivilege)
KERNEL_EXPORT(XamUserCreateAchievementEnumerator)
KERNEL_EXPORT(XamUserCreateStatsEnumerator)
KERNEL_EXPORT(XamUserGetSigninInfo)
KERNEL_EXPORT(XamUserReadProfileSettings)
KERNEL_EXPORT(XamUserWriteProfileSettings)
KERNEL_EXPORT(XamVoiceClose)
KERNEL_EXPORT(XamVoiceCreate)
KERNEL_EXPORT(XamVoiceHeadsetPresent)
KERNEL_EXPORT(XamVoiceSubmitPacket)

// ============================================================================
// Networking - Stubs (not implemented)
// ============================================================================

#define NET_STUB(name)                                        \
  PPC_FUNC(__imp__##name) {                                   \
    static bool warned = false;                               \
    if (!warned) {                                            \
      printf("[NET] Stub: " #name " (not implemented)\n");    \
      warned = true;                                          \
    }                                                         \
    ctx.r3.u64 = 0;                                           \
  }

NET_STUB(NetDll_WSACleanup)
NET_STUB(NetDll_WSAGetLastError)
NET_STUB(NetDll_WSAStartup)
NET_STUB(NetDll_XNetCleanup)
NET_STUB(NetDll_XNetGetConnectStatus)
NET_STUB(NetDll_XNetGetEthernetLinkStatus)
NET_STUB(NetDll_XNetGetTitleXnAddr)
NET_STUB(NetDll_XNetQosListen)
NET_STUB(NetDll_XNetQosLookup)
NET_STUB(NetDll_XNetQosRelease)
NET_STUB(NetDll_XNetServerToInAddr)
NET_STUB(NetDll_XNetStartup)
NET_STUB(NetDll_XNetUnregisterInAddr)
NET_STUB(NetDll_XNetXnAddrToInAddr)
NET_STUB(NetDll_accept)
NET_STUB(NetDll_bind)
NET_STUB(NetDll_closesocket)
NET_STUB(NetDll_connect)
NET_STUB(NetDll_getsockname)
NET_STUB(NetDll_inet_addr)
NET_STUB(NetDll_ioctlsocket)
NET_STUB(NetDll_listen)
NET_STUB(NetDll_recv)
NET_STUB(NetDll_recvfrom)
NET_STUB(NetDll_select)
NET_STUB(NetDll_send)
NET_STUB(NetDll_sendto)
NET_STUB(NetDll_setsockopt)
NET_STUB(NetDll_shutdown)
NET_STUB(NetDll_socket)

// ============================================================================
// C Runtime - Local implementations
// ============================================================================

PPC_FUNC(__imp__sprintf) {
  uint32_t dest = ctx.r3.u32;
  if (dest && base) {
    *(base + dest) = 0;
  }
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp___vsnprintf) {
  uint32_t buf = ctx.r3.u32;
  uint32_t size = ctx.r4.u32;
  uint32_t fmt = ctx.r5.u32;
  if (buf && fmt && size > 0 && base) {
    std::strncpy(reinterpret_cast<char*>(base + buf),
                 reinterpret_cast<char*>(base + fmt), size - 1);
    base[buf + size - 1] = 0;
    ctx.r3.u64 = std::strlen(reinterpret_cast<char*>(base + buf));
  } else {
    ctx.r3.u64 = 0;
  }
}

#undef KERNEL_EXPORT
#undef NET_STUB
