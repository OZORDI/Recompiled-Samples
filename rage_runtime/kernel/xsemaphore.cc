/**
 * XSemaphore Implementation
 */

#include "xsemaphore.h"
#include "kernel_state.h"

namespace xe {
namespace kernel {

XSemaphore::XSemaphore(KernelState* kernel_state)
    : XObject(kernel_state, kObjectType) {}

XSemaphore::~XSemaphore() = default;

void XSemaphore::Initialize(int32_t initial_count, int32_t maximum_count) {
    maximum_count_ = maximum_count;
    semaphore_ = std::make_unique<threading::Semaphore>(initial_count, maximum_count);
    
    auto native = CreateNative<X_KSEMAPHORE>();
    if (native) {
        native->header.type = 5;  // Semaphore type
        native->header.signal_state = initial_count;
        native->limit = maximum_count;
    }
}

bool XSemaphore::InitializeNative(void* native_ptr, X_DISPATCH_HEADER* header) {
    auto sem_header = reinterpret_cast<X_KSEMAPHORE*>(header);
    int32_t initial_count = header->signal_state;
    int32_t maximum_count = sem_header->limit;
    
    if (maximum_count <= 0) {
        return false;
    }
    
    maximum_count_ = maximum_count;
    semaphore_ = std::make_unique<threading::Semaphore>(initial_count, maximum_count);
    SetNativePointer(reinterpret_cast<uint8_t*>(header) - memory_base());
    
    return true;
}

int32_t XSemaphore::ReleaseSemaphore(int32_t release_count) {
    int32_t previous_count = 0;
    semaphore_->Release(release_count, &previous_count);
    
    auto guest_obj = guest_object<X_KSEMAPHORE>();
    guest_obj->header.signal_state = previous_count + release_count;
    
    return previous_count;
}

void XSemaphore::WaitCallback() {
    auto guest_obj = guest_object<X_KSEMAPHORE>();
    int32_t count = guest_obj->header.signal_state;
    if (count > 0) {
        guest_obj->header.signal_state = count - 1;
    }
}

}  // namespace kernel
}  // namespace xe
