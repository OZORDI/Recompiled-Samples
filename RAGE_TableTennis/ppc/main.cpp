/**
 * Rockstar Table Tennis - Static Recompilation Runtime
 * Simplest RAGE engine title - good for testing
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

// Game-specific headers
#include "ppc_config.h"
#include "../ppc_context.h"

// Shared RAGE runtime
#include "../../rage_runtime/rage_runtime.h"

// Real kernel support
#ifdef USE_REAL_KERNEL
#include "kernel/kernel_state.h"
extern "C" void InitializeKernel(uint8_t* memory_base, xe::kernel::FunctionLookupFunc lookup);
extern "C" void ShutdownKernel();
#endif

// Function mappings from recompiler
extern PPCFuncMapping PPCFuncMappings[];

#ifdef USE_REAL_KERNEL
static PPCFunc* KernelFunctionLookup(uint32_t addr) {
    return RageRuntime::LookupFunction(addr);
}
#endif

int main(int argc, char* argv[]) {
    printf("=== Rockstar Table Tennis - Static Recompilation ===\n");
    printf("Image: 0x%08llX - 0x%08llX\n", 
           (unsigned long long)PPC_IMAGE_BASE, 
           (unsigned long long)(PPC_IMAGE_BASE + PPC_IMAGE_SIZE));
    printf("Code:  0x%08llX - 0x%08llX\n", 
           (unsigned long long)PPC_CODE_BASE, 
           (unsigned long long)(PPC_CODE_BASE + PPC_CODE_SIZE));
    
    // Configure runtime
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
    
    // Register functions
    RageRuntime::RegisterFunctions(PPCFuncMappings);
    RageRuntime::InitializeLookupTable(base);
    
#ifdef USE_REAL_KERNEL
    printf("Initializing real kernel...\n");
    InitializeKernel(base, KernelFunctionLookup);
#endif
    
    // Setup thread
    uint32_t stack_base = 0x70100000;
    uint32_t stack_end = stack_base - 0x100000;
    uint32_t pcr_addr = 0x7F000000;
    
    // Get entry point - use first function in mappings (actual XEX entry)
    uint32_t entry = static_cast<uint32_t>(PPCFuncMappings[0].guest);
    if (entry == 0) {
        entry = static_cast<uint32_t>(PPC_CODE_BASE);
    }
    
    RageRuntime::InitializePCR(base, pcr_addr, stack_base, stack_end, 1, entry);
    
    // Initialize context
    PPCContext ctx = {};
    ctx.r1.u32 = stack_base - 0x100;
    ctx.r13.u32 = pcr_addr;
    
    printf("Entry: 0x%08X\n", entry);
    printf("=== Starting ===\n");
    
    bool ok = RageRuntime::Execute(entry, ctx, base);
    
    printf("=== Done ===\n");
    
#ifdef USE_REAL_KERNEL
    ShutdownKernel();
#endif
    
    RageRuntime::Shutdown();
    
    return ok ? 0 : 1;
}
