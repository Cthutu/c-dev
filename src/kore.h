#pragma once

//------------------------------------------------------------------------------
// Kore - a foundational header-only C library
// Copyright (c) 2023 Matt Davies, all rights reserved.
//------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// INDEX
//
// [Config]             Configuration macros and settings
// [Macros]             Basic macros
// [Types]              Basic types and definitions
// [Library]            Library initialisation and shutdown
// [Memory]             Memory management functions
// [Array]              Dynamic array implementation
// [Mutex]              Simple locking for resource protection
// [Output]             Basic output to stdout and stderr
//
//------------------------------------------------------------------------------

//----------------------------------------------------------------------[Config]

#define YES (1)
#define NO (0)

//
// Compiler detection
//

#define KORE_COMPILER_GCC NO
#define KORE_COMPILER_CLANG NO
#define KORE_COMPILER_MSVC NO

#if defined(__CLANG__)
#    undef KORE_COMPILER_CLANG
#    define KORE_COMPILER_CLANG YES
#elif defined(__GNUC__)
#    undef KORE_COMPILER_GCC
#    define KORE_COMPILER_GCC YES
#elif defined(_MSC_VER)
#    undef KORE_COMPILER_MSVC
#    define KORE_COMPILER_MSVC YES
#else
#    error "Unsupported compiler. Please use GCC, Clang, or MSVC."
#endif

//
// OS detection
//

#define KORE_OS_WINDOWS NO
#define KORE_OS_LINUX NO
#define KORE_OS_MACOS NO
#define KORE_OS_BSD NO
#define KORE_OS_POSIX NO

#if defined(_WIN32) || defined(_WIN64)
#    undef KORE_OS_WINDOWS
#    define KORE_OS_WINDOWS YES
#elif defined(__linux__)
#    undef KORE_OS_LINUX
#    define KORE_OS_LINUX YES
#elif defined(__APPLE__) || defined(__MACH__)
#    undef KORE_OS_MACOS
#    define KORE_OS_MACOS YES
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#    undef KORE_OS_BSD
#    define KORE_OS_BSD YES
#else
#    error "Unsupported OS. Please use Windows, Linux, macOS, or BSD."
#endif

#if KORE_OS_LINUX || KORE_OS_MACOS || KORE_OS_BSD
#    undef KORE_OS_POSIX
#    define KORE_OS_POSIX YES
#endif

//
// Architecture detection
//

#define KORE_ARCH_X86 NO
#define KORE_ARCH_X86_64 NO
#define KORE_ARCH_ARM NO
#define KORE_ARCH_ARM64 NO

#if defined(__i386__) || defined(_M_IX86)
#    undef KORE_ARCH_X86
#    define KORE_ARCH_X86 YES
#elif defined(__x86_64__) || defined(_M_X64)
#    undef KORE_ARCH_X86_64
#    define KORE_ARCH_X86_64 YES
#elif defined(__arm__) || defined(_M_ARM)
#    undef KORE_ARCH_ARM
#    define KORE_ARCH_ARM YES
#elif defined(__aarch64__) || defined(_M_ARM64)
#    undef KORE_ARCH_ARM64
#    define KORE_ARCH_ARM64 YES
#else
#    error "Unsupported architecture. Please use x86, x86_64, ARM, or ARM64."
#endif

//
// Build configuration detection
//

#define KORE_DEBUG NO
#define KORE_RELEASE NO
#define KORE_PROFILE NO

#if defined(NDEBUG)
#    undef KORE_RELEASE
#    define KORE_RELEASE YES
#elif defined(FINAL)
#    undef KORE_PROFILE
#    define KORE_PROFILE YES
#else
#    undef KORE_DEBUG
#    define KORE_DEBUG YES
#endif

//
// Debugger support
//

#if KORE_COMPILER_MSVC
#    define KORE_DEBUG_BREAK() __debugbreak()
#elif KORE_COMPILER_GCC || KORE_COMPILER_CLANG
#    define KORE_DEBUG_BREAK() __builtin_trap()
#else
#    error                                                                     \
        "Unsupported compiler for debug break. Please use GCC, Clang, or MSVC."
#endif

//----------------------------------------------------------------------[Macros]

#define internal static
#define global_variable static
#define local_persist static

//-----------------------------------------------------------------------[Types]

typedef uint8_t u8;   // Unsigned 8-bit integer
typedef uint16_t u16; // Unsigned 16-bit integer
typedef uint32_t u32; // Unsigned 32-bit integer
typedef uint64_t u64; // Unsigned 64-bit integer

typedef int8_t i8;   // Signed 8-bit integer
typedef int16_t i16; // Signed 16-bit integer
typedef int32_t i32; // Signed 32-bit integer
typedef int64_t i64; // Signed 64-bit integer

typedef size_t usize;    // Unsigned size type (platform-dependent)
typedef ptrdiff_t isize; // Signed size type (platform-dependent)

typedef float f32;  // 32-bit floating point
typedef double f64; // 64-bit floating point

typedef const char* cstr; // Constant string type

//---------------------------------------------------------------------[Library]

void kore_init(void);
void kore_done(void);

//----------------------------------------------------------------------[Memory]

void* mem_alloc(usize size, const char* file, int line);
void* mem_realloc(void* ptr, usize size, const char* file, int line);
void* mem_free(void* ptr, const char* file, int line);

usize mem_size(const void* ptr);
void mem_leak(void* ptr);

// Memory debugging utilities
#if KORE_DEBUG
void mem_print_leaks(void);
usize mem_get_allocation_count(void);
usize mem_get_total_allocated(void);
#endif // KORE_DEBUG

#define KORE_ALLOC(size) mem_alloc((size), __FILE__, __LINE__)
#define KORE_REALLOC(ptr, size) mem_realloc((ptr), (size), __FILE__, __LINE__)
#define KORE_FREE(ptr) ptr = mem_free((ptr), __FILE__, __LINE__), (ptr) = NULL

void mem_break_on_alloc(u64 index);

//-----------------------------------------------------------------------[Array]

#define Array(T) T*

typedef struct KArrayHeader_t {
    usize count;
} KArrayHeader;

// Level 0 accessor macros - assumes that (a) is non-NULL and is a valid array
#define __array_info(a) ((KArrayHeader*)(a) - 1)
#define __array_bytes_capacity(a)                                              \
    (mem_size(__array_info(a)) - sizeof(KArrayHeader))
#define __array_count(a) (__array_info(a)->count)
#define __array_bytes_size(a) (__array_count(a) * sizeof(*(a)))
#define __array_safe(a, op) ((a) ? (op) : 0)

// Level 1 accessor macros - handles a NULL array
#define array_size(a) __array_safe((a), __array_bytes_size(a))
#define array_capacity(a)                                                      \
    __array_safe((a), __array_bytes_capacity(a) / sizeof(*(a)))
#define array_count(a) __array_safe((a), __array_count(a))

// Forward declaration for internal array function
static void*
array_maybe_grow(void* array, usize element_size, usize required_capacity);

#define array_push(a, value)                                                   \
    ((a) = array_maybe_grow((a), sizeof(*(a)), array_count(a) + 1),            \
     (a)[__array_count(a)++] = (value))

#define array_pop(a) ((a)[__array_count(a)-- - 1])

// Array free function - deallocates array memory
#define array_free(a)                                                          \
    do {                                                                       \
        if ((a)) {                                                             \
            KArrayHeader* header = __array_info(a);                            \
            KORE_FREE(header);                                                 \
            (a) = NULL;                                                        \
        }                                                                      \
    } while (0)

#define array_requires(a, required_capacity)                                   \
    (a) = (typeof(*(a))*)array_maybe_grow(                                     \
        (a), sizeof(*(a)), (required_capacity))

#define array_leak(a) mem_leak(__array_info(a))

//-----------------------------------------------------------------------[Mutex]

#if KORE_OS_WINDOWS
typedef CriticalSection Mutex;
#elif KORE_OS_POSIX
#    include <pthread.h>
typedef pthread_mutex_t Mutex;
#else
#    error "Mutex not implemented for this OS."
#endif

void mutex_init(Mutex* mutex);
void mutex_done(Mutex* mutex);
void mutex_lock(Mutex* mutex);
void mutex_unlock(Mutex* mutex);

//----------------------------------------------------------------------[Output]

void prv(const char* format, va_list args);
void pr(const char* format, ...);
void prn(const char* format, ...);
void eprv(const char* format, va_list args);
void epr(const char* format, ...);
void eprn(const char* format, ...);

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_FAINT "\033[2m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_INVERSED "\033[7m"

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
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#if defined(KORE_IMPLEMENTATION)

#    include <stdio.h>
#    include <stdlib.h>
#    include <threads.h>

#    if KORE_OS_POSIX
#        include <unistd.h>
#    endif // KORE_OS_POSIX

//---------------------------------------------------------------------[Globals]

Mutex g_kore_output_mutex;

//----------------------------------------------------------------------[Memory]

typedef struct KMemoryHeader_t {
    usize size; // Number of bytes allocated

#    if KORE_DEBUG
    const char* file; // File where the allocation was made
    int line;         // Line number where the allocation was made
    u64 index;        // Index of the allocation for debugging purposes

    struct KMemoryHeader_t* next; // Pointer to the next header in a linked list
    bool leaked; // Flag to indicate if this block was leaked and therefore
                 // should not be in the linked list.  This is used to mark
                 // allocations with application lifetimes.
#    endif       // KORE_DEBUG
} KMemoryHeader;

// Global pointer to the head of the linked list of allocated memory blocks
#    if KORE_DEBUG
static KMemoryHeader* g_memory_head = NULL;
static u64 g_memory_index           = 0; // Global index for allocations
static u64 g_memory_break_index     = 0; // Index to break on allocation
#    endif                           // KORE_DEBUG

void* mem_alloc(usize size, const char* file, int line) {
    KMemoryHeader* header =
        (KMemoryHeader*)malloc(sizeof(KMemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size = size;

#    if KORE_DEBUG
    header->file   = file;
    header->line   = line;
    header->leaked = false;            // Initialise leaked flag
    header->index  = ++g_memory_index; // Assign and increment index

    // Check if we should break on this allocation
    if (header->index == g_memory_break_index) {
        KORE_DEBUG_BREAK();
    }

    // Add to linked list
    header->next  = g_memory_head;
    g_memory_head = header;
#    endif // KORE_DEBUG

    return (void*)(header + 1);
}

void* mem_realloc(void* ptr, usize size, const char* file, int line) {
    if (!ptr) {
        return mem_alloc(size, file, line);
    }

    KMemoryHeader* old_header = (KMemoryHeader*)ptr - 1;

#    if KORE_DEBUG
    // Preserve the leaked flag from the old header
    bool was_leaked = old_header->leaked;
#    endif // KORE_DEBUG

// Remove old header from linked list
#    if KORE_DEBUG
    if (!old_header->leaked) {
        if (g_memory_head == old_header) {
            g_memory_head = old_header->next;
        } else {
            KMemoryHeader* current = g_memory_head;
            while (current && current->next != old_header) {
                current = current->next;
            }
            if (current) {
                current->next = old_header->next;
            }
        }
    }
#    endif // KORE_DEBUG

    KMemoryHeader* header =
        (KMemoryHeader*)realloc(old_header, sizeof(KMemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory reallocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size = size;

#    if KORE_DEBUG
    header->file   = file;
    header->line   = line;
    header->leaked = was_leaked; // Preserve the leaked flag
    header->index  = ++g_memory_index;

    if (header->index == g_memory_break_index) {
        KORE_DEBUG_BREAK(); // Break if this allocation matches the break index
    }

    // Add new header to linked list only if it's not leaked
    if (!header->leaked) {
        header->next  = g_memory_head;
        g_memory_head = header;
    }
#    endif // KORE_DEBUG

    return (void*)(header + 1);
}

void* mem_free(void* ptr, const char* file, int line) {
    (void)file; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning

    if (!ptr) {
        return nullptr;
    }

    KMemoryHeader* header = (KMemoryHeader*)ptr - 1;

#    if KORE_DEBUG
    // Remove from linked list
    if (!header->leaked) {
        if (g_memory_head == header) {
            g_memory_head = header->next;
        } else {
            KMemoryHeader* current = g_memory_head;
            while (current && current->next != header) {
                current = current->next;
            }
            if (current) {
                current->next = header->next;
            }
        }
    }
#    endif // KORE_DEBUG

    free(header);
    return nullptr;
}

usize mem_size(const void* ptr) {
    if (!ptr) {
        return 0;
    }

    const KMemoryHeader* header = (const KMemoryHeader*)ptr - 1;
    return header->size;
}

void mem_leak(void* ptr) {
#    if KORE_DEBUG

    if (!ptr) {
        return;
    }

    KMemoryHeader* header = (KMemoryHeader*)ptr - 1;
    header->leaked        = true; // Mark this block as leaked

    // Remove from linked list if it exists
    if (g_memory_head == header) {
        g_memory_head = header->next;
    } else {
        KMemoryHeader* current = g_memory_head;
        while (current && current->next != header) {
            current = current->next;
        }
        if (current) {
            current->next = header->next;
        }
    }

#    endif // KORE_DEBUG
}

void mem_break_on_alloc(u64 index) {
#    if KORE_DEBUG
    g_memory_break_index = index;
#    endif
}

#    if KORE_DEBUG

// Memory debugging utilities
void mem_print_leaks(void) {
    KMemoryHeader* current = g_memory_head;
    usize leak_count       = 0;
    usize total_leaked     = 0;

    if (!current) {
        return;
    }

    eprn(ANSI_BOLD_RED "┌──────────────────────────────────────┐" ANSI_RESET);
    eprn(ANSI_BOLD_RED "│        Memory leaks detected         │" ANSI_RESET);
    eprn(ANSI_BOLD_RED "└──────────────────────────────────────┘" ANSI_RESET);

    while (current) {
        eprn(ANSI_FAINT " %s" ANSI_RESET ANSI_BOLD "[%zu]" ANSI_RESET
                        " %s:%d " ANSI_BOLD_YELLOW "%zu bytes" ANSI_RESET,
             UNICODE_TREE_BRANCH,
             current->index,
             current->file,
             current->line,
             current->size);

        total_leaked += current->size;
        leak_count++;
        current = current->next;
    }

    eprn(" " ANSI_FAINT UNICODE_TREE_LAST_BRANCH ANSI_RESET ANSI_BOLD_RED
         "Total:" ANSI_RESET " %zu leaks, %zu bytes",
         leak_count,
         total_leaked);
}

usize mem_get_allocation_count(void) {
    usize count            = 0;
    KMemoryHeader* current = g_memory_head;

    while (current) {
        count++;
        current = current->next;
    }

    return count;
}

usize mem_get_total_allocated(void) {
    usize total            = 0;
    KMemoryHeader* current = g_memory_head;

    while (current) {
        total += current->size;
        current = current->next;
    }

    return total;
}

#    endif // KORE_DEBUG

//-----------------------------------------------------------------------[Array]

// Internal array growth function
static void*
array_maybe_grow(void* array, usize element_size, usize required_capacity) {
    if (!array) {
        // Initial allocation
        usize initial_capacity = 4;
        if (required_capacity > initial_capacity) {
            initial_capacity = required_capacity;
        }

        KArrayHeader* header = (KArrayHeader*)KORE_ALLOC(
            sizeof(KArrayHeader) + initial_capacity * element_size);
        header->count = 0; // No elements yet

        return (void*)(header + 1);
    }

    // Calculate current capacity from memory size
    KArrayHeader* header            = (KArrayHeader*)array - 1;
    usize current_capacity_bytes    = mem_size(header) - sizeof(KArrayHeader);
    usize current_capacity_elements = current_capacity_bytes / element_size;

    if (required_capacity <= current_capacity_elements) {
        return array; // No growth needed
    }

    // Need to grow the array
    usize new_capacity_elements = current_capacity_elements * 2;
    if (new_capacity_elements < required_capacity) {
        new_capacity_elements = required_capacity;
    }

    usize new_capacity_bytes = new_capacity_elements * element_size;
    KArrayHeader* old_header = header;
    KArrayHeader* new_header = (KArrayHeader*)KORE_REALLOC(
        old_header, sizeof(KArrayHeader) + new_capacity_bytes);

    // count is preserved by realloc

    return (void*)(new_header + 1);
}

//-----------------------------------------------------------------------[Mutex]

#    if KORE_OS_WINDOWS

void mutex_init(Mutex* mutex) { InitializeCriticalSection(mutex); }

void mutex_done(Mutex* mutex) { DeleteCriticalSection(mutex); }

void mutex_lock(Mutex* mutex) { EnterCriticalSection(mutex); }

void mutex_unlock(Mutex* mutex) { LeaveCriticalSection(mutex); }

#    else // KORE_OS_POSIX

void mutex_init(Mutex* mutex) { pthread_mutex_init(mutex, NULL); }

void mutex_done(Mutex* mutex) { pthread_mutex_destroy(mutex); }

void mutex_lock(Mutex* mutex) { pthread_mutex_lock(mutex); }

void mutex_unlock(Mutex* mutex) { pthread_mutex_unlock(mutex); }

#    endif // KORE_OS_WINDOWS

//----------------------------------------------------------------------[Output]

internal cstr _format_output(cstr format, va_list args, usize* out_size) {
    thread_local local_persist Array(char) print_buffer = NULL;

    // Get the size of the formatted string.
    va_list args_copy;
    va_copy(args_copy, args);
    *out_size = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    // Allocate or reallocate the print buffer if necessary.
    array_requires(print_buffer, *out_size + 1);
    array_leak(print_buffer); // Prevent detection in leaks

    // Format the string into the buffer.
    vsnprintf(print_buffer, *out_size + 1, format, args);

    return print_buffer;
}

#    if KORE_OS_POSIX

internal void fprv(int fd, cstr format, va_list args) {
    usize size;
    mutex_lock(&g_kore_output_mutex);
    cstr output = _format_output(format, args, &size);
    write(fd, output, size);
    mutex_unlock(&g_kore_output_mutex);
}

void prv(cstr format, va_list args) { fprv(STDOUT_FILENO, format, args); }
void eprv(cstr format, va_list args) { fprv(STDERR_FILENO, format, args); }

#    elif defined(KORE_OS_WINDOWS)

internal void fprv(HANDLE handle, cstr format, va_list args) {
    usize size;
    mutex_lock(&g_kore_output_mutex);
    cstr output = _format_output(format, args, &size);

    // Write to the file handle.
    DWORD console_mode;
    BOOL is_console = GetConsoleMode(handle, &console_mode);

    DWORD bytes_written;
    if (is_console) {
        WriteConsoleA(handle, output, size, &bytes_written, NULL);
    } else {
        WriteFile(handle, output, size, &bytes_written, NULL);
    }
    mutex_unlock(&g_kore_output_mutex);
}

void prv(cstr format, va_list args) {
    fprv(GetStdHandle(STD_OUTPUT_HANDLE), format, args);
}

void eprv(cstr format, va_list args) {
    fprv(GetStdHandle(STD_ERROR_HANDLE), format, args);
}

#    else
#        error "Output functions not implemented for this OS."

#    endif // KORE_OS_POSIX

void pr(cstr format, ...) {
    va_list args;
    va_start(args, format);
    prv(format, args);
    va_end(args);
}

void prn(cstr format, ...) {
    va_list args;
    va_start(args, format);
    prv(format, args);
    pr("\n");
    va_end(args);
}

void epr(cstr format, ...) {
    va_list args;
    va_start(args, format);
    eprv(format, args);
    va_end(args);
}

void eprn(cstr format, ...) {
    va_list args;
    va_start(args, format);
    eprv(format, args);
    epr("\n");
    va_end(args);
}

//---------------------------------------------------------------------[Library]

void kore_init(void) { mutex_init(&g_kore_output_mutex); }

void kore_done(void) {
    mutex_done(&g_kore_output_mutex);
#    if KORE_DEBUG
    mem_print_leaks();
#    endif // KORE_DEBUG
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
