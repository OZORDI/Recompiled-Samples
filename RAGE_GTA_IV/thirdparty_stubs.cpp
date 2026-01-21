/**
 * Stub implementations for XeniaRT excluded subsystems
 *
 * These stub out the RegisterExports functions for UI/Audio/Input modules
 * that were excluded from the XeniaRT build.
 */

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

// Forward declarations
namespace xe {
namespace cpu {
class ExportResolver;
}
namespace kernel {
class KernelState;
}

namespace threading {
void set_name(std::string_view) {}
}  // namespace threading
}  // namespace xe

// Stub register functions for excluded XAM modules
namespace xe {
namespace kernel {
namespace xam {
void RegisterUIExports(xe::cpu::ExportResolver*, xe::kernel::KernelState*) {}
void RegisterNUIExports(xe::cpu::ExportResolver*, xe::kernel::KernelState*) {}
void RegisterInputExports(xe::cpu::ExportResolver*, xe::kernel::KernelState*) {}
}  // namespace xam
namespace xboxkrnl {
void RegisterAudioExports(xe::cpu::ExportResolver*, xe::kernel::KernelState*) {}
void RegisterAudioXmaExports(xe::cpu::ExportResolver*,
                             xe::kernel::KernelState*) {}
}  // namespace xboxkrnl
}  // namespace kernel
}  // namespace xe

// ImGui stub types and functions
struct ImVec2 {
  float x, y;
  ImVec2() : x(0), y(0) {}
  ImVec2(float _x, float _y) : x(_x), y(_y) {}
};

struct ImGuiIO {};
struct ImGuiInputTextCallbackData {};

namespace ImGui {
bool Begin(const char*, bool*, int) { return true; }
void End() {}
bool BeginPopupModal(const char*, bool*, int) { return false; }
void EndPopup() {}
void OpenPopup(const char*, int) {}
void CloseCurrentPopup() {}
void Text(const char*, ...) {}
void TextWrapped(const char*, ...) {}
bool Button(const char*, const ImVec2&) { return false; }
void SameLine(float, float) {}
void Spacing() {}
bool InputText(const char*, char*, size_t, int,
               int (*)(ImGuiInputTextCallbackData*), void*) {
  return false;
}
void SetNextWindowSize(const ImVec2&, int) {}
void SetNextWindowPos(const ImVec2&, int, const ImVec2&) {}
void SetKeyboardFocusHere(int) {}
void PushStyleVar(int, float) {}
void PushStyleVar(int, const ImVec2&) {}
void PopStyleVar(int) {}
}  // namespace ImGui

// AES-128 function stubs
extern "C" {
void aes_key_schedule_128(const uint8_t*, uint8_t*) {}
void aes_encrypt_128(const uint8_t*, uint8_t*, const uint8_t*) {}
void aes_decrypt_128(const uint8_t*, uint8_t*, const uint8_t*) {}
}
