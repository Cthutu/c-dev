#pragma once

//------------------------------------------------------------------------------
// Kore - a foundational header-only C library
// Copyright (c) 2023 Matt Davies, all rights reserved.
//------------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// INDEX
//
// [Config]             Configuration macros and settings
// [Types]              Basic types and definitions
// [Memory]             Memory management functions
// [Array]              Dynamic array implementation
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

//----------------------------------------------------------------------[Memory]

void* memory_alloc(usize size, const char* file, int line);
void* memory_realloc(void* ptr, usize size, const char* file, int line);
void memory_free(void* ptr, const char* file, int line);

usize memory_size(const void* ptr);
void memory_leak(void* ptr);

// Memory debugging utilities
#if KORE_DEBUG
void memory_print_leaks(void);
usize memory_get_allocation_count(void);
usize memory_get_total_allocated(void);
#endif // KORE_DEBUG

#define KORE_MALLOC(size) memory_alloc((size), __FILE__, __LINE__)
#define KORE_REALLOC(ptr, size)                                                \
    memory_realloc((ptr), (size), __FILE__, __LINE__)
#define KORE_FREE(ptr) memory_free((ptr), __FILE__, __LINE__), (ptr) = NULL

void memory_break_on_alloc(u64 index);

//-----------------------------------------------------------------------[Array]

#define Array(T) T*

typedef struct KArrayHeader_t {
    usize count;
} KArrayHeader;

// Level 0 accessor macros - assumes that (a) is non-NULL and is a valid array
#define __array_info(a) ((KArrayHeader*)(a) - 1)
#define __array_bytes_capacity(a) (memory_size(__array_info(a)))
#define __array_count(a) (__array_info(a)->count)
#define __array_bytes_size(a) (__array_count(a) * sizeof(*(a)))
#define __array_safe(a, op) ((a) ? (op) : 0)

// Level 1 accessor macros - handles a NULL array
#define array_size(a) __array_safe((a), __array_bytes_size(a))
#define array_capacity(a) __array_safe((a), __array_bytes_capacity(a))
#define array_count(a) __array_safe((a), __array_count(a))

// Forward declaration for internal array function
static void*
array_maybe_grow(void* array, usize element_size, usize required_capacity);

#define array_push(a, value)                                                   \
    do {                                                                       \
        usize current_count = array_count(a);                                  \
        (a) = array_maybe_grow((a), sizeof(*(a)), current_count + 1);          \
        (a)[current_count] = (value);                                          \
        __array_info(a)->count += 1;                                           \
    } while (0)

// Array pop function - removes and returns last element
#define array_pop(a)                                                           \
    ({                                                                         \
        usize current_count = array_count(a);                                  \
        typeof(*(a)) result;                                                   \
        if (current_count == 0) {                                              \
            /* Handle empty array - return zero/null value */                  \
            typeof(*(a)) zero_value = {0};                                     \
            result = zero_value;                                               \
        } else {                                                               \
            result = (a)[current_count - 1];                                   \
            /* Decrement the count */                                          \
            __array_info(a)->count -= 1;                                       \
        }                                                                      \
        result;                                                                \
    })

// Array free function - deallocates array memory
#define array_free(a)                                                          \
    do {                                                                       \
        if ((a)) {                                                             \
            KArrayHeader* header = __array_info(a);                            \
            KORE_FREE(header);                                                 \
            (a) = NULL;                                                        \
        }                                                                      \
    } while (0)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#if defined(KORE_IMPLEMENTATION)

#    include <stdio.h>
#    include <stdlib.h>

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

void* memory_alloc(usize size, const char* file, int line) {
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

void* memory_realloc(void* ptr, usize size, const char* file, int line) {
    if (!ptr) {
        return memory_alloc(size, file, line);
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

void memory_free(void* ptr, const char* file, int line) {
    (void)file; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning

    if (!ptr) {
        return;
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
}

usize memory_size(const void* ptr) {
    if (!ptr) {
        return 0;
    }

    const KMemoryHeader* header = (const KMemoryHeader*)ptr - 1;
    return header->size;
}

void memory_leak(void* ptr) {
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

void memory_break_on_alloc(u64 index) {
#    if KORE_DEBUG
    g_memory_break_index = index;
#    endif
}

#    if KORE_DEBUG

// Memory debugging utilities
void memory_print_leaks(void) {
    KMemoryHeader* current = g_memory_head;
    usize leak_count       = 0;
    usize total_leaked     = 0;

    if (!current) {
        printf("No memory leaks detected.\n");
        return;
    }

    printf("Memory leaks detected:\n");
    printf("========================\n");

    while (current) {
        printf("[%zu] %s:%d: %zu bytes allocated\n",
               current->index,
               current->file,
               current->line,
               current->size);
        total_leaked += current->size;
        leak_count++;
        current = current->next;
    }

    printf("========================\n");
    printf("Total: %zu leaks, %zu bytes\n", leak_count, total_leaked);
}

usize memory_get_allocation_count(void) {
    usize count            = 0;
    KMemoryHeader* current = g_memory_head;

    while (current) {
        count++;
        current = current->next;
    }

    return count;
}

usize memory_get_total_allocated(void) {
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

        KArrayHeader* header = (KArrayHeader*)KORE_MALLOC(
            sizeof(KArrayHeader) + initial_capacity * element_size);
        header->count = 0; // No elements yet

        return (void*)(header + 1);
    }

    // Calculate current capacity from memory size
    KArrayHeader* header         = (KArrayHeader*)array - 1;
    usize current_capacity_bytes = memory_size(header) - sizeof(KArrayHeader);
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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
