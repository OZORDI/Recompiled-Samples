/**
 * XMutant Implementation
 */

#include "xmutant.h"
#include "kernel_state.h"

namespace xe {
namespace kernel {

XMutant::XMutant(KernelState* kernel_state)
    : XObject(kernel_state, kObjectType) {}

XMutant::~XMutant() = default;

void XMutant::Initialize(bool initial_owner) {
    mutant_ = std::make_unique<threading::Mutant>(initial_owner);
    
    auto native = CreateNative<X_DISPATCH_HEADER>();
    if (native) {
        native->type = 2;  // Mutant type
        native->signal_state = initial_owner ? 0 : 1;
    }
}

void XMutant::InitializeNative(void* native_ptr, X_DISPATCH_HEADER* header) {
    bool initial_owner = header->signal_state == 0;
    mutant_ = std::make_unique<threading::Mutant>(initial_owner);
    SetNativePointer(reinterpret_cast<uint8_t*>(header) - memory_base());
}

X_STATUS XMutant::ReleaseMutant(uint32_t priority_increment, bool abandon, bool wait) {
    if (!mutant_->Release()) {
        return X_STATUS_MUTANT_NOT_OWNED;
    }
    
    auto guest_obj = guest_object<X_DISPATCH_HEADER>();
    guest_obj->signal_state = 1;
    
    return X_STATUS_SUCCESS;
}

void XMutant::WaitCallback() {
    auto guest_obj = guest_object<X_DISPATCH_HEADER>();
    guest_obj->signal_state = 0;
}

}  // namespace kernel
}  // namespace xe
