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
} KMemoryHeader;

void* k_memory_alloc(usize size, const char* file, int line) {
    KMemoryHeader* header =
        (KMemoryHeader*)malloc(sizeof(KMemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size = size;
    header->file = file;
    header->line = line;

    return (void*)(header + 1);
}

void* k_memory_realloc(void* ptr, usize size, const char* file, int line) {
    if (!ptr) {
        return k_memory_alloc(size, file, line);
    }

    KMemoryHeader* header = (KMemoryHeader*)ptr - 1;
    header = (KMemoryHeader*)realloc(header, sizeof(KMemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory reallocation failed at %s:%d\n", file, line);
        abort();
    }

    header->size = size;
    header->file = file;
    header->line = line;

    return (void*)(header + 1);
}

void k_memory_free(void* ptr, const char* file, int line) {
    if (!ptr) {
        return;
    }

    KMemoryHeader* header = (KMemoryHeader*)ptr - 1;
    free(header);
}

usize k_memory_size(const void* ptr) {
    if (!ptr) {
        return 0;
    }

    const KMemoryHeader* header = (const KMemoryHeader*)ptr - 1;
    return header->size;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
