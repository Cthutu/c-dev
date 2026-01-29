//------------------------------------------------------------------------------
// Core module
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------
//> def: _POSIX_C_SOURCE=200809L
//> def: _GNU_SOURCE

#pragma once

//------------------------------------------------------------------------------
// INDEX
//
// [Config]     Configuration macros and settings
// [Macros]     Basic macros
// [Types]      Basic types and definitions
// [Memory]     Memory management and debugging
// [Array]      Dynamic arrays
// [Arena]      Memory-mapped arenas
// [Mutex]      Simple mutex locking
// [Output]     Basic output functions
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------[Config]

#define YES (1)
#define NO (0)

//
// Compiler detection
//

#define COMPILER_GCC NO
#define COMPILER_CLANG NO
#define COMPILER_MSVC NO

#if defined(__clang__)
#    undef COMPILER_CLANG
#    define COMPILER_CLANG YES
#elif defined(__GNUC__)
#    undef COMPILER_GCC
#    define COMPILER_GCC YES
#elif defined(_MSC_VER)
#    undef COMPILER_MSVC
#    define COMPILER_MSVC YES
#else
#    error "Unsupported compiler. Please use GCC, Clang, or MSVC."
#endif

//
// OS detection
//

#define OS_WINDOWS NO
#define OS_LINUX NO
#define OS_MACOS NO
#define OS_BSD NO
#define OS_POSIX NO

#if defined(_WIN32) || defined(_WIN64)
#    undef OS_WINDOWS
#    define OS_WINDOWS YES
#elif defined(__linux__)
#    undef OS_LINUX
#    define OS_LINUX YES
#elif defined(__APPLE__) || defined(__MACH__)
#    undef OS_MACOS
#    define OS_MACOS YES
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#    undef OS_BSD
#    define OS_BSD YES
#else
#    error "Unsupported OS. Please use Windows, Linux, macOS, or BSD."
#endif

#if OS_LINUX || OS_MACOS || OS_BSD
#    undef OS_POSIX
#    define OS_POSIX YES
#endif

//
// Architecture detection
//

#define ARCH_X86 NO
#define ARCH_X86_64 NO
#define ARCH_ARM NO
#define ARCH_ARM64 NO

#if defined(__i386__) || defined(_M_IX86)
#    undef ARCH_X86
#    define ARCH_X86 YES
#elif defined(__x86_64__) || defined(_M_X64)
#    undef ARCH_X86_64
#    define ARCH_X86_64 YES
#elif defined(__arm__) || defined(_M_ARM)
#    undef ARCH_ARM
#    define ARCH_ARM YES
#elif defined(__aarch64__) || defined(_M_ARM64)
#    undef ARCH_ARM64
#    define ARCH_ARM64 YES
#else
#    error "Unsupported architecture. Please use x86, x86_64, ARM, or ARM64."
#endif

//
// Build configuration detection
//

#if defined(NDEBUG)
#    define BUILD_RELEASE YES
#    define BUILD_DEBUG NO
#else
#    define BUILD_RELEASE NO
#    define BUILD_DEBUG YES
#endif

//
// Debugger support
//

#if COMPILER_MSVC
#    define DEBUG_BREAK() __debugbreak()
#elif COMPILER_GCC || COMPILER_CLANG
#    define DEBUG_BREAK() __builtin_trap()
#else
#    error                                                                     \
        "Unsupported compiler for debug break. Please use GCC, Clang, or MSVC."
#endif

//
// Standard includes
//

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#if OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <windows.h>
#elif OS_POSIX
#    include <unistd.h>
#endif

void dump_config(void);

//------------------------------------------------------------------------------[Macros]

#define internal static
#define local_persist static
#define global_variable static

#define UNUSED(x) (void)(x)

#define KB(x) ((x) * 1024ull)
#define MB(x) (KB(x) * 1024ull)
#define GB(x) (MB(x) * 1024ull)

#define ALIGN_UP(value, alignment)                                             \
    (((value) + ((alignment) - 1)) & ~((alignment) - 1))
#define ALIGN_PTR_UP(type, ptr, alignment)                                     \
    (type*)ALIGN_UP((uintptr_t)(ptr), (alignment))

#define PI 3.14159265358979323846
#define TAU (PI * 2.0)

#define ASSERT(cond, ...)                                                      \
    do {                                                                       \
        if (!(cond)) {                                                         \
            eprn("ASSERTION FAILED: " #cond);                                  \
            eprn(__VA_ARGS__);                                                 \
            DEBUG_BREAK();                                                     \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while (0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, min, max) (MIN(MAX((x), (min)), (max)))

#define DEF_SLICE(type)                                                        \
    typedef struct {                                                           \
        type* data;                                                            \
        usize count;                                                           \
    }

//------------------------------------------------------------------------------[Types]

typedef uint8_t  u8;  // Unsigned 8-bit integer
typedef uint16_t u16; // Unsigned 16-bit integer
typedef uint32_t u32; // Unsigned 32-bit integer
typedef uint64_t u64; // Unsigned 64-bit integer

typedef int8_t  i8;  // Signed 8-bit integer
typedef int16_t i16; // Signed 16-bit integer
typedef int32_t i32; // Signed 32-bit integer
typedef int64_t i64; // Signed 64-bit integer

typedef size_t    usize; // Unsigned size type (platform-dependent)
typedef ptrdiff_t isize; // Signed size type (platform-dependent)

typedef float  f32; // 32-bit floating point
typedef double f64; // 64-bit floating point

typedef const char* cstr;

//------------------------------------------------------------------------------[Memory]

typedef struct MemoryHeader_t {
    usize size; // Size of the allocated block

#if DEBUG
    cstr file;  // File where the allocation was made
    int  line;  // Line number of the allocation
    u64  index; // Allocation index for debugging purposes

    struct MemoryHeader_t* next; // Pointer to the next allocation in the list
    bool leaked; // Flag to indicate if this block was leaked and therefore
                 // should not be in the linked list.  This is used to mark
                 // allocations with application lifetimes.
#endif           // DEBUG
} MemoryHeader;

void* mem_alloc(usize size, cstr file, int line);
void* mem_realloc(void* ptr, usize size, cstr file, int line);
void* mem_free(void* ptr, cstr file, int line);

usize mem_size(const void* ptr);
void  mem_leak(void* ptr);

void mem_check(void* ptr, cstr file, int line);

//
// Memory debugging utilities
//

#if DEBUG

void  mem_dump_leaks(void);
usize mem_get_allocation_count(void);
usize mem_get_total_allocated(void);
void  mem_break_on_alloc(u64 index);

#endif // DEBUG

#define ALLOC(size) mem_alloc(size, __FILE__, __LINE__)
#define REALLOC(ptr, size) mem_realloc(ptr, size, __FILE__, __LINE__)
#define FREE(ptr) (ptr) = mem_free(ptr, __FILE__, __LINE__)

#define ARRAY_ALLOC(type, count)                                               \
    (type*)mem_alloc(sizeof(type) * (count), __FILE__, __LINE__)
#define ARRAY_REALLOC(type, ptr, count)                                        \
    (type*)mem_realloc(ptr, sizeof(type) * (count), __FILE__, __LINE__)
#define ARRAY_FREE(ptr) FREE(ptr)

#define MEM_CHECK(ptr) mem_check(ptr, __FILE__, __LINE__)

//------------------------------------------------------------------------------[Array]

#define Array(T) T*

typedef struct ArrayHeader_t {
    usize count;
} ArrayHeader;

// Level 0 accessor macros - assumes that (a) is non-NULL and is a valid array
#define __array_info(a) ((ArrayHeader*)(a) - 1)
#define __array_bytes_capacity(a)                                              \
    (mem_size(__array_info(a)) - sizeof(ArrayHeader))
#define __array_count(a) (__array_info(a)->count)
#define __array_bytes_size(a) (__array_count(a) * sizeof(*(a)))
#define __array_safe(a, op) ((a) ? (op) : 0)

// Level 1 accessor macros - handles a NULL array
#define array_size(a) __array_safe((a), __array_bytes_size(a))
#define array_capacity(a)                                                      \
    __array_safe((a), __array_bytes_capacity(a) / sizeof(*(a)))
#define array_count(a) __array_safe((a), __array_count(a))

// Internal array growth function
static inline void* array_maybe_grow(void* array,
                                     usize element_size,
                                     usize required_capacity,
                                     cstr  file,
                                     int   line)
{
    if (!array) {
        // Initial allocation
        usize initial_capacity = 4;
        if (required_capacity > initial_capacity) {
            initial_capacity = required_capacity;
        }

        ArrayHeader* header = (ArrayHeader*)mem_alloc(
            sizeof(ArrayHeader) + initial_capacity * element_size, file, line);
        header->count = 0; // No elements yet

        return (void*)(header + 1);
    }

    // Calculate current capacity from memory size
    ArrayHeader* header             = (ArrayHeader*)array - 1;
    usize current_capacity_bytes    = mem_size(header) - sizeof(ArrayHeader);
    usize current_capacity_elements = current_capacity_bytes / element_size;

    if (required_capacity <= current_capacity_elements) {
        return array; // No growth needed
    }

    // Need to grow the array
    usize new_capacity_elements = current_capacity_elements * 2;
    if (new_capacity_elements < required_capacity) {
        new_capacity_elements = required_capacity;
    }

    usize        new_capacity_bytes = new_capacity_elements * element_size;
    ArrayHeader* old_header         = header;
    ArrayHeader* new_header         = (ArrayHeader*)mem_realloc(
        old_header, sizeof(ArrayHeader) + new_capacity_bytes, file, line);

    // count is preserved by realloc

    return (void*)(new_header + 1);
}

#define array_push(a, ...)                                                     \
    do {                                                                       \
        typeof(*(a)) __array_tmp[] = {__VA_ARGS__};                            \
        usize        __array_n = sizeof(__array_tmp) / sizeof(__array_tmp[0]); \
        (a)                    = array_maybe_grow((a),                         \
                               sizeof(*(a)),                \
                               array_count(a) + __array_n,  \
                               __FILE__,                    \
                               __LINE__);                   \
        memcpy((a) + __array_count(a), __array_tmp, __array_n * sizeof(*(a))); \
        __array_count(a) += __array_n;                                         \
    } while (0)

#define array_pop(a) ((a)[__array_count(a)-- - 1])

// Array free function - deallocates array memory
#define array_free(a)                                                          \
    do {                                                                       \
        if ((a)) {                                                             \
            ArrayHeader* header = __array_info(a);                             \
            FREE(header);                                                      \
            (a) = NULL;                                                        \
        }                                                                      \
    } while (0)

#define array_delete(a, index)                                                 \
    do {                                                                       \
        usize __array_index = (index);                                         \
        if (__array_index < array_count(a)) {                                  \
            memmove(&(a)[__array_index],                                       \
                    &(a)[__array_index + 1],                                   \
                    (array_count(a) - __array_index - 1) * sizeof(*(a)));      \
            __array_count(a)--;                                                \
        }                                                                      \
    } while (0)

#define array_clear(a)                                                         \
    do {                                                                       \
        if (a) {                                                               \
            __array_count(a) = 0;                                              \
        }                                                                      \
    } while (0)

// Array ensure capacity macro
#define array_requires(a, required_capacity)                                   \
    (a) = (typeof(*(a))*)array_maybe_grow(                                     \
        (a), sizeof(*(a)), (required_capacity), __FILE__, __LINE__)

// Array ensure size and capacity macro
#define array_reserve(a, required_size)                                        \
    do {                                                                       \
        (a) = (typeof(*(a))*)array_maybe_grow(                                 \
            (a), sizeof(*(a)), (required_size), __FILE__, __LINE__);           \
        __array_count(a) = (required_size);                                    \
    } while (0)

#define array_leak(a) mem_leak(__array_info(a))

//------------------------------------------------------------------------------[Arena]

#define ARENA_DEFAULT_NUM_PAGES_GROW 16

// OS-based arena with reserved memory pages
typedef struct {
    u8*   memory;            // Pointer to the start of the arena memory
    usize cursor;            // Current allocation cursor
    usize committed_size;    // Size of memory currently committed
    usize reserved_size;     // Total size of reserved memory
    usize alloc_granularity; // OS allocation granularity (page size)
    usize grow_rate;         // Number of allocation granules to grow by
} Arena;

//
// Arena lifetime
//

typedef struct {
    usize reserved_size;
    usize grow_rate;
} ArenaDefaultParams;

void _arena_init(Arena* arena, ArenaDefaultParams params);
void arena_done(Arena* arena);

#define arena_init(arena, ...)                                                 \
    _arena_init(arena, (ArenaDefaultParams){__VA_ARGS__})

//
// Arena allocation
//

void* arena_alloc(Arena* arena, usize size);
void  arena_align(Arena* arena, usize alignment);
void* arena_alloc_align(Arena* arena, usize size, usize alignment);

u8*  arena_formatv(Arena* arena, cstr fmt, va_list args);
u8*  arena_format(Arena* arena, cstr fmt, ...);
void arena_null_terminate(Arena* arena);

//
// Arena marks
//

void* arena_store(Arena* arena);
void  arena_restore(Arena* arena, void* mark);
void  arena_reset(Arena* arena);

//
// Arena state
//

usize arena_offset(Arena* arena, void* p);

//------------------------------------------------------------------------------[Mutex]

#if OS_WINDOWS
typedef CRITICAL_SECTION Mutex;
#elif OS_POSIX
#    include <pthread.h>
typedef pthread_mutex_t Mutex;
#endif

void mutex_init(Mutex* mutex);
void mutex_done(Mutex* mutex);
void mutex_lock(Mutex* mutex);
void mutex_unlock(Mutex* mutex);

//------------------------------------------------------------------------------[Output]

void prv(cstr format, va_list args);
void pr(cstr format, ...);
void prn(cstr format, ...);
void eprv(cstr format, va_list args);
void epr(cstr format, ...);
void eprn(cstr format, ...);

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_FAINT "\033[2m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_INVERSE "\033[7m"

#define ANSI_BLACK "\033[30m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN "\033[36m"
#define ANSI_WHITE "\033[37m"

#define ANSI_BOLD_BLACK "\033[1;30m"
#define ANSI_BOLD_RED "\033[1;31m"
#define ANSI_BOLD_GREEN "\033[1;32m"
#define ANSI_BOLD_YELLOW "\033[1;33m"
#define ANSI_BOLD_BLUE "\033[1;34m"
#define ANSI_BOLD_MAGENTA "\033[1;35m"
#define ANSI_BOLD_CYAN "\033[1;36m"
#define ANSI_BOLD_WHITE "\033[1;37m"

#define ANSI_FAINT_BLACK "\033[2;30m"
#define ANSI_FAINT_RED "\033[2;31m"
#define ANSI_FAINT_GREEN "\033[2;32m"
#define ANSI_FAINT_YELLOW "\033[2;33m"
#define ANSI_FAINT_BLUE "\033[2;34m"
#define ANSI_FAINT_MAGENTA "\033[2;35m"
#define ANSI_FAINT_CYAN "\033[2;36m"
#define ANSI_FAINT_WHITE "\033[2;37m"

#define ANSI_BG_BLACK "\033[40m"
#define ANSI_BG_RED "\033[41m"
#define ANSI_BG_GREEN "\033[42m"
#define ANSI_BG_YELLOW "\033[43m"
#define ANSI_BG_BLUE "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN "\033[46m"
#define ANSI_BG_WHITE "\033[47m"

#define ANSI_BG_BOLD_BLACK "\033[1;40m"
#define ANSI_BG_BOLD_RED "\033[1;41m"
#define ANSI_BG_BOLD_GREEN "\033[1;42m"
#define ANSI_BG_BOLD_YELLOW "\033[1;43m"
#define ANSI_BG_BOLD_BLUE "\033[1;44m"
#define ANSI_BG_BOLD_MAGENTA "\033[1;45m"
#define ANSI_BG_BOLD_CYAN "\033[1;46m"
#define ANSI_BG_BOLD_WHITE "\033[1;47m"

#define ANSI_BG_FAINT_BLACK "\033[2;40m"
#define ANSI_BG_FAINT_RED "\033[2;41m"
#define ANSI_BG_FAINT_GREEN "\033[2;42m"
#define ANSI_BG_FAINT_YELLOW "\033[2;43m"
#define ANSI_BG_FAINT_BLUE "\033[2;44m"
#define ANSI_BG_FAINT_MAGENTA "\033[2;45m"
#define ANSI_BG_FAINT_CYAN "\033[2;46m"
#define ANSI_BG_FAINT_WHITE "\033[2;47m"

#define UNICODE_TREE_BRANCH "├─ "
#define UNICODE_TREE_LAST_BRANCH "└─ "
#define UNICODE_TREE_VERTICAL "│  "

#define UNICODE_TABLE_TOP_LEFT "┌"
#define UNICODE_TABLE_TOP_RIGHT "┐"
#define UNICODE_TABLE_BOTTOM_LEFT "└"
#define UNICODE_TABLE_BOTTOM_RIGHT "┘"
#define UNICODE_TABLE_HORIZONTAL "─"
#define UNICODE_TABLE_VERTICAL "│"
#define UNICODE_TABLE_T_LEFT "├"
#define UNICODE_TABLE_T_RIGHT "┤"
#define UNICODE_TABLE_T_TOP "┬"
#define UNICODE_TABLE_T_BOTTOM "┴"
#define UNICODE_TABLE_CROSS "┼"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
