/**
 * XThread - Thread object for static recompilation
 * Adapted from Xenia - executes recompiled code via static dispatch
 */

#ifndef RAGE_RUNTIME_XTHREAD_H_
#define RAGE_RUNTIME_XTHREAD_H_

#include <atomic>
#include <memory>
#include <thread>

#include "xobject.h"
#include "threading.h"

// PPCContext is provided by game - forward declare here
struct PPCContext;

namespace xe {
namespace kernel {

// Guest KTHREAD structure layout
struct X_KTHREAD {
    X_DISPATCH_HEADER header;           // 0x00
    char unk_10[0x3C];                   // 0x10
    uint8_t suspend_count;               // 0x4C
    char unk_4D[0x03];                   // 0x4D
    char unk_50[0x04];                   // 0x50
    uint8_t priority;                    // 0x54
    uint8_t unk_55;                      // 0x55
    uint8_t unk_56;                      // 0x56
    uint8_t unk_57;                      // 0x57
    xe::be<uint32_t> unk_58;             // 0x58
    xe::be<uint32_t> stack_base;         // 0x5C
    xe::be<uint32_t> stack_limit;        // 0x60
    char unk_64[0x04];                   // 0x64
    xe::be<uint32_t> tls_address;        // 0x68
    char unk_6C[0x80];                   // 0x6C
    xe::be<uint32_t> thread_id;          // 0xEC (actually at 0x14C relative to PCR)
    xe::be<uint32_t> last_error;         // 0xF0 (actually at 0x160 relative to PCR)
};

class XThread : public XObject {
public:
    static const XObject::Type kObjectType = XObject::Type::Thread;
    
    struct CreationParams {
        uint32_t stack_size = 64 * 1024;
        uint32_t xapi_thread_startup = 0;
        uint32_t start_address = 0;
        uint32_t start_context = 0;
        uint32_t creation_flags = 0;
    };
    
    XThread(KernelState* kernel_state, uint32_t stack_size,
            uint32_t xapi_thread_startup, uint32_t start_address,
            uint32_t start_context, uint32_t creation_flags,
            bool guest_thread = true, bool main_thread = false);
    ~XThread() override;
    
    // Thread state
    uint32_t thread_id() const { return thread_id_; }
    bool is_guest_thread() const { return guest_thread_; }
    bool is_main_thread() const { return main_thread_; }
    bool is_running() const { return running_; }
    
    // Stack info
    uint32_t stack_base() const { return stack_base_; }
    uint32_t stack_limit() const { return stack_limit_; }
    
    // PCR (Processor Control Region) - thread-local data
    uint32_t pcr_address() const { return pcr_address_; }
    
    // TLS
    uint32_t tls_static_address() const { return tls_static_address_; }
    
    // PPCContext for this thread (allocated by caller via SetContext)
    PPCContext* context() { return context_; }
    void SetContext(PPCContext* ctx) { context_ = ctx; }
    
    // Last error
    uint32_t last_error() const;
    void set_last_error(uint32_t error);
    
    // Thread lifecycle
    X_STATUS Create();
    X_STATUS Exit(uint32_t exit_code);
    
    // Suspend/Resume
    X_STATUS Resume(uint32_t* out_suspend_count = nullptr);
    X_STATUS Suspend(uint32_t* out_suspend_count = nullptr);
    
    // Current thread
    static bool IsInThread();
    static XThread* GetCurrentThread();
    static uint32_t GetCurrentThreadHandle();
    static uint32_t GetCurrentThreadId();
    
    // Name
    void set_name(const std::string& name);
    const std::string& name() const { return name_; }
    
protected:
    threading::WaitHandle* GetWaitHandle() override;
    
private:
    void Execute();
    void ExecuteEntry();
    bool AllocateStack(uint32_t size);
    void FreeStack();
    bool InitializePCR();
    
    static thread_local XThread* current_thread_;
    
    uint32_t thread_id_ = 0;
    bool guest_thread_ = true;
    bool main_thread_ = false;
    std::atomic<bool> running_{false};
    std::atomic<bool> terminated_{false};
    
    CreationParams creation_params_;
    
    // Memory regions (guest addresses)
    uint32_t stack_base_ = 0;
    uint32_t stack_limit_ = 0;
    uint32_t stack_alloc_base_ = 0;
    uint32_t stack_alloc_size_ = 0;
    uint32_t pcr_address_ = 0;
    uint32_t tls_static_address_ = 0;
    
    // Thread context (raw pointer - allocated externally or via context allocator)
    PPCContext* context_ = nullptr;
    bool owns_context_ = false;
    
    // Host thread
    std::unique_ptr<std::thread> thread_;
    std::unique_ptr<threading::Event> exit_event_;
    
    std::string name_;
    std::atomic<uint32_t> suspend_count_{0};
    std::mutex suspend_mutex_;
    std::condition_variable suspend_cv_;
};

}  // namespace kernel
}  // namespace xe

#endif  // RAGE_RUNTIME_XTHREAD_H_
