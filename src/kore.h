#pragma once

//------------------------------------------------------------------------------
// Kore - a foundational header-only C library
// Copyright (c) 2023 Matt Davies, all rights reserved.
//------------------------------------------------------------------------------

#include <stdint.h>

//------------------------------------------------------------------------------
// INDEX
//
// [Config]             Configuration macros and settings
// [Types]              Basic types and definitions
// [Memory]             Memory management functions
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

void* k_memory_alloc(usize size, const char* file, int line);
void* k_memory_realloc(void* ptr, usize size, const char* file, int line);
void k_memory_free(void* ptr, const char* file, int line);

usize k_memory_size(const void* ptr);

// Memory debugging utilities
#if KORE_DEBUG
void k_memory_print_leaks(void);
usize k_memory_get_allocation_count(void);
usize k_memory_get_total_allocated(void);
#endif // KORE_DEBUG

#define KORE_MALLOC(size) k_memory_alloc((size), __FILE__, __LINE__)
#define KORE_REALLOC(ptr, size)                                                \
    k_memory_realloc((ptr), (size), __FILE__, __LINE__)
#define KORE_FREE(ptr) k_memory_free((ptr), __FILE__, __LINE__), (ptr) = NULL

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
    usize size;

#    if KORE_DEBUG
    const char* file;
    int line;

    struct KMemoryHeader_t* next; // Pointer to the next header in a linked list
#    endif                        // KORE_DEBUG
} KMemoryHeader;

// Global pointer to the head of the linked list of allocated memory blocks
#    if KORE_DEBUG
static KMemoryHeader* g_memory_head = NULL;
#    endif // KORE_DEBUG

void* k_memory_alloc(usize size, const char* file, int line) {
    KMemoryHeader* header =
        (KMemoryHeader*)malloc(sizeof(KMemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size = size;

#    if KORE_DEBUG
    header->file  = file;
    header->line  = line;

    // Add to linked list
    header->next  = g_memory_head;
    g_memory_head = header;
#    endif // KORE_DEBUG

    return (void*)(header + 1);
}

void* k_memory_realloc(void* ptr, usize size, const char* file, int line) {
    if (!ptr) {
        return k_memory_alloc(size, file, line);
    }

    KMemoryHeader* old_header = (KMemoryHeader*)ptr - 1;

// Remove old header from linked list
#    if KORE_DEBUG
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
#    endif // KORE_DEBUG

    KMemoryHeader* header =
        (KMemoryHeader*)realloc(old_header, sizeof(KMemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory reallocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size = size;

#    if KORE_DEBUG
    header->file  = file;
    header->line  = line;

    // Add new header to linked list
    header->next  = g_memory_head;
    g_memory_head = header;
#    endif // KORE_DEBUG

    return (void*)(header + 1);
}

void k_memory_free(void* ptr, const char* file, int line) {
    (void)file; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning

    if (!ptr) {
        return;
    }

    KMemoryHeader* header = (KMemoryHeader*)ptr - 1;

#    if KORE_DEBUG
    // Remove from linked list
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

    free(header);
}

usize k_memory_size(const void* ptr) {
    if (!ptr) {
        return 0;
    }

    const KMemoryHeader* header = (const KMemoryHeader*)ptr - 1;
    return header->size;
}

#    if KORE_DEBUG

// Memory debugging utilities
void k_memory_print_leaks(void) {
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
        printf("Leak #%zu: %zu bytes allocated at %s:%d\n",
               leak_count + 1,
               current->size,
               current->file,
               current->line);
        total_leaked += current->size;
        leak_count++;
        current = current->next;
    }

    printf("========================\n");
    printf("Total: %zu leaks, %zu bytes\n", leak_count, total_leaked);
}

usize k_memory_get_allocation_count(void) {
    usize count            = 0;
    KMemoryHeader* current = g_memory_head;

    while (current) {
        count++;
        current = current->next;
    }

    return count;
}

usize k_memory_get_total_allocated(void) {
    usize total            = 0;
    KMemoryHeader* current = g_memory_head;

    while (current) {
        total += current->size;
        current = current->next;
    }

    return total;
}

#    endif // KORE_DEBUG

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
