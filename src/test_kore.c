#define TEST_IMPLEMENTATION
#define KORE_IMPLEMENTATION

#include "kore.h"
#include "test.h"
#include <string.h>

TEST_CASE(memory, simple) {
    void* p = KORE_MALLOC(100);
    TEST_ASSERT_NOT_NULL(p);

    usize size = memory_size(p);
    TEST_ASSERT_EQ(size, 100);

    KORE_FREE(p);
    TEST_ASSERT_NULL(p); // p should still be NULL after free
}

TEST_CASE(memory, multiple_sizes) {
    // Test various allocation sizes
    void* p1 = KORE_MALLOC(1);
    void* p2 = KORE_MALLOC(1024);
    void* p3 = KORE_MALLOC(4096);

    TEST_ASSERT_NOT_NULL(p1);
    TEST_ASSERT_NOT_NULL(p2);
    TEST_ASSERT_NOT_NULL(p3);

    TEST_ASSERT_EQ(memory_size(p1), 1);
    TEST_ASSERT_EQ(memory_size(p2), 1024);
    TEST_ASSERT_EQ(memory_size(p3), 4096);

    KORE_FREE(p1);
    KORE_FREE(p2);
    KORE_FREE(p3);

    TEST_ASSERT_NULL(p1);
    TEST_ASSERT_NULL(p2);
    TEST_ASSERT_NULL(p3);
}

TEST_CASE(memory, basic) {
    void* p = KORE_MALLOC(100);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQ(memory_size(p), 100);

    // Grow the allocation
    p = KORE_REALLOC(p, 200);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQ(memory_size(p), 200);

    // Shrink the allocation
    p = KORE_REALLOC(p, 50);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQ(memory_size(p), 50);

    KORE_FREE(p);
    TEST_ASSERT_NULL(p);
}

TEST_CASE(memory, null_ptr) {
    // Realloc with NULL pointer should behave like malloc
    void* p = KORE_REALLOC(NULL, 100);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQ(memory_size(p), 100);

    KORE_FREE(p);
    TEST_ASSERT_NULL(p);
}

TEST_CASE(memory, free_null) {
    // Freeing NULL should not crash
    void* p = NULL;
    KORE_FREE(p); // Should be safe
    TEST_ASSERT_NULL(p);
}

TEST_CASE(memory, size_null) {
    // Size of NULL pointer should return 0
    usize size = memory_size(NULL);
    TEST_ASSERT_EQ(size, 0);
}

TEST_CASE(memory, allocation_tracking) {
    // Test allocation count tracking (KORE_DEBUG only)
    usize initial_count = memory_get_allocation_count();
    usize initial_total = memory_get_total_allocated();

    void* p1            = KORE_MALLOC(100);
    void* p2            = KORE_MALLOC(200);

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 2);
    TEST_ASSERT_EQ(memory_get_total_allocated(), initial_total + 300);

    KORE_FREE(p1);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);
    TEST_ASSERT_EQ(memory_get_total_allocated(), initial_total + 200);

    KORE_FREE(p2);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
    TEST_ASSERT_EQ(memory_get_total_allocated(), initial_total);
}

TEST_CASE(memory, realloc_tracking) {
    // Test that realloc properly updates tracking
    usize initial_count = memory_get_allocation_count();
    usize initial_total = memory_get_total_allocated();

    void* p             = KORE_MALLOC(100);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);
    TEST_ASSERT_EQ(memory_get_total_allocated(), initial_total + 100);

    p = KORE_REALLOC(p, 300);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);
    TEST_ASSERT_EQ(memory_get_total_allocated(), initial_total + 300);

    p = KORE_REALLOC(p, 50);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);
    TEST_ASSERT_EQ(memory_get_total_allocated(), initial_total + 50);

    KORE_FREE(p);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
    TEST_ASSERT_EQ(memory_get_total_allocated(), initial_total);
}

TEST_CASE(memory, basic_detection) {
    // Test leak detection by intentionally leaking memory
    usize initial_count = memory_get_allocation_count();

    void* leak1         = KORE_MALLOC(100);
    void* leak2         = KORE_MALLOC(200);

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 2);

    // Note: We intentionally don't free leak1 and leak2 here
    // The leak detection would catch these when memory_print_leaks() is
    // called For the test, we'll clean up to avoid affecting other tests
    KORE_FREE(leak1);
    KORE_FREE(leak2);

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory, many_allocations) {
    // Stress test with many small allocations
    const usize num_allocs = 100;
    void* ptrs[num_allocs];

    usize initial_count = memory_get_allocation_count();

    // Allocate many small blocks
    for (usize i = 0; i < num_allocs; i++) {
        ptrs[i] = KORE_MALLOC(i + 1);
        TEST_ASSERT_NOT_NULL(ptrs[i]);
        TEST_ASSERT_EQ(memory_size(ptrs[i]), i + 1);
    }

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + num_allocs);

    // Free them all
    for (usize i = 0; i < num_allocs; i++) {
        KORE_FREE(ptrs[i]);
        TEST_ASSERT_NULL(ptrs[i]);
    }

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory, basic_structure) {
    // Test the internal linked list structure
    // Access the global memory head (declared in kore.h implementation)
    extern KMemoryHeader* g_memory_head;

    // Start with a clean slate
    usize initial_count = memory_get_allocation_count();

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

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory, realloc_list_update) {
    // Test that realloc properly updates the linked list
    extern KMemoryHeader* g_memory_head;

    usize initial_count            = memory_get_allocation_count();

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

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory, file_line_tracking) {
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

TEST_CASE(memory, integrity_stress) {
    // Stress test the linked list with multiple allocations and random frees
    extern KMemoryHeader* g_memory_head;

    usize initial_count    = memory_get_allocation_count();
    const usize num_allocs = 10;
    void* ptrs[num_allocs];

    // Allocate multiple blocks
    for (usize i = 0; i < num_allocs; i++) {
        ptrs[i] = KORE_MALLOC((i + 1) * 10);
        TEST_ASSERT_NOT_NULL(ptrs[i]);
    }

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + num_allocs);

    // Free every other block to test list removal from middle
    for (usize i = 0; i < num_allocs; i += 2) {
        KORE_FREE(ptrs[i]);
        ptrs[i] = NULL;
    }

    TEST_ASSERT_EQ(memory_get_allocation_count(),
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

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory, basic_leak_marking) {
    // Test basic leak marking functionality
    extern KMemoryHeader* g_memory_head;

    usize initial_count = memory_get_allocation_count();

    // Allocate some memory
    void* p1            = KORE_MALLOC(100);
    void* p2            = KORE_MALLOC(200);

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 2);

    // Mark p1 as leaked
    memory_leak(p1);

    // p1 should be removed from the linked list, so allocation count should
    // decrease
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);

    // Verify p1's header is not in the linked list anymore
    KMemoryHeader* current   = g_memory_head;
    KMemoryHeader* p1_header = (KMemoryHeader*)p1 - 1;
    bool found_p1            = false;

    while (current) {
        if (current == p1_header) {
            found_p1 = true;
            break;
        }
        current = current->next;
    }

    TEST_ASSERT(!found_p1); // p1 should not be in the list

    // p2 should still be in the list
    current                  = g_memory_head;
    KMemoryHeader* p2_header = (KMemoryHeader*)p2 - 1;
    bool found_p2            = false;

    while (current) {
        if (current == p2_header) {
            found_p2 = true;
            break;
        }
        current = current->next;
    }

    TEST_ASSERT(found_p2); // p2 should be in the list

    // Clean up p2 (p1 is leaked intentionally)
    KORE_FREE(p2);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory, realloc_preserves_flag) {
    // Test that realloc preserves the leaked flag
    extern KMemoryHeader* g_memory_head;

    usize initial_count = memory_get_allocation_count();

    // Allocate memory
    void* p1            = KORE_MALLOC(100);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);

    // Mark as leaked
    memory_leak(p1);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);

    // Realloc the leaked memory
    void* p2 = KORE_REALLOC(p1, 200);

    // Should still be leaked (not in the linked list)
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);

    // Verify the reallocated memory is not in the linked list
    KMemoryHeader* current   = g_memory_head;
    KMemoryHeader* p2_header = (KMemoryHeader*)p2 - 1;
    bool found_p2            = false;

    while (current) {
        if (current == p2_header) {
            found_p2 = true;
            break;
        }
        current = current->next;
    }

    TEST_ASSERT(!found_p2); // p2 should not be in the list (still leaked)

    // Verify the leaked flag is set
    TEST_ASSERT(p2_header->leaked);
}

TEST_CASE(memory, realloc_then_mark) {
    // Test marking as leaked after realloc
    extern KMemoryHeader* g_memory_head;

    usize initial_count = memory_get_allocation_count();

    // Allocate memory
    void* p1            = KORE_MALLOC(100);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);

    // Realloc first
    void* p2 = KORE_REALLOC(p1, 200);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);

    // Then mark as leaked
    memory_leak(p2);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);

    // Verify p2 is not in the linked list
    KMemoryHeader* current   = g_memory_head;
    KMemoryHeader* p2_header = (KMemoryHeader*)p2 - 1;
    bool found_p2            = false;

    while (current) {
        if (current == p2_header) {
            found_p2 = true;
            break;
        }
        current = current->next;
    }

    TEST_ASSERT(!found_p2);         // p2 should not be in the list
    TEST_ASSERT(p2_header->leaked); // Should be marked as leaked
}

TEST_CASE(memory, multiple_operations) {
    // Test complex scenario with multiple allocations, leaks, and reallocs
    extern KMemoryHeader* g_memory_head;

    usize initial_count = memory_get_allocation_count();

    // Allocate several blocks
    void* p1            = KORE_MALLOC(100);
    void* p2            = KORE_MALLOC(200);
    void* p3            = KORE_MALLOC(300);

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 3);

    // Mark p2 as leaked
    memory_leak(p2);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 2);

    // Realloc p1
    void* p1_new = KORE_REALLOC(p1, 150);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 2);

    // Realloc the leaked p2
    void* p2_new = KORE_REALLOC(p2, 250);
    TEST_ASSERT_EQ(memory_get_allocation_count(),
                   initial_count + 2); // Still leaked

    // Mark p3 as leaked
    memory_leak(p3);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);

    // Only p1_new should be in the linked list now
    usize count            = 0;
    KMemoryHeader* current = g_memory_head;
    while (current) {
        count++;
        current = current->next;
    }
    TEST_ASSERT_EQ(count, 1);

    // Clean up the non-leaked allocation
    KORE_FREE(p1_new);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);

    // Clean up the leaked allocations
    KORE_FREE(p2_new);
    KORE_FREE(p3);
}

TEST_CASE(memory, null_pointer) {
    // Test that leaking a NULL pointer is safe
    usize initial_count = memory_get_allocation_count();

    // This should not crash or affect anything
    memory_leak(NULL);

    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);
}

TEST_CASE(memory, double_mark) {
    // Test marking the same allocation as leaked twice
    extern KMemoryHeader* g_memory_head;

    usize initial_count = memory_get_allocation_count();

    void* p             = KORE_MALLOC(100);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count + 1);

    // Mark as leaked first time
    memory_leak(p);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);

    // Mark as leaked second time - should be safe
    memory_leak(p);
    TEST_ASSERT_EQ(memory_get_allocation_count(), initial_count);

    // Verify it's still not in the linked list
    KMemoryHeader* current  = g_memory_head;
    KMemoryHeader* p_header = (KMemoryHeader*)p - 1;
    bool found              = false;

    while (current) {
        if (current == p_header) {
            found = true;
            break;
        }
        current = current->next;
    }

    TEST_ASSERT(!found); // Should not be in the list
}

TEST_CASE(memory, sequential) {
    // Test that allocation indices are assigned sequentially
    extern u64 g_memory_index;
    extern KMemoryHeader* g_memory_head;

    u64 initial_index      = g_memory_index;

    void* p1               = KORE_MALLOC(100);
    void* p2               = KORE_MALLOC(200);
    void* p3               = KORE_MALLOC(300);

    KMemoryHeader* header1 = (KMemoryHeader*)p1 - 1;
    KMemoryHeader* header2 = (KMemoryHeader*)p2 - 1;
    KMemoryHeader* header3 = (KMemoryHeader*)p3 - 1;

    // Indices should be sequential
    TEST_ASSERT_EQ(header1->index, initial_index + 1);
    TEST_ASSERT_EQ(header2->index, initial_index + 2);
    TEST_ASSERT_EQ(header3->index, initial_index + 3);

    // Global index should have advanced
    TEST_ASSERT_EQ(g_memory_index, initial_index + 3);

    KORE_FREE(p1);
    KORE_FREE(p2);
    KORE_FREE(p3);
}

TEST_CASE(memory, realloc_gets_new_index) {
    // Test that realloc assigns a new index
    extern u64 g_memory_index;

    u64 initial_index             = g_memory_index;

    void* p                       = KORE_MALLOC(100);
    KMemoryHeader* initial_header = (KMemoryHeader*)p - 1;
    u64 first_index               = initial_header->index;

    TEST_ASSERT_EQ(first_index, initial_index + 1);

    // Realloc should get a new index
    p                         = KORE_REALLOC(p, 200);
    KMemoryHeader* new_header = (KMemoryHeader*)p - 1;
    u64 second_index          = new_header->index;

    TEST_ASSERT_EQ(second_index, initial_index + 2);
    TEST_ASSERT(second_index > first_index);
    TEST_ASSERT_EQ(g_memory_index, initial_index + 2);

    KORE_FREE(p);
}

TEST_CASE(memory, mixed_operations) {
    // Test indices with mixed malloc/realloc/free operations
    extern u64 g_memory_index;

    u64 initial_index      = g_memory_index;

    void* p1               = KORE_MALLOC(100); // index: initial + 1
    void* p2               = KORE_MALLOC(200); // index: initial + 2

    p1                     = KORE_REALLOC(p1, 150); // index: initial + 3

    void* p3               = KORE_MALLOC(300); // index: initial + 4

    p2                     = KORE_REALLOC(p2, 250); // index: initial + 5

    KMemoryHeader* header1 = (KMemoryHeader*)p1 - 1;
    KMemoryHeader* header2 = (KMemoryHeader*)p2 - 1;
    KMemoryHeader* header3 = (KMemoryHeader*)p3 - 1;

    TEST_ASSERT_EQ(header1->index, initial_index + 3); // realloc got new index
    TEST_ASSERT_EQ(header2->index, initial_index + 5); // realloc got new index
    TEST_ASSERT_EQ(header3->index, initial_index + 4); // malloc index
    TEST_ASSERT_EQ(g_memory_index, initial_index + 5);

    KORE_FREE(p1);
    KORE_FREE(p2);
    KORE_FREE(p3);
}

TEST_CASE(memory, leak_preserves_index) {
    // Test that marking as leaked preserves the original index
    extern u64 g_memory_index;

    u64 initial_index     = g_memory_index;

    void* p               = KORE_MALLOC(100);
    KMemoryHeader* header = (KMemoryHeader*)p - 1;
    u64 original_index    = header->index;

    TEST_ASSERT_EQ(original_index, initial_index + 1);

    // Mark as leaked
    memory_leak(p);

    // Index should remain the same
    TEST_ASSERT_EQ(header->index, original_index);
    TEST_ASSERT_EQ(header->leaked, true);

    // Global index should not have changed
    TEST_ASSERT_EQ(g_memory_index, initial_index + 1);
}

TEST_CASE(memory, realloc_leaked_gets_new_index) {
    // Test that realloc of leaked memory still gets a new index
    extern u64 g_memory_index;

    u64 initial_index     = g_memory_index;

    void* p               = KORE_MALLOC(100); // index: initial + 1
    KMemoryHeader* header = (KMemoryHeader*)p - 1;
    u64 first_index       = header->index;

    // Mark as leaked
    memory_leak(p);
    TEST_ASSERT_EQ(header->index, first_index); // Index preserved

    // Realloc leaked memory should get new index
    p                = KORE_REALLOC(p, 200); // index: initial + 2
    header           = (KMemoryHeader*)p - 1;
    u64 second_index = header->index;

    TEST_ASSERT_EQ(second_index, initial_index + 2);
    TEST_ASSERT(second_index > first_index);
    TEST_ASSERT_EQ(header->leaked, true); // Should still be leaked
    TEST_ASSERT_EQ(g_memory_index, initial_index + 2);

    // Clean up (free the leaked memory)
    KORE_FREE(p);
}

TEST_CASE(memory, global_advances_correctly) {
    // Test that global index advances correctly and never goes backwards
    extern u64 g_memory_index;

    u64 start_index     = g_memory_index;
    const usize num_ops = 10;
    void* ptrs[num_ops];

    // Perform a series of operations and verify index always increases
    for (usize i = 0; i < num_ops; i++) {
        u64 before_op = g_memory_index;

        if (i % 3 == 0) {
            // Malloc
            ptrs[i] = KORE_MALLOC((i + 1) * 10);
        } else if (i % 3 == 1 && i > 0) {
            // Realloc previous allocation
            ptrs[i]     = KORE_REALLOC(ptrs[i - 1], (i + 1) * 20);
            ptrs[i - 1] = NULL; // Mark as handled
        } else {
            // Another malloc
            ptrs[i] = KORE_MALLOC((i + 1) * 15);
        }

        u64 after_op = g_memory_index;
        TEST_ASSERT(after_op > before_op);       // Index must increase
        TEST_ASSERT_EQ(after_op, before_op + 1); // Should increase by exactly 1

        KMemoryHeader* header = (KMemoryHeader*)ptrs[i] - 1;
        TEST_ASSERT_EQ(header->index,
                       after_op); // Header should have the new index
    }

    TEST_ASSERT_EQ(g_memory_index, start_index + num_ops);

    // Clean up
    for (usize i = 0; i < num_ops; i++) {
        if (ptrs[i]) {
            KORE_FREE(ptrs[i]);
        }
    }
}

TEST_CASE(memory, break_on_alloc_basic) {
    // Test basic break-on-alloc functionality (without actually breaking)
    extern u64 g_memory_break_index;
    extern u64 g_memory_index;

    u64 original_break_index = g_memory_break_index;
    u64 initial_index        = g_memory_index;

    // Set break index to a value that won't be hit
    u64 safe_index           = initial_index + 1000; // Far ahead, won't be hit
    memory_break_on_alloc(safe_index);
    TEST_ASSERT_EQ(g_memory_break_index, safe_index);

    // Allocate normally (these won't hit the break index)
    void* p1               = KORE_MALLOC(100); // index: initial + 1
    KMemoryHeader* header1 = (KMemoryHeader*)p1 - 1;
    TEST_ASSERT_EQ(header1->index, initial_index + 1);

    void* p2               = KORE_MALLOC(200); // index: initial + 2
    KMemoryHeader* header2 = (KMemoryHeader*)p2 - 1;
    TEST_ASSERT_EQ(header2->index, initial_index + 2);

    void* p3               = KORE_MALLOC(300); // index: initial + 3
    KMemoryHeader* header3 = (KMemoryHeader*)p3 - 1;
    TEST_ASSERT_EQ(header3->index, initial_index + 3);

    // Verify the break index is still set correctly
    TEST_ASSERT_EQ(g_memory_break_index, safe_index);

    // Clean up
    KORE_FREE(p1);
    KORE_FREE(p2);
    KORE_FREE(p3);

    // Restore original break index
    memory_break_on_alloc(original_break_index);
}

TEST_CASE(memory, break_on_alloc_realloc) {
    // Test break-on-alloc with realloc operations (without actually breaking)
    extern u64 g_memory_break_index;
    extern u64 g_memory_index;

    u64 original_break_index = g_memory_break_index;
    u64 initial_index        = g_memory_index;

    void* p                  = KORE_MALLOC(100); // index: initial + 1

    // Set break index for a safe value that won't be hit
    u64 safe_index           = initial_index + 1000;
    memory_break_on_alloc(safe_index);

    // Realloc should get next index (initial + 2), which is safe
    p                     = KORE_REALLOC(p, 200); // index: initial + 2
    KMemoryHeader* header = (KMemoryHeader*)p - 1;
    TEST_ASSERT_EQ(header->index, initial_index + 2);

    // Verify break index is still set
    TEST_ASSERT_EQ(g_memory_break_index, safe_index);

    // Clean up
    KORE_FREE(p);

    // Restore original break index
    memory_break_on_alloc(original_break_index);
}

TEST_CASE(memory, uniqueness) {
    // Test that all allocated blocks have unique indices
    extern u64 g_memory_index;

    const usize num_allocs = 20;
    void* ptrs[num_allocs];
    u64 indices[num_allocs];

    // Allocate multiple blocks
    for (usize i = 0; i < num_allocs; i++) {
        ptrs[i]               = KORE_MALLOC((i + 1) * 10);
        KMemoryHeader* header = (KMemoryHeader*)ptrs[i] - 1;
        indices[i]            = header->index;
    }

    // Check that all indices are unique
    for (usize i = 0; i < num_allocs; i++) {
        for (usize j = i + 1; j < num_allocs; j++) {
            TEST_ASSERT(indices[i] != indices[j]); // All indices must be unique
        }
    }

    // Check that indices are in ascending order
    for (usize i = 1; i < num_allocs; i++) {
        TEST_ASSERT(indices[i] >
                    indices[i - 1]); // Should be strictly increasing
    }

    // Clean up
    for (usize i = 0; i < num_allocs; i++) {
        KORE_FREE(ptrs[i]);
    }
}

TEST_CASE(array, basic_array) {
    Array(int) arr = NULL;

    TEST_ASSERT_EQ(array_size(arr), 0);
    TEST_ASSERT_EQ(array_count(arr), 0);
    TEST_ASSERT_EQ(array_capacity(arr), 0);
}

TEST_CASE(array, push_pop) {
    Array(int) arr = NULL;

    // Push elements
    for (int i = 0; i < 10; i++) {
        array_push(arr, i);
    }

    TEST_ASSERT_EQ(array_size(arr), 10 * sizeof(int));  // size in bytes
    TEST_ASSERT_EQ(array_count(arr), 10);               // count of elements
    TEST_ASSERT_GT(array_capacity(arr), 10 * sizeof(int));

    // Pop elements
    for (int i = 9; i >= 0; i--) {
        int value = array_pop(arr);
        TEST_ASSERT_EQ(value, i);
    }

    TEST_ASSERT_EQ(array_size(arr), 0);   // size in bytes (0 after popping all)
    TEST_ASSERT_EQ(array_count(arr), 0);  // count of elements (0 after popping all)
}

TEST_SUITE_BEGIN()
RUN_ALL_TESTS();
TEST_SUITE_END()
