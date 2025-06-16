#pragma once

//------------------------------------------------------------------------------
// Kore - a foundational header-only C library
// Copyright (c) 2023 Matt Davies, all rights reserved.
//------------------------------------------------------------------------------

#include <stdint.h>

//------------------------------------------------------------------------------
// INDEX
//
// [Types]              Basic types and definitions
// [Memory]             Memory management functions
//
//------------------------------------------------------------------------------

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
void k_memory_print_leaks(void);
usize k_memory_get_allocation_count(void);
usize k_memory_get_total_allocated(void);

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
    const char* file;
    int line;

    struct KMemoryHeader_t* next; // Pointer to the next header in a linked list
} KMemoryHeader;

// Global pointer to the head of the linked list of allocated memory blocks
static KMemoryHeader* g_memory_head = NULL;

void* k_memory_alloc(usize size, const char* file, int line) {
    KMemoryHeader* header =
        (KMemoryHeader*)malloc(sizeof(KMemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size  = size;
    header->file  = file;
    header->line  = line;

    // Add to linked list
    header->next  = g_memory_head;
    g_memory_head = header;

    return (void*)(header + 1);
}

void* k_memory_realloc(void* ptr, usize size, const char* file, int line) {
    if (!ptr) {
        return k_memory_alloc(size, file, line);
    }

    KMemoryHeader* old_header = (KMemoryHeader*)ptr - 1;

    // Remove old header from linked list
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

    KMemoryHeader* header =
        (KMemoryHeader*)realloc(old_header, sizeof(KMemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory reallocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size  = size;
    header->file  = file;
    header->line  = line;

    // Add new header to linked list
    header->next  = g_memory_head;
    g_memory_head = header;

    return (void*)(header + 1);
}

void k_memory_free(void* ptr, const char* file, int line) {
    (void)file; // Suppress unused parameter warning
    (void)line; // Suppress unused parameter warning

    if (!ptr) {
        return;
    }

    KMemoryHeader* header = (KMemoryHeader*)ptr - 1;

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

    free(header);
}

usize k_memory_size(const void* ptr) {
    if (!ptr) {
        return 0;
    }

    const KMemoryHeader* header = (const KMemoryHeader*)ptr - 1;
    return header->size;
}

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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
