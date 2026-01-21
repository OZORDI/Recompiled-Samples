/**
 * GTA IV Static Recompilation - XeniaRT Launcher
 *
 * Uses XeniaRT's full Xenia kernel for Xbox 360 kernel support.
 * This initializes xe::Memory, xe::kernel::KernelState, and xe::vfs properly.
 */

#include <unistd.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <thread>

#include "ppc_config.h"
#include "ppc_context.h"

// XeniaRT - Full Xenia integration
#include "xenia/emulator.h"
#include "xenia/kernel/kernel_state.h"
#include "xenia/kernel/xthread.h"
#include "xenia/memory.h"
#include "xenia/xbox.h"
#include "xeniart/static_dispatch.h"
#include "xeniart/kernel_api.h"

#define LOG(fmt, ...) printf("[GTA4] " fmt "\n", ##__VA_ARGS__)

// Global emulator pointer for kernel calls
static xe::Emulator* g_emulator = nullptr;

// Global TLS data - provided by recomp project, loaded at startup
// These are extern'd in xthread.cc for generic TLS initialization
extern "C" const uint8_t* g_xeniart_tls_data = nullptr;
extern "C" size_t g_xeniart_tls_size = 0;
static std::vector<uint8_t> g_tls_data_storage;

// Data section storage - loaded from basefile
static std::vector<uint8_t> g_data_section_storage;

// Load data section from basefile into guest memory
// Regular Xenia loads the entire XEX (code+data) into memory
// Static recomp only compiles code - we need to load data section separately
static bool LoadDataSection(xe::Memory* memory, const char* basefile,
                           uint32_t base_addr, uint32_t data_start, uint32_t data_end) {
  FILE* f = fopen(basefile, "rb");
  if (!f) {
    LOG("Basefile not found: %s", basefile);
    return false;
  }
  
  // Calculate file offset and size
  uint32_t file_offset = data_start - base_addr;
  uint32_t data_size = data_end - data_start;
  
  // Read data section from basefile
  fseek(f, file_offset, SEEK_SET);
  g_data_section_storage.resize(data_size);
  size_t read = fread(g_data_section_storage.data(), 1, data_size, f);
  fclose(f);
  
  if (read != data_size) {
    LOG("Failed to read data section: expected %u bytes, got %zu", data_size, read);
    return false;
  }
  
  // Copy data section to guest memory
  uint8_t* guest_ptr = memory->TranslateVirtual(data_start);
  std::memcpy(guest_ptr, g_data_section_storage.data(), data_size);
  
  LOG("Loaded data section: 0x%08X - 0x%08X (%u bytes) from %s",
      data_start, data_end, data_size, basefile);
  return true;
}

// Load TLS data from recomp project file
// Returns true if TLS data was loaded successfully
static bool LoadTLSData(const char* filename) {
  FILE* f = fopen(filename, "rb");
  if (!f) {
    LOG("TLS data file not found: %s (this is expected if recompiler didn't extract it)", filename);
    return false;
  }
  
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);
  
  if (size == 0) {
    fclose(f);
    LOG("TLS data file is empty: %s", filename);
    return false;
  }
  
  g_tls_data_storage.resize(size);
  size_t read = fread(g_tls_data_storage.data(), 1, size, f);
  fclose(f);
  
  if (read != size) {
    LOG("Failed to read TLS data: expected %zu bytes, got %zu", size, read);
    g_tls_data_storage.clear();
    return false;
  }
  
  // Set global pointers for xthread.cc to use
  g_xeniart_tls_data = g_tls_data_storage.data();
  g_xeniart_tls_size = g_tls_data_storage.size();
  
  LOG("Loaded TLS data: %zu bytes from %s", size, filename);
  return true;
}

// Global init functions list - loaded from recomp project
static std::vector<uint32_t> g_init_functions;

// Load init functions from recomp project file
// These are called before entry point to initialize TLS and global state
static bool LoadInitFunctions(const char* filename) {
  FILE* f = fopen(filename, "r");
  if (!f) {
    LOG("Init functions file not found: %s", filename);
    return false;
  }
  
  char line[256];
  while (fgets(line, sizeof(line), f)) {
    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
    
    // Parse hex address
    uint32_t addr = static_cast<uint32_t>(strtoul(line, nullptr, 16));
    if (addr != 0) {
      g_init_functions.push_back(addr);
    }
  }
  fclose(f);
  
  LOG("Loaded %zu init functions from %s", g_init_functions.size(), filename);
  return !g_init_functions.empty();
}

// C-linkage function called by kernel_bridge_xeniart.cpp
// Takes register values as individual parameters and returns result via out pointers
extern "C" void xeniart_call_kernel_ex(const char* name,
                                       uint64_t r3, uint64_t r4, uint64_t r5,
                                       uint64_t r6, uint64_t r7, uint64_t r8,
                                       uint64_t r9, uint64_t r10,
                                       uint64_t* out_r3, uint64_t* out_r4) {
  // Get current thread's context from thread-local storage
  auto* thread = xe::kernel::XThread::GetCurrentThread();
  if (!thread) {
    printf("[XENIART] ERROR: No current thread for kernel call %s\n", name);
    *out_r3 = 0;
    *out_r4 = 0;
    return;
  }
  
  auto* ctx = thread->thread_state()->context();
  
  // Debug: Log before call
  static int call_count = 0;
  bool verbose = (call_count < 30);  // Only log first 30 calls
  if (verbose) {
    printf("[BRIDGE] #%d Calling %s\n", call_count, name);
    printf("[BRIDGE]   IN: r3=%016llx r4=%016llx r5=%016llx r6=%016llx\n",
           r3, r4, r5, r6);
    printf("[BRIDGE]   r1(stack)=%016llx r13(tls)=%016llx\n", 
           ctx->r[1], ctx->r[13]);
  }
  call_count++;
  
  // Copy input registers to Xenia context (r3-r10 are arguments)
  ctx->r[3] = r3;
  ctx->r[4] = r4;
  ctx->r[5] = r5;
  ctx->r[6] = r6;
  ctx->r[7] = r7;
  ctx->r[8] = r8;
  ctx->r[9] = r9;
  ctx->r[10] = r10;
  
  // Call Xenia's kernel export
  bool found = xeniart::CallKernelExport(name, ctx);
  if (!found) {
    static thread_local const char* last_warned = nullptr;
    if (last_warned != name) {
      printf("[XENIART] Kernel export not found: %s\n", name);
      last_warned = name;
    }
    ctx->r[3] = 0;
  }
  
  // Debug: Log after call and check memory writes
  if (verbose) {
    printf("[BRIDGE]   OUT: r3=%016llx r4=%016llx (found=%d)\n",
           ctx->r[3], ctx->r[4], found);
    
    // For ObReferenceObjectByHandle, check if r5 (output ptr) was written
    if (strcmp(name, "ObReferenceObjectByHandle") == 0 && r5 != 0) {
      auto* memory = thread->kernel_state()->memory();
      uint8_t* host_ptr = memory->TranslateVirtual(static_cast<uint32_t>(r5));
      if (host_ptr) {
        uint32_t written_val = *reinterpret_cast<uint32_t*>(host_ptr);
        printf("[BRIDGE]   Memory at r5(%08llx) = %08x (host=%p)\n",
               r5, written_val, host_ptr);
      } else {
        printf("[BRIDGE]   r5(%08llx) could not be translated!\n", r5);
      }
    }
  }
  
  // Copy output registers back (r3 is return value, r4 sometimes used)
  *out_r3 = ctx->r[3];
  *out_r4 = ctx->r[4];
}

// Thread-local PPCContext for recompiled code
// This bridges between Xenia's PPCContext layout and the local struct layout
static thread_local PPCContext g_local_ctx;

// Sync Xenia's context to local PPCContext before calling recompiled code
static void SyncFromXenia(xe::cpu::ppc::PPCContext* xenia_ctx) {
  g_local_ctx.r0.u64 = xenia_ctx->r[0];
  g_local_ctx.r1.u64 = xenia_ctx->r[1];
  g_local_ctx.r2.u64 = xenia_ctx->r[2];
  g_local_ctx.r3.u64 = xenia_ctx->r[3];
  g_local_ctx.r4.u64 = xenia_ctx->r[4];
  g_local_ctx.r5.u64 = xenia_ctx->r[5];
  g_local_ctx.r6.u64 = xenia_ctx->r[6];
  g_local_ctx.r7.u64 = xenia_ctx->r[7];
  g_local_ctx.r8.u64 = xenia_ctx->r[8];
  g_local_ctx.r9.u64 = xenia_ctx->r[9];
  g_local_ctx.r10.u64 = xenia_ctx->r[10];
  g_local_ctx.r11.u64 = xenia_ctx->r[11];
  g_local_ctx.r12.u64 = xenia_ctx->r[12];
  g_local_ctx.r13.u64 = xenia_ctx->r[13];
  g_local_ctx.r14.u64 = xenia_ctx->r[14];
  g_local_ctx.r15.u64 = xenia_ctx->r[15];
  g_local_ctx.r16.u64 = xenia_ctx->r[16];
  g_local_ctx.r17.u64 = xenia_ctx->r[17];
  g_local_ctx.r18.u64 = xenia_ctx->r[18];
  g_local_ctx.r19.u64 = xenia_ctx->r[19];
  g_local_ctx.r20.u64 = xenia_ctx->r[20];
  g_local_ctx.r21.u64 = xenia_ctx->r[21];
  g_local_ctx.r22.u64 = xenia_ctx->r[22];
  g_local_ctx.r23.u64 = xenia_ctx->r[23];
  g_local_ctx.r24.u64 = xenia_ctx->r[24];
  g_local_ctx.r25.u64 = xenia_ctx->r[25];
  g_local_ctx.r26.u64 = xenia_ctx->r[26];
  g_local_ctx.r27.u64 = xenia_ctx->r[27];
  g_local_ctx.r28.u64 = xenia_ctx->r[28];
  g_local_ctx.r29.u64 = xenia_ctx->r[29];
  g_local_ctx.r30.u64 = xenia_ctx->r[30];
  g_local_ctx.r31.u64 = xenia_ctx->r[31];
  g_local_ctx.lr = xenia_ctx->lr;
}

// Sync local PPCContext back to Xenia after recompiled code returns
static void SyncToXenia(xe::cpu::ppc::PPCContext* xenia_ctx) {
  xenia_ctx->r[0] = g_local_ctx.r0.u64;
  xenia_ctx->r[1] = g_local_ctx.r1.u64;
  xenia_ctx->r[2] = g_local_ctx.r2.u64;
  xenia_ctx->r[3] = g_local_ctx.r3.u64;
  xenia_ctx->r[4] = g_local_ctx.r4.u64;
  xenia_ctx->r[5] = g_local_ctx.r5.u64;
  xenia_ctx->r[6] = g_local_ctx.r6.u64;
  xenia_ctx->r[7] = g_local_ctx.r7.u64;
  xenia_ctx->r[8] = g_local_ctx.r8.u64;
  xenia_ctx->r[9] = g_local_ctx.r9.u64;
  xenia_ctx->r[10] = g_local_ctx.r10.u64;
  xenia_ctx->r[11] = g_local_ctx.r11.u64;
  xenia_ctx->r[12] = g_local_ctx.r12.u64;
  xenia_ctx->r[13] = g_local_ctx.r13.u64;
  xenia_ctx->r[14] = g_local_ctx.r14.u64;
  xenia_ctx->r[15] = g_local_ctx.r15.u64;
  xenia_ctx->r[16] = g_local_ctx.r16.u64;
  xenia_ctx->r[17] = g_local_ctx.r17.u64;
  xenia_ctx->r[18] = g_local_ctx.r18.u64;
  xenia_ctx->r[19] = g_local_ctx.r19.u64;
  xenia_ctx->r[20] = g_local_ctx.r20.u64;
  xenia_ctx->r[21] = g_local_ctx.r21.u64;
  xenia_ctx->r[22] = g_local_ctx.r22.u64;
  xenia_ctx->r[23] = g_local_ctx.r23.u64;
  xenia_ctx->r[24] = g_local_ctx.r24.u64;
  xenia_ctx->r[25] = g_local_ctx.r25.u64;
  xenia_ctx->r[26] = g_local_ctx.r26.u64;
  xenia_ctx->r[27] = g_local_ctx.r27.u64;
  xenia_ctx->r[28] = g_local_ctx.r28.u64;
  xenia_ctx->r[29] = g_local_ctx.r29.u64;
  xenia_ctx->r[30] = g_local_ctx.r30.u64;
  xenia_ctx->r[31] = g_local_ctx.r31.u64;
  xenia_ctx->lr = g_local_ctx.lr;
}

// C-linkage function to execute recompiled code with proper context bridging
extern "C" void xeniart_execute_recompiled(uint32_t guest_addr, 
                                           xe::cpu::ppc::PPCContext* xenia_ctx,
                                           uint8_t* memory_base) {
  // Find the function
  PPCFunc* func = nullptr;
  const PPCFuncMapping* m = PPCFuncMappings;
  while (m->guest != 0) {
    if (m->guest == guest_addr && m->host != nullptr) {
      func = m->host;
      break;
    }
    m++;
  }
  
  if (!func) {
    printf("[XENIART] ERROR: No function at 0x%08X\n", guest_addr);
    return;
  }
  
  printf("[EXEC] Found func at %08X, syncing context...\n", guest_addr);
  fflush(stdout);
  
  // Sync context from Xenia to local
  SyncFromXenia(xenia_ctx);
  
  printf("[EXEC] Context synced. Calling function...\n");
  fflush(stdout);
  
  // Debug: Log key registers at entry
  printf("[ENTRY] func=%08X r10=%016llx r13=%016llx r1=%016llx\n",
         guest_addr, g_local_ctx.r10.u64, g_local_ctx.r13.u64, g_local_ctx.r1.u64);
  fflush(stdout);
  
  // Debug: Dump memory at TLS address (0x1E000) to verify contents
  static bool dumped = false;
  if (!dumped) {
    uint32_t tls_addr = 0x1E000;
    uint8_t* tls_host = memory_base + tls_addr;
    printf("[DEBUG] Memory at TLS 0x%08X (host %p):\n", tls_addr, tls_host);
    printf("[DEBUG]   Raw bytes: %02X %02X %02X %02X %02X %02X %02X %02X\n",
           tls_host[0], tls_host[1], tls_host[2], tls_host[3],
           tls_host[4], tls_host[5], tls_host[6], tls_host[7]);
    printf("[DEBUG]   As BE32: %08X, As LE32: %08X\n",
           __builtin_bswap32(*(uint32_t*)tls_host), *(uint32_t*)tls_host);
    dumped = true;
  }
  
  // Call the recompiled function with local context
  printf("[EXEC] About to call func at %p with ctx=%p base=%p\n", 
         (void*)func, (void*)&g_local_ctx, (void*)memory_base);
  fflush(stdout);
  
  // Test: Call a simple lambda first to verify calling works
  auto test_func = [](PPCContext& ctx, uint8_t* base) {
    printf("[TEST] Lambda called! r1=%08X base=%p\n", ctx.r1.u32, base);
    fflush(stdout);
  };
  printf("[EXEC] Testing lambda call...\n");
  fflush(stdout);
  test_func(g_local_ctx, memory_base);
  printf("[EXEC] Lambda OK.\n");
  fflush(stdout);
  
  // Call the actual recompiled function
  func(g_local_ctx, memory_base);
  
  printf("[EXEC] Function returned!\n");
  fflush(stdout);
  
  // Sync context back to Xenia
  SyncToXenia(xenia_ctx);
}

// External function mappings from recompiled code
extern PPCFuncMapping PPCFuncMappings[];

// Register recompiled functions with static dispatch
static void RegisterRecompiledFunctions() {
  auto& dispatch = xeniart::StaticDispatch::GetInstance();
  size_t count = 0;
  const PPCFuncMapping* m = PPCFuncMappings;

  while (m->guest != 0) {
    if (m->host != nullptr) {
      dispatch.RegisterFunction(static_cast<uint32_t>(m->guest),
                                reinterpret_cast<xeniart::PPCFunc>(m->host));
      count++;
    }
    m++;
  }

  LOG("Registered %zu recompiled functions", count);
}

// Initialize lookup table for indirect calls
static void InitializeLookupTable(uint8_t* base) {
  uint8_t* table = base + PPC_IMAGE_BASE + PPC_IMAGE_SIZE;
  size_t table_size = (PPC_CODE_SIZE / 4) * sizeof(void*) * 2;
  memset(table, 0, table_size);

  size_t populated = 0;
  const PPCFuncMapping* m = PPCFuncMappings;

  while (m->guest != 0) {
    uint32_t addr = static_cast<uint32_t>(m->guest);
    if (m->host && addr >= PPC_CODE_BASE &&
        addr < PPC_CODE_BASE + PPC_CODE_SIZE) {
      uint32_t offset = addr - PPC_CODE_BASE;
      void** entry =
          reinterpret_cast<void**>(table + static_cast<uint64_t>(offset) * 2);
      *entry = reinterpret_cast<void*>(m->host);
      populated++;
    }
    m++;
  }

  LOG("Lookup table: %zu entries", populated);
}

// Initialize PCR (Processor Control Region)
static void InitializePCR(uint8_t* base, uint32_t pcr_addr, uint32_t stack_base,
                          uint32_t stack_end, uint32_t thread_id,
                          uint32_t entry_point) {
  auto store32 = [base](uint32_t addr, uint32_t val) {
    *reinterpret_cast<uint32_t*>(base + addr) = __builtin_bswap32(val);
  };

  store32(pcr_addr + 0x00, pcr_addr);
  store32(pcr_addr + 0x30, pcr_addr);
  store32(pcr_addr + 0x70, stack_base);
  store32(pcr_addr + 0x74, stack_end);
  store32(pcr_addr + 0x100, pcr_addr + 0x200);
  store32(pcr_addr + 0x10C, 0);

  uint32_t kthread = pcr_addr + 0x200;
  store32(kthread + 0x00, 0x00060006);
  store32(kthread + 0x5C, stack_base);
  store32(kthread + 0x60, stack_end);
  store32(kthread + 0x68, pcr_addr);
  store32(kthread + 0x14C, thread_id);
  store32(kthread + 0x150, entry_point);
}

int main(int argc, char* argv[]) {
  LOG("==============================================");
  LOG("  GTA IV Static Recompilation (XeniaRT)");
  LOG("  Using Xenia's Full Kernel Implementation");
  LOG("==============================================");

  // Parse game path
  std::filesystem::path game_path =
      "/Users/Ozordi/Downloads/xenia/RAGE Games/extracted/gta_iv";
  if (argc > 1) {
    game_path = argv[1];
  }

  LOG("Game Path: %s", game_path.string().c_str());
  LOG("Image: 0x%08X - 0x%08X", PPC_IMAGE_BASE,
      PPC_IMAGE_BASE + PPC_IMAGE_SIZE);
  LOG("Code:  0x%08X - 0x%08X", PPC_CODE_BASE, PPC_CODE_BASE + PPC_CODE_SIZE);

  // Create paths for Xenia emulator
  std::filesystem::path storage_root = game_path / "storage";
  std::filesystem::path cache_root = game_path / "cache";

  // Ensure directories exist
  std::filesystem::create_directories(storage_root);
  std::filesystem::create_directories(cache_root);

  // =========================================================================
  // Initialize Xenia Emulator (Memory, VFS, Kernel)
  // =========================================================================
  LOG("Initializing Xenia subsystems...");

  auto emulator =
      std::make_unique<xe::Emulator>(game_path,     // command_line
                                     storage_root,  // storage_root
                                     game_path,     // content_root (game files)
                                     cache_root     // cache_root
      );

  if (!emulator->Initialize()) {
    LOG("ERROR: Failed to initialize Xenia emulator");
    return 1;
  }

  // Get memory base from Xenia's memory subsystem
  xe::Memory* memory = emulator->memory();
  uint8_t* base = memory->virtual_membase();
  LOG("Memory base: %p (Xenia xe::Memory)", static_cast<void*>(base));

  // =========================================================================
  // Load ENTIRE image from basefile into guest memory
  // Regular Xenia loads entire XEX (code+data) into memory
  // Static recomp compiles code to native but embedded data (vtables, globals)
  // in code section still needs to be accessible
  // =========================================================================
  // GTA IV image: 0x82000000 - 0x83150000 (code + data)
  if (!LoadDataSection(memory, "gta4_base.exe", 
                       0x82000000,   // base address
                       0x82000000,   // image start (include code section for embedded data)
                       0x83150000)) { // image end
    LOG("WARNING: Image not loaded. Game will likely crash.");
  }

  // =========================================================================
  // Wire up StaticDispatch with Xenia's kernel
  // =========================================================================
  auto& dispatch = xeniart::StaticDispatch::GetInstance();
  dispatch.SetMemoryBase(base);
  dispatch.SetKernelState(emulator->kernel_state());
  LOG("KernelState: %p", static_cast<void*>(emulator->kernel_state()));

  // Register recompiled functions
  RegisterRecompiledFunctions();
  LOG("Static dispatch: %zu functions registered", dispatch.function_count());

  // Initialize lookup table
  InitializeLookupTable(base);

  // Get entry point - use XEX entry point (_xstart) which runs CRT initialization
  // This is critical: _xstart runs global constructors that populate BSS data
  // Using 0x82120000 skips initialization and causes crashes
  uint32_t entry = 0x829A0860;  // _xstart - the real XEX entry point
  LOG("Entry point: 0x%08X (_xstart - CRT startup)", entry);

  // =========================================================================
  // Load TLS data from recomp project (if available)
  // The runtime is generic - TLS data comes from the recomp project, not XEX
  // =========================================================================
  if (!LoadTLSData("gta4_tls.bin") && 
      !LoadTLSData("tls.bin") &&
      !LoadTLSData("tls_data.bin")) {
    LOG("WARNING: No TLS data file found. Game may crash if it uses TLS variables.");
  }

  // =========================================================================
  // Load init functions from recomp project (if available)
  // These initialize TLS and global state before entry point
  // =========================================================================
  if (!LoadInitFunctions("init_functions.txt") &&
      !LoadInitFunctions("gta4_init.txt")) {
    LOG("WARNING: No init functions file found. TLS may not be initialized.");
  }

  LOG("==============================================");
  LOG("  Starting GTA IV...");
  LOG("  VFS: game: -> %s", game_path.string().c_str());
  LOG("==============================================");
  fflush(stdout);

  // Setup crash handlers
  auto crash_handler = +[](int sig) {
    const char* sig_name = "UNKNOWN";
    switch (sig) {
      case SIGSEGV:
        sig_name = "SIGSEGV (Segmentation Fault)";
        break;
      case SIGILL:
        sig_name = "SIGILL (Illegal Instruction)";
        break;
      case SIGBUS:
        sig_name = "SIGBUS (Bus Error)";
        break;
      case SIGFPE:
        sig_name = "SIGFPE (Floating Point Exception)";
        break;
      case SIGABRT:
        sig_name = "SIGABRT (Abort)";
        break;
    }
    printf("\n[CRASH] Signal %d: %s at entry point!\n", sig, sig_name);
    printf("[CRASH] The entry function crashed immediately.\n");
    fflush(stdout);
    exit(1);
  };

  signal(SIGSEGV, crash_handler);
  signal(SIGILL, crash_handler);
  signal(SIGBUS, crash_handler);
  signal(SIGFPE, crash_handler);
  signal(SIGABRT, crash_handler);

  // Create main thread using Xenia's thread system (this allocates stack
  // properly!)
  auto main_thread = xe::kernel::object_ref<xe::kernel::XThread>(
      new xe::kernel::XThread(emulator->kernel_state(),
                              256 * 1024,  // 256KB stack
                              0,           // xapi_thread_startup
                              entry,       // start_address
                              0,           // start_context
                              0,           // creation_flags (not suspended)
                              true,        // guest_thread
                              true));      // main_thread

  // IMPORTANT: Set the static dispatcher so XThread::Execute calls our native
  // code
  main_thread->SetStaticDispatcher(&dispatch);

  // Create the thread (allocates stack, PCR, TLS)
  xe::X_STATUS result = main_thread->Create();
  if (XFAILED(result)) {
    LOG("ERROR: Failed to create main thread: %08X", result);
    return 1;
  }

  // Call init functions BEFORE entry point to populate TLS
  if (!g_init_functions.empty()) {
    LOG("Calling %zu init functions before entry point...", g_init_functions.size());
    
    // Get thread context and memory base for calling init functions
    auto* ppc_ctx = main_thread->thread_state()->context();
    uint8_t* mem_base = emulator->memory()->virtual_membase();
    
    for (uint32_t init_addr : g_init_functions) {
      LOG("  [INIT] Calling 0x%08X", init_addr);
      
      // Find the function
      PPCFunc* func = nullptr;
      const PPCFuncMapping* m = PPCFuncMappings;
      while (m->guest != 0) {
        if (m->guest == init_addr && m->host != nullptr) {
          func = m->host;
          break;
        }
        m++;
      }
      
      if (func) {
        // Sync context and call init function
        SyncFromXenia(ppc_ctx);
        func(g_local_ctx, mem_base);
        SyncToXenia(ppc_ctx);
        LOG("  [INIT] 0x%08X completed", init_addr);
      } else {
        LOG("  [INIT] WARNING: Function 0x%08X not found in mappings", init_addr);
      }
    }
    LOG("Init functions complete");
  }

  LOG("Main thread created successfully - game is now running");
  fflush(stdout);

  // Keep the process alive while the main thread runs
  // Check every 100ms if still running
  while (!main_thread->is_guest_thread() || main_thread->thread_state()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  LOG("==============================================");
  LOG("  GTA IV Exited");

  return 0;
}
