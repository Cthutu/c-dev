//------------------------------------------------------------------------------
// S-Expressions module
//
// Copyright (C)2025 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#include <kore/kore.h>
#include <kore/string.h>

//------------------------------------------------------------------------------
// S-Exp contexts

typedef u64 Atom;

typedef struct {
    Atom head;
    Atom tail;
} SexpCons;

typedef struct {
    Arena     cons_arena;
    SexpCons* free_list;
} SexpContext;

void sexp_context_init(SexpContext* ctx);
void sexp_context_done(SexpContext* ctx);

//------------------------------------------------------------------------------
// Construction of a S-exp value

#define ATOM_TYPE_MASK 0x0full

#define ATOM_TYPE_PTR 0x0ull
#define ATOM_TYPE_I32 0x1ull
#define ATOM_TYPE_U32 0x2ull
#define ATOM_TYPE_F32 0x3ull
#define ATOM_TYPE_SHORT_STRING 0x4ull
#define ATOM_TYPE_CONS 0x5ull

Atom sexp_make_ptr(void* ptr);
Atom sexp_make_i32(i32 v);
Atom sexp_make_u32(u32 v);
Atom sexp_make_f32(f32 v);
Atom sexp_make_null(void);
Atom sexp_make_cons(SexpContext* ctx, Atom car, Atom cdr);
Atom sexp_make_link(SexpCons* cons);

SexpCons* sexp_alloc_cons(SexpContext* ctx, Atom head, Atom tail);
SexpCons* sexp_alloc_list(SexpContext* ctx, usize count);

void* sexp_get_ptr(Atom atom);
i32   sexp_get_i32(Atom atom);
u32   sexp_get_u32(Atom atom);
f32   sexp_get_f32(Atom atom);
Atom  sexp_get_head(Atom atom);
Atom  sexp_get_tail(Atom atom);

u32  sexp_get_type(Atom atom);
bool sexp_is_null(Atom atom);
bool sexp_is_true(Atom atom);
bool sexp_is_false(Atom atom);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// I M P L E M E N T A T I O N
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef KORE_IMPLEMENTATION

//------------------------------------------------------------------------------
// S-Expression contexts

void sexp_context_init(SexpContext* ctx)
{
    *ctx = (SexpContext){0};
    arena_init(&ctx->cons_arena);
}

void sexp_context_done(SexpContext* ctx) { arena_done(&ctx->cons_arena); }

//------------------------------------------------------------------------------
// Cons cell allocation

SexpCons* sexp_alloc_cons(SexpContext* ctx, Atom head, Atom tail)
{
    SexpCons* cons =
        (SexpCons*)arena_alloc_align(&ctx->cons_arena, sizeof(SexpCons), 16);
    cons->head = head;
    cons->tail = tail;
    return cons;
}

SexpCons* sexp_alloc_list(SexpContext* ctx, usize count)
{
    SexpCons* list = (SexpCons*)arena_alloc_align(
        &ctx->cons_arena, sizeof(SexpCons) * count, 16);

    SexpCons* scan = list;
    for (usize i = 0; i < count; i++) {
        scan->head = sexp_make_null();
        scan->tail = sexp_make_link(scan + 1);
        scan++;
    }
    scan[-1].tail = sexp_make_null();
    return list;
}

//------------------------------------------------------------------------------
// Atom construction

internal inline Atom sexp_make_atom(u64 type, u64 value)
{
    return (value & ~ATOM_TYPE_MASK) | (type & ATOM_TYPE_MASK);
}

Atom sexp_make_ptr(void* ptr)
{
    KORE_ASSERT(((uintptr_t)ptr & 0xfull) == 0,
                "Pointer not properly aligned.");
    return sexp_make_atom(ATOM_TYPE_PTR, (u64)(uintptr_t)ptr);
}

Atom sexp_make_i32(i32 v)
{
    return sexp_make_atom(ATOM_TYPE_I32, (u64)(i64)(v << 4));
}

Atom sexp_make_u32(u32 v)
{
    return sexp_make_atom(ATOM_TYPE_U32, (u64)(v << 4));
}

Atom sexp_make_f32(f32 v)
{
    union {
        f32 f;
        u32 u;
    } conv;
    conv.f = v;
    return sexp_make_atom(ATOM_TYPE_F32, (u64)conv.u << 4);
}

Atom sexp_make_null(void) { return sexp_make_atom(ATOM_TYPE_PTR, 0); }

Atom sexp_make_cons(SexpContext* ctx, Atom car, Atom cdr)
{
    SexpCons* cons = sexp_alloc_cons(ctx, car, cdr);
    return sexp_make_atom(ATOM_TYPE_CONS, (u64)(uintptr_t)cons);
}

Atom sexp_make_link(SexpCons* cons)
{
    return sexp_make_atom(ATOM_TYPE_CONS, (u64)(uintptr_t)cons);
}

void* sexp_get_ptr(Atom atom)
{
    return (void*)(uintptr_t)(atom & ~ATOM_TYPE_MASK);
}

i32 sexp_get_i32(Atom atom)
{
    return (i32)((i64)(atom & ~ATOM_TYPE_MASK) >> 4);
}

u32 sexp_get_u32(Atom atom) { return (u32)((atom & ~ATOM_TYPE_MASK) >> 4); }

f32 sexp_get_f32(Atom atom)
{
    union {
        f32 f;
        u32 u;
    } conv;
    conv.u = (u32)((atom & ~ATOM_TYPE_MASK) >> 4);
    return conv.f;
}

Atom sexp_get_head(Atom atom)
{
    SexpCons* cons = (SexpCons*)(uintptr_t)(atom & ~ATOM_TYPE_MASK);
    return cons->head;
}

Atom sexp_get_tail(Atom atom)
{
    SexpCons* cons = (SexpCons*)(uintptr_t)(atom & ~ATOM_TYPE_MASK);
    return cons->tail;
}

u32 sexp_get_type(Atom atom) { return (u32)(atom & ATOM_TYPE_MASK); }

//------------------------------------------------------------------------------
// Query functions

bool sexp_is_null(Atom atom) { return atom == 0; }

bool sexp_is_true(Atom atom)
{
    return (sexp_get_type(atom) == ATOM_TYPE_I32 && sexp_get_i32(atom) != 0) ||
           (sexp_get_type(atom) == ATOM_TYPE_U32 && sexp_get_u32(atom) != 0);
}

bool sexp_is_false(Atom atom) { return !sexp_is_true(atom); }

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
