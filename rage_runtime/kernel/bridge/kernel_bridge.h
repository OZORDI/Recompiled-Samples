/**
 * Kernel Bridge - Connects static recompilation to Xenia's actual kernel
 * 
 * This provides the glue between PPC function calls and Xenia's real
 * kernel implementations from src/xenia/kernel/xboxkrnl/
 */

#ifndef KERNEL_BRIDGE_H_
#define KERNEL_BRIDGE_H_

#include <cstdint>

// Forward declare PPCContext from game's ppc_context.h
struct PPCContext;

namespace xe {
namespace kernel {
namespace bridge {

// Initialize the kernel bridge with memory base
void Initialize(uint8_t* memory_base, uint32_t memory_size);
void Shutdown();

// Get memory base for kernel operations
uint8_t* memory_base();

// Dispatch a kernel call by ordinal
// Returns true if handled, false if unknown ordinal
bool DispatchKernelCall(uint32_t ordinal, PPCContext& ctx, uint8_t* base);

// Register a function lookup callback for thread execution
using FunctionLookupFunc = void* (*)(uint32_t guest_addr);
void SetFunctionLookup(FunctionLookupFunc lookup);

}  // namespace bridge
}  // namespace kernel
}  // namespace xe

#endif  // KERNEL_BRIDGE_H_
