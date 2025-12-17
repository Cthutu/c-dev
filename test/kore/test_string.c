#include <kore/string.h>
#include <string.h>
#include <test/test.h>

TEST_CASE(string, from_cstr_null)
{
    string s = string_from_cstr(NULL);
    TEST_ASSERT_EQ(s.count, 0);
    TEST_ASSERT_NULL(s.data);
}

TEST_CASE(string, from_cstr_basic)
{
    const char* text = "abc";
    string      s    = string_from_cstr(text);

    TEST_ASSERT_EQ(s.count, 3);
    TEST_ASSERT(s.data == (u8*)text);
    TEST_ASSERT(memcmp(s.data, "abc", 3) == 0);
}

TEST_CASE(string, from_raw_binary)
{
    u8     bytes[] = {'x', 0, 'y'};
    string s       = string_from(bytes, sizeof(bytes));

    TEST_ASSERT_EQ(s.count, sizeof(bytes));
    TEST_ASSERT(s.data == bytes);
    TEST_ASSERT(memcmp(s.data, bytes, sizeof(bytes)) == 0);
}

TEST_CASE(string, format_basic)
{
    Arena arena;
    arena_init(&arena, .reserved_size = KORE_KB(64), .grow_rate = 1);

    string s = string_format(&arena, "Hello %s %d", "test", 42);

    TEST_ASSERT_EQ(s.count, strlen("Hello test 42"));
    TEST_ASSERT(memcmp(s.data, "Hello test 42", s.count) == 0);
    TEST_ASSERT(((char*)s.data)[s.count] == '\0');

    arena_done(&arena);
}

TEST_CASE(string, builder_append_and_format)
{
    Arena arena;
    arena_init(&arena, .reserved_size = KORE_KB(64), .grow_rate = 1);

    StringBuilder sb;
    sb_init(&sb, &arena);
    sb_append_cstr(&sb, "Hello");
    sb_append_char(&sb, ' ');
    sb_format(&sb, "%s %d", "world", 7);

    string s = sb_to_string(&sb);
    TEST_ASSERT_EQ(s.count, strlen("Hello world 7"));
    TEST_ASSERT(memcmp(s.data, "Hello world 7", s.count) == 0);

    sb_append_null(&sb);
    string s_null = sb_to_string(&sb);
    TEST_ASSERT_EQ(s_null.count, s.count + 1);
    TEST_ASSERT(((char*)s_null.data)[s.count] == '\0');

    arena_done(&arena);
}
