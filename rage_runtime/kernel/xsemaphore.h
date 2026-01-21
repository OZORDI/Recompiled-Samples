/**
 * XSemaphore - Semaphore synchronization primitive
 * Adapted from Xenia for static recompilation
 */

#ifndef RAGE_RUNTIME_XSEMAPHORE_H_
#define RAGE_RUNTIME_XSEMAPHORE_H_

#include "xobject.h"

namespace xe {
namespace kernel {

struct X_KSEMAPHORE {
    X_DISPATCH_HEADER header;
    xe::be<int32_t> limit;
};

class XSemaphore : public XObject {
public:
    static const XObject::Type kObjectType = XObject::Type::Semaphore;
    
    XSemaphore(KernelState* kernel_state);
    ~XSemaphore() override;
    
    void Initialize(int32_t initial_count, int32_t maximum_count);
    bool InitializeNative(void* native_ptr, X_DISPATCH_HEADER* header);
    
    int32_t ReleaseSemaphore(int32_t release_count);
    
protected:
    void WaitCallback() override;
    threading::WaitHandle* GetWaitHandle() override { return semaphore_.get(); }
    
private:
    std::unique_ptr<threading::Semaphore> semaphore_;
    int32_t maximum_count_ = 0;
};

}  // namespace kernel
}  // namespace xe

#endif  // RAGE_RUNTIME_XSEMAPHORE_H_
