/**
 * Kernel Exports - Bridge layer connecting PPC_FUNC to real kernel
 * 
 * This header provides the real kernel implementations that replace
 * the stub functions in kernel_impl.cpp
 */

#ifndef RAGE_RUNTIME_KERNEL_EXPORTS_H_
#define RAGE_RUNTIME_KERNEL_EXPORTS_H_

#include "kernel_state.h"
#include "xthread.h"
#include "xevent.h"
#include "xmutant.h"
#include "xsemaphore.h"
#include "xbox_types.h"

// PPCContext should be included before this header via the game's ppc_context.h

namespace xe {
namespace kernel {
namespace exports {

// Memory access helpers
inline uint32_t GuestLoad32(uint8_t* base, uint32_t addr) {
    return __builtin_bswap32(*reinterpret_cast<uint32_t*>(base + addr));
}

inline void GuestStore32(uint8_t* base, uint32_t addr, uint32_t val) {
    *reinterpret_cast<uint32_t*>(base + addr) = __builtin_bswap32(val);
}

inline uint64_t GuestLoad64(uint8_t* base, uint32_t addr) {
    return __builtin_bswap64(*reinterpret_cast<uint64_t*>(base + addr));
}

inline void GuestStore64(uint8_t* base, uint32_t addr, uint64_t val) {
    *reinterpret_cast<uint64_t*>(base + addr) = __builtin_bswap64(val);
}

inline uint16_t GuestLoad16(uint8_t* base, uint32_t addr) {
    return __builtin_bswap16(*reinterpret_cast<uint16_t*>(base + addr));
}

inline void GuestStore16(uint8_t* base, uint32_t addr, uint16_t val) {
    *reinterpret_cast<uint16_t*>(base + addr) = __builtin_bswap16(val);
}

//=============================================================================
// Threading Exports
//=============================================================================

// ExCreateThread - Create a new thread
inline X_STATUS ExCreateThread(
    KernelState* kernel,
    uint8_t* base,
    uint32_t handle_ptr,
    uint32_t stack_size,
    uint32_t thread_id_ptr,
    uint32_t xapi_thread_startup,
    uint32_t start_address,
    uint32_t start_context,
    uint32_t creation_flags
) {
    // Default stack size from executable if 0
    if (stack_size == 0) {
        stack_size = 64 * 1024;
    }
    
    // Create thread object
    auto thread = new XThread(kernel, stack_size, xapi_thread_startup,
                              start_address, start_context, creation_flags,
                              true, false);
    
    X_STATUS result = thread->Create();
    
    if (XSUCCEEDED(result)) {
        if (handle_ptr) {
            if (creation_flags & 0x80) {
                GuestStore32(base, handle_ptr, thread->guest_object());
            } else {
                GuestStore32(base, handle_ptr, thread->handle());
            }
        }
        if (thread_id_ptr) {
            GuestStore32(base, thread_id_ptr, thread->thread_id());
        }
    } else {
        thread->Release();
    }
    
    return result;
}

// NtResumeThread
inline X_STATUS NtResumeThread(
    KernelState* kernel,
    X_HANDLE handle,
    uint32_t* suspend_count
) {
    auto thread = kernel->object_table()->LookupObject<XThread>(handle);
    if (!thread) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    uint32_t count = 0;
    X_STATUS result = thread->Resume(&count);
    if (suspend_count) {
        *suspend_count = count;
    }
    return result;
}

// NtSuspendThread
inline X_STATUS NtSuspendThread(
    KernelState* kernel,
    X_HANDLE handle,
    uint32_t* suspend_count
) {
    auto thread = kernel->object_table()->LookupObject<XThread>(handle);
    if (!thread) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    uint32_t count = 0;
    X_STATUS result = thread->Suspend(&count);
    if (suspend_count) {
        *suspend_count = count;
    }
    return result;
}

// ExTerminateThread
inline X_STATUS ExTerminateThread(KernelState* kernel, uint32_t exit_code) {
    XThread* thread = XThread::GetCurrentThread();
    if (thread) {
        return thread->Exit(exit_code);
    }
    return X_STATUS_INVALID_HANDLE;
}

//=============================================================================
// Synchronization Exports
//=============================================================================

// NtCreateEvent
inline X_STATUS NtCreateEvent(
    KernelState* kernel,
    uint8_t* base,
    uint32_t handle_ptr,
    uint32_t obj_attrs_ptr,
    uint32_t event_type,
    uint32_t initial_state
) {
    auto ev = new XEvent(kernel);
    ev->Initialize(event_type == 0, initial_state != 0);
    
    if (obj_attrs_ptr) {
        ev->SetAttributes(obj_attrs_ptr);
    }
    
    if (handle_ptr) {
        GuestStore32(base, handle_ptr, ev->handle());
    }
    
    return X_STATUS_SUCCESS;
}

// KeSetEvent
inline int32_t KeSetEvent(
    KernelState* kernel,
    uint8_t* base,
    uint32_t event_ptr,
    uint32_t increment,
    uint32_t wait
) {
    auto ev = XObject::GetNativeObject<XEvent>(kernel, base + event_ptr);
    if (ev) {
        return ev->Set(increment, wait != 0);
    }
    return 0;
}

// KeResetEvent
inline int32_t KeResetEvent(
    KernelState* kernel,
    uint8_t* base,
    uint32_t event_ptr
) {
    auto ev = XObject::GetNativeObject<XEvent>(kernel, base + event_ptr);
    if (ev) {
        return ev->Reset();
    }
    return 0;
}

// NtCreateMutant
inline X_STATUS NtCreateMutant(
    KernelState* kernel,
    uint8_t* base,
    uint32_t handle_ptr,
    uint32_t obj_attrs_ptr,
    uint32_t initial_owner
) {
    auto mutant = new XMutant(kernel);
    mutant->Initialize(initial_owner != 0);
    
    if (obj_attrs_ptr) {
        mutant->SetAttributes(obj_attrs_ptr);
    }
    
    if (handle_ptr) {
        GuestStore32(base, handle_ptr, mutant->handle());
    }
    
    return X_STATUS_SUCCESS;
}

// NtReleaseMutant
inline X_STATUS NtReleaseMutant(
    KernelState* kernel,
    X_HANDLE handle,
    int32_t* previous_count
) {
    auto mutant = kernel->object_table()->LookupObject<XMutant>(handle);
    if (!mutant) {
        return X_STATUS_INVALID_HANDLE;
    }
    return mutant->ReleaseMutant(0, false, false);
}

// NtCreateSemaphore
inline X_STATUS NtCreateSemaphore(
    KernelState* kernel,
    uint8_t* base,
    uint32_t handle_ptr,
    uint32_t obj_attrs_ptr,
    int32_t initial_count,
    int32_t maximum_count
) {
    auto sem = new XSemaphore(kernel);
    sem->Initialize(initial_count, maximum_count);
    
    if (obj_attrs_ptr) {
        sem->SetAttributes(obj_attrs_ptr);
    }
    
    if (handle_ptr) {
        GuestStore32(base, handle_ptr, sem->handle());
    }
    
    return X_STATUS_SUCCESS;
}

// NtReleaseSemaphore
inline X_STATUS NtReleaseSemaphore(
    KernelState* kernel,
    X_HANDLE handle,
    int32_t release_count,
    int32_t* previous_count
) {
    auto sem = kernel->object_table()->LookupObject<XSemaphore>(handle);
    if (!sem) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    int32_t prev = sem->ReleaseSemaphore(release_count);
    if (previous_count) {
        *previous_count = prev;
    }
    return X_STATUS_SUCCESS;
}

//=============================================================================
// Wait Exports
//=============================================================================

// KeWaitForSingleObject
inline X_STATUS KeWaitForSingleObject(
    KernelState* kernel,
    uint8_t* base,
    uint32_t object_ptr,
    uint32_t wait_reason,
    uint32_t processor_mode,
    uint32_t alertable,
    uint64_t* timeout_ptr
) {
    auto obj = XObject::GetNativeObject<XObject>(kernel, base + object_ptr);
    if (!obj) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    return obj->Wait(wait_reason, processor_mode, alertable, timeout_ptr);
}

// NtWaitForSingleObjectEx
inline X_STATUS NtWaitForSingleObjectEx(
    KernelState* kernel,
    X_HANDLE handle,
    uint32_t wait_mode,
    uint32_t alertable,
    uint64_t* timeout_ptr
) {
    auto obj = kernel->object_table()->LookupObject<XObject>(handle);
    if (!obj) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    return obj->Wait(3, wait_mode, alertable, timeout_ptr);
}

// NtWaitForMultipleObjectsEx
inline X_STATUS NtWaitForMultipleObjectsEx(
    KernelState* kernel,
    uint8_t* base,
    uint32_t count,
    uint32_t handles_ptr,
    uint32_t wait_type,
    uint32_t wait_mode,
    uint32_t alertable,
    uint64_t* timeout_ptr
) {
    if (count == 0 || count > 64) {
        return X_STATUS_INVALID_PARAMETER;
    }
    
    std::vector<XObject*> objects(count);
    std::vector<object_ref<XObject>> refs(count);
    
    for (uint32_t i = 0; i < count; ++i) {
        X_HANDLE handle = GuestLoad32(base, handles_ptr + i * 4);
        refs[i] = kernel->object_table()->LookupObject<XObject>(handle);
        if (!refs[i]) {
            return X_STATUS_INVALID_HANDLE;
        }
        objects[i] = refs[i].get();
    }
    
    return XObject::WaitMultiple(count, objects.data(), wait_type, 3,
                                  wait_mode, alertable, timeout_ptr);
}

//=============================================================================
// Object Management
//=============================================================================

// NtClose
inline X_STATUS NtClose(KernelState* kernel, X_HANDLE handle) {
    return kernel->object_table()->ReleaseHandle(handle);
}

// ObReferenceObjectByHandle
inline X_STATUS ObReferenceObjectByHandle(
    KernelState* kernel,
    uint8_t* base,
    X_HANDLE handle,
    uint32_t object_type,
    uint32_t object_ptr_ptr
) {
    auto obj = kernel->object_table()->LookupObject<XObject>(handle);
    if (!obj) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    if (object_ptr_ptr) {
        GuestStore32(base, object_ptr_ptr, obj->guest_object());
    }
    
    return X_STATUS_SUCCESS;
}

//=============================================================================
// Process Info
//=============================================================================

inline uint32_t KeGetCurrentProcessType(KernelState* kernel) {
    return kernel->process_type();
}

}  // namespace exports
}  // namespace kernel
}  // namespace xe

#endif  // RAGE_RUNTIME_KERNEL_EXPORTS_H_
