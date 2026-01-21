// Remaining kernel exports - to be wired to Xenia implementations
// These are temporary stubs until full implementation

#define STUB_TODO(name) PPC_FUNC(__imp__##name) { ctx.r3.u64 = 0; }
#define STUB_RET_TODO(name, ret) PPC_FUNC(__imp__##name) { ctx.r3.u64 = (ret); }

// Pool allocation
STUB_TODO(ExAllocatePoolTypeWithTag)
STUB_TODO(ExFreePool)
STUB_TODO(ExGetXConfigSetting)
STUB_TODO(ExRegisterTitleTerminateNotification)

// HAL
STUB_TODO(HalReturnToFirmware)

// I/O
STUB_TODO(IoCheckShareAccess)
STUB_TODO(IoCompleteRequest)
STUB_TODO(IoCreateDevice)
STUB_TODO(IoDeleteDevice)
STUB_TODO(IoDismountVolume)
STUB_TODO(IoDismountVolumeByFileHandle)
STUB_TODO(IoInvalidDeviceRequest)
STUB_TODO(IoRemoveShareAccess)
STUB_TODO(IoSetShareAccess)

// Spinlocks
STUB_TODO(KeAcquireSpinLockAtRaisedIrql)
STUB_TODO(KeReleaseSpinLockFromRaisedIrql)
STUB_TODO(KeTryToAcquireSpinLockAtRaisedIrql)
STUB_TODO(KfAcquireSpinLock)
STUB_TODO(KfReleaseSpinLock)

// IRQL
STUB_TODO(KeRaiseIrqlToDpcLevel)
STUB_TODO(KfLowerIrql)
STUB_TODO(KeEnterCriticalRegion)
STUB_TODO(KeLeaveCriticalRegion)

// L2 Cache
STUB_TODO(KeLockL2)
STUB_TODO(KeUnlockL2)

// Semaphores
STUB_TODO(KeInitializeSemaphore)
STUB_TODO(KeReleaseSemaphore)
STUB_TODO(NtCreateSemaphore)
STUB_TODO(NtReleaseSemaphore)

// Mutants
STUB_TODO(KeInitializeMutant)
STUB_TODO(KeReleaseMutant)
STUB_TODO(NtCreateMutant)
STUB_TODO(NtReleaseMutant)

// Timers
STUB_TODO(KeInitializeTimerEx)
STUB_TODO(KeCancelTimer)
STUB_TODO(KeSetTimerEx)
STUB_TODO(NtCreateTimer)
STUB_TODO(NtSetTimerEx)
STUB_TODO(NtCancelTimer)

// APC
STUB_TODO(KiApcNormalRoutineNop)

// Resume thread
STUB_TODO(KeResumeThread)

// Memory
STUB_TODO(MmFreePhysicalMemory)
STUB_TODO(MmGetPhysicalAddress)
STUB_TODO(MmIsAddressValid)
STUB_TODO(MmMapIoSpace)
STUB_TODO(MmQueryAllocationSize)
STUB_TODO(MmQueryStatistics)
STUB_TODO(MmSetAddressProtect)
STUB_TODO(MmUnmapIoSpace)

// File I/O
STUB_TODO(NtCreateFile)
STUB_TODO(NtOpenFile)
STUB_TODO(NtReadFile)
STUB_TODO(NtWriteFile)
STUB_TODO(NtReadFileScatter)
STUB_TODO(NtWriteFileGather)
STUB_TODO(NtQueryInformationFile)
STUB_TODO(NtSetInformationFile)
STUB_TODO(NtQueryVolumeInformationFile)
STUB_TODO(NtQueryDirectoryFile)
STUB_TODO(NtQueryFullAttributesFile)
STUB_TODO(NtFlushBuffersFile)
STUB_TODO(NtDeviceIoControlFile)
STUB_TODO(FscSetCacheElementCount)

// Object manager
STUB_TODO(ObCreateObject)
STUB_TODO(ObOpenObjectByName)
STUB_TODO(ObOpenObjectByPointer)
STUB_TODO(ObLookupAnyThreadByThreadId)

// Error handling
STUB_TODO(RtlNtStatusToDosError)
STUB_TODO(RtlImageXexHeaderField)
STUB_TODO(RtlRaiseStatus)

// String functions
STUB_TODO(RtlAnsiStringToUnicodeString)
STUB_TODO(RtlAppendStringToString)
STUB_TODO(RtlCompareMemory)
STUB_TODO(RtlCompareMemoryUlong)
STUB_TODO(RtlCompareString)
STUB_TODO(RtlCompareStringN)
STUB_TODO(RtlCompareUnicodeString)
STUB_TODO(RtlCopyString)
STUB_TODO(RtlCopyUnicodeString)
STUB_TODO(RtlDowncaseUnicodeChar)
STUB_TODO(RtlEqualString)
STUB_TODO(RtlEqualUnicodeString)
STUB_TODO(RtlFillMemoryUlong)
STUB_TODO(RtlFreeAnsiString)
STUB_TODO(RtlFreeUnicodeString)
STUB_TODO(RtlGetLastError)
STUB_TODO(RtlImageXexReleaseSection)
STUB_TODO(RtlInitAnsiString)
STUB_TODO(RtlInitUnicodeString)
STUB_TODO(RtlLowerChar)
STUB_TODO(RtlMultiByteToUnicodeN)
STUB_TODO(RtlMultiByteToUnicodeSize)
STUB_TODO(RtlScprintf)
STUB_TODO(RtlSetLastError)
STUB_TODO(RtlSetLastNTError)
STUB_TODO(RtlSprintf)
STUB_TODO(RtlTimeFieldsToTime)
STUB_TODO(RtlTimeToTimeFields)
STUB_TODO(RtlUnicodeStringToAnsiString)
STUB_TODO(RtlUnicodeToMultiByteN)
STUB_TODO(RtlUnwind)
STUB_TODO(RtlUpcaseUnicodeChar)

// STFS
STUB_TODO(StfsControlDevice)
STUB_TODO(StfsCreateDevice)

// Video/Graphics
STUB_TODO(VdCallGraphicsNotificationRoutines)
STUB_TODO(VdEnableDisableClockGating)
STUB_TODO(VdEnableRingBufferRPtrWriteBack)
STUB_TODO(VdGetCurrentDisplayGamma)
STUB_TODO(VdGetCurrentDisplayInformation)
STUB_TODO(VdGetSystemCommandBuffer)
STUB_TODO(VdInitializeEngines)
STUB_TODO(VdInitializeRingBuffer)
STUB_TODO(VdInitializeScalerCommandBuffer)
STUB_TODO(VdIsHSIOTrainingSucceeded)
STUB_TODO(VdPersistDisplay)
STUB_TODO(VdQueryVideoFlags)
STUB_TODO(VdQueryVideoMode)
STUB_TODO(VdRetrainEDRAM)
STUB_TODO(VdRetrainEDRAMWorker)
STUB_TODO(VdSetDisplayMode)
STUB_TODO(VdSetGraphicsInterruptCallback)
STUB_TODO(VdSetSystemCommandBufferGpuIdentifierAddress)
STUB_TODO(VdShutdownEngines)
STUB_TODO(VdSwap)

// Audio
STUB_TODO(XAudioGetSpeakerConfig)
STUB_TODO(XAudioGetVoiceCategoryVolume)
STUB_TODO(XAudioGetVoiceCategoryVolumeChangeMask)
STUB_TODO(XAudioRegisterRenderDriverClient)
STUB_TODO(XAudioSubmitRenderDriverFrame)
STUB_TODO(XAudioUnregisterRenderDriverClient)

// XMA
STUB_TODO(XMACreateContext)
STUB_TODO(XMAReleaseContext)

// System info
STUB_RET_TODO(XGetAVPack, 6)  // HDMI
STUB_RET_TODO(XGetGameRegion, 0xFF)
STUB_RET_TODO(XGetLanguage, 1)  // English
STUB_TODO(XGetVideoMode)

// XMsg
STUB_TODO(XMsgCancelIORequest)
STUB_TODO(XMsgInProcessCall)
STUB_TODO(XMsgStartIORequest)
STUB_TODO(XMsgStartIORequestEx)

// Notify
STUB_TODO(XNotifyGetNext)
STUB_TODO(XNotifyPositionUI)

// XAM memory
STUB_TODO(XamAlloc)
STUB_TODO(XamFree)

// XAM content
STUB_TODO(XamContentClose)
STUB_TODO(XamContentCreateEnumerator)
STUB_TODO(XamContentCreateEx)
STUB_TODO(XamContentGetCreator)
STUB_TODO(XamContentGetDeviceData)
STUB_TODO(XamContentSetThumbnail)

// XAM enumerator
STUB_TODO(XamCreateEnumeratorHandle)
STUB_TODO(XamEnumerate)
STUB_TODO(XamGetPrivateEnumStructureFromHandle)

// XAM misc
STUB_TODO(XamEnableInactivityProcessing)
STUB_TODO(XamGetExecutionId)
STUB_TODO(XamGetSystemVersion)
STUB_TODO(XamResetInactivity)

// XAM input
STUB_TODO(XamInputGetCapabilities)
STUB_TODO(XamInputGetKeystrokeEx)
STUB_TODO(XamInputGetState)
STUB_TODO(XamInputSetState)

// XAM loader
STUB_TODO(XamLoaderLaunchTitle)
STUB_TODO(XamLoaderTerminateTitle)

// XAM notify
STUB_TODO(XamNotifyCreateListener)

// XAM session
STUB_TODO(XamSessionCreateHandle)
STUB_TODO(XamSessionRefObjByHandle)

// XAM UI
STUB_TODO(XamShowDeviceSelectorUI)
STUB_TODO(XamShowDirtyDiscErrorUI)
STUB_TODO(XamShowGamerCardUIForXUID)
STUB_TODO(XamShowMessageBoxUIEx)
STUB_TODO(XamShowPlayerReviewUI)
STUB_TODO(XamShowSigninUI)

// XAM task
STUB_TODO(XamTaskCloseHandle)
STUB_TODO(XamTaskSchedule)
STUB_RET_TODO(XamTaskShouldExit, 0)

// XAM user
STUB_TODO(XamUserAreUsersFriends)
STUB_TODO(XamUserCheckPrivilege)
STUB_TODO(XamUserCreateAchievementEnumerator)
STUB_TODO(XamUserCreateStatsEnumerator)
STUB_TODO(XamUserGetName)
STUB_TODO(XamUserGetSigninInfo)
STUB_RET_TODO(XamUserGetSigninState, 1)  // Signed in
STUB_TODO(XamUserGetXUID)
STUB_TODO(XamUserReadProfileSettings)
STUB_TODO(XamUserWriteProfileSettings)

// XAM voice
STUB_TODO(XamVoiceClose)
STUB_TODO(XamVoiceCreate)
STUB_RET_TODO(XamVoiceHeadsetPresent, 0)
STUB_TODO(XamVoiceSubmitPacket)

// Crypto
STUB_TODO(XeCryptSha)
STUB_TODO(XeKeysConsolePrivateKeySign)
STUB_TODO(XeKeysConsoleSignatureVerification)

// XEX
STUB_TODO(XexCheckExecutablePrivilege)
STUB_TODO(XexGetModuleHandle)
STUB_TODO(XexGetProcedureAddress)

// C runtime
STUB_TODO(__C_specific_handler)

#undef STUB_TODO
#undef STUB_RET_TODO
