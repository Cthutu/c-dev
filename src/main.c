#define KORE_IMPLEMENTATION
#include "kore.h"
#include <stdio.h>

int main() {
    printf("Hello, World!\n");

    void* p1 = KORE_MALLOC(100);
    void* p2 = KORE_MALLOC(200);
    void* p3 = KORE_REALLOC(p1, 150);
    void* p4 = KORE_MALLOC(50);

    k_memory_print_leaks();

    KORE_FREE(p2);
    KORE_FREE(p3);
    KORE_FREE(p4);

    k_memory_print_leaks();

    return 0;
}
