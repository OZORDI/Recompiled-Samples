/**
 * XEvent Implementation
 */

#include "xevent.h"
#include "kernel_state.h"

namespace xe {
namespace kernel {

XEvent::XEvent(KernelState* kernel_state)
    : XObject(kernel_state, kObjectType) {}

XEvent::~XEvent() = default;

void XEvent::Initialize(bool manual_reset, bool initial_state) {
    event_ = std::make_unique<threading::Event>(manual_reset, initial_state);
    
    // Create guest object if needed
    auto native = CreateNative<X_DISPATCH_HEADER>();
    if (native) {
        native->type = manual_reset ? 0 : 1;  // Notification vs Synchronization
        native->signal_state = initial_state ? 1 : 0;
    }
}

void XEvent::InitializeNative(void* native_ptr, X_DISPATCH_HEADER* header) {
    bool manual_reset = header->type == 0;
    bool initial_state = header->signal_state != 0;
    
    event_ = std::make_unique<threading::Event>(manual_reset, initial_state);
    SetNativePointer(reinterpret_cast<uint8_t*>(header) - memory_base());
}

int32_t XEvent::Set(uint32_t priority_increment, bool wait) {
    event_->Set();
    
    auto guest_obj = guest_object<X_DISPATCH_HEADER>();
    int32_t prev_state = guest_obj->signal_state;
    guest_obj->signal_state = 1;
    
    return prev_state;
}

int32_t XEvent::Pulse(uint32_t priority_increment, bool wait) {
    event_->Pulse();
    
    auto guest_obj = guest_object<X_DISPATCH_HEADER>();
    int32_t prev_state = guest_obj->signal_state;
    guest_obj->signal_state = 0;
    
    return prev_state;
}

int32_t XEvent::Reset() {
    event_->Reset();
    
    auto guest_obj = guest_object<X_DISPATCH_HEADER>();
    int32_t prev_state = guest_obj->signal_state;
    guest_obj->signal_state = 0;
    
    return prev_state;
}

void XEvent::Clear() {
    event_->Reset();
    
    auto guest_obj = guest_object<X_DISPATCH_HEADER>();
    guest_obj->signal_state = 0;
}

void XEvent::WaitCallback() {
    auto guest_obj = guest_object<X_DISPATCH_HEADER>();
    if (guest_obj->type == 1) {  // Synchronization event
        guest_obj->signal_state = 0;
    }
}

}  // namespace kernel
}  // namespace xe
