/**
 * XMutant - Mutex synchronization primitive
 * Adapted from Xenia for static recompilation
 */

#ifndef RAGE_RUNTIME_XMUTANT_H_
#define RAGE_RUNTIME_XMUTANT_H_

#include "xobject.h"

namespace xe {
namespace kernel {

class XMutant : public XObject {
public:
    static const XObject::Type kObjectType = XObject::Type::Mutant;
    
    XMutant(KernelState* kernel_state);
    ~XMutant() override;
    
    void Initialize(bool initial_owner);
    void InitializeNative(void* native_ptr, X_DISPATCH_HEADER* header);
    
    X_STATUS ReleaseMutant(uint32_t priority_increment, bool abandon, bool wait);
    
protected:
    void WaitCallback() override;
    threading::WaitHandle* GetWaitHandle() override { return mutant_.get(); }
    
private:
    std::unique_ptr<threading::Mutant> mutant_;
};

}  // namespace kernel
}  // namespace xe

#endif  // RAGE_RUNTIME_XMUTANT_H_
