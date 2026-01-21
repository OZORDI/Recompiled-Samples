/**
 * XObject - Base class for all kernel objects
 * Adapted from Xenia for static recompilation
 */

#ifndef RAGE_RUNTIME_XOBJECT_H_
#define RAGE_RUNTIME_XOBJECT_H_

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "xbox_types.h"
#include "threading.h"

namespace xe {
namespace kernel {

constexpr fourcc_t kXObjSignature = make_fourcc('X', 'E', 'N', '\0');

class KernelState;

template <typename T>
class object_ref;

class XObject {
public:
    static constexpr uint32_t kHandleBase = 0xF8000000;
    
    enum class Type : uint32_t {
        Undefined,
        Enumerator,
        Event,
        File,
        IOCompletion,
        Module,
        Mutant,
        NotifyListener,
        Semaphore,
        Session,
        Socket,
        SymbolicLink,
        Thread,
        Timer,
    };
    
    XObject(Type type);
    XObject(KernelState* kernel_state, Type type);
    virtual ~XObject();
    
    KernelState* kernel_state() const { return kernel_state_; }
    uint8_t* memory_base() const;
    
    Type type() const { return type_; }
    X_HANDLE handle() const { return handles_.empty() ? 0 : handles_[0]; }
    std::vector<X_HANDLE>& handles() { return handles_; }
    const std::string& name() const { return name_; }
    uint32_t guest_object() const { return guest_object_ptr_; }
    
    template <typename T>
    T* guest_object() {
        return reinterpret_cast<T*>(memory_base() + guest_object_ptr_);
    }
    
    void RetainHandle();
    bool ReleaseHandle();
    void Retain();
    void Release();
    X_STATUS Delete();
    
    void SetAttributes(uint32_t obj_attributes_ptr);
    
    // Wait operations
    X_STATUS Wait(uint32_t wait_reason, uint32_t processor_mode,
                  uint32_t alertable, uint64_t* opt_timeout);
    static X_STATUS SignalAndWait(XObject* signal_object, XObject* wait_object,
                                  uint32_t wait_reason, uint32_t processor_mode,
                                  uint32_t alertable, uint64_t* opt_timeout);
    static X_STATUS WaitMultiple(uint32_t count, XObject** objects,
                                 uint32_t wait_type, uint32_t wait_reason,
                                 uint32_t processor_mode, uint32_t alertable,
                                 uint64_t* opt_timeout);
    
    static object_ref<XObject> GetNativeObject(KernelState* kernel_state,
                                               void* native_ptr,
                                               int32_t as_type = -1);
    
    template <typename T>
    static object_ref<T> GetNativeObject(KernelState* kernel_state,
                                         void* native_ptr,
                                         int32_t as_type = -1);
    
protected:
    virtual void WaitCallback() {}
    virtual threading::WaitHandle* GetWaitHandle() { return nullptr; }
    
    uint8_t* CreateNative(uint32_t size);
    
    template <typename T>
    T* CreateNative() {
        return reinterpret_cast<T*>(CreateNative(sizeof(T)));
    }
    
    void SetNativePointer(uint32_t native_ptr);
    
    static void StashHandle(X_DISPATCH_HEADER* header, uint32_t handle) {
        header->wait_list_flink = kXObjSignature;
        header->wait_list_blink = handle;
    }
    
    static uint32_t TimeoutTicksToMs(int64_t timeout_ticks);
    
    KernelState* kernel_state_ = nullptr;
    
private:
    std::atomic<int32_t> pointer_ref_count_{1};
    Type type_;
    std::vector<X_HANDLE> handles_;
    std::string name_;
    uint32_t guest_object_ptr_ = 0;
    bool allocated_guest_object_ = false;
};

// Smart pointer for kernel objects
template <typename T>
class object_ref {
public:
    object_ref() noexcept : value_(nullptr) {}
    object_ref(std::nullptr_t) noexcept : value_(nullptr) {}
    
    explicit object_ref(T* value) noexcept : value_(value) {}
    
    object_ref(const object_ref& right) noexcept {
        value_ = right.get();
        if (value_) value_->Retain();
    }
    
    object_ref(object_ref&& right) noexcept : value_(right.release()) {}
    
    ~object_ref() noexcept {
        if (value_) {
            value_->Release();
            value_ = nullptr;
        }
    }
    
    object_ref& operator=(const object_ref& right) noexcept {
        if (this != &right) {
            if (value_) value_->Release();
            value_ = right.get();
            if (value_) value_->Retain();
        }
        return *this;
    }
    
    object_ref& operator=(object_ref&& right) noexcept {
        if (this != &right) {
            if (value_) value_->Release();
            value_ = right.release();
        }
        return *this;
    }
    
    object_ref& operator=(std::nullptr_t) noexcept {
        if (value_) value_->Release();
        value_ = nullptr;
        return *this;
    }
    
    T& operator*() const { return *value_; }
    T* operator->() const noexcept { return value_; }
    T* get() const noexcept { return value_; }
    explicit operator bool() const noexcept { return value_ != nullptr; }
    
    T* release() noexcept {
        T* value = value_;
        value_ = nullptr;
        return value;
    }
    
    void reset(T* value = nullptr) noexcept {
        if (value_) value_->Release();
        value_ = value;
    }
    
private:
    T* value_ = nullptr;
};

template <typename T>
object_ref<T> retain_object(T* ptr) {
    if (ptr) ptr->Retain();
    return object_ref<T>(ptr);
}

// Template implementation (must be after object_ref definition)
template <typename T>
object_ref<T> XObject::GetNativeObject(KernelState* kernel_state,
                                       void* native_ptr,
                                       int32_t as_type) {
    auto obj = GetNativeObject(kernel_state, native_ptr, as_type);
    return object_ref<T>(reinterpret_cast<T*>(obj.release()));
}

}  // namespace kernel
}  // namespace xe

#endif  // RAGE_RUNTIME_XOBJECT_H_
