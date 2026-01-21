/**
 * RAGE Engine Static Recompilation - Main Entry Point Template
 * 
 * Copy this file to your game directory and customize as needed.
 * Works with: Table Tennis, GTA IV, Red Dead Redemption, Max Payne 3, etc.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

// Game-specific headers (must be included in this order)
#include "ppc_config.h"
#include "ppc_context.h"

// Shared runtime
#include "../rage_runtime/rage_runtime.h"

// External function mappings from recompiler output
extern PPCFuncMapping PPCFuncMappings[];

int main(int argc, char* argv[]) {
    printf("=== RAGE Engine Static Recompilation Runtime ===\n");
    printf("Image: 0x%08llX - 0x%08llX\n", 
           (unsigned long long)PPC_IMAGE_BASE, 
           (unsigned long long)(PPC_IMAGE_BASE + PPC_IMAGE_SIZE));
    printf("Code:  0x%08llX - 0x%08llX\n", 
           (unsigned long long)PPC_CODE_BASE, 
           (unsigned long long)(PPC_CODE_BASE + PPC_CODE_SIZE));
    
    // Configure runtime with game-specific values
    RageRuntime::Configure(
        static_cast<uint32_t>(PPC_IMAGE_BASE),
        static_cast<uint32_t>(PPC_IMAGE_SIZE),
        static_cast<uint32_t>(PPC_CODE_BASE),
        static_cast<uint32_t>(PPC_CODE_SIZE)
    );
    
    // Initialize memory
    if (!RageRuntime::Initialize()) {
        return 1;
    }
    
    uint8_t* base = RageRuntime::GetMemoryBase();
    
    // Register all recompiled functions
    RageRuntime::RegisterFunctions(PPCFuncMappings);
    
    // Initialize lookup table for indirect calls
    RageRuntime::InitializeLookupTable(base);
    
    // Setup thread context
    uint32_t stack_size = 0x100000;  // 1MB stack
    uint32_t stack_base = 0x70100000;
    uint32_t stack_end = stack_base - stack_size;
    uint32_t pcr_addr = 0x7F000000;
    uint32_t thread_id = 1;
    uint32_t entry_point = static_cast<uint32_t>(PPC_CODE_BASE);
    
    RageRuntime::InitializePCR(base, pcr_addr, stack_base, stack_end, 
                                thread_id, entry_point);
    
    // Initialize PPCContext
    PPCContext ctx = {};
    ctx.r1.u32 = stack_base - 0x100;  // Stack pointer
    ctx.r13.u32 = pcr_addr;            // TLS base
    
    printf("Entry: 0x%08X\n", entry_point);
    printf("=== Starting Execution ===\n");
    
    // Execute entry point
    bool success = RageRuntime::Execute(entry_point, ctx, base);
    
    printf("=== Execution Complete ===\n");
    RageRuntime::Shutdown();
    
    return success ? 0 : 1;
}
