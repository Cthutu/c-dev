//------------------------------------------------------------------------------
// String interning module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <kore/string.h>

//------------------------------------------------------------------------------

#define INTERN_SEED 0x7f4a7c1506fe123dull
#define INTERN_EXPECTED_SIZE 1024
#define INTERN_MAX_LOAD_FACTOR 0.85f
#define INTERN_SIZE_MASK (INTERN_EXPECTED_SIZE - 1)
#define INTERN_ALIGNMENT 16

typedef struct {
    u64 hash;
    u8  len;
    u8  str[0];
} InternedString;

typedef struct {
    u64             hash; // 0 = empty
    InternedString* str;  // Pointer to string in intern arena
    u64             psl;  // Probe sequence length
} InternSlot;

typedef struct {
    Array(InternSlot) slots;
    Arena intern_arena;
    u64   capacity;        // Always a power of two
    u64   capacity_mask;   // capacity - 1
    u64   count;           // Number of live entries
    f64   max_load_factor; // How full should it get before we grow
    void* slots_mark;      // Arena mark for slots
    u64   seed;            // Hash seed
} Interner;

typedef struct {
    u64 expected_size;
    f64 max_load_factor;
    u64 seed;
} InternInitParams;

void _intern_init(Interner* interner, InternInitParams params);

#define intern_init(interner, ...)                                             \
    _intern_init((interner), (InternInitParams){__VA_ARGS__})

string intern_cstr(Interner* interner, cstr str);
string intern_find(Interner* interner, string str);
string intern_add(Interner* interner, string str);

#define INTERN_NOTFOUND (-1u)

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef KORE_IMPLEMENTATION

//------------------------------------------------------------------------------
// Hashing

internal inline u64 int_rot_left_64(u64 x, int k)
{
    return (x << k) | (x >> (64 - k));
}

internal inline u64 int_split_mix_64_next(u64* x)
{
    u64 z = (*x += 0x9e3779b97f4a7c15ull);
    z     = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z     = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
}

internal inline u64 int_hash_bytes_64(u8* data, u8 len, u64 seed)
{
    u64 s = seed;
    u64 h = 0;

    // Body hashing
    while (len >= 8) {
        u64 v;
        memcpy(&v, data, 8);
        data += 8;
        len -= 8;
        s ^= v;
        u64 m = int_split_mix_64_next(&s);
        h ^= int_rot_left_64(m, 17);
        h += 0x9ddfea08eb382d69ull;
    }

    // Tail hashing
    u64 t = 0;
    for (u32 i = 0; i < len; i++) {
        t |= ((u64)data[i]) << (i * 8);
    }
    s ^= t;
    h ^= int_split_mix_64_next(&s);
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdull;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ull;
    h ^= h >> 33;

    return h ? h : 0x9e3779b97f4a7c15ull;
}

internal inline u64 int_next_power_of_two(u64 v)
{
#    if KORE_ARCH_X86_64 || (KORE_COMPILER_GCC || KORE_COMPILER_CLANG)
    if (v < 2) {
        return 2;
    }
    return 1ull << (64 - __builtin_clzll(v - 1));
#    else
    if (v < 2) {
        return 2;
    }
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return v + 1;
#    endif // KORE_ARCH_X86_64
}

//------------------------------------------------------------------------------
// Interner

void _intern_init(Interner* interner, InternInitParams params)
{
    // Ensure all fields start from a known state, especially the slots pointer
    // that dynamic array helpers expect to be NULL.
    *interner = (Interner){0};

    arena_init(&interner->intern_arena);
    interner->max_load_factor = params.max_load_factor ? params.max_load_factor
                                                       : INTERN_MAX_LOAD_FACTOR;
    interner->seed            = params.seed ? params.seed : INTERN_SEED;

    u64 cap_needed =
        (u64)((f64)params.expected_size / interner->max_load_factor) + 2;
    interner->capacity      = int_next_power_of_two(cap_needed);
    interner->capacity_mask = interner->capacity - 1;
    interner->count         = 0;

    array_reserve(interner->slots, interner->capacity);
    memset(interner->slots, 0, sizeof(InternSlot) * interner->capacity);
}

internal inline bool intern_load_exceeded(Interner* interner)
{
    return (f64)interner->count >
           interner->max_load_factor * (f64)interner->capacity;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
