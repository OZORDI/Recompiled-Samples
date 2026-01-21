/**
 * XThread Implementation - Real thread creation with static dispatch
 */

#include "xthread.h"
#include <future>
#include "kernel_state.h"
#include <cstring>

// PPCContext is defined in ppc_context.h (included via kernel_state.h)

namespace xe {
namespace kernel {

// Thread-local current thread
thread_local XThread* XThread::current_thread_ = nullptr;

static std::atomic<uint32_t> next_thread_id_{0x1000};

XThread::XThread(KernelState* kernel_state, uint32_t stack_size,
                 uint32_t xapi_thread_startup, uint32_t start_address,
                 uint32_t start_context, uint32_t creation_flags,
                 bool guest_thread, bool main_thread)
    : XObject(kernel_state, kObjectType),
      thread_id_(next_thread_id_++),
      guest_thread_(guest_thread),
      main_thread_(main_thread) {
    
    creation_params_.stack_size = stack_size ? stack_size : 64 * 1024;
    creation_params_.xapi_thread_startup = xapi_thread_startup;
    creation_params_.start_address = start_address;
    creation_params_.start_context = start_context;
    creation_params_.creation_flags = creation_flags;
    
    // Minimum stack of 16KB
    if (creation_params_.stack_size < 16 * 1024) {
        creation_params_.stack_size = 16 * 1024;
    }
    
    // Round up to page boundary
    creation_params_.stack_size = (creation_params_.stack_size + 0xFFF) & ~0xFFF;
    
    // Suspended by default?
    if (creation_flags & X_CREATE_SUSPENDED) {
        suspend_count_.store(1, std::memory_order_seq_cst);
        printf("[XTHREAD] Thread %p created SUSPENDED (count=%d, addr=%p)\n", 
               this, suspend_count_.load(), &suspend_count_);
    }
    
    // Create exit event
    exit_event_ = std::make_unique<threading::Event>(true, false);
    
    // Register with kernel
    kernel_state->RegisterThread(this);
}

XThread::~XThread() {
    // Unregister from kernel
    if (kernel_state_) {
        kernel_state_->UnregisterThread(this);
    }
    
    // Wait for thread to finish
    if (thread_ && thread_->joinable()) {
        terminated_ = true;
        suspend_cv_.notify_all();
        thread_->join();
    }
    
    // Free memory
    FreeStack();
    if (owns_context_ && context_) {
        free(context_);
        context_ = nullptr;
    }
}

X_STATUS XThread::Create() {
    // Context must be set by caller before Create() or we allocate raw memory
    if (!context_) {
        // Allocate raw memory for context (caller should set via SetContext ideally)
        // Size estimate: 32 GPRs + 32 FPRs + 128 VRs + misc = ~4KB should be safe
        context_ = reinterpret_cast<PPCContext*>(calloc(1, 4096));
        owns_context_ = true;
        if (!context_) {
            return X_STATUS_NO_MEMORY;
        }
    }
    
    // Allocate stack
    if (!AllocateStack(creation_params_.stack_size)) {
        if (owns_context_) { free(context_); context_ = nullptr; }
        return X_STATUS_NO_MEMORY;
    }
    
    // Initialize PCR
    if (!InitializePCR()) {
        FreeStack();
        if (owns_context_) { free(context_); context_ = nullptr; }
        return X_STATUS_NO_MEMORY;
    }
    
    // Set up initial context - use byte offsets for portability
    // r[1] is at offset 1*16 = 16, u32 is at +0
    uint32_t* ctx_bytes = reinterpret_cast<uint32_t*>(context_);
    ctx_bytes[1*4 + 0] = stack_base_ - 0x80;  // r1.u32 = stack pointer
    ctx_bytes[13*4 + 0] = pcr_address_;        // r13.u32 = TLS/PCR
    ctx_bytes[3*4 + 0] = creation_params_.start_context;  // r3.u32 = arg
    
    // Create native thread with proper synchronization
    // Use a promise/future to ensure suspend state is checked correctly
    std::promise<void> start_promise;
    std::future<void> start_future = start_promise.get_future();
    
    uint32_t initial_suspend = suspend_count_.load();
    
    thread_ = std::make_unique<std::thread>([this, initial_suspend, &start_promise]() {
        // Signal that we've captured the suspend count
        start_promise.set_value();
        
        current_thread_ = this;
        
        // If created suspended, wait for resume
        if (initial_suspend > 0) {
            std::unique_lock<std::mutex> lock(suspend_mutex_);
            printf("[XTHREAD] Thread %u waiting (initial_suspend=%d)\n", thread_id_, initial_suspend);
            while (suspend_count_ > 0 && !terminated_) {
                suspend_cv_.wait(lock);
            }
            printf("[XTHREAD] Thread %u released\n", thread_id_);
        }
        
        if (!terminated_) {
            ExecuteEntry();
        }
        
        current_thread_ = nullptr;
    });
    
    // Wait for thread to capture suspend state
    start_future.wait();
    
    running_ = true;
    return X_STATUS_SUCCESS;
}

void XThread::Execute() {
    // Legacy - redirect to ExecuteEntry
    ExecuteEntry();
}

void XThread::ExecuteEntry() {
    // Get start address
    uint32_t entry = creation_params_.start_address;
    if (creation_params_.xapi_thread_startup) {
        // XapiThreadStartup wrapper - set up r4 with actual entry
        uint32_t* ctx_bytes = reinterpret_cast<uint32_t*>(context_);
        ctx_bytes[4*4 + 0] = entry;  // r4.u32 = actual entry
        entry = creation_params_.xapi_thread_startup;
    }
    
    // Notify kernel that thread is starting
    kernel_state_->OnThreadStarted();
    
    // Execute the entry point via kernel's dispatch
    printf("[XTHREAD] Thread %u starting at 0x%08X (context=0x%08X)\n",
           thread_id_, entry, creation_params_.start_context);
    
    kernel_state_->Execute(entry, context_);
    
    printf("[XTHREAD] Thread %u finished\n", thread_id_);
    
    // Notify kernel that thread finished
    kernel_state_->OnThreadFinished();
    
    // Signal exit
    running_ = false;
    exit_event_->Set();
}

X_STATUS XThread::Exit(uint32_t exit_code) {
    terminated_ = true;
    running_ = false;
    suspend_cv_.notify_all();
    exit_event_->Set();
    return X_STATUS_SUCCESS;
}

X_STATUS XThread::Resume(uint32_t* out_suspend_count) {
    std::lock_guard<std::mutex> lock(suspend_mutex_);
    
    uint32_t prev_count = suspend_count_;
    if (out_suspend_count) {
        *out_suspend_count = prev_count;
    }
    
    printf("[XTHREAD] Resume thread %u (prev_count=%d)\n", thread_id_, prev_count);
    
    if (suspend_count_ > 0) {
        suspend_count_--;
        if (suspend_count_ == 0) {
            suspend_cv_.notify_all();
        }
    }
    
    return X_STATUS_SUCCESS;
}

X_STATUS XThread::Suspend(uint32_t* out_suspend_count) {
    std::lock_guard<std::mutex> lock(suspend_mutex_);
    
    uint32_t prev_count = suspend_count_;
    if (out_suspend_count) {
        *out_suspend_count = prev_count;
    }
    
    suspend_count_++;
    return X_STATUS_SUCCESS;
}

bool XThread::AllocateStack(uint32_t size) {
    uint8_t* base = kernel_state_->memory_base();
    if (!base) return false;
    
    // Allocate in guest memory region (0x70000000 - 0x7F000000 for stacks)
    static std::atomic<uint32_t> stack_alloc_ptr{0x70000000};
    
    uint32_t alloc_size = size + 0x1000;  // Extra guard page
    uint32_t alloc_base = stack_alloc_ptr.fetch_add(alloc_size);
    
    if (alloc_base + alloc_size > 0x7F000000) {
        return false;  // Out of stack space
    }
    
    stack_alloc_base_ = alloc_base;
    stack_alloc_size_ = alloc_size;
    stack_limit_ = alloc_base + 0x1000;  // Guard page at bottom
    stack_base_ = alloc_base + alloc_size;  // Stack grows down
    
    // Zero the stack memory
    std::memset(base + stack_limit_, 0, size);
    
    return true;
}

void XThread::FreeStack() {
    // In a real implementation, we'd return stack memory to the allocator
    stack_alloc_base_ = 0;
    stack_alloc_size_ = 0;
    stack_base_ = 0;
    stack_limit_ = 0;
}

bool XThread::InitializePCR() {
    uint8_t* base = kernel_state_->memory_base();
    if (!base) return false;
    
    // Allocate PCR in guest memory
    static std::atomic<uint32_t> pcr_alloc_ptr{0x7F000000};
    
    uint32_t pcr_size = 0x2D8;  // Size of PCR structure
    pcr_address_ = pcr_alloc_ptr.fetch_add((pcr_size + 0xFFF) & ~0xFFF);
    
    if (pcr_address_ + pcr_size > 0x80000000) {
        return false;
    }
    
    // Zero the PCR
    std::memset(base + pcr_address_, 0, pcr_size);
    
    // Set up key PCR fields
    // PCR + 0x100 = pointer to KTHREAD
    // For now, just point to a dummy area
    uint32_t kthread_ptr = pcr_address_ + 0x100;
    store_and_swap<uint32_t>(base + pcr_address_ + 0x100, kthread_ptr);
    
    // KTHREAD + 0x14C = thread ID
    store_and_swap<uint32_t>(base + kthread_ptr + 0x4C, thread_id_);
    
    // TLS static address (optional)
    static std::atomic<uint32_t> tls_alloc_ptr{0x7F100000};
    tls_static_address_ = tls_alloc_ptr.fetch_add(0x1000);
    
    return true;
}

uint32_t XThread::last_error() const {
    if (!kernel_state_ || !kernel_state_->memory_base()) return 0;
    uint8_t* base = kernel_state_->memory_base();
    uint32_t kthread = load_and_swap<uint32_t>(base + pcr_address_ + 0x100);
    return load_and_swap<uint32_t>(base + kthread + 0x60);  // last_error offset
}

void XThread::set_last_error(uint32_t error) {
    if (!kernel_state_ || !kernel_state_->memory_base()) return;
    uint8_t* base = kernel_state_->memory_base();
    uint32_t kthread = load_and_swap<uint32_t>(base + pcr_address_ + 0x100);
    store_and_swap<uint32_t>(base + kthread + 0x60, error);
}

bool XThread::IsInThread() {
    return current_thread_ != nullptr;
}

XThread* XThread::GetCurrentThread() {
    return current_thread_;
}

uint32_t XThread::GetCurrentThreadHandle() {
    return current_thread_ ? current_thread_->handle() : 0;
}

uint32_t XThread::GetCurrentThreadId() {
    return current_thread_ ? current_thread_->thread_id() : 0;
}

void XThread::set_name(const std::string& name) {
    name_ = name;
}

threading::WaitHandle* XThread::GetWaitHandle() {
    return exit_event_.get();
}

}  // namespace kernel
}  // namespace xe
