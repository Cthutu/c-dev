#include <kore/intern.h>
#include <stdio.h>
#include <string.h>
#include <test/test.h>

static void interner_cleanup(Interner* interner)
{
    array_free(interner->slots);
    arena_done(&interner->intern_arena);
}

TEST_CASE(intern, basic_same_string)
{
    Interner interner;
    intern_init(&interner, .expected_size = 8);

    string first  = intern_cstr(&interner, "Hello, World!");
    string second = intern_cstr(&interner, "Hello, World!");

    TEST_ASSERT(first.data == second.data);
    TEST_ASSERT_EQ(first.count, second.count);
    TEST_ASSERT_EQ(interner.count, 1);

    interner_cleanup(&interner);
}

TEST_CASE(intern, find_not_found)
{
    Interner interner;
    intern_init(&interner, .expected_size = 8);

    intern_cstr(&interner, "alpha");
    string missing = intern_find(&interner, string_from_cstr("beta"));

    TEST_ASSERT_EQ(missing.count, 0);
    TEST_ASSERT_NULL(missing.data);

    interner_cleanup(&interner);
}

TEST_CASE(intern, growth_reinsert_and_duplicates)
{
    Interner interner;
    intern_init(&interner, .expected_size = 2, .max_load_factor = 0.5);

    u64 initial_capacity = interner.capacity;

    enum { kTotal = 128 };
    char   texts[kTotal][32];
    string interned[kTotal];

    for (int i = 0; i < kTotal; i++) {
        snprintf(texts[i], sizeof(texts[i]), "item_%d", i);
        interned[i] = intern_cstr(&interner, texts[i]);
    }

    TEST_ASSERT_GT(interner.capacity, initial_capacity);
    TEST_ASSERT_EQ(interner.count, (u64)kTotal);

    for (int i = 0; i < kTotal; i++) {
        string found = intern_find(&interner, string_from_cstr(texts[i]));
        TEST_ASSERT(found.data == interned[i].data);
        TEST_ASSERT_EQ(found.count, interned[i].count);
        TEST_ASSERT(memcmp(found.data, texts[i], found.count) == 0);
    }

    u64    count_before = interner.count;
    string again        = intern_cstr(&interner, texts[42]);
    TEST_ASSERT(again.data == interned[42].data);
    TEST_ASSERT_EQ(interner.count, count_before);

    interner_cleanup(&interner);
}

TEST_CASE(intern, zero_length_and_binary)
{
    Interner interner;
    intern_init(&interner, .expected_size = 4);

    string empty = intern_cstr(&interner, "");
    TEST_ASSERT_EQ(empty.count, 0);
    TEST_ASSERT_NOT_NULL(empty.data);

    u8     bytes[] = {'a', 0, 'b'};
    string raw     = string_from(bytes, sizeof(bytes));
    string stored  = intern_add(&interner, raw);
    string found   = intern_find(&interner, raw);

    TEST_ASSERT(found.data == stored.data);
    TEST_ASSERT_EQ(found.count, stored.count);
    TEST_ASSERT_EQ(found.count, sizeof(bytes));
    TEST_ASSERT(memcmp(found.data, bytes, sizeof(bytes)) == 0);

    interner_cleanup(&interner);
}
