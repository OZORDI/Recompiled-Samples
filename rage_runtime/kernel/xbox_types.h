/**
 * Xbox 360 Type Definitions for Static Recompilation Runtime
 * Extracted from Xenia - adapted for standalone use
 */

#ifndef RAGE_RUNTIME_XBOX_TYPES_H_
#define RAGE_RUNTIME_XBOX_TYPES_H_

#include <cstdint>
#include <type_traits>

namespace xe {

// Big-endian wrapper for Xbox 360 compatibility
template <typename T>
struct be {
    T value;
    
    be() = default;
    be(T v) : value(swap(v)) {}
    
    operator T() const { return swap(value); }
    be& operator=(T v) { value = swap(v); return *this; }
    
    static T swap(T v) {
        if constexpr (sizeof(T) == 1) return v;
        else if constexpr (sizeof(T) == 2) return __builtin_bswap16(v);
        else if constexpr (sizeof(T) == 4) return __builtin_bswap32(v);
        else if constexpr (sizeof(T) == 8) return __builtin_bswap64(v);
        else return v;
    }
};

template <>
struct be<float> {
    uint32_t value;
    be() = default;
    be(float v) { uint32_t tmp; memcpy(&tmp, &v, 4); value = __builtin_bswap32(tmp); }
    operator float() const { uint32_t tmp = __builtin_bswap32(value); float r; memcpy(&r, &tmp, 4); return r; }
};

// Load/store helpers
template <typename T>
inline T load_and_swap(const void* ptr) {
    T value = *reinterpret_cast<const T*>(ptr);
    if constexpr (sizeof(T) == 1) return value;
    else if constexpr (sizeof(T) == 2) return __builtin_bswap16(value);
    else if constexpr (sizeof(T) == 4) return __builtin_bswap32(value);
    else if constexpr (sizeof(T) == 8) return __builtin_bswap64(value);
    return value;
}

template <typename T>
inline void store_and_swap(void* ptr, T value) {
    if constexpr (sizeof(T) == 1) *reinterpret_cast<T*>(ptr) = value;
    else if constexpr (sizeof(T) == 2) *reinterpret_cast<T*>(ptr) = __builtin_bswap16(value);
    else if constexpr (sizeof(T) == 4) *reinterpret_cast<T*>(ptr) = __builtin_bswap32(value);
    else if constexpr (sizeof(T) == 8) *reinterpret_cast<T*>(ptr) = __builtin_bswap64(value);
}

// fourcc helper
constexpr uint32_t make_fourcc(char a, char b, char c, char d) {
    return (uint32_t(a) << 24) | (uint32_t(b) << 16) | (uint32_t(c) << 8) | uint32_t(d);
}
using fourcc_t = uint32_t;

// Handle types
typedef uint32_t X_HANDLE;
#define X_INVALID_HANDLE_VALUE ((X_HANDLE)-1)

// NT_STATUS codes
typedef uint32_t X_STATUS;
#define XSUCCEEDED(s) ((s & 0xC0000000) == 0)
#define XFAILED(s)    (!XSUCCEEDED(s))

#define X_STATUS_SUCCESS                    ((X_STATUS)0x00000000L)
#define X_STATUS_ABANDONED_WAIT_0           ((X_STATUS)0x00000080L)
#define X_STATUS_USER_APC                   ((X_STATUS)0x000000C0L)
#define X_STATUS_ALERTED                    ((X_STATUS)0x00000101L)
#define X_STATUS_TIMEOUT                    ((X_STATUS)0x00000102L)
#define X_STATUS_PENDING                    ((X_STATUS)0x00000103L)
#define X_STATUS_OBJECT_NAME_EXISTS         ((X_STATUS)0x40000000L)
#define X_STATUS_UNSUCCESSFUL               ((X_STATUS)0xC0000001L)
#define X_STATUS_NOT_IMPLEMENTED            ((X_STATUS)0xC0000002L)
#define X_STATUS_INVALID_INFO_CLASS         ((X_STATUS)0xC0000003L)
#define X_STATUS_INFO_LENGTH_MISMATCH       ((X_STATUS)0xC0000004L)
#define X_STATUS_ACCESS_VIOLATION           ((X_STATUS)0xC0000005L)
#define X_STATUS_INVALID_HANDLE             ((X_STATUS)0xC0000008L)
#define X_STATUS_INVALID_PARAMETER          ((X_STATUS)0xC000000DL)
#define X_STATUS_NO_SUCH_FILE               ((X_STATUS)0xC000000FL)
#define X_STATUS_END_OF_FILE                ((X_STATUS)0xC0000011L)
#define X_STATUS_NO_MEMORY                  ((X_STATUS)0xC0000017L)
#define X_STATUS_ALREADY_COMMITTED          ((X_STATUS)0xC0000021L)
#define X_STATUS_ACCESS_DENIED              ((X_STATUS)0xC0000022L)
#define X_STATUS_BUFFER_TOO_SMALL           ((X_STATUS)0xC0000023L)
#define X_STATUS_OBJECT_TYPE_MISMATCH       ((X_STATUS)0xC0000024L)
#define X_STATUS_OBJECT_NAME_INVALID        ((X_STATUS)0xC0000033L)
#define X_STATUS_OBJECT_NAME_NOT_FOUND      ((X_STATUS)0xC0000034L)
#define X_STATUS_OBJECT_NAME_COLLISION      ((X_STATUS)0xC0000035L)
#define X_STATUS_MUTANT_NOT_OWNED           ((X_STATUS)0xC0000046L)

// Win32 error codes
typedef uint32_t X_RESULT;
#define X_ERROR_SUCCESS         ((X_RESULT)0x00000000L)
#define X_ERROR_FILE_NOT_FOUND  ((X_RESULT)0x00000002L)
#define X_ERROR_ACCESS_DENIED   ((X_RESULT)0x00000005L)
#define X_ERROR_INVALID_HANDLE  ((X_RESULT)0x00000006L)
#define X_ERROR_NO_MORE_FILES   ((X_RESULT)0x00000012L)
#define X_ERROR_IO_PENDING      ((X_RESULT)0x000003E5L)
#define X_ERROR_NOT_FOUND       ((X_RESULT)0x00000490L)

// Memory flags
enum X_MEM : uint32_t {
    X_MEM_COMMIT      = 0x00001000,
    X_MEM_RESERVE     = 0x00002000,
    X_MEM_DECOMMIT    = 0x00004000,
    X_MEM_RELEASE     = 0x00008000,
    X_MEM_FREE        = 0x00010000,
    X_MEM_RESET       = 0x00080000,
    X_MEM_TOP_DOWN    = 0x00100000,
    X_MEM_LARGE_PAGES = 0x20000000,
};

// Page protection
enum X_PAGE : uint32_t {
    X_PAGE_NOACCESS          = 0x00000001,
    X_PAGE_READONLY          = 0x00000002,
    X_PAGE_READWRITE         = 0x00000004,
    X_PAGE_EXECUTE           = 0x00000010,
    X_PAGE_EXECUTE_READ      = 0x00000020,
    X_PAGE_EXECUTE_READWRITE = 0x00000040,
    X_PAGE_GUARD             = 0x00000100,
    X_PAGE_NOCACHE           = 0x00000200,
    X_PAGE_WRITECOMBINE      = 0x00000400,
};

// Thread creation flags
#define X_CREATE_SUSPENDED 0x00000001

// Dispatcher header (for sync objects)
struct X_DISPATCH_HEADER {
    uint8_t type;
    uint8_t abandoned;
    uint8_t size;
    uint8_t inserted;
    xe::be<uint32_t> signal_state;
    xe::be<uint32_t> wait_list_flink;
    xe::be<uint32_t> wait_list_blink;
};
static_assert(sizeof(X_DISPATCH_HEADER) == 0x10, "X_DISPATCH_HEADER size");

// Object attributes
struct X_OBJECT_ATTRIBUTES {
    xe::be<uint32_t> root_directory;
    xe::be<uint32_t> name_ptr;
    xe::be<uint32_t> attributes;
};

// ANSI string
struct X_ANSI_STRING {
    xe::be<uint16_t> length;
    xe::be<uint16_t> maximum_length;
    xe::be<uint32_t> pointer;
};
static_assert(sizeof(X_ANSI_STRING) == 8, "X_ANSI_STRING size");

// IO status block
struct X_IO_STATUS_BLOCK {
    union {
        xe::be<X_STATUS> status;
        xe::be<uint32_t> pointer;
    };
    xe::be<uint32_t> information;
};

// List entry
struct X_LIST_ENTRY {
    xe::be<uint32_t> flink_ptr;
    xe::be<uint32_t> blink_ptr;
};
static_assert(sizeof(X_LIST_ENTRY) == 8, "X_LIST_ENTRY size");

}  // namespace xe

#endif  // RAGE_RUNTIME_XBOX_TYPES_H_
