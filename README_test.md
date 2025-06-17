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

### Output Verbosity

The test framework supports two output modes:

- **Quiet mode (default)**: Only shows test names for passing tests and full details for failing tests
- **Verbose mode**: Shows all assertion details for both passing and failing tests

#### Enabling Verbose Mode

There are two ways to enable verbose mode:

**1. Environment Variable (Recommended)**
Set the `TEST_VERBOSE` environment variable before running your tests:

```bash
# Enable verbose mode
TEST_VERBOSE=1 ./test_program

# Alternative values that enable verbose mode
TEST_VERBOSE=true ./test_program
TEST_VERBOSE=yes ./test_program

# Disable verbose mode (default)
TEST_VERBOSE=0 ./test_program
# or simply don't set the variable
./test_program
```

**2. Compile-time Define (Legacy)**
Define `TEST_VERBOSE` as `1` before including the header:

```c
#define TEST_VERBOSE 1
#define TEST_IMPLEMENTATION
#include "test.h"
```

The environment variable takes precedence over the compile-time define if both are present.

#### Quiet Mode Output (Default)
```
=== Test Suite Started ===
✓ test_addition_success
✓ test_multiplication_success
  ✗ ASSERTION FAILED at test_file.c:30
    Expected: buggy_divide(10, 2) == 5
✗ test_division_failure FAILED (1 assertions failed)
```

#### Verbose Mode Output
```
=== Test Suite Started ===
Running test: test_addition_success
  ✓ add(2, 3) == 5
  ✓ add(-1, 1) == 0
✓ test_addition_success
Running test: test_division_failure
  ✗ ASSERTION FAILED at test_file.c:30
    Expected: buggy_divide(10, 2) == 5
✗ test_division_failure FAILED (1 assertions failed)
```

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

# Run with default quiet mode
./test_program

# Run with verbose mode
TEST_VERBOSE=1 ./test_program
```

### Output

The framework provides colourised output with two verbosity modes:

#### Quiet Mode (Default)
- 🟢 Green checkmark and test name for passing tests
- 🔴 Red details for failing assertions and test summary
- 🔵 Blue for test suite headers
- Much cleaner output that makes failures easy to spot

#### Verbose Mode  
- Shows all assertion details like the original behaviour
- Useful for debugging or when you need to see every assertion
- Enable with environment variable `TEST_VERBOSE=1` or compile-time `#define TEST_VERBOSE 1`

The framework uses the following colours:
- 🟢 Green for passing tests and assertions
- 🔴 Red for failing tests and assertions  
- 🔵 Blue for test suite headers
- 🟡 Yellow for warnings (if any)

### Return Codes

The test program returns:
- `0` if all tests pass
- `1` if any tests fail

This makes it suitable for use in CI/CD pipelines and build systems.

## Colour Codes

The framework uses the following ANSI colour codes:
- `TEST_COLOUR_RED` - Red text
- `TEST_COLOUR_GREEN` - Green text
- `TEST_COLOUR_BLUE` - Blue text
- `TEST_COLOUR_YELLOW` - Yellow text
- `TEST_COLOUR_CYAN` - Cyan text
- `TEST_COLOUR_MAGENTA` - Magenta text
- `TEST_COLOUR_WHITE` - White text
- `TEST_COLOUR_BOLD` - Bold text
- `TEST_COLOUR_RESET` - Reset to default

You can use these in your own output if needed.

## Licence

This is a simple header-only library. Feel free to use it in your projects.
