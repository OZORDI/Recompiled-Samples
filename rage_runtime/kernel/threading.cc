/**
 * Threading Primitives Implementation
 */

#include "threading.h"
#include <atomic>

namespace xe {
namespace threading {

// Thread-local storage for current thread
thread_local Thread* Thread::current_thread_ = nullptr;
static std::atomic<uint32_t> next_thread_id_{1};

//=============================================================================
// Event
//=============================================================================
Event::Event(bool manual_reset, bool initial_state)
    : manual_reset_(manual_reset), signaled_(initial_state) {}

Event::~Event() = default;

void Event::Set() {
    std::lock_guard<std::mutex> lock(mutex_);
    signaled_ = true;
    if (manual_reset_) {
        cv_.notify_all();
    } else {
        cv_.notify_one();
    }
}

void Event::Reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    signaled_ = false;
}

void Event::Pulse() {
    std::lock_guard<std::mutex> lock(mutex_);
    signaled_ = true;
    if (manual_reset_) {
        cv_.notify_all();
    } else {
        cv_.notify_one();
    }
    signaled_ = false;
}

void* Event::native_handle() { return this; }

bool Event::Wait(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (timeout == std::chrono::milliseconds::max()) {
        cv_.wait(lock, [this] { return signaled_; });
    } else {
        if (!cv_.wait_for(lock, timeout, [this] { return signaled_; })) {
            return false;  // Timeout
        }
    }
    if (!manual_reset_) {
        signaled_ = false;
    }
    return true;
}

//=============================================================================
// Mutant (Recursive Mutex)
//=============================================================================
Mutant::Mutant(bool initial_owner) {
    if (initial_owner) {
        mutex_.lock();
        owner_ = std::this_thread::get_id();
        recursion_count_ = 1;
    }
}

Mutant::~Mutant() {
    while (recursion_count_ > 0) {
        mutex_.unlock();
        recursion_count_--;
    }
}

bool Mutant::Release() {
    if (owner_ != std::this_thread::get_id()) {
        return false;  // Not owned
    }
    recursion_count_--;
    if (recursion_count_ == 0) {
        owner_ = std::thread::id();
    }
    mutex_.unlock();
    return true;
}

void* Mutant::native_handle() { return this; }

bool Mutant::Wait(std::chrono::milliseconds timeout) {
    if (timeout == std::chrono::milliseconds::max()) {
        mutex_.lock();
    } else {
        // std::recursive_mutex doesn't have try_lock_for, so use polling
        auto start = std::chrono::steady_clock::now();
        while (!mutex_.try_lock()) {
            if (std::chrono::steady_clock::now() - start > timeout) {
                return false;
            }
            std::this_thread::yield();
        }
    }
    owner_ = std::this_thread::get_id();
    recursion_count_++;
    return true;
}

//=============================================================================
// Semaphore
//=============================================================================
Semaphore::Semaphore(int32_t initial_count, int32_t maximum_count)
    : count_(initial_count), maximum_count_(maximum_count) {}

Semaphore::~Semaphore() = default;

bool Semaphore::Release(int32_t release_count, int32_t* previous_count) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (previous_count) {
        *previous_count = count_;
    }
    if (count_ + release_count > maximum_count_) {
        return false;
    }
    count_ += release_count;
    for (int32_t i = 0; i < release_count; ++i) {
        cv_.notify_one();
    }
    return true;
}

void* Semaphore::native_handle() { return this; }

bool Semaphore::Wait(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (timeout == std::chrono::milliseconds::max()) {
        cv_.wait(lock, [this] { return count_ > 0; });
    } else {
        if (!cv_.wait_for(lock, timeout, [this] { return count_ > 0; })) {
            return false;
        }
    }
    count_--;
    return true;
}

//=============================================================================
// Timer
//=============================================================================
Timer::Timer() = default;
Timer::~Timer() = default;

bool Timer::SetOnce(std::chrono::nanoseconds due_time) {
    // TODO: Implement timer scheduling
    return true;
}

bool Timer::SetRepeating(std::chrono::nanoseconds due_time, std::chrono::milliseconds period) {
    // TODO: Implement repeating timer
    return true;
}

bool Timer::Cancel() {
    std::lock_guard<std::mutex> lock(mutex_);
    signaled_ = false;
    return true;
}

void* Timer::native_handle() { return this; }

//=============================================================================
// Thread
//=============================================================================
Thread::Thread() : thread_id_(next_thread_id_++) {}

Thread::~Thread() {
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

bool Thread::Create(EntryPoint entry, void* param, uint32_t stack_size) {
    thread_ = std::make_unique<std::thread>([this, entry, param]() {
        current_thread_ = this;
        entry(param);
        current_thread_ = nullptr;
    });
    return thread_ != nullptr;
}

void Thread::set_name(const std::string& name) {
    name_ = name;
#ifdef __APPLE__
    // macOS only allows setting name from the thread itself
#elif defined(__linux__)
    if (thread_ && thread_->native_handle()) {
        pthread_setname_np(thread_->native_handle(), name.substr(0, 15).c_str());
    }
#endif
}

bool Thread::Resume(uint32_t* suspend_count) {
    // Host threads don't have true suspend/resume
    if (suspend_count) *suspend_count = 0;
    return true;
}

bool Thread::Suspend(uint32_t* suspend_count) {
    // Host threads don't have true suspend/resume
    if (suspend_count) *suspend_count = 1;
    return true;
}

void Thread::Join() {
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

bool Thread::IsInThread() {
    return current_thread_ != nullptr;
}

Thread* Thread::GetCurrentThread() {
    return current_thread_;
}

//=============================================================================
// Wait Functions
//=============================================================================
WaitResult Wait(WaitHandle* handle, bool alertable, std::chrono::milliseconds timeout) {
    if (!handle) return WaitResult::kFailed;
    
    // Try to cast to each type and wait
    if (auto* event = dynamic_cast<Event*>(handle)) {
        return event->Wait(timeout) ? WaitResult::kSuccess : WaitResult::kTimeout;
    }
    if (auto* mutant = dynamic_cast<Mutant*>(handle)) {
        return mutant->Wait(timeout) ? WaitResult::kSuccess : WaitResult::kTimeout;
    }
    if (auto* sem = dynamic_cast<Semaphore*>(handle)) {
        return sem->Wait(timeout) ? WaitResult::kSuccess : WaitResult::kTimeout;
    }
    
    return WaitResult::kFailed;
}

std::pair<WaitResult, size_t> WaitAny(std::vector<WaitHandle*> handles, bool alertable,
                                       std::chrono::milliseconds timeout) {
    // Simple polling implementation
    auto start = std::chrono::steady_clock::now();
    while (true) {
        for (size_t i = 0; i < handles.size(); ++i) {
            auto result = Wait(handles[i], alertable, std::chrono::milliseconds(0));
            if (result == WaitResult::kSuccess) {
                return {WaitResult::kSuccess, i};
            }
        }
        
        if (timeout != std::chrono::milliseconds::max()) {
            if (std::chrono::steady_clock::now() - start > timeout) {
                return {WaitResult::kTimeout, 0};
            }
        }
        std::this_thread::yield();
    }
}

WaitResult WaitAll(std::vector<WaitHandle*> handles, bool alertable,
                   std::chrono::milliseconds timeout) {
    for (auto* handle : handles) {
        auto result = Wait(handle, alertable, timeout);
        if (result != WaitResult::kSuccess) {
            return result;
        }
    }
    return WaitResult::kSuccess;
}

WaitResult SignalAndWait(WaitHandle* signal_handle, WaitHandle* wait_handle,
                         bool alertable, std::chrono::milliseconds timeout) {
    // Signal the first handle
    if (auto* event = dynamic_cast<Event*>(signal_handle)) {
        event->Set();
    } else if (auto* mutant = dynamic_cast<Mutant*>(signal_handle)) {
        mutant->Release();
    } else if (auto* sem = dynamic_cast<Semaphore*>(signal_handle)) {
        sem->Release(1, nullptr);
    }
    
    // Wait on the second
    return Wait(wait_handle, alertable, timeout);
}

void MaybeYield() {
    std::this_thread::yield();
}

void Sleep(std::chrono::milliseconds duration) {
    std::this_thread::sleep_for(duration);
}

}  // namespace threading
}  // namespace xe
