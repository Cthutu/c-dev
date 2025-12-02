#define KORE_IMPLEMENTATION
#include "kore.h"
#include <stdio.h>

int main() {
    Array(int) arr = nullptr;

    printf("Capacity of array: %zu\n", array_capacity(arr));
    array_push(arr, 10);
    printf("Capacity of array: %zu\n", array_capacity(arr));
    array_push(arr, 20);
    printf("Capacity of array: %zu\n", array_capacity(arr));
    array_push(arr, 42);
    printf("Capacity of array: %zu\n", array_capacity(arr));

    printf("%d\n", array_pop(arr)); // Should print 42
    printf("%d\n", array_pop(arr)); // Should print 20
    printf("%d\n", array_pop(arr)); // Should print 10

    printf("Size of array: %zu\n", array_count(arr));

    array_free(arr);

    memory_print_leaks();
}
