/**
 * XObject Implementation
 */

#include "xobject.h"
#include "kernel_state.h"
#include "xevent.h"
#include "xmutant.h"
#include "xsemaphore.h"

namespace xe {
namespace kernel {

XObject::XObject(Type type)
    : kernel_state_(nullptr), type_(type) {
    handles_.reserve(4);
}

XObject::XObject(KernelState* kernel_state, Type type)
    : kernel_state_(kernel_state), type_(type) {
    handles_.reserve(4);
    
    if (kernel_state_) {
        kernel_state_->object_table()->AddHandle(this, nullptr);
    }
}

XObject::~XObject() {
    if (allocated_guest_object_ && kernel_state_) {
        // Free guest memory - simplified version
        // In full implementation, would use proper heap management
    }
}

uint8_t* XObject::memory_base() const {
    return kernel_state_ ? kernel_state_->memory_base() : nullptr;
}

void XObject::RetainHandle() {
    if (kernel_state_ && !handles_.empty()) {
        kernel_state_->object_table()->RetainHandle(handles_[0]);
    }
}

bool XObject::ReleaseHandle() {
    if (kernel_state_ && !handles_.empty()) {
        return kernel_state_->object_table()->ReleaseHandle(handles_[0]) == X_STATUS_SUCCESS;
    }
    return false;
}

void XObject::Retain() {
    ++pointer_ref_count_;
}

void XObject::Release() {
    if (--pointer_ref_count_ == 0) {
        delete this;
    }
}

X_STATUS XObject::Delete() {
    if (!kernel_state_) {
        return X_STATUS_SUCCESS;
    }
    if (!name_.empty()) {
        kernel_state_->object_table()->RemoveNameMapping(name_);
    }
    if (!handles_.empty()) {
        return kernel_state_->object_table()->RemoveHandle(handles_[0]);
    }
    return X_STATUS_SUCCESS;
}

void XObject::SetAttributes(uint32_t obj_attributes_ptr) {
    if (!obj_attributes_ptr || !kernel_state_) return;
    
    uint8_t* base = memory_base();
    if (!base) return;
    
    // Read name pointer from object attributes
    uint32_t name_ptr = load_and_swap<uint32_t>(base + obj_attributes_ptr + 4);
    if (name_ptr) {
        // X_ANSI_STRING: length, max_length, pointer
        uint32_t str_ptr = load_and_swap<uint32_t>(base + name_ptr + 4);
        if (str_ptr) {
            name_ = std::string(reinterpret_cast<char*>(base + str_ptr));
            if (!name_.empty() && !handles_.empty()) {
                kernel_state_->object_table()->AddNameMapping(name_, handles_[0]);
            }
        }
    }
}

uint32_t XObject::TimeoutTicksToMs(int64_t timeout_ticks) {
    if (timeout_ticks > 0) {
        // Absolute time - not commonly used
        return 0;
    } else if (timeout_ticks < 0) {
        // Relative time in 100ns units
        return static_cast<uint32_t>((-timeout_ticks) / 10000);
    }
    return 0;
}

X_STATUS XObject::Wait(uint32_t wait_reason, uint32_t processor_mode,
                       uint32_t alertable, uint64_t* opt_timeout) {
    auto wait_handle = GetWaitHandle();
    if (!wait_handle) {
        return X_STATUS_SUCCESS;
    }
    
    auto timeout_ms = opt_timeout 
        ? std::chrono::milliseconds(TimeoutTicksToMs(*opt_timeout))
        : std::chrono::milliseconds::max();
    
    auto result = threading::Wait(wait_handle, alertable != 0, timeout_ms);
    
    switch (result) {
        case threading::WaitResult::kSuccess:
            WaitCallback();
            return X_STATUS_SUCCESS;
        case threading::WaitResult::kUserCallback:
            return X_STATUS_USER_APC;
        case threading::WaitResult::kTimeout:
            threading::MaybeYield();
            return X_STATUS_TIMEOUT;
        default:
            return X_STATUS_ABANDONED_WAIT_0;
    }
}

X_STATUS XObject::SignalAndWait(XObject* signal_object, XObject* wait_object,
                                uint32_t wait_reason, uint32_t processor_mode,
                                uint32_t alertable, uint64_t* opt_timeout) {
    auto timeout_ms = opt_timeout
        ? std::chrono::milliseconds(TimeoutTicksToMs(*opt_timeout))
        : std::chrono::milliseconds::max();
    
    auto result = threading::SignalAndWait(
        signal_object->GetWaitHandle(),
        wait_object->GetWaitHandle(),
        alertable != 0, timeout_ms);
    
    switch (result) {
        case threading::WaitResult::kSuccess:
            wait_object->WaitCallback();
            return X_STATUS_SUCCESS;
        case threading::WaitResult::kUserCallback:
            return X_STATUS_USER_APC;
        case threading::WaitResult::kTimeout:
            threading::MaybeYield();
            return X_STATUS_TIMEOUT;
        default:
            return X_STATUS_ABANDONED_WAIT_0;
    }
}

X_STATUS XObject::WaitMultiple(uint32_t count, XObject** objects,
                               uint32_t wait_type, uint32_t wait_reason,
                               uint32_t processor_mode, uint32_t alertable,
                               uint64_t* opt_timeout) {
    std::vector<threading::WaitHandle*> wait_handles(count);
    for (uint32_t i = 0; i < count; ++i) {
        wait_handles[i] = objects[i]->GetWaitHandle();
    }
    
    auto timeout_ms = opt_timeout
        ? std::chrono::milliseconds(TimeoutTicksToMs(*opt_timeout))
        : std::chrono::milliseconds::max();
    
    if (wait_type) {
        // WaitAny
        auto result = threading::WaitAny(std::move(wait_handles), 
                                          alertable != 0, timeout_ms);
        switch (result.first) {
            case threading::WaitResult::kSuccess:
                objects[result.second]->WaitCallback();
                return X_STATUS(result.second);
            case threading::WaitResult::kUserCallback:
                return X_STATUS_USER_APC;
            case threading::WaitResult::kTimeout:
                threading::MaybeYield();
                return X_STATUS_TIMEOUT;
            default:
                return X_STATUS(X_STATUS_ABANDONED_WAIT_0 + result.second);
        }
    } else {
        // WaitAll
        auto result = threading::WaitAll(std::move(wait_handles),
                                          alertable != 0, timeout_ms);
        switch (result) {
            case threading::WaitResult::kSuccess:
                for (uint32_t i = 0; i < count; ++i) {
                    objects[i]->WaitCallback();
                }
                return X_STATUS_SUCCESS;
            case threading::WaitResult::kUserCallback:
                return X_STATUS_USER_APC;
            case threading::WaitResult::kTimeout:
                threading::MaybeYield();
                return X_STATUS_TIMEOUT;
            default:
                return X_STATUS_ABANDONED_WAIT_0;
        }
    }
}

uint8_t* XObject::CreateNative(uint32_t size) {
    if (!kernel_state_) return nullptr;
    
    uint8_t* base = memory_base();
    if (!base) return nullptr;
    
    // Allocate from guest heap (simplified - use a bump allocator)
    static std::atomic<uint32_t> heap_ptr{0x50000000};
    uint32_t alloc_size = (size + 0xF) & ~0xF;  // 16-byte aligned
    
    uint32_t guest_ptr = heap_ptr.fetch_add(alloc_size);
    if (guest_ptr + alloc_size > 0x60000000) {
        return nullptr;  // Out of heap space
    }
    
    allocated_guest_object_ = true;
    guest_object_ptr_ = guest_ptr;
    
    // Zero the memory
    std::memset(base + guest_ptr, 0, alloc_size);
    
    return base + guest_ptr;
}

void XObject::SetNativePointer(uint32_t native_ptr) {
    guest_object_ptr_ = native_ptr;
}

object_ref<XObject> XObject::GetNativeObject(KernelState* kernel_state,
                                             void* native_ptr,
                                             int32_t as_type) {
    if (!native_ptr || !kernel_state) return nullptr;
    
    uint8_t* base = kernel_state->memory_base();
    auto header = reinterpret_cast<X_DISPATCH_HEADER*>(native_ptr);
    
    if (as_type == -1) {
        as_type = header->type;
    }
    
    // Check if already initialized (our signature in wait_list_flink)
    if (header->wait_list_flink == kXObjSignature) {
        uint32_t handle = header->wait_list_blink;
        return kernel_state->object_table()->LookupObject<XObject>(handle);
    }
    
    // Create new object based on type
    XObject* object = nullptr;
    switch (as_type) {
        case 0:  // EventNotificationObject
        case 1:  // EventSynchronizationObject
        {
            auto ev = new XEvent(kernel_state);
            ev->InitializeNative(native_ptr, header);
            object = ev;
            break;
        }
        case 2:  // MutantObject
        {
            auto mutant = new XMutant(kernel_state);
            mutant->InitializeNative(native_ptr, header);
            object = mutant;
            break;
        }
        case 5:  // SemaphoreObject
        {
            auto sem = new XSemaphore(kernel_state);
            sem->InitializeNative(native_ptr, header);
            object = sem;
            break;
        }
        default:
            return nullptr;
    }
    
    // Stash our handle in the header for future lookups
    StashHandle(header, object->handle());
    
    return object_ref<XObject>(object);
}

}  // namespace kernel
}  // namespace xe
