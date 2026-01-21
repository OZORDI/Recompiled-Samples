/**
 * KernelState Implementation
 */

#include "kernel_state.h"
#include "xthread.h"
#include <chrono>
#include <thread>

namespace xe {
namespace kernel {

KernelState* KernelState::shared_kernel_state_ = nullptr;

KernelState::KernelState() {
    // Set as shared instance if none exists
    if (!shared_kernel_state_) {
        shared_kernel_state_ = this;
    }
}

KernelState::~KernelState() {
    if (shared_kernel_state_ == this) {
        shared_kernel_state_ = nullptr;
    }
    object_table_.Reset();
}

KernelState* KernelState::shared() {
    return shared_kernel_state_;
}

void KernelState::set_shared(KernelState* state) {
    shared_kernel_state_ = state;
}

uint32_t KernelState::AllocateTLS() {
    // Find first free bit in TLS bitmap
    uint64_t bitmap = tls_bitmap_.load();
    for (uint32_t slot = 0; slot < 64; ++slot) {
        if (!(bitmap & (1ULL << slot))) {
            // Try to claim it
            uint64_t new_bitmap = bitmap | (1ULL << slot);
            if (tls_bitmap_.compare_exchange_strong(bitmap, new_bitmap)) {
                return slot;
            }
            // Failed, retry
            bitmap = tls_bitmap_.load();
        }
    }
    return 0xFFFFFFFF;  // No free slots
}

void KernelState::FreeTLS(uint32_t slot) {
    if (slot < 64) {
        tls_bitmap_.fetch_and(~(1ULL << slot));
    }
}

void KernelState::RegisterThread(XThread* thread) {
    std::lock_guard<std::recursive_mutex> lock(global_mutex_);
    threads_by_id_[thread->thread_id()] = thread;
}

void KernelState::UnregisterThread(XThread* thread) {
    std::lock_guard<std::recursive_mutex> lock(global_mutex_);
    threads_by_id_.erase(thread->thread_id());
}

object_ref<XThread> KernelState::GetThreadByID(uint32_t thread_id) {
    std::lock_guard<std::recursive_mutex> lock(global_mutex_);
    auto it = threads_by_id_.find(thread_id);
    if (it != threads_by_id_.end()) {
        it->second->Retain();
        return object_ref<XThread>(it->second);
    }
    return nullptr;
}

std::vector<object_ref<XThread>> KernelState::GetAllThreads() {
    std::lock_guard<std::recursive_mutex> lock(global_mutex_);
    std::vector<object_ref<XThread>> threads;
    for (auto& [id, thread] : threads_by_id_) {
        thread->Retain();
        threads.push_back(object_ref<XThread>(thread));
    }
    return threads;
}

static std::atomic<int> s_exec_count{0};
static std::atomic<int> s_exec_miss{0};
void KernelState::Execute(uint32_t guest_addr, PPCContext* ctx) {
    int count = ++s_exec_count;
    if (count <= 20) {
        printf("[EXECUTE %d] Function 0x%08X\n", count, guest_addr);
        fflush(stdout);
    }
    
    if (function_lookup_) {
        PPCFunc* func = function_lookup_(guest_addr);
        if (func) {
            func(*ctx, memory_base_);
            return;
        } else {
            int miss = ++s_exec_miss;
            if (miss <= 20) {
                printf("[EXECUTE MISS %d] 0x%08X not found!\n", miss, guest_addr);
                fflush(stdout);
            }
        }
    }
    
    if (dispatch_func_) {
        dispatch_func_(guest_addr, ctx, memory_base_);
    } else if (s_exec_count <= 10) {
        printf("[EXECUTE] No dispatch for 0x%08X\n", guest_addr);
    }
}

size_t KernelState::thread_count() const {
    std::lock_guard<std::recursive_mutex> lock(const_cast<std::recursive_mutex&>(global_mutex_));
    return threads_by_id_.size();
}

void KernelState::OnThreadStarted() {
    size_t count = running_thread_count_.fetch_add(1) + 1;
    printf("[KERNEL] Thread started (running=%zu)\n", count);
}

void KernelState::OnThreadFinished() {
    size_t count = running_thread_count_.fetch_sub(1) - 1;
    printf("[KERNEL] Thread finished (running=%zu)\n", count);
}

size_t KernelState::running_thread_count() const {
    return running_thread_count_.load();
}

void KernelState::WaitForThreads(uint32_t timeout_ms) {
    auto start = std::chrono::steady_clock::now();
    
    printf("[KERNEL] Waiting for threads (registered=%zu, running=%zu)...\n", 
           thread_count(), running_thread_count());
    
    while (true) {
        size_t running = running_thread_count();
        if (running == 0) {
            printf("[KERNEL] All threads finished\n");
            break;
        }
        
        if (timeout_ms > 0) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            if (elapsed >= timeout_ms) {
                printf("[KERNEL] Timeout waiting for %zu running threads\n", running);
                break;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

}  // namespace kernel
}  // namespace xe
