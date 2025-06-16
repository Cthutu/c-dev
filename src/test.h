#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI color codes for output
#define TEST_COLOR_RESET   "\033[0m"
#define TEST_COLOR_RED     "\033[31m"
#define TEST_COLOR_GREEN   "\033[32m"
#define TEST_COLOR_YELLOW  "\033[33m"
#define TEST_COLOR_BLUE    "\033[34m"
#define TEST_COLOR_MAGENTA "\033[35m"
#define TEST_COLOR_CYAN    "\033[36m"
#define TEST_COLOR_WHITE   "\033[37m"
#define TEST_COLOR_BOLD    "\033[1m"

// Test framework macros
#define TEST_ASSERT(condition) \
    do { \
        test_total_assertions++; \
        if (!(condition)) { \
            printf("  " TEST_COLOR_RED "✗ ASSERTION FAILED" TEST_COLOR_RESET " at %s:%d\n", __FILE__, __LINE__); \
            printf("    Expected: %s\n", #condition); \
            test_current_failures++; \
            test_total_failures++; \
        } else { \
            printf("  " TEST_COLOR_GREEN "✓" TEST_COLOR_RESET " %s\n", #condition); \
        } \
    } while(0)

#define TEST_ASSERT_EQ(a, b) \
    do { \
        test_total_assertions++; \
        if ((a) != (b)) { \
            printf("  " TEST_COLOR_RED "✗ ASSERTION FAILED" TEST_COLOR_RESET " at %s:%d\n", __FILE__, __LINE__); \
            printf("    Expected: %s == %s\n", #a, #b); \
            test_current_failures++; \
            test_total_failures++; \
        } else { \
            printf("  " TEST_COLOR_GREEN "✓" TEST_COLOR_RESET " %s == %s\n", #a, #b); \
        } \
    } while(0)

#define TEST_ASSERT_STR_EQ(a, b) \
    do { \
        test_total_assertions++; \
        if (strcmp((a), (b)) != 0) { \
            printf("  " TEST_COLOR_RED "✗ ASSERTION FAILED" TEST_COLOR_RESET " at %s:%d\n", __FILE__, __LINE__); \
            printf("    Expected: \"%s\" == \"%s\"\n", (a), (b)); \
            test_current_failures++; \
            test_total_failures++; \
        } else { \
            printf("  " TEST_COLOR_GREEN "✓" TEST_COLOR_RESET " \"%s\" == \"%s\"\n", (a), (b)); \
        } \
    } while(0)

#define TEST_ASSERT_NULL(ptr) \
    do { \
        test_total_assertions++; \
        if ((ptr) != NULL) { \
            printf("  " TEST_COLOR_RED "✗ ASSERTION FAILED" TEST_COLOR_RESET " at %s:%d\n", __FILE__, __LINE__); \
            printf("    Expected: %s to be NULL\n", #ptr); \
            test_current_failures++; \
            test_total_failures++; \
        } else { \
            printf("  " TEST_COLOR_GREEN "✓" TEST_COLOR_RESET " %s is NULL\n", #ptr); \
        } \
    } while(0)

#define TEST_ASSERT_NOT_NULL(ptr) \
    do { \
        test_total_assertions++; \
        if ((ptr) == NULL) { \
            printf("  " TEST_COLOR_RED "✗ ASSERTION FAILED" TEST_COLOR_RESET " at %s:%d\n", __FILE__, __LINE__); \
            printf("    Expected: %s to be non-NULL\n", #ptr); \
            test_current_failures++; \
            test_total_failures++; \
        } else { \
            printf("  " TEST_COLOR_GREEN "✓" TEST_COLOR_RESET " %s is not NULL\n", #ptr); \
        } \
    } while(0)

#define TEST_CASE(name) \
    void test_##name(void); \
    void test_##name(void)

#define RUN_TEST(name) \
    do { \
        test_current_failures = 0; \
        printf(TEST_COLOR_CYAN TEST_COLOR_BOLD "Running test: %s" TEST_COLOR_RESET "\n", #name); \
        test_##name(); \
        if (test_current_failures == 0) { \
            printf(TEST_COLOR_GREEN "✓ Test %s PASSED" TEST_COLOR_RESET "\n\n", #name); \
            test_passed_tests++; \
        } else { \
            printf(TEST_COLOR_RED "✗ Test %s FAILED (%d assertions failed)" TEST_COLOR_RESET "\n\n", #name, test_current_failures); \
            test_failed_tests++; \
        } \
        test_total_tests++; \
    } while(0)

#define TEST_SUITE_BEGIN() \
    int main(void) { \
        test_init(); \
        printf(TEST_COLOR_BOLD TEST_COLOR_BLUE "=== Test Suite Started ===" TEST_COLOR_RESET "\n\n");

#define TEST_SUITE_END() \
        test_summary(); \
        return test_failed_tests > 0 ? 1 : 0; \
    }

// Function declarations
void test_init(void);
void test_summary(void);

// Global test counters (declared here, defined in implementation)
extern int test_total_tests;
extern int test_passed_tests;
extern int test_failed_tests;
extern int test_total_assertions;
extern int test_total_failures;
extern int test_current_failures;

#ifdef TEST_IMPLEMENTATION

// Global test counters
int test_total_tests = 0;
int test_passed_tests = 0;
int test_failed_tests = 0;
int test_total_assertions = 0;
int test_total_failures = 0;
int test_current_failures = 0;

void test_init(void) {
    test_total_tests = 0;
    test_passed_tests = 0;
    test_failed_tests = 0;
    test_total_assertions = 0;
    test_total_failures = 0;
    test_current_failures = 0;
}

void test_summary(void) {
    printf(TEST_COLOR_BOLD TEST_COLOR_BLUE "=== Test Suite Summary ===" TEST_COLOR_RESET "\n");
    printf("Total tests run: %d\n", test_total_tests);
    
    if (test_passed_tests > 0) {
        printf(TEST_COLOR_GREEN "Tests passed: %d" TEST_COLOR_RESET "\n", test_passed_tests);
    }
    
    if (test_failed_tests > 0) {
        printf(TEST_COLOR_RED "Tests failed: %d" TEST_COLOR_RESET "\n", test_failed_tests);
    }
    
    printf("Total assertions: %d\n", test_total_assertions);
    
    if (test_total_failures > 0) {
        printf(TEST_COLOR_RED "Failed assertions: %d" TEST_COLOR_RESET "\n", test_total_failures);
    }
    
    printf("\n");
    
    if (test_failed_tests == 0) {
        printf(TEST_COLOR_GREEN TEST_COLOR_BOLD "🎉 ALL TESTS PASSED! 🎉" TEST_COLOR_RESET "\n");
    } else {
        printf(TEST_COLOR_RED TEST_COLOR_BOLD "❌ SOME TESTS FAILED ❌" TEST_COLOR_RESET "\n");
    }
    
    printf(TEST_COLOR_BOLD TEST_COLOR_BLUE "=========================" TEST_COLOR_RESET "\n");
}

#endif // TEST_IMPLEMENTATION


