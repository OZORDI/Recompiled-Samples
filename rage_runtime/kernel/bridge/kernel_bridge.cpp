/**
 * Kernel Bridge - Connects static recompilation to Xenia's actual kernel
 * 
 * This file creates wrapper functions that call Xenia's real _entry implementations
 * from src/xenia/kernel/xboxkrnl/
 */

#include "kernel_bridge.h"

// Include Xenia's actual kernel implementations
// These are the real implementations from src/xenia/kernel/
#include "xenia/kernel/kernel_state.h"
#include "xenia/kernel/xthread.h"
#include "xenia/kernel/xevent.h"
#include "xenia/kernel/xmutant.h"
#include "xenia/kernel/xsemaphore.h"
#include "xenia/kernel/xtimer.h"
#include "xenia/kernel/xfile.h"
#include "xenia/kernel/util/shim_utils.h"
#include "xenia/xbox.h"

// Include the actual xboxkrnl implementations
#include "xenia/kernel/xboxkrnl/xboxkrnl_threading.h"
#include "xenia/kernel/xboxkrnl/xboxkrnl_private.h"

#include <unordered_map>

namespace xe {
namespace kernel {
namespace bridge {

static uint8_t* g_memory_base = nullptr;
static uint32_t g_memory_size = 0;
static KernelState* g_kernel_state = nullptr;
static FunctionLookupFunc g_function_lookup = nullptr;

void Initialize(uint8_t* memory_base, uint32_t memory_size) {
    g_memory_base = memory_base;
    g_memory_size = memory_size;
    
    // Create Xenia's actual KernelState
    // This is the real kernel state from src/xenia/kernel/kernel_state.cc
    g_kernel_state = new KernelState();
}

void Shutdown() {
    delete g_kernel_state;
    g_kernel_state = nullptr;
    g_memory_base = nullptr;
}

uint8_t* memory_base() {
    return g_memory_base;
}

void SetFunctionLookup(FunctionLookupFunc lookup) {
    g_function_lookup = lookup;
}

KernelState* kernel_state() {
    return g_kernel_state;
}

// Helper to translate guest address to host pointer
template<typename T>
T* TranslateVirtual(uint32_t guest_addr) {
    if (!guest_addr || !g_memory_base) return nullptr;
    return reinterpret_cast<T*>(g_memory_base + guest_addr);
}

// Kernel call dispatch - routes ordinals to Xenia's actual implementations
bool DispatchKernelCall(uint32_t ordinal, PPCContext& ctx, uint8_t* base) {
    // This would dispatch to Xenia's actual _entry functions based on ordinal
    // For now, return false to indicate unhandled
    return false;
}

}  // namespace bridge
}  // namespace kernel
}  // namespace xe
