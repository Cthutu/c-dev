#define TEST_IMPLEMENTATION
#include "test.h"

// Example function to test
int add(int a, int b) { return a + b; }

int multiply(int a, int b) { return a * b; }

const char* get_greeting(void) { return "Hello, World!"; }

// Test cases
TEST_CASE(test_add_positive_numbers) {
    TEST_ASSERT_EQ(add(2, 3), 5);
    TEST_ASSERT_EQ(add(10, 20), 30);
    TEST_ASSERT_EQ(add(1, 1), 2);
}

TEST_CASE(test_add_negative_numbers) {
    TEST_ASSERT_EQ(add(-2, -3), -5);
    TEST_ASSERT_EQ(add(-10, 5), -5);
    TEST_ASSERT_EQ(add(10, -5), 5);
}

TEST_CASE(test_multiply) {
    TEST_ASSERT_EQ(multiply(2, 3), 6);
    TEST_ASSERT_EQ(multiply(4, 5), 20);
    TEST_ASSERT_EQ(multiply(0, 10), 0);
}

TEST_CASE(test_string_operations) {
    const char* greeting = get_greeting();
    TEST_ASSERT_NOT_NULL(greeting);
    TEST_ASSERT_STR_EQ(greeting, "Hello, World!");
}

TEST_CASE(test_boolean_conditions) {
    TEST_ASSERT(1 == 1);
    TEST_ASSERT(2 > 1);
    TEST_ASSERT(add(2, 2) == 4);
}

TEST_CASE(test_null_pointers) {
    char* null_ptr     = NULL;
    char* non_null_ptr = "test";

    TEST_ASSERT_NULL(null_ptr);
    TEST_ASSERT_NOT_NULL(non_null_ptr);
}

// This test will fail to demonstrate failure output
TEST_CASE(test_intentional_failure) {
    TEST_ASSERT_EQ(add(2, 2), 5);         // This should fail
    TEST_ASSERT_STR_EQ("hello", "world"); // This should also fail
}

TEST_SUITE_BEGIN()
RUN_TEST(test_add_positive_numbers);
RUN_TEST(test_add_negative_numbers);
RUN_TEST(test_multiply);
RUN_TEST(test_string_operations);
RUN_TEST(test_boolean_conditions);
RUN_TEST(test_null_pointers);
RUN_TEST(test_intentional_failure);
TEST_SUITE_END()
