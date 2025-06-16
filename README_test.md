# Test.h - Header-Only C Test Framework

A lightweight, header-only test framework for C with coloured output support using ANSI escape codes.

## Features

- Header-only implementation (single file: `test.h`)
- Colourised output with ANSI escape codes
- Multiple assertion types
- Test case organization
- Automatic test statistics and summary
- Cross-platform compatible

## Usage

### Basic Setup

1. Include the header in your test file:
```c
#define TEST_IMPLEMENTATION  // Define this in exactly ONE source file
#include "test.h"
```

2. Write test cases using the `TEST_CASE` macro:
```c
TEST_CASE(my_test_name) {
    TEST_ASSERT_EQ(2 + 2, 4);
    TEST_ASSERT(1 < 2);
}
```

3. Create a test suite:
```c
TEST_SUITE_BEGIN()
    RUN_TEST(my_test_name);
TEST_SUITE_END()
```

### Available Assertions

- `TEST_ASSERT(condition)` - Assert that a condition is true
- `TEST_ASSERT_EQ(a, b)` - Assert that two values are equal
- `TEST_ASSERT_STR_EQ(a, b)` - Assert that two strings are equal
- `TEST_ASSERT_NULL(ptr)` - Assert that a pointer is NULL
- `TEST_ASSERT_NOT_NULL(ptr)` - Assert that a pointer is not NULL

### Complete Example

```c
#define TEST_IMPLEMENTATION
#include "test.h"

// Function to test
int add(int a, int b) {
    return a + b;
}

// Test case
TEST_CASE(test_addition) {
    TEST_ASSERT_EQ(add(2, 3), 5);
    TEST_ASSERT_EQ(add(-1, 1), 0);
}

// Test suite
TEST_SUITE_BEGIN()
    RUN_TEST(test_addition);
TEST_SUITE_END()
```

### Compilation

Compile your test file like any other C program:
```bash
clang -o test_program test_file.c -std=c23
./test_program
```

### Output

The framework provides colorized output:
- 🟢 Green for passing tests and assertions
- 🔴 Red for failing tests and assertions  
- 🔵 Blue for test suite headers
- 🟡 Yellow for warnings (if any)

### Return Codes

The test program returns:
- `0` if all tests pass
- `1` if any tests fail

This makes it suitable for use in CI/CD pipelines and build systems.

## Color Codes

The framework uses the following ANSI color codes:
- `TEST_COLOR_RED` - Red text
- `TEST_COLOR_GREEN` - Green text
- `TEST_COLOR_BLUE` - Blue text
- `TEST_COLOR_YELLOW` - Yellow text
- `TEST_COLOR_CYAN` - Cyan text
- `TEST_COLOR_MAGENTA` - Magenta text
- `TEST_COLOR_WHITE` - White text
- `TEST_COLOR_BOLD` - Bold text
- `TEST_COLOR_RESET` - Reset to default

You can use these in your own output if needed.

## License

This is a simple header-only library. Feel free to use it in your projects.
