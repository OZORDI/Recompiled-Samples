/**
 * RAGE Engine Static Recompilation Runtime
 * Shared runtime for all RAGE engine games (GTA IV, Table Tennis, Red Dead,
 * etc.)
 *
 * This file provides the common infrastructure for running statically
 * recompiled Xbox 360 RAGE engine games.
 */

#ifndef RAGE_RUNTIME_H_
#define RAGE_RUNTIME_H_

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

// Forward declarations - actual definitions come from game's ppc_context.h
struct PPCContext;
struct PPCFuncMapping;
typedef void PPCFunc(PPCContext& ctx, uint8_t* base);

namespace RageRuntime {

// ============================================================================
// Global State
// ============================================================================

inline uint8_t* g_memory_base = nullptr;
inline size_t g_memory_size = 0x100000000ULL;  // 4GB
inline std::unordered_map<uint32_t, PPCFunc*> g_functions;
inline std::atomic<uint64_t> g_call_count{0};
inline std::atomic<uint64_t> g_hit_count{0};
inline std::atomic<uint64_t> g_miss_count{0};

// Game-specific config (set from ppc_config.h)
inline uint32_t g_image_base = 0;
inline uint32_t g_image_size = 0;
inline uint32_t g_code_base = 0;
inline uint32_t g_code_size = 0;

// ============================================================================
// Memory Allocation
// ============================================================================

inline uint8_t* AllocateMemory(size_t size) {
#ifdef _WIN32
  void* mem =
      VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
  void* mem = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mem == MAP_FAILED) mem = nullptr;
#endif
  return static_cast<uint8_t*>(mem);
}

inline void FreeMemory(uint8_t* mem, size_t size) {
  if (!mem) return;
#ifdef _WIN32
  VirtualFree(mem, 0, MEM_RELEASE);
#else
  munmap(mem, size);
#endif
}

// ============================================================================
// Byte Swap Helpers
// ============================================================================

inline uint16_t bswap16(uint16_t v) { return __builtin_bswap16(v); }
inline uint32_t bswap32(uint32_t v) { return __builtin_bswap32(v); }
inline uint64_t bswap64(uint64_t v) { return __builtin_bswap64(v); }

inline uint32_t GuestLoad32(uint8_t* base, uint32_t addr) {
  return bswap32(*reinterpret_cast<uint32_t*>(base + addr));
}

inline void GuestStore32(uint8_t* base, uint32_t addr, uint32_t val) {
  *reinterpret_cast<uint32_t*>(base + addr) = bswap32(val);
}

// ============================================================================
// PCR (Processor Control Region) Initialization
// ============================================================================

inline void InitializePCR(uint8_t* base, uint32_t pcr_addr, uint32_t stack_base,
                          uint32_t stack_end, uint32_t thread_id,
                          uint32_t entry_point) {
  // PCR structure
  GuestStore32(base, pcr_addr + 0x00, pcr_addr);    // tls_ptr
  GuestStore32(base, pcr_addr + 0x30, pcr_addr);    // pcr_ptr (self-reference)
  GuestStore32(base, pcr_addr + 0x70, stack_base);  // stack_base_ptr
  GuestStore32(base, pcr_addr + 0x74, stack_end);   // stack_end_ptr
  GuestStore32(base, pcr_addr + 0x100, pcr_addr + 0x200);  // current_thread ptr
  GuestStore32(base, pcr_addr + 0x10C, 0);                 // current_cpu

  // KTHREAD at pcr + 0x200
  uint32_t kthread = pcr_addr + 0x200;
  GuestStore32(base, kthread + 0x00, 0x00060006);    // header.type = Thread
  GuestStore32(base, kthread + 0x5C, stack_base);    // stack_base
  GuestStore32(base, kthread + 0x60, stack_end);     // stack_limit
  GuestStore32(base, kthread + 0x68, pcr_addr);      // tls_address
  GuestStore32(base, kthread + 0x14C, thread_id);    // thread_id
  GuestStore32(base, kthread + 0x150, entry_point);  // start_address
}

// ============================================================================
// Function Registration and Lookup
// ============================================================================

inline void RegisterFunctions(const PPCFuncMapping* mappings) {
  size_t count = 0;
  // PPCFuncMapping layout: { size_t guest, PPCFunc* host }
  while (true) {
    size_t guest = mappings[count].guest;
    PPCFunc* func = mappings[count].host;

    if (guest == 0) break;
    if (func != nullptr) {
      g_functions[static_cast<uint32_t>(guest)] = func;
    }
    count++;
  }
  printf("[RAGE] Registered %zu functions\n", g_functions.size());
}

inline PPCFunc* LookupFunction(uint32_t addr) {
  auto it = g_functions.find(addr);
  return (it != g_functions.end()) ? it->second : nullptr;
}

// ============================================================================
// Lookup Table for Indirect Calls
// ============================================================================

inline void InitializeLookupTable(uint8_t* base) {
  if (g_code_base == 0 || g_code_size == 0) {
    printf("[RAGE] ERROR: Code base/size not set!\n");
    return;
  }

  // Perfect hash table location: base + IMAGE_BASE + IMAGE_SIZE
  uint8_t* table = base + g_image_base + g_image_size;
  size_t table_size = (g_code_size / 4) * sizeof(PPCFunc*) * 2;

  memset(table, 0, table_size);

  size_t populated = 0;
  for (const auto& [addr, func] : g_functions) {
    if (addr >= g_code_base && addr < g_code_base + g_code_size) {
      uint32_t offset = addr - g_code_base;
      PPCFunc** entry = reinterpret_cast<PPCFunc**>(
          table + static_cast<uint64_t>(offset) * 2);
      *entry = func;
      populated++;
    }
  }

  printf("[RAGE] Lookup table: %zu entries\n", populated);
}

// ============================================================================
// Execution
// ============================================================================

inline bool Execute(uint32_t addr, PPCContext& ctx, uint8_t* base) {
  g_call_count++;

  PPCFunc* func = LookupFunction(addr);
  if (func) {
    g_hit_count++;
    func(ctx, base);
    return true;
  }

  g_miss_count++;
  printf("[RAGE] Missing function at 0x%08X\n", addr);
  return false;
}

// ============================================================================
// Indirect Call Handler (Game-Agnostic)
// ============================================================================

// This is the improved indirect call handler that handles:
// 1. Null addresses (CRT callbacks not initialized)
// 2. Stack addresses (function pointers stored on stack)
// 3. Code addresses (normal function calls)
inline void CallIndirect(uint32_t addr, PPCContext& ctx, uint8_t* base) {
  if (addr == 0) {
    // Null callback - common for uninitialized CRT function pointers
    // This is expected during startup, just skip
    return;
  }

  // Check if address is in code region
  if (addr >= g_code_base && addr < g_code_base + g_code_size) {
    // Direct code address - lookup and call
    PPCFunc* func = LookupFunction(addr);
    if (func) {
      func(ctx, base);
    } else {
      printf("[RAGE] No function at code address 0x%08X\n", addr);
    }
    return;
  }

  // Address is outside code region - could be a function pointer in memory
  // Read the actual target address from memory
  if (addr < g_memory_size && addr >= 0x10000) {
    // Read the function pointer from guest memory (big-endian)
    uint32_t target = GuestLoad32(base, addr);

    if (target == 0) {
      // Null function pointer
      return;
    }

    if (target >= g_code_base && target < g_code_base + g_code_size) {
      PPCFunc* func = LookupFunction(target);
      if (func) {
        func(ctx, base);
        return;
      }
    }

    printf("[RAGE] Indirect call: 0x%08X -> 0x%08X (not found)\n", addr,
           target);
  } else {
    printf("[RAGE] Invalid indirect call address: 0x%08X\n", addr);
  }
}

// ============================================================================
// Configuration
// ============================================================================

inline void Configure(uint32_t image_base, uint32_t image_size,
                      uint32_t code_base, uint32_t code_size) {
  g_image_base = image_base;
  g_image_size = image_size;
  g_code_base = code_base;
  g_code_size = code_size;
}

// ============================================================================
// Initialization
// ============================================================================

inline bool Initialize() {
  g_memory_base = AllocateMemory(g_memory_size);
  if (!g_memory_base) {
    printf("[RAGE] Failed to allocate 4GB memory\n");
    return false;
  }
  printf("[RAGE] Memory: %p (4GB)\n", static_cast<void*>(g_memory_base));
  return true;
}

inline void Shutdown() {
  printf("[RAGE] Stats: calls=%llu hits=%llu miss=%llu\n",
         static_cast<unsigned long long>(g_call_count.load()),
         static_cast<unsigned long long>(g_hit_count.load()),
         static_cast<unsigned long long>(g_miss_count.load()));

  FreeMemory(g_memory_base, g_memory_size);
  g_memory_base = nullptr;
  g_functions.clear();
}

inline uint8_t* GetMemoryBase() { return g_memory_base; }

}  // namespace RageRuntime

#endif  // RAGE_RUNTIME_H_
