/**
 * KernelState - Central kernel singleton for static recompilation
 * Adapted from Xenia - removes emulator dependencies
 */

#ifndef RAGE_RUNTIME_KERNEL_STATE_H_
#define RAGE_RUNTIME_KERNEL_STATE_H_

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "xbox_types.h"
#include "xobject.h"
#include "util/object_table.h"
#include "ppc_context.h"

namespace xe {
namespace kernel {

class XThread;
class XEvent;
class XMutant;
class XSemaphore;

// Process type constants
constexpr uint32_t X_PROCTYPE_IDLE = 0;
constexpr uint32_t X_PROCTYPE_USER = 1;
constexpr uint32_t X_PROCTYPE_SYSTEM = 2;

// Static dispatch function type - for executing recompiled code
using StaticDispatchFunc = void(*)(uint32_t guest_addr, PPCContext* ctx, uint8_t* base);
using PPCFunc = void(PPCContext& ctx, uint8_t* base);

// Function lookup callback
using FunctionLookupFunc = PPCFunc*(*)(uint32_t guest_addr);

class KernelState {
public:
    KernelState();
    ~KernelState();
    
    // Singleton access
    static KernelState* shared();
    static void set_shared(KernelState* state);
    
    // Memory access
    uint8_t* memory_base() const { return memory_base_; }
    void set_memory_base(uint8_t* base) { memory_base_ = base; }
    
    // Static dispatch configuration
    void set_dispatch_function(StaticDispatchFunc func) { dispatch_func_ = func; }
    StaticDispatchFunc dispatch_function() const { return dispatch_func_; }
    
    void set_function_lookup(FunctionLookupFunc func) { function_lookup_ = func; }
    FunctionLookupFunc function_lookup() const { return function_lookup_; }
    
    // Object table
    util::ObjectTable* object_table() { return &object_table_; }
    
    // Process info
    uint32_t process_type() const { return process_type_; }
    void set_process_type(uint32_t value) { process_type_ = value; }
    
    // TLS management
    uint32_t AllocateTLS();
    void FreeTLS(uint32_t slot);
    
    // Thread management
    void RegisterThread(XThread* thread);
    void UnregisterThread(XThread* thread);
    object_ref<XThread> GetThreadByID(uint32_t thread_id);
    std::vector<object_ref<XThread>> GetAllThreads();
    
    // Code region (for validation)
    void SetCodeRegion(uint32_t base, uint32_t size) {
        code_base_ = base;
        code_size_ = size;
    }
    uint32_t code_base() const { return code_base_; }
    uint32_t code_size() const { return code_size_; }
    
    // Execute a function at guest address
    void Execute(uint32_t guest_addr, PPCContext* ctx);
    
    // Wait for all threads to finish (or timeout)
    void WaitForThreads(uint32_t timeout_ms = 0);
    
    // Get thread count (registered threads)
    size_t thread_count() const;
    
    // Running thread tracking
    void OnThreadStarted();
    void OnThreadFinished();
    size_t running_thread_count() const;
    
private:
    static KernelState* shared_kernel_state_;
    
    uint8_t* memory_base_ = nullptr;
    StaticDispatchFunc dispatch_func_ = nullptr;
    FunctionLookupFunc function_lookup_ = nullptr;
    
    std::recursive_mutex global_mutex_;
    util::ObjectTable object_table_;
    
    std::unordered_map<uint32_t, XThread*> threads_by_id_;
    
    uint32_t process_type_ = X_PROCTYPE_USER;
    uint32_t code_base_ = 0;
    uint32_t code_size_ = 0;
    
    // TLS bitmap
    std::atomic<uint64_t> tls_bitmap_{0};
    
    // Running thread counter (threads currently executing)
    std::atomic<size_t> running_thread_count_{0};
};

}  // namespace kernel
}  // namespace xe

#endif  // RAGE_RUNTIME_KERNEL_STATE_H_
