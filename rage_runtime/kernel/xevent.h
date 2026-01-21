/**
 * XEvent - Event synchronization primitive
 * Adapted from Xenia for static recompilation
 */

#ifndef RAGE_RUNTIME_XEVENT_H_
#define RAGE_RUNTIME_XEVENT_H_

#include "xobject.h"

namespace xe {
namespace kernel {

class XEvent : public XObject {
public:
    static const XObject::Type kObjectType = XObject::Type::Event;
    
    XEvent(KernelState* kernel_state);
    ~XEvent() override;
    
    void Initialize(bool manual_reset, bool initial_state);
    void InitializeNative(void* native_ptr, X_DISPATCH_HEADER* header);
    
    int32_t Set(uint32_t priority_increment, bool wait);
    int32_t Pulse(uint32_t priority_increment, bool wait);
    int32_t Reset();
    
    void Clear();
    
protected:
    void WaitCallback() override;
    threading::WaitHandle* GetWaitHandle() override { return event_.get(); }
    
private:
    std::unique_ptr<threading::Event> event_;
};

}  // namespace kernel
}  // namespace xe

#endif  // RAGE_RUNTIME_XEVENT_H_
