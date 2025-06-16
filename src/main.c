#define KORE_IMPLEMENTATION
#include "kore.h"
#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    printf("Testing leak tracking with realloc...\n");

    // Test original sequence but fixed
    void* p1 = KORE_MALLOC(100);
    void* p2 = KORE_MALLOC(200);
    void* p3 = KORE_REALLOC(p1, 150); // p3 is the reallocated p1
    void* p4 = KORE_MALLOC(50);

    k_memory_leak(p3); // Mark p3 as leaked (p3 is the reallocated p1)

    printf("\nFirst leak check (should show p2 and p4):\n");
#if KORE_DEBUG
    k_memory_print_leaks();
#endif // KORE_DEBUG

    KORE_FREE(p2);
    KORE_FREE(p4);
    // Note: we don't free p3 because it's marked as leaked

    printf("\nSecond leak check (should show nothing):\n");
#if KORE_DEBUG
    k_memory_print_leaks();
#endif // KORE_DEBUG

    // Test: marking as leaked before realloc
    printf("\nTesting: mark as leaked before realloc\n");
    void* p5 = KORE_MALLOC(75);
    k_memory_leak(p5);
    printf("After marking p5 as leaked:\n");
#if KORE_DEBUG
    k_memory_print_leaks();
#endif // KORE_DEBUG

    void* p6 = KORE_REALLOC(p5, 125);
    (void)p6; // Suppress unused variable warning
    printf("After reallocating p5 to p6 (should still be leaked):\n");
#if KORE_DEBUG
    k_memory_print_leaks();
#endif // KORE_DEBUG

    return 0;
}
