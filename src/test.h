#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI colour codes for output
#define TEST_COLOUR_RESET "\033[0m"
#define TEST_COLOUR_RED "\033[31m"
#define TEST_COLOUR_GREEN "\033[32m"
#define TEST_COLOUR_YELLOW "\033[33m"
#define TEST_COLOUR_BLUE "\033[34m"
#define TEST_COLOUR_MAGENTA "\033[35m"
#define TEST_COLOUR_CYAN "\033[36m"
#define TEST_COLOUR_WHITE "\033[37m"
#define TEST_COLOUR_BOLD "\033[1m"

// Test output verbosity control (can be overridden by TEST_VERBOSE environment
// variable)
#ifndef TEST_VERBOSE
#    define TEST_VERBOSE                                                       \
        0 // 0 = quiet (only show failures), 1 = verbose (show all assertions)
#endif

// Test framework macros
#define TEST_ASSERT(condition)                                                 \
    do {                                                                       \
        test_total_assertions++;                                               \
        if (!(condition)) {                                                    \
            printf("  " TEST_COLOUR_RED "✗ ASSERTION FAILED" TEST_COLOUR_RESET \
                   " at %s:%d\n",                                              \
                   __FILE__,                                                   \
                   __LINE__);                                                  \
            printf("    Expected: %s\n", #condition);                          \
            test_current_failures++;                                           \
            test_total_failures++;                                             \
        } else if (test_verbose_output) {                                      \
            printf("  " TEST_COLOUR_GREEN "✓" TEST_COLOUR_RESET " %s\n",       \
                   #condition);                                                \
        }                                                                      \
    } while (0)

#define TEST_ASSERT_EQ(a, b)                                                   \
    do {                                                                       \
        test_total_assertions++;                                               \
        if ((a) != (b)) {                                                      \
            printf("  " TEST_COLOUR_RED "✗ ASSERTION FAILED" TEST_COLOUR_RESET \
                   " at %s:%d\n",                                              \
                   __FILE__,                                                   \
                   __LINE__);                                                  \
            printf("    Expected: %s == %s\n", #a, #b);                        \
            test_current_failures++;                                           \
            test_total_failures++;                                             \
        } else if (test_verbose_output) {                                      \
            printf("  " TEST_COLOUR_GREEN "✓" TEST_COLOUR_RESET " %s == %s\n", \
                   #a,                                                         \
                   #b);                                                        \
        }                                                                      \
    } while (0)

#define TEST_ASSERT_STR_EQ(a, b)                                               \
    do {                                                                       \
        test_total_assertions++;                                               \
        if (strcmp((a), (b)) != 0) {                                           \
            printf("  " TEST_COLOUR_RED "✗ ASSERTION FAILED" TEST_COLOUR_RESET \
                   " at %s:%d\n",                                              \
                   __FILE__,                                                   \
                   __LINE__);                                                  \
            printf("    Expected: \"%s\" == \"%s\"\n", (a), (b));              \
            test_current_failures++;                                           \
            test_total_failures++;                                             \
        } else if (test_verbose_output) {                                      \
            printf("  " TEST_COLOUR_GREEN "✓" TEST_COLOUR_RESET                \
                   " \"%s\" == \"%s\"\n",                                      \
                   (a),                                                        \
                   (b));                                                       \
        }                                                                      \
    } while (0)

#define TEST_ASSERT_NULL(ptr)                                                  \
    do {                                                                       \
        test_total_assertions++;                                               \
        if ((ptr) != NULL) {                                                   \
            printf("  " TEST_COLOUR_RED "✗ ASSERTION FAILED" TEST_COLOUR_RESET \
                   " at %s:%d\n",                                              \
                   __FILE__,                                                   \
                   __LINE__);                                                  \
            printf("    Expected: %s to be NULL\n", #ptr);                     \
            test_current_failures++;                                           \
            test_total_failures++;                                             \
        } else if (test_verbose_output) {                                      \
            printf("  " TEST_COLOUR_GREEN "✓" TEST_COLOUR_RESET                \
                   " %s is NULL\n",                                            \
                   #ptr);                                                      \
        }                                                                      \
    } while (0)

#define TEST_ASSERT_NOT_NULL(ptr)                                              \
    do {                                                                       \
        test_total_assertions++;                                               \
        if ((ptr) == NULL) {                                                   \
            printf("  " TEST_COLOUR_RED "✗ ASSERTION FAILED" TEST_COLOUR_RESET \
                   " at %s:%d\n",                                              \
                   __FILE__,                                                   \
                   __LINE__);                                                  \
            printf("    Expected: %s to be non-NULL\n", #ptr);                 \
            test_current_failures++;                                           \
            test_total_failures++;                                             \
        } else if (test_verbose_output) {                                      \
            printf("  " TEST_COLOUR_GREEN "✓" TEST_COLOUR_RESET                \
                   " %s is not NULL\n",                                        \
                   #ptr);                                                      \
        }                                                                      \
    } while (0)

#define TEST_CASE(name)                                                        \
    void test_##name(void);                                                    \
    void test_##name(void)

#define RUN_TEST(name)                                                         \
    do {                                                                       \
        test_current_failures = 0;                                             \
        if (test_verbose_output) {                                             \
            printf(TEST_COLOUR_CYAN TEST_COLOUR_BOLD                           \
                   "Running test: %s" TEST_COLOUR_RESET "\n",                  \
                   #name);                                                     \
        }                                                                      \
        test_##name();                                                         \
        if (test_current_failures == 0) {                                      \
            printf(TEST_COLOUR_GREEN "✓" TEST_COLOUR_RESET " %s\n", #name);    \
            test_passed_tests++;                                               \
        } else {                                                               \
            printf(TEST_COLOUR_RED                                             \
                   "✗ %s FAILED (%d assertions failed)" TEST_COLOUR_RESET      \
                   "\n",                                                       \
                   #name,                                                      \
                   test_current_failures);                                     \
            test_failed_tests++;                                               \
        }                                                                      \
        test_total_tests++;                                                    \
    } while (0)

#define TEST_SUITE_BEGIN()                                                     \
    int main(void) {                                                           \
        test_init();                                                           \
        printf(TEST_COLOUR_BOLD TEST_COLOUR_BLUE                               \
               "=== Test Suite Started ===" TEST_COLOUR_RESET "\n");

#define TEST_SUITE_END()                                                       \
    printf("\n");                                                              \
    test_summary();                                                            \
    return test_failed_tests > 0 ? 1 : 0;                                      \
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

// Global verbosity flag (set at runtime from environment variable)
extern int test_verbose_output;

#ifdef TEST_IMPLEMENTATION

// Global test counters
int test_total_tests      = 0;
int test_passed_tests     = 0;
int test_failed_tests     = 0;
int test_total_assertions = 0;
int test_total_failures   = 0;
int test_current_failures = 0;

// Global verbosity flag (set at runtime from environment variable)
int test_verbose_output   = TEST_VERBOSE;

void test_init(void) {
    test_total_tests      = 0;
    test_passed_tests     = 0;
    test_failed_tests     = 0;
    test_total_assertions = 0;
    test_total_failures   = 0;
    test_current_failures = 0;

    // Check for TEST_VERBOSE environment variable
#    ifdef _MSC_VER
#        pragma warning(push)
#        pragma warning(disable : 4996) // Suppress deprecated function warning
#    endif
    const char* verbose_env = getenv("TEST_VERBOSE");
#    ifdef _MSC_VER
#        pragma warning(pop)
#    endif
    if (verbose_env != NULL) {
        test_verbose_output = (strcmp(verbose_env, "1") == 0 ||
                               strcmp(verbose_env, "true") == 0 ||
                               strcmp(verbose_env, "yes") == 0);
    } else {
        // Fall back to compile-time default
        test_verbose_output = TEST_VERBOSE;
    }
}

void test_summary(void) {
    // Calculate column widths for proper alignment
    const int label_width = 20;
    const int value_width = 10;
    const int total_width =
        label_width + value_width + 1; // +1 for separator inside

    // Top border
    printf(TEST_COLOUR_BOLD TEST_COLOUR_BLUE "┌");
    for (int i = 0; i < total_width; i++) {
        printf("─");
    }
    printf("┐" TEST_COLOUR_RESET "\n");

    // Header
    printf(TEST_COLOUR_BOLD TEST_COLOUR_BLUE "│");
    int header_padding =
        (total_width - 18) / 2; // "Test Suite Summary" is 18 chars
    for (int i = 0; i < header_padding; i++) {
        printf(" ");
    }
    printf("Test Suite Summary");
    for (int i = 0; i < total_width - header_padding - 18; i++) {
        printf(" ");
    }
    printf("│" TEST_COLOUR_RESET "\n");

    // Separator
    printf(TEST_COLOUR_BOLD TEST_COLOUR_BLUE "├");
    for (int i = 0; i < label_width; i++) {
        printf("─");
    }
    printf("┬");
    for (int i = 0; i < value_width; i++) {
        printf("─");
    }
    printf("┤" TEST_COLOUR_RESET "\n");

    // Column headers
    printf(TEST_COLOUR_BOLD TEST_COLOUR_BLUE "│ %-*s │ %-*s │" TEST_COLOUR_RESET
                                             "\n",
           label_width - 2,
           "Metric",
           value_width - 2,
           "Value");

    // Separator
    printf(TEST_COLOUR_BOLD TEST_COLOUR_BLUE "├");
    for (int i = 0; i < label_width; i++) {
        printf("─");
    }
    printf("┼");
    for (int i = 0; i < value_width; i++) {
        printf("─");
    }
    printf("┤" TEST_COLOUR_RESET "\n");

    // Data rows
    printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET " %-*s " TEST_COLOUR_BLUE
                            "│" TEST_COLOUR_RESET " %*d " TEST_COLOUR_BLUE
                            "│" TEST_COLOUR_RESET "\n",
           label_width - 2,
           "Total tests run",
           value_width - 2,
           test_total_tests);

    printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET " %-*s " TEST_COLOUR_BLUE
                            "│" TEST_COLOUR_RESET " " TEST_COLOUR_GREEN
                            "%*d" TEST_COLOUR_RESET " " TEST_COLOUR_BLUE
                            "│" TEST_COLOUR_RESET "\n",
           label_width - 2,
           "Tests passed",
           value_width - 2,
           test_passed_tests);

    if (test_failed_tests > 0) {
        printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET " %-*s " TEST_COLOUR_BLUE
                                "│" TEST_COLOUR_RESET " " TEST_COLOUR_RED
                                "%*d" TEST_COLOUR_RESET " " TEST_COLOUR_BLUE
                                "│" TEST_COLOUR_RESET "\n",
               label_width - 2,
               "Tests failed",
               value_width - 2,
               test_failed_tests);
    }

    printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET " %-*s " TEST_COLOUR_BLUE
                            "│" TEST_COLOUR_RESET " %*d " TEST_COLOUR_BLUE
                            "│" TEST_COLOUR_RESET "\n",
           label_width - 2,
           "Total assertions",
           value_width - 2,
           test_total_assertions);

    if (test_total_failures > 0) {
        printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET " %-*s " TEST_COLOUR_BLUE
                                "│" TEST_COLOUR_RESET " " TEST_COLOUR_RED
                                "%*d" TEST_COLOUR_RESET " " TEST_COLOUR_BLUE
                                "│" TEST_COLOUR_RESET "\n",
               label_width - 2,
               "Failed assertions",
               value_width - 2,
               test_total_failures);
    }

    // Status section separator
    printf(TEST_COLOUR_BOLD TEST_COLOUR_BLUE "├");
    for (int i = 0; i < label_width; i++) {
        printf("─");
    }
    printf("┴");
    for (int i = 0; i < value_width; i++) {
        printf("─");
    }
    printf("┤" TEST_COLOUR_RESET "\n");

    // Status message
    if (test_failed_tests == 0) {
        const char* status_msg = "🎉 ALL TESTS PASSED! 🎉";
        int status_len =
            23; // Length without emoji (emojis count as different widths)
        int status_padding_left = (total_width - status_len) / 2;
        int status_padding_right =
            total_width - status_len - status_padding_left;

        printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET);
        for (int i = 0; i < status_padding_left; i++) {
            printf(" ");
        }
        printf(TEST_COLOUR_GREEN TEST_COLOUR_BOLD "%s" TEST_COLOUR_RESET,
               status_msg);
        for (int i = 0; i < status_padding_right; i++) {
            printf(" ");
        }
        printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET "\n");
    } else {
        const char* status_msg  = "❌ SOME TESTS FAILED ❌";
        int status_len          = 23; // Length without emoji
        int status_padding_left = (total_width - status_len) / 2;
        int status_padding_right =
            total_width - status_len - status_padding_left;

        printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET);
        for (int i = 0; i < status_padding_left; i++) {
            printf(" ");
        }
        printf(TEST_COLOUR_RED TEST_COLOUR_BOLD "%s" TEST_COLOUR_RESET,
               status_msg);
        for (int i = 0; i < status_padding_right; i++) {
            printf(" ");
        }
        printf(TEST_COLOUR_BLUE "│" TEST_COLOUR_RESET "\n");
    }

    // Bottom border
    printf(TEST_COLOUR_BOLD TEST_COLOUR_BLUE "└");
    for (int i = 0; i < total_width; i++) {
        printf("─");
    }
    printf("┘" TEST_COLOUR_RESET "\n");
}

#endif // TEST_IMPLEMENTATION
