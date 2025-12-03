#define KORE_IMPLEMENTATION
#include "kore.h"
#include <stdio.h>

int main() {
    Array(int) arr = nullptr;
    kore_init();

    prn("Capacity of array: %zu", array_capacity(arr));
    array_push(arr, 10);
    prn("Capacity of array: %zu", array_capacity(arr));
    array_push(arr, 20);
    prn("Capacity of array: %zu", array_capacity(arr));
    array_push(arr, 42);
    prn("Capacity of array: %zu", array_capacity(arr));

    prn("%d", array_pop(arr)); // Should print 42
    prn("%d", array_pop(arr)); // Should print 20
    prn("%d", array_pop(arr)); // Should print 10

    prn("Size of array: %zu", array_count(arr));

    array_free(arr);

    kore_done();
}
