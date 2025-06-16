#define TEST_IMPLEMENTATION
#define KORE_IMPLEMENTATION

#include "kore.h"
#include "test.h"

TEST_CASE(allocate_simple) {
    void* p = KORE_MALLOC(100);
    TEST_ASSERT_NOT_NULL(p);

    usize size = k_memory_size(p);
    TEST_ASSERT_EQ(size, 100);

    KORE_FREE(p);
    TEST_ASSERT_NULL(p); // p should still be NULL after free
}

TEST_SUITE_BEGIN()
RUN_TEST(allocate_simple);
TEST_SUITE_END()
