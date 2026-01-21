/**
 * GTA IV Static Recompilation - Main Entry Point
 * Uses shared RAGE runtime for Xbox 360 kernel support
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ppc_config.h"
#include "ppc_context.h"
#include "../rage_runtime/rage_runtime.h"

// Real kernel support (when USE_REAL_KERNEL is defined)
#ifdef USE_REAL_KERNEL
#include "kernel/kernel_state.h"
extern "C" void InitializeKernel(uint8_t* memory_base, xe::kernel::FunctionLookupFunc lookup);
extern "C" void ShutdownKernel();
extern "C" void WaitForKernelThreads(uint32_t timeout_ms);
#endif

#define LOG(fmt, ...) do { printf("[GTA4] " fmt "\n", ##__VA_ARGS__); fflush(stdout); } while(0)

// External function mappings
extern PPCFuncMapping PPCFuncMappings[];

#ifdef USE_REAL_KERNEL
// Function lookup for real kernel threading
static PPCFunc* KernelFunctionLookup(uint32_t addr) {
    return RageRuntime::LookupFunction(addr);
}
#endif

int main(int argc, char* argv[]) {
    LOG("=== GTA IV Static Recompilation ===");
    LOG("Image: 0x%08llX - 0x%08llX", 
        (unsigned long long)PPC_IMAGE_BASE, 
        (unsigned long long)(PPC_IMAGE_BASE + PPC_IMAGE_SIZE));
    LOG("Code:  0x%08llX - 0x%08llX", 
        (unsigned long long)PPC_CODE_BASE, 
        (unsigned long long)(PPC_CODE_BASE + PPC_CODE_SIZE));
    
    // Configure shared runtime
    RageRuntime::Configure(
        static_cast<uint32_t>(PPC_IMAGE_BASE),
        static_cast<uint32_t>(PPC_IMAGE_SIZE),
        static_cast<uint32_t>(PPC_CODE_BASE),
        static_cast<uint32_t>(PPC_CODE_SIZE)
    );
    
    if (!RageRuntime::Initialize()) {
        return 1;
    }
    
    uint8_t* base = RageRuntime::GetMemoryBase();
    
    // Register functions and initialize lookup table
    RageRuntime::RegisterFunctions(PPCFuncMappings);
    RageRuntime::InitializeLookupTable(base);
    
#ifdef USE_REAL_KERNEL
    // Initialize real kernel for threading support
    LOG("Initializing real kernel...");
    InitializeKernel(base, KernelFunctionLookup);
#endif
    
    // Setup thread context
    uint32_t stack_base = 0x70100000;
    uint32_t stack_end = stack_base - 0x100000;
    uint32_t pcr_addr = 0x7F000000;
    
    // Use first function as entry point
    uint32_t entry = static_cast<uint32_t>(PPCFuncMappings[0].guest);
    if (entry == 0) entry = static_cast<uint32_t>(PPC_CODE_BASE);
    
    RageRuntime::InitializePCR(base, pcr_addr, stack_base, stack_end, 1, entry);
    
    // Initialize context
    PPCContext ctx = {};
    ctx.r1.u32 = stack_base - 0x100;
    ctx.r13.u32 = pcr_addr;
    
    LOG("Entry: 0x%08X", entry);
    LOG("=== Starting ===");
    fflush(stdout);
    
    LOG("Calling entry function...");
    fflush(stdout);
    
    bool ok = RageRuntime::Execute(entry, ctx, base);
    
    fflush(stdout);
    LOG("=== Entry returned (ok=%d, r3=0x%llX) ===", ok, (unsigned long long)ctx.r3.u64);
    fflush(stdout);
    
#ifdef USE_REAL_KERNEL
    // Wait for worker threads (with 10 second timeout)
    WaitForKernelThreads(10000);
    
    ShutdownKernel();
#endif
    
    RageRuntime::Shutdown();
    
    return ok ? 0 : 1;
}
