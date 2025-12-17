#include <kore/sexp.h>
#include <string.h>
#include <test/test.h>

TEST_CASE(sexp, make_and_get_primitives)
{
    Arena arena;
    arena_init(&arena, .reserved_size = KORE_KB(64), .grow_rate = 1);

    void* ptr = arena_alloc_align(&arena, 16, 16);
    Atom  ap  = sexp_make_ptr(ptr);
    TEST_ASSERT_EQ(sexp_get_type(ap), ATOM_TYPE_PTR);
    TEST_ASSERT(sexp_get_ptr(ap) == ptr);

    Atom ai = sexp_make_i32(-42);
    TEST_ASSERT_EQ(sexp_get_type(ai), ATOM_TYPE_I32);
    TEST_ASSERT_EQ(sexp_get_i32(ai), -42);

    Atom au = sexp_make_u32(123456u);
    TEST_ASSERT_EQ(sexp_get_type(au), ATOM_TYPE_U32);
    TEST_ASSERT_EQ(sexp_get_u32(au), 123456u);

    Atom af = sexp_make_f32(3.5f);
    TEST_ASSERT_EQ(sexp_get_type(af), ATOM_TYPE_F32);
    TEST_ASSERT(sexp_get_f32(af) == 3.5f);

    arena_done(&arena);
}

TEST_CASE(sexp, null_and_truthiness)
{
    Atom null_atom = sexp_make_null();
    TEST_ASSERT(sexp_is_null(null_atom));
    TEST_ASSERT(sexp_is_false(null_atom));

    Atom t1 = sexp_make_i32(1);
    Atom t2 = sexp_make_u32(7);
    Atom f1 = sexp_make_i32(0);
    Atom f2 = sexp_make_u32(0);

    TEST_ASSERT(sexp_is_true(t1));
    TEST_ASSERT(sexp_is_true(t2));
    TEST_ASSERT(sexp_is_false(f1));
    TEST_ASSERT(sexp_is_false(f2));
}

TEST_CASE(sexp, cons_cells)
{
    SexpContext ctx;
    sexp_context_init(&ctx);

    Atom head = sexp_make_i32(5);
    Atom tail = sexp_make_null();
    Atom cons = sexp_make_cons(&ctx, head, tail);

    TEST_ASSERT_EQ(sexp_get_type(cons), ATOM_TYPE_CONS);
    TEST_ASSERT_EQ(sexp_get_i32(sexp_get_head(cons)), 5);
    TEST_ASSERT(sexp_is_null(sexp_get_tail(cons)));

    sexp_context_done(&ctx);
}

TEST_CASE(sexp, alloc_list_links)
{
    SexpContext ctx;
    sexp_context_init(&ctx);

    SexpCons* list = sexp_alloc_list(&ctx, 3);
    Atom      node = sexp_make_link(list);

    TEST_ASSERT_EQ(sexp_get_type(node), ATOM_TYPE_CONS);
    TEST_ASSERT(sexp_is_null(sexp_get_head(node)));

    Atom tail1 = sexp_get_tail(node);
    TEST_ASSERT_EQ(sexp_get_type(tail1), ATOM_TYPE_CONS);

    Atom tail2 = sexp_get_tail(tail1);
    TEST_ASSERT_EQ(sexp_get_type(tail2), ATOM_TYPE_CONS);

    Atom tail3 = sexp_get_tail(tail2);
    TEST_ASSERT(sexp_is_null(tail3));

    sexp_context_done(&ctx);
}
