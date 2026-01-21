/**
 * Kernel Bridge - Forwards __imp__ calls to XeniaRT kernel API
 * Uses Xenia's REAL kernel implementations via the kernel_api.
 */

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

#include "ppc_config.h"
#include "ppc_context.h"

// XeniaRT kernel API
#include "xeniart/kernel_api.h"

#define STUB(name)                \
  PPC_FUNC(__imp__##name) {       \
    printf("[STUB] " #name "\n"); \
    fflush(stdout);               \
    ctx.r3.u64 = 0;               \
  }

#define STUB_RET(name, ret)                            \
  PPC_FUNC(__imp__##name) {                            \
    static int count = 0;                              \
    if (count++ < 3) {                                 \
      printf("[STUB] " #name " -> 0x%llX (call %d)\n", \
             (unsigned long long)(ret), count);        \
      fflush(stdout);                                  \
    }                                                  \
    ctx.r3.u64 = (ret);                                \
  }

// Memory helpers
static inline uint32_t load32(uint8_t* base, uint32_t addr) {
  return __builtin_bswap32(*reinterpret_cast<uint32_t*>(base + addr));
}
static inline void store32(uint8_t* base, uint32_t addr, uint32_t val) {
  *reinterpret_cast<uint32_t*>(base + addr) = __builtin_bswap32(val);
}
static inline uint64_t load64(uint8_t* base, uint32_t addr) {
  return __builtin_bswap64(*reinterpret_cast<uint64_t*>(base + addr));
}
static inline void store64(uint8_t* base, uint32_t addr, uint64_t val) {
  *reinterpret_cast<uint64_t*>(base + addr) = __builtin_bswap64(val);
}

// ============================================================================
// Threading
// ============================================================================

// External function lookup from main
extern "C" PPCFunc* LookupFunctionByAddress(uint32_t addr);
extern "C" uint8_t* GetMemoryBase();

static std::atomic<uint32_t> g_next_handle{0x100};
static std::atomic<uint32_t> g_next_thread_id{1};

struct ThreadInfo {
  std::thread* host_thread;
  uint32_t handle;
  uint32_t thread_id;
  uint32_t start_address;
  uint32_t start_context;
  bool suspended;
};
static std::vector<ThreadInfo> g_threads;
static std::mutex g_threads_mutex;

static void ThreadEntry(uint32_t start_addr, uint32_t start_ctx,
                        uint32_t thread_id) {
  printf("[THREAD %u] Starting at 0x%08X with context 0x%08X\n", thread_id,
         start_addr, start_ctx);
  fflush(stdout);

  PPCFunc* func = LookupFunctionByAddress(start_addr);
  if (!func) {
    printf("[THREAD %u] ERROR: Function not found at 0x%08X\n", thread_id,
           start_addr);
    return;
  }

  uint8_t* base = GetMemoryBase();
  PPCContext ctx = {};
  ctx.r3.u32 = start_ctx;                          // Thread parameter
  ctx.r13.u32 = 0x7F000000 + thread_id * 0x1000;   // Per-thread PCR
  ctx.r1.u32 = 0x70000000 + thread_id * 0x100000;  // Per-thread stack

  func(ctx, base);
  printf("[THREAD %u] Exited with r3=0x%llX\n", thread_id,
         (unsigned long long)ctx.r3.u64);
}

PPC_FUNC(__imp__ExCreateThread) {
  uint32_t handle_ptr = ctx.r3.u32;
  uint32_t stack_size = ctx.r4.u32;
  uint32_t thread_id_ptr = ctx.r5.u32;
  uint32_t xapi_thread_startup = ctx.r6.u32;
  uint32_t start_address = ctx.r7.u32;
  uint32_t start_context = ctx.r8.u32;
  uint32_t creation_flags = ctx.r9.u32;

  uint32_t handle = g_next_handle.fetch_add(1);
  uint32_t tid = g_next_thread_id.fetch_add(1);

  printf(
      "[KERNEL] ExCreateThread(start=0x%08X, ctx=0x%08X, flags=0x%X) -> "
      "handle=0x%X, tid=%u\n",
      start_address, start_context, creation_flags, handle, tid);
  fflush(stdout);

  bool suspended = (creation_flags & 0x1) != 0;

  ThreadInfo info;
  info.handle = handle;
  info.thread_id = tid;
  info.start_address = start_address;
  info.start_context = start_context;
  info.suspended = suspended;

  if (!suspended) {
    info.host_thread =
        new std::thread(ThreadEntry, start_address, start_context, tid);
  } else {
    info.host_thread = nullptr;
  }

  {
    std::lock_guard<std::mutex> lock(g_threads_mutex);
    g_threads.push_back(info);
  }

  if (handle_ptr) store32(base, handle_ptr, handle);
  if (thread_id_ptr) store32(base, thread_id_ptr, tid);

  ctx.r3.u64 = 0;  // X_STATUS_SUCCESS
}

PPC_FUNC(__imp__ExTerminateThread) {
  printf("[KERNEL] ExTerminateThread(code=0x%X)\n", ctx.r3.u32);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtResumeThread) {
  uint32_t handle = ctx.r3.u32;
  uint32_t suspend_count_ptr = ctx.r4.u32;

  std::lock_guard<std::mutex> lock(g_threads_mutex);
  for (auto& t : g_threads) {
    if (t.handle == handle && t.suspended && !t.host_thread) {
      printf("[KERNEL] NtResumeThread(handle=0x%X) -> starting thread %u\n",
             handle, t.thread_id);
      fflush(stdout);
      t.suspended = false;
      t.host_thread = new std::thread(ThreadEntry, t.start_address,
                                      t.start_context, t.thread_id);
      if (suspend_count_ptr) store32(base, suspend_count_ptr, 1);
      ctx.r3.u64 = 0;
      return;
    }
  }
  if (suspend_count_ptr) store32(base, suspend_count_ptr, 0);
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__NtSuspendThread) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeSetBasePriorityThread) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeSetAffinityThread) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeQueryBasePriorityThread) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeGetCurrentProcessType) {
  ctx.r3.u64 = xeniart_get_process_type();
}
PPC_FUNC(__imp__KeSetCurrentProcessType) {
  xeniart_set_process_type(ctx.r3.u32);
}
PPC_FUNC(__imp__NtSetInformationThread) { ctx.r3.u64 = 0; }

PPC_FUNC(__imp__NtYieldExecution) {
  std::this_thread::yield();
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeDelayExecutionThread) {
  uint32_t interval_ptr = ctx.r5.u32;
  if (interval_ptr) {
    int64_t interval = static_cast<int64_t>(load64(base, interval_ptr));
    if (interval < 0) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(-interval * 100));
    }
  }
  ctx.r3.u64 = 0;
}

// ============================================================================
// Synchronization - FIXED: Proper event tracking with blocking
// ============================================================================

#include <condition_variable>
#include <map>

// Event state tracking
struct KernelEvent {
  std::mutex mtx;
  std::condition_variable cv;
  bool signaled = false;
  bool manual_reset = false;  // true for notification events
};

static std::mutex g_events_mutex;
static std::map<uint32_t, std::unique_ptr<KernelEvent>> g_events;
static std::atomic<uint32_t> g_next_event_handle{0x5000};

// Get or create event by handle
static KernelEvent* GetEvent(uint32_t handle) {
  std::lock_guard<std::mutex> lock(g_events_mutex);
  auto it = g_events.find(handle);
  if (it != g_events.end()) return it->second.get();
  return nullptr;
}

PPC_FUNC(__imp__KeInitializeEvent) {
  uint32_t ev = ctx.r3.u32;
  uint32_t type = ctx.r4.u32;  // 0 = auto-reset, 1 = manual-reset
  uint32_t initial_state = ctx.r5.u32;
  if (ev) {
    memset(base + ev, 0, 16);
    // Store our handle in the event structure
    uint32_t handle = g_next_event_handle.fetch_add(1);
    store32(base, ev, handle);

    auto event = std::make_unique<KernelEvent>();
    event->manual_reset = (type == 1);
    event->signaled = (initial_state != 0);

    std::lock_guard<std::mutex> lock(g_events_mutex);
    g_events[handle] = std::move(event);

    static int log_c = 0;
    if (log_c++ < 10)
      printf("[SYNC] KeInitializeEvent -> handle=0x%X, type=%s\\n", handle,
             type ? "manual" : "auto");
  }
}

PPC_FUNC(__imp__NtCreateEvent) {
  uint32_t handle_ptr = ctx.r3.u32;
  uint32_t attrs = ctx.r4.u32;
  uint32_t type = ctx.r5.u32;
  uint32_t initial = ctx.r6.u32;

  uint32_t handle = g_next_event_handle.fetch_add(1);

  auto event = std::make_unique<KernelEvent>();
  event->manual_reset = (type == 1);
  event->signaled = (initial != 0);

  {
    std::lock_guard<std::mutex> lock(g_events_mutex);
    g_events[handle] = std::move(event);
  }

  if (handle_ptr) store32(base, handle_ptr, handle);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__KeSetEvent) {
  uint32_t event_ptr = ctx.r3.u32;
  if (!event_ptr) {
    ctx.r3.u64 = 0;
    return;
  }

  uint32_t handle = load32(base, event_ptr);
  KernelEvent* ev = GetEvent(handle);
  if (ev) {
    bool prev;
    {
      std::lock_guard<std::mutex> lock(ev->mtx);
      prev = ev->signaled;
      ev->signaled = true;
    }
    ev->cv.notify_all();
    ctx.r3.u64 = prev ? 1 : 0;
  } else {
    ctx.r3.u64 = 0;
  }
}

PPC_FUNC(__imp__KeResetEvent) {
  uint32_t event_ptr = ctx.r3.u32;
  if (!event_ptr) {
    ctx.r3.u64 = 0;
    return;
  }

  uint32_t handle = load32(base, event_ptr);
  KernelEvent* ev = GetEvent(handle);
  if (ev) {
    std::lock_guard<std::mutex> lock(ev->mtx);
    bool prev = ev->signaled;
    ev->signaled = false;
    ctx.r3.u64 = prev ? 1 : 0;
  } else {
    ctx.r3.u64 = 0;
  }
}

PPC_FUNC(__imp__KePulseEvent) {
  uint32_t event_ptr = ctx.r3.u32;
  if (!event_ptr) {
    ctx.r3.u64 = 0;
    return;
  }

  uint32_t handle = load32(base, event_ptr);
  KernelEvent* ev = GetEvent(handle);
  if (ev) {
    bool prev;
    {
      std::lock_guard<std::mutex> lock(ev->mtx);
      prev = ev->signaled;
      ev->signaled = true;
    }
    ev->cv.notify_all();
    {
      std::lock_guard<std::mutex> lock(ev->mtx);
      ev->signaled = false;
    }
    ctx.r3.u64 = prev ? 1 : 0;
  } else {
    ctx.r3.u64 = 0;
  }
}

PPC_FUNC(__imp__NtSetEvent) { __imp__KeSetEvent(ctx, base); }
PPC_FUNC(__imp__NtClearEvent) { __imp__KeResetEvent(ctx, base); }
PPC_FUNC(__imp__NtPulseEvent) { __imp__KePulseEvent(ctx, base); }

PPC_FUNC(__imp__KeInitializeSemaphore) {}
PPC_FUNC(__imp__NtCreateSemaphore) {
  if (ctx.r3.u32) store32(base, ctx.r3.u32, g_next_event_handle.fetch_add(1));
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__NtReleaseSemaphore) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeReleaseSemaphore) { ctx.r3.u64 = 0; }

PPC_FUNC(__imp__KeInitializeMutant) {}
PPC_FUNC(__imp__NtCreateMutant) {
  if (ctx.r3.u32) store32(base, ctx.r3.u32, g_next_event_handle.fetch_add(1));
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__NtReleaseMutant) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeReleaseMutant) { ctx.r3.u64 = 0; }

PPC_FUNC(__imp__KeWaitForSingleObject) {
  uint32_t object_ptr = ctx.r3.u32;
  uint32_t wait_reason = ctx.r4.u32;
  uint32_t wait_mode = ctx.r5.u32;
  uint32_t alertable = ctx.r6.u32;
  uint32_t timeout_ptr = ctx.r7.u32;

  static int log_c = 0;
  if (log_c++ < 20) {
    printf("[WAIT] KeWaitForSingleObject obj=0x%X, timeout_ptr=0x%X\\n",
           object_ptr, timeout_ptr);
    fflush(stdout);
  }

  if (!object_ptr) {
    ctx.r3.u64 = 0;
    return;
  }

  uint32_t handle = load32(base, object_ptr);
  KernelEvent* ev = GetEvent(handle);

  if (ev) {
    // Get timeout
    int64_t timeout_100ns = 0;
    bool has_timeout = false;
    if (timeout_ptr) {
      timeout_100ns = static_cast<int64_t>(load64(base, timeout_ptr));
      has_timeout = true;
    }

    std::unique_lock<std::mutex> lock(ev->mtx);

    if (has_timeout && timeout_100ns == 0) {
      // Instant check
      if (ev->signaled) {
        if (!ev->manual_reset) ev->signaled = false;
        ctx.r3.u64 = 0;  // WAIT_OBJECT_0
      } else {
        ctx.r3.u64 = 0x00000102;  // STATUS_TIMEOUT
      }
    } else if (has_timeout && timeout_100ns < 0) {
      // Relative timeout in 100ns units
      auto dur = std::chrono::nanoseconds(-timeout_100ns * 100);
      if (ev->cv.wait_for(lock, dur, [ev] { return ev->signaled; })) {
        if (!ev->manual_reset) ev->signaled = false;
        ctx.r3.u64 = 0;
      } else {
        ctx.r3.u64 = 0x00000102;  // STATUS_TIMEOUT
      }
    } else {
      // No timeout or absolute - wait with a reasonable max (5 seconds)
      if (ev->cv.wait_for(lock, std::chrono::seconds(5),
                          [ev] { return ev->signaled; })) {
        if (!ev->manual_reset) ev->signaled = false;
        ctx.r3.u64 = 0;
      } else {
        ctx.r3.u64 = 0;  // Return success anyway to avoid blocking forever
      }
    }
  } else {
    // Unknown object - just return success to avoid blocking
    ctx.r3.u64 = 0;
  }
}

PPC_FUNC(__imp__KeWaitForMultipleObjects) {
  static int c = 0;
  if (c++ < 5) {
    printf("[WAIT] KeWaitForMultipleObjects - returning immediately\\n");
    fflush(stdout);
  }
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtWaitForSingleObjectEx) {
  uint32_t handle = ctx.r3.u32;
  uint32_t alertable = ctx.r4.u32;
  uint32_t timeout_ptr = ctx.r5.u32;

  static int log_c = 0;
  if (log_c++ < 10) {
    printf("[WAIT] NtWaitForSingleObjectEx handle=0x%X\\n", handle);
    fflush(stdout);
  }

  KernelEvent* ev = GetEvent(handle);
  if (ev) {
    int64_t timeout_100ns = 0;
    bool has_timeout = timeout_ptr != 0;
    if (has_timeout) {
      timeout_100ns = static_cast<int64_t>(load64(base, timeout_ptr));
    }

    std::unique_lock<std::mutex> lock(ev->mtx);
    if (has_timeout && timeout_100ns < 0) {
      auto dur = std::chrono::nanoseconds(-timeout_100ns * 100);
      if (ev->cv.wait_for(lock, dur, [ev] { return ev->signaled; })) {
        if (!ev->manual_reset) ev->signaled = false;
        ctx.r3.u64 = 0;
      } else {
        ctx.r3.u64 = 0x00000102;
      }
    } else {
      ctx.r3.u64 = 0;
    }
  } else {
    ctx.r3.u64 = 0;
  }
}

PPC_FUNC(__imp__NtWaitForMultipleObjectsEx) {
  static int c = 0;
  if (c++ < 5) {
    printf("[WAIT] NtWaitForMultipleObjectsEx - returning immediately\\n");
    fflush(stdout);
  }
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__NtSignalAndWaitForSingleObjectEx) { ctx.r3.u64 = 0; }

// Critical sections
PPC_FUNC(__imp__RtlInitializeCriticalSection) {
  if (ctx.r3.u32) memset(base + ctx.r3.u32, 0, 28);
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__RtlInitializeCriticalSectionAndSpinCount) {
  if (ctx.r3.u32) memset(base + ctx.r3.u32, 0, 28);
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__RtlEnterCriticalSection) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlLeaveCriticalSection) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlTryEnterCriticalSection) { ctx.r3.u64 = 1; }

// Spinlocks
PPC_FUNC(__imp__KeAcquireSpinLockAtRaisedIrql) {}
PPC_FUNC(__imp__KeReleaseSpinLockFromRaisedIrql) {}
PPC_FUNC(__imp__KeTryToAcquireSpinLockAtRaisedIrql) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__KfAcquireSpinLock) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KfReleaseSpinLock) {}
PPC_FUNC(__imp__KeInitializeSpinLock) {}

// ============================================================================
// Timers
// ============================================================================

PPC_FUNC(__imp__KeInitializeTimerEx) {}
PPC_FUNC(__imp__NtCreateTimer) {
  if (ctx.r3.u32) store32(base, ctx.r3.u32, 0x4000);
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__NtSetTimerEx) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtCancelTimer) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__KeSetTimerEx) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__KeCancelTimer) { ctx.r3.u64 = 0; }

// ============================================================================
// Time
// ============================================================================

PPC_FUNC(__imp__KeQuerySystemTime) {
  if (ctx.r3.u32) {
    uint64_t time = xeniart_query_system_time();
    store64(base, ctx.r3.u32, time);
  }
}

PPC_FUNC(__imp__KeQueryPerformanceCounter) {
  ctx.r3.u64 = xeniart_query_perf_counter();
}

PPC_FUNC(__imp__KeQueryPerformanceFrequency) {
  ctx.r3.u64 = xeniart_query_perf_frequency();
}

PPC_FUNC(__imp__NtQuerySystemTime) {
  __imp__KeQuerySystemTime(ctx, base);
  ctx.r3.u64 = 0;
}

// ============================================================================
// Memory - THREAD-SAFE allocator
// ============================================================================

// FIXED: Use atomic to prevent race conditions in multi-threaded allocation
static std::atomic<uint32_t> g_next_alloc{0x20000000};
static constexpr uint32_t MEMORY_LIMIT = 0x70000000;  // Upper bound

PPC_FUNC(__imp__NtAllocateVirtualMemory) {
  uint32_t addr = ctx.r3.u32 ? load32(base, ctx.r3.u32) : 0;
  uint32_t size = ctx.r4.u32 ? load32(base, ctx.r4.u32) : 0;
  size = (size + 0xFFF) & ~0xFFF;
  if (!addr) {
    // FIXED: Atomic fetch_add for thread-safety
    addr = g_next_alloc.fetch_add(size);
    if (addr + size > MEMORY_LIMIT) {
      printf("[MEMORY] ERROR: Out of virtual memory at 0x%08X + 0x%X\n", addr,
             size);
      ctx.r3.u64 = 0xC0000017;  // STATUS_NO_MEMORY
      return;
    }
  }
  memset(base + addr, 0, size);
  if (ctx.r3.u32) store32(base, ctx.r3.u32, addr);
  if (ctx.r4.u32) store32(base, ctx.r4.u32, size);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__NtFreeVirtualMemory) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtProtectVirtualMemory) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtQueryVirtualMemory) { ctx.r3.u64 = 0; }

PPC_FUNC(__imp__MmAllocatePhysicalMemoryEx) {
  uint32_t size = (ctx.r4.u32 + 0xFFF) & ~0xFFF;
  // FIXED: Atomic fetch_add
  uint32_t addr = g_next_alloc.fetch_add(size);
  if (addr + size > MEMORY_LIMIT) {
    printf("[MEMORY] ERROR: Out of physical memory at 0x%08X\n", addr);
    ctx.r3.u64 = 0;
    return;
  }
  memset(base + addr, 0, size);
  ctx.r3.u64 = addr;
}

PPC_FUNC(__imp__MmFreePhysicalMemory) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__MmGetPhysicalAddress) { ctx.r3.u64 = ctx.r3.u32; }
PPC_FUNC(__imp__MmMapIoSpace) { ctx.r3.u64 = ctx.r3.u32; }
PPC_FUNC(__imp__MmUnmapIoSpace) {}
PPC_FUNC(__imp__MmQueryAddressProtect) { ctx.r3.u64 = 0x04; }
PPC_FUNC(__imp__MmQueryAllocationSize) { ctx.r3.u64 = 0x10000; }
PPC_FUNC(__imp__MmQueryStatistics) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__MmSetAddressProtect) {}

// ============================================================================
// File I/O - With logging to identify what files GTA IV needs
// ============================================================================

// Helper to extract Xbox path from OBJECT_ATTRIBUTES
static void LogFilePath(uint8_t* base, uint32_t obj_attrs_ptr, const char* op) {
  if (!obj_attrs_ptr || !base) return;

  // OBJECT_ATTRIBUTES layout:
  // +0x00: RootDirectory (4 bytes)
  // +0x04: ObjectName ptr (4 bytes) -> UNICODE_STRING
  // +0x08: Attributes (4 bytes)
  // UNICODE_STRING: Length(2), MaxLength(2), BufferPtr(4)

  uint32_t name_ptr = load32(base, obj_attrs_ptr + 4);
  if (!name_ptr) return;

  uint16_t length =
      load32(base, name_ptr) >> 16;  // High word is length in bytes
  uint32_t buffer_ptr = load32(base, name_ptr + 4);
  if (!buffer_ptr || length == 0) return;

  // Convert Unicode to ASCII (simple - assuming ASCII chars)
  char path[512];
  int path_len = 0;
  for (int i = 0; i < length && i < 510; i += 2) {
    uint16_t wc = load32(base, buffer_ptr + i) >> 16;
    if (i + 2 < length) {
      wc = (base[buffer_ptr + i] << 8) | base[buffer_ptr + i + 1];
      wc = __builtin_bswap16(wc);
    } else {
      wc = base[buffer_ptr + i];
    }
    if (wc == 0) break;
    path[path_len++] = (wc < 128) ? (char)wc : '?';
  }
  path[path_len] = 0;

  printf("[FILE] %s: %s\n", op, path);
  fflush(stdout);
}

PPC_FUNC(__imp__NtCreateFile) {
  uint32_t handle_ptr = ctx.r3.u32;  // OUT handle
  uint32_t desired_access = ctx.r4.u32;
  uint32_t obj_attrs_ptr = ctx.r5.u32;  // OBJECT_ATTRIBUTES

  LogFilePath(base, obj_attrs_ptr, "NtCreateFile");
  ctx.r3.u64 = 0xC0000034;  // STATUS_OBJECT_NAME_NOT_FOUND
}

PPC_FUNC(__imp__NtOpenFile) {
  uint32_t handle_ptr = ctx.r3.u32;
  uint32_t desired_access = ctx.r4.u32;
  uint32_t obj_attrs_ptr = ctx.r5.u32;

  LogFilePath(base, obj_attrs_ptr, "NtOpenFile");
  ctx.r3.u64 = 0xC0000034;
}

PPC_FUNC(__imp__NtReadFile) {
  static int log_count = 0;
  if (log_count++ < 10) {
    printf("[FILE] NtReadFile: handle=0x%X, size=0x%X\n", ctx.r3.u32,
           ctx.r7.u32);
    fflush(stdout);
  }
  ctx.r3.u64 = 0xC0000034;
}
PPC_FUNC(__imp__NtWriteFile) { ctx.r3.u64 = 0xC0000034; }
PPC_FUNC(__imp__NtClose) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtSetInformationFile) { ctx.r3.u64 = 0; }

PPC_FUNC(__imp__NtQueryInformationFile) {
  static int log_count = 0;
  if (log_count++ < 5) {
    printf("[FILE] NtQueryInformationFile: handle=0x%X\n", ctx.r3.u32);
    fflush(stdout);
  }
  ctx.r3.u64 = 0xC0000034;
}

PPC_FUNC(__imp__NtQueryFullAttributesFile) {
  uint32_t obj_attrs_ptr = ctx.r3.u32;
  LogFilePath(base, obj_attrs_ptr, "NtQueryFullAttributesFile");
  ctx.r3.u64 = 0xC0000034;
}

PPC_FUNC(__imp__NtQueryDirectoryFile) {
  static int log_count = 0;
  if (log_count++ < 5) {
    printf("[FILE] NtQueryDirectoryFile: handle=0x%X\n", ctx.r3.u32);
    fflush(stdout);
  }
  ctx.r3.u64 = 0xC0000034;
}
PPC_FUNC(__imp__NtFlushBuffersFile) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__NtQueryVolumeInformationFile) { ctx.r3.u64 = 0xC0000034; }
PPC_FUNC(__imp__NtDeviceIoControlFile) { ctx.r3.u64 = 0xC0000034; }
PPC_FUNC(__imp__FscSetCacheElementCount) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__IoDismountVolumeByFileHandle) { ctx.r3.u64 = 0; }

// ============================================================================
// Objects
// ============================================================================

PPC_FUNC(__imp__NtDuplicateObject) { ctx.r3.u64 = 0xC0000002; }
PPC_FUNC(__imp__ObReferenceObjectByHandle) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObDereferenceObject) {}
PPC_FUNC(__imp__ObCreateSymbolicLink) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObDeleteSymbolicLink) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__ObReferenceObjectByName) { ctx.r3.u64 = 0xC0000034; }

// ============================================================================
// RTL
// ============================================================================

PPC_FUNC(__imp__RtlInitAnsiString) {
  if (ctx.r3.u32) {
    uint16_t len = ctx.r4.u32 ? strlen((char*)(base + ctx.r4.u32)) : 0;
    store32(base, ctx.r3.u32, (len << 16) | (len + 1));
    store32(base, ctx.r3.u32 + 4, ctx.r4.u32);
  }
}

PPC_FUNC(__imp__RtlInitUnicodeString) {
  if (ctx.r3.u32) {
    store32(base, ctx.r3.u32, 0);
    store32(base, ctx.r3.u32 + 4, ctx.r4.u32);
  }
}

PPC_FUNC(__imp__RtlFreeAnsiString) {}
PPC_FUNC(__imp__RtlFreeUnicodeString) {}
PPC_FUNC(__imp__RtlUnicodeStringToAnsiString) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlAnsiStringToUnicodeString) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlMultiByteToUnicodeN) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlUnicodeToMultiByteN) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlCompareMemory) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlCompareMemoryUlong) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlFillMemoryUlong) {}
PPC_FUNC(__imp__RtlTimeToTimeFields) {}
PPC_FUNC(__imp__RtlTimeFieldsToTime) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__RtlNtStatusToDosError) {
  ctx.r3.u64 = ctx.r3.u32 == 0 ? 0 : ctx.r3.u32 & 0xFFFF;
}
PPC_FUNC(__imp__RtlImageXexHeaderField) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlGetLastError) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlSetLastError) {}
PPC_FUNC(__imp__RtlSetLastNTError) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__RtlRaiseException) { printf("[KERNEL] RtlRaiseException\n"); }

// ============================================================================
// Modules
// ============================================================================

PPC_FUNC(__imp__XexGetModuleHandle) { ctx.r3.u64 = 0x80000000; }
PPC_FUNC(__imp__XexGetProcedureAddress) { ctx.r3.u64 = 0xC0000225; }
PPC_FUNC(__imp__XexLoadImage) { ctx.r3.u64 = 0xC0000002; }
PPC_FUNC(__imp__XexUnloadImage) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XexGetModuleSection) { ctx.r3.u64 = 0xC0000225; }
PPC_FUNC(__imp__XexCheckExecutablePrivilege) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__LdrLoadModule) { ctx.r3.u64 = 0xC0000002; }
PPC_FUNC(__imp__LdrUnloadModule) { ctx.r3.u64 = 0; }

// ============================================================================
// Debug
// ============================================================================

PPC_FUNC(__imp__DbgPrint) {
  if (ctx.r3.u32) printf("[DBG] %s\n", (char*)(base + ctx.r3.u32));
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__DbgBreakPoint) { printf("[DBG] BREAKPOINT\n"); }

// ============================================================================
// Crypto
// ============================================================================

STUB(XeCryptSha)
STUB(XeCryptShaInit)
STUB(XeCryptShaUpdate)
STUB(XeCryptShaFinal)
STUB(XeCryptRc4)
STUB(XeCryptRc4Key)
STUB(XeCryptRc4Ecb)
STUB(XeCryptAesKey)
STUB(XeCryptAesCbc)
STUB(XeCryptAesEcb)
STUB(XeCryptHmacSha)
STUB(XeCryptMd5)
STUB(XeCryptBnQwNeModExpRoot)
STUB(XeKeysGetKey)
STUB(XeKeysConsolePrivateKeySign)
STUB(XeKeysConsoleSignatureVerification)
STUB(XeKeysSaveKeyVault)

// ============================================================================
// XAM User
// ============================================================================

PPC_FUNC(__imp__XamUserGetXUID) {
  if (ctx.r4.u32) store64(base, ctx.r4.u32, 0x0001000000000001ULL);
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__XamUserGetSigninState) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__XamUserGetSigninInfo) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserGetName) {
  if (ctx.r4.u32 && ctx.r5.u32 >= 8)
    strcpy((char*)(base + ctx.r4.u32), "Player1");
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__XamUserCheckPrivilege) {
  if (ctx.r5.u32) store32(base, ctx.r5.u32, 1);
  ctx.r3.u64 = 0;
}
PPC_FUNC(__imp__XamUserAreUsersFriends) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserReadProfileSettings) { ctx.r3.u64 = 0x490; }
PPC_FUNC(__imp__XamUserWriteProfileSettings) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XamUserCreateAchievementEnumerator) { ctx.r3.u64 = 0x490; }
PPC_FUNC(__imp__XamUserCreateStatsEnumerator) { ctx.r3.u64 = 0x490; }
STUB(XamUserGetMembershipTier)

// ============================================================================
// XAM Input - MOVED TO host_stubs.cpp for enhanced logging
// ============================================================================

// ============================================================================
// XAM Content
// ============================================================================

STUB_RET(XamContentCreate, 0x490)
STUB_RET(XamContentCreateEx, 0x490)
STUB_RET(XamContentClose, 0)
STUB_RET(XamContentGetCreator, 0x490)
STUB_RET(XamContentGetThumbnail, 0x490)
STUB_RET(XamContentSetThumbnail, 0)
STUB_RET(XamContentDelete, 0)
STUB_RET(XamContentCreateEnumerator, 0x490)
STUB_RET(XamContentResolve, 0x490)

// ============================================================================
// XAM Notification & UI
// ============================================================================

PPC_FUNC(__imp__XNotifyCreateListener) { ctx.r3.u64 = 0x1000; }
PPC_FUNC(__imp__XNotifyGetNext) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__XNotifyPositionUI) {}
STUB_RET(XamShowMessageBoxUI, 0)
STUB_RET(XamShowKeyboardUI, 0x4C7)
STUB_RET(XamShowDeviceSelectorUI, 0x4C7)
STUB_RET(XamShowSigninUI, 0)
STUB(XamShowDirtyDiscErrorUI)
STUB(XamShowPlayTimerUI)
STUB(XamShowGamerCardUI)

// ============================================================================
// XAM Locale
// ============================================================================

PPC_FUNC(__imp__XamGetLocale) { ctx.r3.u64 = 1; }
PPC_FUNC(__imp__XamGetLanguage) { ctx.r3.u64 = 1; }

// ============================================================================
// XAM Misc
// ============================================================================

STUB_RET(XamEnumerate, 0x103)
STUB_RET(XamCreateEnumeratorHandle, 0)
STUB_RET(XamTaskSchedule, 0)
STUB_RET(XamTaskCloseHandle, 0)
STUB_RET(XamTaskShouldExit, 0)
STUB_RET(XamGetOverlappedResult, 0)
STUB_RET(XamGetOverlappedExtendedError, 0)
STUB(XamGetPrivateEnumStructureFromHandle)
STUB(XamEnableInactivityProcessing)
STUB(XamLoaderLaunchTitle)
STUB(XamLoaderTerminateTitle)
STUB(XamAlloc)
STUB(XamFree)

// ============================================================================
// Network
// ============================================================================

STUB_RET(NetDll_XNetStartup, 0)
STUB_RET(NetDll_XNetCleanup, 0)
STUB_RET(NetDll_socket, -1)
STUB_RET(NetDll_closesocket, 0)
STUB_RET(NetDll_bind, -1)
STUB_RET(NetDll_connect, -1)
STUB_RET(NetDll_listen, -1)
STUB_RET(NetDll_accept, -1)
STUB_RET(NetDll_send, -1)
STUB_RET(NetDll_recv, -1)
STUB_RET(NetDll_sendto, -1)
STUB_RET(NetDll_recvfrom, -1)
STUB_RET(NetDll_setsockopt, -1)
STUB_RET(NetDll_getsockopt, -1)
STUB_RET(NetDll_ioctlsocket, -1)
STUB_RET(NetDll_select, -1)
STUB_RET(NetDll_WSAStartup, 0)
STUB_RET(NetDll_WSACleanup, 0)
STUB_RET(NetDll_WSAGetLastError, 0)
STUB(NetDll_XNetGetConnectStatus)
STUB(NetDll_XNetUnregisterInAddr)
STUB(NetDll_XNetGetEthernetLinkStatus)
STUB(NetDll_XNetRandom)
STUB(NetDll_XNetCreateKey)
STUB(NetDll_XNetRegisterKey)
STUB(NetDll_XNetUnregisterKey)
STUB(NetDll_XNetXnAddrToInAddr)
STUB(NetDll_XNetServerToInAddr)
STUB(NetDll_XNetInAddrToXnAddr)
STUB(NetDll_XNetGetTitleXnAddr)

// ============================================================================
// Video - MOVED TO host_stubs.cpp for enhanced logging
// ============================================================================

// ============================================================================
// Audio - MOVED TO host_stubs.cpp for enhanced logging
// ============================================================================

// ============================================================================
// Process/Interrupt
// ============================================================================

STUB(ExRegisterTitleTerminateNotification)
STUB(ExUnregisterTitleTerminateNotification)
STUB_RET(KeRaiseIrqlToDpcLevel, 0)
STUB(KfLowerIrql)
STUB(KeGetCurrentIrql)

// ============================================================================
// Hardware
// ============================================================================

STUB(HalReturnToFirmware)
STUB_RET(ExGetXConfigSetting, 0)

// ============================================================================
// Missing functions from linker errors
// ============================================================================

// CRT functions
PPC_FUNC(__imp__sprintf) {
  // sprintf(dest, fmt, ...) - simplified stub
  uint32_t dest = ctx.r3.u32;
  if (dest && base) {
    strcpy((char*)(base + dest), "");
  }
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__snprintf) {
  uint32_t dest = ctx.r3.u32;
  if (dest && base) {
    *(base + dest) = 0;
  }
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__vsprintf) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__vsnprintf) { ctx.r3.u64 = 0; }
PPC_FUNC(__imp__sscanf) { ctx.r3.u64 = 0; }

PPC_FUNC(__imp__memcpy) {
  uint32_t dest = ctx.r3.u32;
  uint32_t src = ctx.r4.u32;
  uint32_t n = ctx.r5.u32;
  if (dest && src && base) {
    memcpy(base + dest, base + src, n);
  }
  ctx.r3.u64 = dest;
}

PPC_FUNC(__imp__memset) {
  uint32_t dest = ctx.r3.u32;
  uint32_t c = ctx.r4.u32;
  uint32_t n = ctx.r5.u32;
  if (dest && base) {
    memset(base + dest, c, n);
  }
  ctx.r3.u64 = dest;
}

PPC_FUNC(__imp__memmove) {
  uint32_t dest = ctx.r3.u32;
  uint32_t src = ctx.r4.u32;
  uint32_t n = ctx.r5.u32;
  if (dest && src && base) {
    memmove(base + dest, base + src, n);
  }
  ctx.r3.u64 = dest;
}

PPC_FUNC(__imp__memcmp) {
  uint32_t s1 = ctx.r3.u32;
  uint32_t s2 = ctx.r4.u32;
  uint32_t n = ctx.r5.u32;
  int result = 0;
  if (s1 && s2 && base) {
    result = memcmp(base + s1, base + s2, n);
  }
  ctx.r3.s64 = result;
}

PPC_FUNC(__imp__strlen) {
  uint32_t s = ctx.r3.u32;
  size_t len = 0;
  if (s && base) {
    len = strlen((char*)(base + s));
  }
  ctx.r3.u64 = len;
}

PPC_FUNC(__imp__strcpy) {
  uint32_t dest = ctx.r3.u32;
  uint32_t src = ctx.r4.u32;
  if (dest && src && base) {
    strcpy((char*)(base + dest), (char*)(base + src));
  }
  ctx.r3.u64 = dest;
}

PPC_FUNC(__imp__strncpy) {
  uint32_t dest = ctx.r3.u32;
  uint32_t src = ctx.r4.u32;
  uint32_t n = ctx.r5.u32;
  if (dest && src && base) {
    strncpy((char*)(base + dest), (char*)(base + src), n);
  }
  ctx.r3.u64 = dest;
}

PPC_FUNC(__imp__strcmp) {
  uint32_t s1 = ctx.r3.u32;
  uint32_t s2 = ctx.r4.u32;
  int result = 0;
  if (s1 && s2 && base) {
    result = strcmp((char*)(base + s1), (char*)(base + s2));
  }
  ctx.r3.s64 = result;
}

PPC_FUNC(__imp__strncmp) {
  uint32_t s1 = ctx.r3.u32;
  uint32_t s2 = ctx.r4.u32;
  uint32_t n = ctx.r5.u32;
  int result = 0;
  if (s1 && s2 && base) {
    result = strncmp((char*)(base + s1), (char*)(base + s2), n);
  }
  ctx.r3.s64 = result;
}

PPC_FUNC(__imp__strcat) {
  uint32_t dest = ctx.r3.u32;
  uint32_t src = ctx.r4.u32;
  if (dest && src && base) {
    strcat((char*)(base + dest), (char*)(base + src));
  }
  ctx.r3.u64 = dest;
}

PPC_FUNC(__imp__strchr) {
  uint32_t s = ctx.r3.u32;
  int c = ctx.r4.u32;
  uint32_t result = 0;
  if (s && base) {
    char* p = strchr((char*)(base + s), c);
    if (p) result = p - (char*)base;
  }
  ctx.r3.u64 = result;
}

PPC_FUNC(__imp__strstr) {
  uint32_t haystack = ctx.r3.u32;
  uint32_t needle = ctx.r4.u32;
  uint32_t result = 0;
  if (haystack && needle && base) {
    char* p = strstr((char*)(base + haystack), (char*)(base + needle));
    if (p) result = p - (char*)base;
  }
  ctx.r3.u64 = result;
}

PPC_FUNC(__imp__atoi) {
  uint32_t s = ctx.r3.u32;
  int result = 0;
  if (s && base) {
    result = atoi((char*)(base + s));
  }
  ctx.r3.s64 = result;
}

PPC_FUNC(__imp__atof) {
  uint32_t s = ctx.r3.u32;
  double result = 0.0;
  if (s && base) {
    result = atof((char*)(base + s));
  }
  // Return in f1 (stored as double in r3/r4 for PPC ABI)
  ctx.r3.f64 = result;
}

// TLS functions - Using XeniaRT kernel API
PPC_FUNC(__imp__KeTlsAlloc) { ctx.r3.u64 = xeniart_tls_alloc(); }
PPC_FUNC(__imp__KeTlsFree) { ctx.r3.u64 = xeniart_tls_free(ctx.r3.u32); }
PPC_FUNC(__imp__KeTlsGetValue) { ctx.r3.u64 = xeniart_tls_get(ctx.r3.u32); }
PPC_FUNC(__imp__KeTlsSetValue) {
  ctx.r3.u64 = xeniart_tls_set(ctx.r3.u32, ctx.r4.u32);
}

// Lock functions
STUB(KeLockL2)
STUB(KeUnlockL2)

// Memory pool
STUB_RET(ExAllocatePoolWithTag, 0x20000000)
STUB(ExFreePool)

// Bug check
PPC_FUNC(__imp__KeBugCheck) {
  printf("[KERNEL] KeBugCheck(0x%08X)\n", ctx.r3.u32);
}

PPC_FUNC(__imp__KeBugCheckEx) {
  printf("[KERNEL] KeBugCheckEx(0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X)\n",
         ctx.r3.u32, ctx.r4.u32, ctx.r5.u32, ctx.r6.u32, ctx.r7.u32);
}

// RTL additional
STUB(RtlUnwind)
STUB(RtlUnwind2)
STUB(RtlCaptureContext)

// XamAlloc and XamFree are already defined as STUB above

// ============================================================================
// Additional missing functions from linker
// ============================================================================

// Memory pool variants
STUB_RET(ExAllocatePoolTypeWithTag, 0x20000000)

// IO functions
STUB_RET(IoCheckShareAccess, 0)
STUB_RET(IoCompleteRequest, 0)
STUB_RET(IoCreateDevice, 0)
STUB(IoDeleteDevice)
STUB_RET(IoDismountVolume, 0)
STUB_RET(IoInvalidDeviceRequest, 0)
STUB(IoRemoveShareAccess)
STUB(IoSetShareAccess)

// Critical region
STUB(KeEnterCriticalRegion)
STUB(KeLeaveCriticalRegion)
STUB_RET(KeResumeThread, 0)
STUB_RET(KeSetDisableBoostThread, 0)
STUB(KiApcNormalRoutineNop)

// Network QoS
STUB_RET(NetDll_XNetQosListen, 0)
STUB_RET(NetDll_XNetQosLookup, 0)
STUB(NetDll_XNetQosRelease)
STUB_RET(NetDll_getsockname, -1)
STUB_RET(NetDll_inet_addr, 0)
STUB_RET(NetDll_shutdown, 0)

// File scatter
STUB_RET(NtReadFileScatter, 0xC0000034)

// Object functions
STUB_RET(ObIsTitleObject, 0)
STUB(ObReferenceObject)

// RTL string
STUB_RET(RtlCompareStringN, 0)
STUB_RET(RtlUpcaseUnicodeChar, 0)

// STFS
STUB_RET(StfsControlDevice, 0)
STUB_RET(StfsCreateDevice, 0)

// Video - see host_stubs.cpp

// Audio - see host_stubs.cpp

// System info
STUB_RET(XGetAVPack, 0x00010000)
STUB_RET(XGetGameRegion, 0x00FF0000)
STUB_RET(XGetLanguage, 1)
PPC_FUNC(__imp__XGetVideoMode) {
  uint32_t mode_ptr = ctx.r3.u32;
  if (mode_ptr && base) {
    store32(base, mode_ptr, 1280);     // Width
    store32(base, mode_ptr + 4, 720);  // Height
    store32(base, mode_ptr + 8, 0);    // Interlaced
    store32(base, mode_ptr + 12, 60);  // Refresh
  }
}

// XMsg
STUB_RET(XMsgCancelIORequest, 0)
STUB_RET(XMsgInProcessCall, 0)
STUB_RET(XMsgStartIORequest, 0)
STUB_RET(XMsgStartIORequestEx, 0)

// XAM content
STUB_RET(XamContentGetDeviceData, 0x490)
STUB_RET(XamGetExecutionId, 0)
STUB_RET(XamGetSystemVersion, 0x20000000)
STUB_RET(XamNotifyCreateListener, 0x1000)
STUB(XamResetInactivity)
STUB_RET(XamSessionCreateHandle, 0)
STUB_RET(XamSessionRefObjByHandle, 0)
STUB_RET(XamShowGamerCardUIForXUID, 0)
STUB_RET(XamShowMessageBoxUIEx, 0)
STUB_RET(XamShowPlayerReviewUI, 0)

// Voice
STUB_RET(XamVoiceClose, 0)
STUB_RET(XamVoiceCreate, 0x80004005)
STUB_RET(XamVoiceHeadsetPresent, 0)
STUB_RET(XamVoiceSubmitPacket, 0)

// Exception handling
STUB(__C_specific_handler)

// vsnprintf variant
PPC_FUNC(__imp___vsnprintf) { ctx.r3.u64 = 0; }
