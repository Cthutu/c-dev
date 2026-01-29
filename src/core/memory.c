//------------------------------------------------------------------------------
// Memory management
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------
// Global variables

// Global pointer to the head of the linked list of allocated, tracked memory
// blocks.

#if DEBUG

global_variable MemoryHeader* g_memory_head = nullptr;
global_variable u64 g_memory_index          = 0; // Global index for allocations
global_variable u64 g_memory_break_index    = 0; // Index to break on

#endif // DEBUG

//------------------------------------------------------------------------------
// mem_alloc
// Allocates memory and tracks it for debugging purposes
//------------------------------------------------------------------------------

void* mem_alloc(usize size, cstr file, int line)
{
    MemoryHeader* header = (MemoryHeader*)malloc(sizeof(MemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory allocation failed at %s:%d\n", file, line);
        exit(EXIT_FAILURE);
    }

    header->size = size;

#if DEBUG
    header->file   = file;
    header->line   = line;
    header->leaked = false;            // Initialise leaked flag
    header->index  = ++g_memory_index; // Assign and increment index

    // Check if we should break on this allocation
    if (header->index == g_memory_break_index) {
        DEBUG_BREAK();
    }

    // Add to linked list
    header->next  = g_memory_head;
    g_memory_head = header;
#endif // DEBUG

    return (void*)(header + 1);
}

//------------------------------------------------------------------------------
// mem_realloc
// Reallocates memory and updates tracking information
//------------------------------------------------------------------------------

void* mem_realloc(void* ptr, usize size, cstr file, int line)
{
    if (!ptr) {
        return mem_alloc(size, file, line);
    }

    MemoryHeader* old_header = (MemoryHeader*)ptr - 1;

#if DEBUG
    // Preserve the leaked flag from the old header
    bool was_leaked = old_header->leaked;
#endif // DEBUG

    // Remove old header from linked list
#if DEBUG
    if (!old_header->leaked) {
        if (g_memory_head == old_header) {
            g_memory_head = old_header->next;
        } else {
            MemoryHeader* current = g_memory_head;
            while (current && current->next != old_header) {
                current = current->next;
            }
            if (current) {
                current->next = old_header->next;
            }
        }
    }
#endif // DEBUG

    MemoryHeader* header =
        (MemoryHeader*)realloc(old_header, sizeof(MemoryHeader) + size);
    if (!header) {
        fprintf(stderr, "Memory reallocation failed at %s:%d\n", file, line);
        exit(EXIT_FAILURE);
    }

    header->size = size;

#if DEBUG
    header->file   = file;
    header->line   = line;
    header->leaked = was_leaked;       // Restore leaked flag
    header->index  = ++g_memory_index; // Assign and increment index

    // Check if we should break on this allocation
    if (header->index == g_memory_break_index) {
        DEBUG_BREAK();
    }

    // Add to linked list
    header->next  = g_memory_head;
    g_memory_head = header;
#endif // DEBUG

    return (void*)(header + 1);
}

//------------------------------------------------------------------------------
// mem_free
// Frees memory and removes it from tracking.  Always returns nullptr.
//------------------------------------------------------------------------------

void* mem_free(void* ptr, cstr file, int line)
{
    UNUSED(file); // Suppress unused parameter warning
    UNUSED(line); // Suppress unused parameter warning

    if (!ptr) {
        return nullptr;
    }

    MemoryHeader* header = (MemoryHeader*)ptr - 1;

#if DEBUG
    // Remove from linked list if it exists
    if (!header->leaked) {
        if (g_memory_head == header) {
            g_memory_head = header->next;
        } else {
            MemoryHeader* current = g_memory_head;
            while (current && current->next != header) {
                current = current->next;
            }
            if (current) {
                current->next = header->next;
            }
        }
    }
#endif // DEBUG

    free(header);
    return nullptr;
}

//------------------------------------------------------------------------------
// mem_size
// Returns the size of an allocated memory block
//------------------------------------------------------------------------------

usize mem_size(const void* ptr)
{
    if (!ptr) {
        return 0;
    }

    const MemoryHeader* header = (const MemoryHeader*)ptr - 1;
    return header->size;
}

//------------------------------------------------------------------------------
// mem_leak
// Marks a memory block as leaked so it is not tracked or reported
//------------------------------------------------------------------------------

void mem_leak(void* ptr)
{
#if DEBUG
    if (!ptr) {
        return;
    }

    MemoryHeader* header = (MemoryHeader*)ptr - 1;
    header->leaked       = true; // Mark this block as leaked

    // Remove from linked list if it exists
    if (g_memory_head == header) {
        g_memory_head = header->next;
    } else {
        MemoryHeader* current = g_memory_head;
        while (current && current->next != header) {
            current = current->next;
        }
        if (current) {
            current->next = header->next;
        }
    }
#else
    UNUSED(ptr); // Suppress unused parameter warning
#endif // DEBUG
}

//------------------------------------------------------------------------------
// mem_break_on_alloc
// Set the allocation index to breakpoint on
//------------------------------------------------------------------------------

#if DEBUG

void mem_break_on_alloc(u64 index) { g_memory_break_index = index; }

//------------------------------------------------------------------------------
// mem_dump_leaks
// Dumps all currently allocated memory blocks to the console
//------------------------------------------------------------------------------

void mem_print_leaks(void)
{
    MemoryHeader* current      = g_memory_head;
    usize         leak_count   = 0;
    usize         total_leaked = 0;

    if (!current) {
        return;
    }

    eprn(ANSI_BOLD_RED "┌──────────────────────────────────────┐" ANSI_RESET);
    eprn(ANSI_BOLD_RED "│        Memory leaks detected         │" ANSI_RESET);
    eprn(ANSI_BOLD_RED "└──────────────────────────────────────┘" ANSI_RESET);

    while (current) {
        eprn(ANSI_FAINT " %s" ANSI_RESET ANSI_BOLD "[%zu]" ANSI_RESET
                        " %s:%d " ANSI_BOLD_YELLOW "%zu bytes" ANSI_RESET,
             UNICODE_TREE_BRANCH,
             current->index,
             current->file,
             current->line,
             current->size);

        total_leaked += current->size;
        leak_count++;
        current = current->next;
    }

    eprn(" " ANSI_FAINT UNICODE_TREE_LAST_BRANCH ANSI_RESET ANSI_BOLD_RED
         "Total:" ANSI_RESET " %zu leaks, %zu bytes",
         leak_count,
         total_leaked);
}

//------------------------------------------------------------------------------
// mem_get_allocation_count
// Returns the number of currently allocated memory blocks
//------------------------------------------------------------------------------

usize mem_get_allocation_count(void)
{
    usize         count   = 0;
    MemoryHeader* current = g_memory_head;

    while (current) {
        count++;
        current = current->next;
    }

    return count;
}

//------------------------------------------------------------------------------
// mem_get_total_allocated
// Returns the total size of currently allocated memory
//------------------------------------------------------------------------------

usize mem_get_total_allocated(void)
{
    usize         total   = 0;
    MemoryHeader* current = g_memory_head;

    while (current) {
        total += current->size;
        current = current->next;
    }

    return total;
}

#endif // DEBUG

//------------------------------------------------------------------------------
// mem_check
// Validates a memory pointer and if it isn't valid, will print a huge message
// and exit the program.
//------------------------------------------------------------------------------

void mem_check(void* ptr, cstr file, int line)
{
    if (!ptr) {

        eprn(ANSI_BOLD_RED
             "┌──────────────────────────────────────┐" ANSI_RESET);
        eprn(ANSI_BOLD_RED
             "│        Out of memory error           │" ANSI_RESET);
        eprn(ANSI_BOLD_RED
             "└──────────────────────────────────────┘" ANSI_RESET);

        eprn("Attempted to allocate memory at %s:%d but the allocation "
             "returned NULL.",
             file,
             line);
        exit(EXIT_FAILURE);
    }
}

//------------------------------------------------------------------------------
// End of core/memory.c
//------------------------------------------------------------------------------
