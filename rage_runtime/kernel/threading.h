/**
 * Threading Primitives for Static Recompilation Runtime
 * Wraps host OS primitives (similar to Xenia's xe::threading)
 */

#ifndef RAGE_RUNTIME_THREADING_H_
#define RAGE_RUNTIME_THREADING_H_

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

namespace xe {
namespace threading {

enum class WaitResult {
    kSuccess,
    kTimeout,
    kAbandoned,
    kFailed,
    kUserCallback,
};

// Base wait handle
class WaitHandle {
public:
    virtual ~WaitHandle() = default;
    virtual void* native_handle() = 0;
};

// Event - manual or auto-reset
class Event : public WaitHandle {
public:
    Event(bool manual_reset, bool initial_state);
    ~Event() override;
    
    void Set();
    void Reset();
    void Pulse();
    void* native_handle() override;
    
    bool Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
    
private:
    bool manual_reset_;
    bool signaled_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// Mutex/Mutant
class Mutant : public WaitHandle {
public:
    Mutant(bool initial_owner);
    ~Mutant() override;
    
    bool Release();
    void* native_handle() override;
    
    bool Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
    
private:
    std::recursive_mutex mutex_;
    std::thread::id owner_;
    int recursion_count_ = 0;
};

// Semaphore
class Semaphore : public WaitHandle {
public:
    Semaphore(int32_t initial_count, int32_t maximum_count);
    ~Semaphore() override;
    
    bool Release(int32_t release_count, int32_t* previous_count);
    void* native_handle() override;
    
    bool Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
    
private:
    int32_t count_;
    int32_t maximum_count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

// Timer
class Timer : public WaitHandle {
public:
    Timer();
    ~Timer() override;
    
    bool SetOnce(std::chrono::nanoseconds due_time);
    bool SetRepeating(std::chrono::nanoseconds due_time, std::chrono::milliseconds period);
    bool Cancel();
    void* native_handle() override;
    
private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool signaled_ = false;
};

// Thread wrapper
class Thread {
public:
    using EntryPoint = void(*)(void* param);
    
    Thread();
    ~Thread();
    
    bool Create(EntryPoint entry, void* param, uint32_t stack_size = 0);
    void set_name(const std::string& name);
    
    bool Resume(uint32_t* suspend_count = nullptr);
    bool Suspend(uint32_t* suspend_count = nullptr);
    void Join();
    
    static bool IsInThread();
    static Thread* GetCurrentThread();
    
    uint32_t thread_id() const { return thread_id_; }
    
private:
    std::unique_ptr<std::thread> thread_;
    uint32_t thread_id_ = 0;
    std::string name_;
    
    static thread_local Thread* current_thread_;
};

// Wait functions
WaitResult Wait(WaitHandle* handle, bool alertable, std::chrono::milliseconds timeout);
std::pair<WaitResult, size_t> WaitAny(std::vector<WaitHandle*> handles, bool alertable, 
                                       std::chrono::milliseconds timeout);
WaitResult WaitAll(std::vector<WaitHandle*> handles, bool alertable,
                   std::chrono::milliseconds timeout);
WaitResult SignalAndWait(WaitHandle* signal_handle, WaitHandle* wait_handle,
                         bool alertable, std::chrono::milliseconds timeout);

void MaybeYield();
void Sleep(std::chrono::milliseconds duration);

}  // namespace threading
}  // namespace xe

#endif  // RAGE_RUNTIME_THREADING_H_
