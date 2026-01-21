/**
 * PPCContext - Forward declarations for kernel use
 * The game's ppc_context.h provides the actual definition.
 * This header just ensures the kernel can compile standalone.
 */

#ifndef RAGE_RUNTIME_KERNEL_PPC_CONTEXT_H_
#define RAGE_RUNTIME_KERNEL_PPC_CONTEXT_H_

#include <cstdint>

// If the game's ppc_context.h was already included, use its definitions
#ifdef PPC_CONTEXT_DEFINED

// Game already defined PPCContext, nothing to do

#else

// Minimal forward declaration for kernel compilation
// The actual definition comes from the game's ppc_context.h
struct PPCContext;

#endif  // PPC_CONTEXT_DEFINED

#endif  // RAGE_RUNTIME_KERNEL_PPC_CONTEXT_H_
