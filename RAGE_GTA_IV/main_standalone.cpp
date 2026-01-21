/**
 * GTA IV Static Recompilation - Standalone Launcher
 * 
 * Usage: ./gta4_standalone [game_path]
 * Default game path: /Users/Ozordi/Downloads/xenia/RAGE Games/extracted/gta_iv
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <unordered_map>

#ifdef __APPLE__
#include <sys/mman.h>
#endif

#include "ppc_config.h"
#include "ppc_context.h"

#define LOG(fmt, ...) printf("[GTA4] " fmt "\n", ##__VA_ARGS__)

// External function mappings from recompiled code
extern PPCFuncMapping PPCFuncMappings[];

// Global state
static uint8_t* g_memory_base = nullptr;
static size_t g_memory_size = 0x100000000ULL;  // 4GB
static std::unordered_map<uint32_t, PPCFunc*> g_functions;
static std::string g_game_path;

// Memory allocation
static uint8_t* AllocateMemory(size_t size) {
    void* mem = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) return nullptr;
    return static_cast<uint8_t*>(mem);
}

// Byte swap helpers
static inline uint32_t bswap32(uint32_t v) { return __builtin_bswap32(v); }
static inline void store32(uint8_t* base, uint32_t addr, uint32_t val) {
    *reinterpret_cast<uint32_t*>(base + addr) = bswap32(val);
}

// Register functions
static void RegisterFunctions() {
    size_t count = 0;
    const PPCFuncMapping* m = PPCFuncMappings;
    
    while (m->guest != 0) {
        if (m->host != nullptr) {
            g_functions[static_cast<uint32_t>(m->guest)] = m->host;
            count++;
        }
        m++;
    }
    LOG("Registered %zu recompiled functions", count);
}

// Lookup function
static PPCFunc* LookupFunction(uint32_t addr) {
    auto it = g_functions.find(addr);
    return (it != g_functions.end()) ? it->second : nullptr;
}

// Initialize lookup table for indirect calls
static void InitializeLookupTable() {
    uint8_t* table = g_memory_base + PPC_IMAGE_BASE + PPC_IMAGE_SIZE;
    size_t table_size = (PPC_CODE_SIZE / 4) * sizeof(void*) * 2;
    memset(table, 0, table_size);
    
    size_t populated = 0;
    for (const auto& [addr, func] : g_functions) {
        if (addr >= PPC_CODE_BASE && addr < PPC_CODE_BASE + PPC_CODE_SIZE) {
            uint32_t offset = addr - PPC_CODE_BASE;
            void** entry = reinterpret_cast<void**>(table + static_cast<uint64_t>(offset) * 2);
            *entry = reinterpret_cast<void*>(func);
            populated++;
        }
    }
    LOG("Lookup table: %zu entries", populated);
}

// Initialize PCR
static void InitializePCR(uint32_t pcr_addr, uint32_t stack_base, 
                          uint32_t stack_end, uint32_t thread_id, uint32_t entry) {
    store32(g_memory_base, pcr_addr + 0x00, pcr_addr);
    store32(g_memory_base, pcr_addr + 0x30, pcr_addr);
    store32(g_memory_base, pcr_addr + 0x70, stack_base);
    store32(g_memory_base, pcr_addr + 0x74, stack_end);
    store32(g_memory_base, pcr_addr + 0x100, pcr_addr + 0x200);
    store32(g_memory_base, pcr_addr + 0x10C, 0);
    
    uint32_t kthread = pcr_addr + 0x200;
    store32(g_memory_base, kthread + 0x00, 0x00060006);
    store32(g_memory_base, kthread + 0x5C, stack_base);
    store32(g_memory_base, kthread + 0x60, stack_end);
    store32(g_memory_base, kthread + 0x68, pcr_addr);
    store32(g_memory_base, kthread + 0x14C, thread_id);
    store32(g_memory_base, kthread + 0x150, entry);
}

// Getters for kernel_bridge
extern "C" {
    uint8_t* GetMemoryBase() { return g_memory_base; }
    const char* GetGamePath() { return g_game_path.c_str(); }
    PPCFunc* LookupFunctionByAddress(uint32_t addr) { return LookupFunction(addr); }
}

int main(int argc, char* argv[]) {
    LOG("==============================================");
    LOG("  GTA IV Static Recompilation");
    LOG("==============================================");
    
    // Default game path
    g_game_path = "/Users/Ozordi/Downloads/xenia/RAGE Games/extracted/gta_iv";
    if (argc > 1) {
        g_game_path = argv[1];
    }
    
    LOG("Game Path: %s", g_game_path.c_str());
    LOG("Image: 0x%08llX - 0x%08llX", 
        (unsigned long long)PPC_IMAGE_BASE, 
        (unsigned long long)(PPC_IMAGE_BASE + PPC_IMAGE_SIZE));
    LOG("Code:  0x%08llX - 0x%08llX", 
        (unsigned long long)PPC_CODE_BASE, 
        (unsigned long long)(PPC_CODE_BASE + PPC_CODE_SIZE));
    
    // Allocate 4GB memory
    g_memory_base = AllocateMemory(g_memory_size);
    if (!g_memory_base) {
        LOG("ERROR: Failed to allocate 4GB memory");
        return 1;
    }
    LOG("Memory base: %p", static_cast<void*>(g_memory_base));
    
    // Register recompiled functions
    RegisterFunctions();
    
    // Initialize lookup table
    InitializeLookupTable();
    
    // Setup thread context
    uint32_t stack_base = 0x70100000;
    uint32_t stack_end = stack_base - 0x100000;
    uint32_t pcr_addr = 0x7F000000;
    
    // Get entry point
    uint32_t entry = static_cast<uint32_t>(PPCFuncMappings[0].guest);
    if (entry == 0) entry = PPC_CODE_BASE;
    
    LOG("Entry point: 0x%08X", entry);
    
    // Initialize PCR
    InitializePCR(pcr_addr, stack_base, stack_end, 1, entry);
    
    // Create PPC context
    PPCContext ctx = {};
    ctx.r1.u32 = stack_base - 0x100;
    ctx.r13.u32 = pcr_addr;
    
    LOG("==============================================");
    LOG("  Starting GTA IV...");
    LOG("==============================================");
    fflush(stdout);
    
    // Execute entry point
    PPCFunc* func = LookupFunction(entry);
    if (func) {
        func(ctx, g_memory_base);
        LOG("Entry function returned (r3=0x%llX)", (unsigned long long)ctx.r3.u64);
    } else {
        LOG("ERROR: Entry function not found at 0x%08X", entry);
    }
    
    LOG("==============================================");
    LOG("  GTA IV Exited");
    LOG("==============================================");
    
    munmap(g_memory_base, g_memory_size);
    return 0;
}
