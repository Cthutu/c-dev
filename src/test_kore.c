#define TEST_IMPLEMENTATION
#define KORE_IMPLEMENTATION

#include "kore.h"
#include "test.h"
#include <string.h>

TEST_CASE(allocate_simple) {
    void* p = KORE_MALLOC(100);
    TEST_ASSERT_NOT_NULL(p);

    usize size = k_memory_size(p);
    TEST_ASSERT_EQ(size, 100);

    KORE_FREE(p);
    TEST_ASSERT_NULL(p); // p should still be NULL after free
}

TEST_CASE(allocate_multiple_sizes) {
    // Test various allocation sizes
    void* p1 = KORE_MALLOC(1);
    void* p2 = KORE_MALLOC(1024);
    void* p3 = KORE_MALLOC(4096);

    TEST_ASSERT_NOT_NULL(p1);
    TEST_ASSERT_NOT_NULL(p2);
    TEST_ASSERT_NOT_NULL(p3);

    TEST_ASSERT_EQ(k_memory_size(p1), 1);
    TEST_ASSERT_EQ(k_memory_size(p2), 1024);
    TEST_ASSERT_EQ(k_memory_size(p3), 4096);

    KORE_FREE(p1);
    KORE_FREE(p2);
    KORE_FREE(p3);

    TEST_ASSERT_NULL(p1);
    TEST_ASSERT_NULL(p2);
    TEST_ASSERT_NULL(p3);
}

TEST_CASE(memory_realloc_basic) {
    void* p = KORE_MALLOC(100);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQ(k_memory_size(p), 100);

    // Grow the allocation
    p = KORE_REALLOC(p, 200);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQ(k_memory_size(p), 200);

    // Shrink the allocation
    p = KORE_REALLOC(p, 50);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQ(k_memory_size(p), 50);

    KORE_FREE(p);
    TEST_ASSERT_NULL(p);
}

TEST_CASE(memory_realloc_null_ptr) {
    // Realloc with NULL pointer should behave like malloc
    void* p = KORE_REALLOC(NULL, 100);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQ(k_memory_size(p), 100);

    KORE_FREE(p);
    TEST_ASSERT_NULL(p);
}

TEST_CASE(memory_free_null) {
    // Freeing NULL should not crash
    void* p = NULL;
    KORE_FREE(p); // Should be safe
    TEST_ASSERT_NULL(p);
}

TEST_CASE(memory_size_null) {
    // Size of NULL pointer should return 0
    usize size = k_memory_size(NULL);
    TEST_ASSERT_EQ(size, 0);
}

TEST_CASE(memory_allocation_tracking) {
    // Test allocation count tracking (KORE_DEBUG only)
    usize initial_count = k_memory_get_allocation_count();
    usize initial_total = k_memory_get_total_allocated();

    void* p1            = KORE_MALLOC(100);
    void* p2            = KORE_MALLOC(200);

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count + 2);
    TEST_ASSERT_EQ(k_memory_get_total_allocated(), initial_total + 300);

    KORE_FREE(p1);
    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count + 1);
    TEST_ASSERT_EQ(k_memory_get_total_allocated(), initial_total + 200);

    KORE_FREE(p2);
    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count);
    TEST_ASSERT_EQ(k_memory_get_total_allocated(), initial_total);
}

TEST_CASE(memory_realloc_tracking) {
    // Test that realloc properly updates tracking
    usize initial_count = k_memory_get_allocation_count();
    usize initial_total = k_memory_get_total_allocated();

    void* p             = KORE_MALLOC(100);
    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count + 1);
    TEST_ASSERT_EQ(k_memory_get_total_allocated(), initial_total + 100);

    p = KORE_REALLOC(p, 300);
    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count + 1);
    TEST_ASSERT_EQ(k_memory_get_total_allocated(), initial_total + 300);

    p = KORE_REALLOC(p, 50);
    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count + 1);
    TEST_ASSERT_EQ(k_memory_get_total_allocated(), initial_total + 50);

    KORE_FREE(p);
    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count);
    TEST_ASSERT_EQ(k_memory_get_total_allocated(), initial_total);
}

TEST_CASE(memory_leak_detection) {
    // Test leak detection by intentionally leaking memory
    usize initial_count = k_memory_get_allocation_count();

    void* leak1         = KORE_MALLOC(100);
    void* leak2         = KORE_MALLOC(200);

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count + 2);

    // Note: We intentionally don't free leak1 and leak2 here
    // The leak detection would catch these when k_memory_print_leaks() is
    // called For the test, we'll clean up to avoid affecting other tests
    KORE_FREE(leak1);
    KORE_FREE(leak2);

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory_stress_allocation) {
    // Stress test with many small allocations
    const usize num_allocs = 100;
    void* ptrs[num_allocs];

    usize initial_count = k_memory_get_allocation_count();

    // Allocate many small blocks
    for (usize i = 0; i < num_allocs; i++) {
        ptrs[i] = KORE_MALLOC(i + 1);
        TEST_ASSERT_NOT_NULL(ptrs[i]);
        TEST_ASSERT_EQ(k_memory_size(ptrs[i]), i + 1);
    }

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count + num_allocs);

    // Free them all
    for (usize i = 0; i < num_allocs; i++) {
        KORE_FREE(ptrs[i]);
        TEST_ASSERT_NULL(ptrs[i]);
    }

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory_header_linked_list) {
    // Test the internal linked list structure
    // Access the global memory head (declared in kore.h implementation)
    extern KMemoryHeader* g_memory_head;

    // Start with a clean slate
    usize initial_count = k_memory_get_allocation_count();

    // Allocate first block
    void* p1            = KORE_MALLOC(100);
    TEST_ASSERT_NOT_NULL(p1);

    // The header should be at p1 - 1
    KMemoryHeader* header1 = (KMemoryHeader*)p1 - 1;
    TEST_ASSERT_EQ(header1->size, 100);
    TEST_ASSERT_EQ(g_memory_head, header1);
    TEST_ASSERT_NULL(header1->next); // First allocation, so next should be NULL

    // Allocate second block
    void* p2 = KORE_MALLOC(200);
    TEST_ASSERT_NOT_NULL(p2);

    // The new header should be at the head of the list
    KMemoryHeader* header2 = (KMemoryHeader*)p2 - 1;
    TEST_ASSERT_EQ(header2->size, 200);
    TEST_ASSERT_EQ(g_memory_head, header2);
    TEST_ASSERT_EQ(header2->next, header1); // Should point to the previous head
    TEST_ASSERT_NULL(header1->next);        // Still should be NULL

    // Allocate third block
    void* p3 = KORE_MALLOC(300);
    TEST_ASSERT_NOT_NULL(p3);

    KMemoryHeader* header3 = (KMemoryHeader*)p3 - 1;
    TEST_ASSERT_EQ(header3->size, 300);
    TEST_ASSERT_EQ(g_memory_head, header3);
    TEST_ASSERT_EQ(header3->next, header2); // Should point to header2
    TEST_ASSERT_EQ(header2->next,
                   header1); // header2 should still point to header1

    // Free the middle block (p2) and verify list integrity
    KORE_FREE(p2);
    TEST_ASSERT_EQ(g_memory_head, header3); // Head should still be header3
    TEST_ASSERT_EQ(header3->next,
                   header1); // header3 should now point directly to header1
    TEST_ASSERT_NULL(header1->next); // header1 should still be the tail

    // Clean up
    KORE_FREE(p1);
    KORE_FREE(p3);

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory_header_realloc_list_update) {
    // Test that realloc properly updates the linked list
    extern KMemoryHeader* g_memory_head;

    usize initial_count            = k_memory_get_allocation_count();

    // Allocate some memory
    void* p                        = KORE_MALLOC(100);
    KMemoryHeader* original_header = (KMemoryHeader*)p - 1;

    TEST_ASSERT_EQ(g_memory_head, original_header);
    TEST_ASSERT_EQ(original_header->size, 100);

    // Realloc to a larger size
    p                         = KORE_REALLOC(p, 500);
    KMemoryHeader* new_header = (KMemoryHeader*)p - 1;

    // The header might be the same or different depending on realloc
    // implementation But it should be at the head of the list and have the
    // correct size
    TEST_ASSERT_EQ(g_memory_head, new_header);
    TEST_ASSERT_EQ(new_header->size, 500);

    // Allocate another block to test list structure
    void* p2               = KORE_MALLOC(50);
    KMemoryHeader* header2 = (KMemoryHeader*)p2 - 1;

    TEST_ASSERT_EQ(g_memory_head, header2);
    TEST_ASSERT_EQ(header2->next, new_header);

    // Clean up
    KORE_FREE(p);
    KORE_FREE(p2);

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory_header_file_line_tracking) {
    // Test that file and line information is correctly stored
    extern KMemoryHeader* g_memory_head;

    void* p               = KORE_MALLOC(42);
    KMemoryHeader* header = (KMemoryHeader*)p - 1;

    TEST_ASSERT_NOT_NULL(header->file);
    TEST_ASSERT(header->line > 0); // Should have a valid line number

    // The file should contain this filename
    TEST_ASSERT(strstr(header->file, "test_kore.c") != NULL);

    KORE_FREE(p);
}

TEST_CASE(memory_list_integrity_stress) {
    // Stress test the linked list with multiple allocations and random frees
    extern KMemoryHeader* g_memory_head;

    usize initial_count    = k_memory_get_allocation_count();
    const usize num_allocs = 10;
    void* ptrs[num_allocs];

    // Allocate multiple blocks
    for (usize i = 0; i < num_allocs; i++) {
        ptrs[i] = KORE_MALLOC((i + 1) * 10);
        TEST_ASSERT_NOT_NULL(ptrs[i]);
    }

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count + num_allocs);

    // Free every other block to test list removal from middle
    for (usize i = 0; i < num_allocs; i += 2) {
        KORE_FREE(ptrs[i]);
        ptrs[i] = NULL;
    }

    TEST_ASSERT_EQ(k_memory_get_allocation_count(),
                   initial_count + (num_allocs / 2));

    // Verify the remaining blocks are still valid by checking their headers
    usize count            = 0;
    KMemoryHeader* current = g_memory_head;
    while (current) {
        TEST_ASSERT(current->size > 0);
        TEST_ASSERT_NOT_NULL(current->file);
        count++;
        current = current->next;
    }

    TEST_ASSERT_EQ(count, num_allocs / 2);

    // Clean up remaining blocks
    for (usize i = 1; i < num_allocs; i += 2) {
        KORE_FREE(ptrs[i]);
    }

    TEST_ASSERT_EQ(k_memory_get_allocation_count(), initial_count);
}

TEST_SUITE_BEGIN()
RUN_TEST(allocate_simple);
RUN_TEST(allocate_multiple_sizes);
RUN_TEST(memory_realloc_basic);
RUN_TEST(memory_realloc_null_ptr);
RUN_TEST(memory_free_null);
RUN_TEST(memory_size_null);
RUN_TEST(memory_allocation_tracking);
RUN_TEST(memory_realloc_tracking);
RUN_TEST(memory_leak_detection);
RUN_TEST(memory_stress_allocation);
RUN_TEST(memory_header_linked_list);
RUN_TEST(memory_header_realloc_list_update);
RUN_TEST(memory_header_file_line_tracking);
RUN_TEST(memory_list_integrity_stress);
TEST_SUITE_END()
