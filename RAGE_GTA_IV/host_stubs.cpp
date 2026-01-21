/**
 * Host Layer Stubs - GPU, Audio, Input
 *
 * These stubs allow GTA IV to reach its main loop without actual rendering.
 * Enhanced logging tracks which functions are called and how often.
 */

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ppc_config.h"
#include "ppc_context.h"

// ============================================================================
// Function Call Tracking
// ============================================================================

static std::mutex g_track_mutex;
static std::unordered_map<std::string, uint64_t> g_call_counts;
static std::unordered_set<std::string> g_first_calls;
static std::atomic<uint64_t> g_total_calls{0};
static auto g_start_time = std::chrono::steady_clock::now();

#define TRACK_CALL(name)                                                    \
  do {                                                                      \
    std::lock_guard<std::mutex> lock(g_track_mutex);                        \
    g_call_counts[#name]++;                                                 \
    g_total_calls++;                                                        \
    if (g_first_calls.insert(#name).second) {                               \
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>( \
                         std::chrono::steady_clock::now() - g_start_time)   \
                         .count();                                          \
      printf("[HOST +%lldms] First call: %s\n", (long long)elapsed, #name); \
      fflush(stdout);                                                       \
    }                                                                       \
  } while (0)

#define LOG_STUB(name, max_logs)                            \
  do {                                                      \
    static int count_##name = 0;                            \
    TRACK_CALL(name);                                       \
    if (++count_##name <= max_logs) {                       \
      printf("[HOST] %s (call %d)\n", #name, count_##name); \
      fflush(stdout);                                       \
    }                                                       \
  } while (0)

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
// GPU Stubs - Video Display
// ============================================================================

// Frame counter for main loop detection
static std::atomic<uint64_t> g_frame_count{0};
static std::atomic<uint64_t> g_swap_count{0};

PPC_FUNC(__imp__VdSwap) {
  LOG_STUB(VdSwap, 10);
  g_swap_count++;

  // Log frame rate every 60 frames
  static auto last_time = std::chrono::steady_clock::now();
  if (g_swap_count % 60 == 0) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time)
            .count();
    if (elapsed > 0) {
      double fps = 60000.0 / elapsed;
      printf("[GPU] Frame %llu, ~%.1f FPS (simulated)\n",
             (unsigned long long)g_swap_count.load(), fps);
      fflush(stdout);
    }
    last_time = now;
  }

  ctx.r3.u64 = 0;  // S_OK
}

PPC_FUNC(__imp__VdSetDisplayMode) {
  uint32_t mode = ctx.r3.u32;
  LOG_STUB(VdSetDisplayMode, 5);
  printf("[GPU] VdSetDisplayMode(mode=0x%X)\n", mode);
  fflush(stdout);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__VdSetDisplayModeOverride) {
  LOG_STUB(VdSetDisplayModeOverride, 3);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__VdGetCurrentDisplayInformation) {
  LOG_STUB(VdGetCurrentDisplayInformation, 3);
  uint32_t info_ptr = ctx.r3.u32;
  if (info_ptr && base) {
    // Fake 1280x720 display info
    store32(base, info_ptr + 0x00, 1280);  // Width
    store32(base, info_ptr + 0x04, 720);   // Height
    store32(base, info_ptr + 0x08, 60);    // Refresh
    store32(base, info_ptr + 0x0C, 0);     // Flags
  }
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__VdQueryVideoMode) {
  LOG_STUB(VdQueryVideoMode, 3);
  if (ctx.r3.u32 && base) {
    store32(base, ctx.r3.u32, 1280);     // Width
    store32(base, ctx.r3.u32 + 4, 720);  // Height
  }
}

PPC_FUNC(__imp__VdQueryVideoFlags) {
  LOG_STUB(VdQueryVideoFlags, 3);
  ctx.r3.u64 = 0x07;  // HDTV | Progressive | Widescreen
}

PPC_FUNC(__imp__VdInitializeEngines) {
  LOG_STUB(VdInitializeEngines, 1);
  printf("[GPU] VdInitializeEngines - GPU init starting\n");
  fflush(stdout);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__VdShutdownEngines) {
  LOG_STUB(VdShutdownEngines, 1);
  printf("[GPU] VdShutdownEngines - GPU shutdown\n");
  fflush(stdout);
}

PPC_FUNC(__imp__VdInitializeRingBuffer) {
  LOG_STUB(VdInitializeRingBuffer, 1);
  printf("[GPU] VdInitializeRingBuffer - Command buffer init\n");
  fflush(stdout);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__VdEnableRingBufferRPtrWriteBack) {
  LOG_STUB(VdEnableRingBufferRPtrWriteBack, 1);
}

PPC_FUNC(__imp__VdGetSystemCommandBuffer) {
  LOG_STUB(VdGetSystemCommandBuffer, 3);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__VdGetCurrentDisplayGamma) {
  LOG_STUB(VdGetCurrentDisplayGamma, 3);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__VdSetGraphicsInterruptCallback) {
  LOG_STUB(VdSetGraphicsInterruptCallback, 1);
  printf("[GPU] Graphics interrupt callback registered\n");
  fflush(stdout);
}

PPC_FUNC(__imp__VdCallGraphicsNotificationRoutines) {
  TRACK_CALL(VdCallGraphicsNotificationRoutines);
}

PPC_FUNC(__imp__VdIsHSIOTrainingSucceeded) {
  LOG_STUB(VdIsHSIOTrainingSucceeded, 1);
  ctx.r3.u64 = 1;  // Success
}

PPC_FUNC(__imp__VdPersistDisplay) { LOG_STUB(VdPersistDisplay, 3); }

PPC_FUNC(__imp__VdRetrainEDRAMWorker) { LOG_STUB(VdRetrainEDRAMWorker, 3); }

PPC_FUNC(__imp__VdRetrainEDRAM) { LOG_STUB(VdRetrainEDRAM, 3); }

PPC_FUNC(__imp__VdInitializeScalerCommandBuffer) {
  LOG_STUB(VdInitializeScalerCommandBuffer, 1);
}

PPC_FUNC(__imp__VdSetSystemCommandBufferGpuIdentifierAddress) {
  LOG_STUB(VdSetSystemCommandBufferGpuIdentifierAddress, 1);
}

PPC_FUNC(__imp__VdEnableDisableClockGating) {
  LOG_STUB(VdEnableDisableClockGating, 1);
}

// ============================================================================
// Audio Stubs
// ============================================================================

static std::atomic<uint64_t> g_audio_frames{0};

PPC_FUNC(__imp__XAudioSubmitRenderDriverFrame) {
  TRACK_CALL(XAudioSubmitRenderDriverFrame);
  g_audio_frames++;

  // Log periodically
  static int log_count = 0;
  if (log_count++ < 5 || g_audio_frames % 1000 == 0) {
    printf("[AUDIO] Frame %llu submitted\n",
           (unsigned long long)g_audio_frames.load());
    fflush(stdout);
  }

  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XAudioGetSpeakerConfig) {
  LOG_STUB(XAudioGetSpeakerConfig, 3);
  ctx.r3.u32 = 0;  // Stereo
}

PPC_FUNC(__imp__XAudioRegisterRenderDriverClient) {
  LOG_STUB(XAudioRegisterRenderDriverClient, 1);
  printf("[AUDIO] Render driver client registered\n");
  fflush(stdout);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XAudioUnregisterRenderDriverClient) {
  LOG_STUB(XAudioUnregisterRenderDriverClient, 1);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XAudioGetVoiceCategoryVolumeChangeMask) {
  TRACK_CALL(XAudioGetVoiceCategoryVolumeChangeMask);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XAudioGetVoiceCategoryVolume) {
  TRACK_CALL(XAudioGetVoiceCategoryVolume);
  ctx.r3.u64 = 0;
}

// XMA Audio Decoding - FIXED: Return valid non-zero handles
static std::atomic<uint32_t> g_next_xma_context{0x80010000};  // Start non-zero

PPC_FUNC(__imp__XMACreateContext) {
  LOG_STUB(XMACreateContext, 10);
  uint32_t out_ptr = ctx.r4.u32;  // Output handle pointer
  if (out_ptr && base) {
    uint32_t handle = g_next_xma_context.fetch_add(0x10);
    store32(base, out_ptr, handle);
    printf("[AUDIO] XMACreateContext -> handle 0x%08X\n", handle);
    fflush(stdout);
  }
  ctx.r3.u64 = 0;  // S_OK
}

PPC_FUNC(__imp__XMAInitializeContext) {
  LOG_STUB(XMAInitializeContext, 10);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMAReleaseContext) {
  LOG_STUB(XMAReleaseContext, 10);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMAEnableContext) {
  TRACK_CALL(XMAEnableContext);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMADisableContext) {
  TRACK_CALL(XMADisableContext);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMAGetOutputBufferWriteOffset) {
  TRACK_CALL(XMAGetOutputBufferWriteOffset);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMASetOutputBufferReadOffset) {
  TRACK_CALL(XMASetOutputBufferReadOffset);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMAGetOutputBufferReadOffset) {
  TRACK_CALL(XMAGetOutputBufferReadOffset);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMASetOutputBufferValid) {
  TRACK_CALL(XMASetOutputBufferValid);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMAIsOutputBufferValid) {
  TRACK_CALL(XMAIsOutputBufferValid);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMASetInputBuffer0Valid) {
  TRACK_CALL(XMASetInputBuffer0Valid);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMAIsInputBuffer0Valid) {
  TRACK_CALL(XMAIsInputBuffer0Valid);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMASetInputBuffer1Valid) {
  TRACK_CALL(XMASetInputBuffer1Valid);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMAIsInputBuffer1Valid) {
  TRACK_CALL(XMAIsInputBuffer1Valid);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMASetInputBuffer0) {
  TRACK_CALL(XMASetInputBuffer0);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMASetInputBuffer1) {
  TRACK_CALL(XMASetInputBuffer1);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMAGetPacketMetadata) {
  TRACK_CALL(XMAGetPacketMetadata);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XMABlockWhileInUse) {
  TRACK_CALL(XMABlockWhileInUse);
  ctx.r3.u64 = 0;
}

// ============================================================================
// Input Stubs
// ============================================================================

static std::atomic<uint64_t> g_input_polls{0};

PPC_FUNC(__imp__XamInputGetState) {
  TRACK_CALL(XamInputGetState);
  g_input_polls++;

  uint32_t user_index = ctx.r3.u32;
  uint32_t flags = ctx.r4.u32;
  uint32_t state_ptr = ctx.r5.u32;

  // Log periodically
  static int log_count = 0;
  if (log_count++ < 5 || g_input_polls % 1000 == 0) {
    printf("[INPUT] Poll %llu: user=%u, flags=0x%X\n",
           (unsigned long long)g_input_polls.load(), user_index, flags);
    fflush(stdout);
  }

  // Zero out state (no buttons pressed)
  if (state_ptr && base) {
    memset(base + state_ptr, 0, 16);
  }

  // Return ERROR_DEVICE_NOT_CONNECTED for non-primary controller
  ctx.r3.u64 = (user_index == 0) ? 0 : 0x048F;
}

PPC_FUNC(__imp__XamInputSetState) {
  TRACK_CALL(XamInputSetState);
  ctx.r3.u64 = 0;
}

PPC_FUNC(__imp__XamInputGetCapabilities) {
  uint32_t user_index = ctx.r3.u32;
  LOG_STUB(XamInputGetCapabilities, 5);
  // FIXED: Return success for controller 0
  ctx.r3.u64 = (user_index == 0) ? 0 : 0x048F;  // ERROR_DEVICE_NOT_CONNECTED
}

PPC_FUNC(__imp__XamInputGetKeystroke) {
  TRACK_CALL(XamInputGetKeystroke);
  ctx.r3.u64 = 0x048F;
}

PPC_FUNC(__imp__XamInputGetKeystrokeEx) {
  TRACK_CALL(XamInputGetKeystrokeEx);
  ctx.r3.u64 = 0x048F;
}

// ============================================================================
// Stats Reporting
// ============================================================================

extern "C" void PrintHostStats() {
  std::lock_guard<std::mutex> lock(g_track_mutex);

  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                     std::chrono::steady_clock::now() - g_start_time)
                     .count();

  printf("\n");
  printf("==============================================\n");
  printf("  Host Layer Statistics\n");
  printf("==============================================\n");
  printf("Runtime: %lld seconds\n", (long long)elapsed);
  printf("Total calls: %llu\n", (unsigned long long)g_total_calls.load());
  printf("VdSwap (frames): %llu\n", (unsigned long long)g_swap_count.load());
  printf("Audio frames: %llu\n", (unsigned long long)g_audio_frames.load());
  printf("Input polls: %llu\n", (unsigned long long)g_input_polls.load());
  printf("\nFunction call counts:\n");

  // Sort by count
  std::vector<std::pair<std::string, uint64_t>> sorted(g_call_counts.begin(),
                                                       g_call_counts.end());
  std::sort(sorted.begin(), sorted.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

  for (const auto& p : sorted) {
    printf("  %s: %llu\n", p.first.c_str(), (unsigned long long)p.second);
  }
  printf("==============================================\n\n");
  fflush(stdout);
}

// Auto-print stats on exit
static struct StatsReporter {
  ~StatsReporter() { PrintHostStats(); }
} g_stats_reporter;
