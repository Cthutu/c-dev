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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // KORE_IMPLEMENTATION
