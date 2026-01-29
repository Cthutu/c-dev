//------------------------------------------------------------------------------
// Arena implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

#if OS_POSIX
#    include <sys/mman.h>
#endif

//------------------------------------------------------------------------------
// Data structure
//------------------------------------------------------------------------------

typedef struct {
    usize alloc_granularity;   // OS allocation granularity (page size)
    usize reserve_granularity; // OS reservation granularity (allocation unit)
} ArenaMemoryInfo;

//------------------------------------------------------------------------------
// get_arena_memory_info
// Retrieves OS-specific arena memory information
//------------------------------------------------------------------------------

internal ArenaMemoryInfo get_arena_memory_info(void)
{
#if OS_WINDOWS
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    return (ArenaMemoryInfo){
        .alloc_granularity   = sys_info.dwAllocationGranularity,
        .reserve_granularity = sys_info.dwAllocationGranularity,
    };
#elif OS_POSIX
    usize page_size = (usize)sysconf(_SC_PAGESIZE);
    return (ArenaMemoryInfo){
        .alloc_granularity   = page_size,
        .reserve_granularity = page_size,
    };
#else
#    error "Arena memory info not implemented for this OS."
#endif
}

//------------------------------------------------------------------------------
// _arena_init
// Initialises the fields of an Arena structure
//------------------------------------------------------------------------------

void _arena_init(Arena* arena, ArenaDefaultParams params)
{
    ArenaMemoryInfo mem_info = get_arena_memory_info();

    if (params.grow_rate == 0) {
        params.grow_rate = ARENA_DEFAULT_NUM_PAGES_GROW;
    }
    if (params.reserved_size == 0) {
        params.reserved_size = GB(4);
    }

    params.reserved_size =
        ALIGN_UP(params.reserved_size, mem_info.reserve_granularity);
    usize initial_alloc_size = mem_info.alloc_granularity * params.grow_rate;

    ASSERT(params.reserved_size >= initial_alloc_size,
           "Arena reserved size must be at least %zu bytes.",
           initial_alloc_size);

#if OS_WINDOWS
    u8* memory = (u8*)VirtualAlloc(
        nullptr, params.reserved_size, MEM_RESERVE, PAGE_NOACCESS);
    mem_check(
        VirtualAlloc(memory, initial_alloc_size, MEM_COMMIT, PAGE_READWRITE));

#elif OS_POSIX
    u8* memory = (u8*)mmap(nullptr,
                           params.reserved_size,
                           PROT_NONE,
                           MAP_PRIVATE | MAP_ANONYMOUS,
                           -1,
                           0);
    MEM_CHECK(memory);

    // Allocate the first block.
    if (mprotect(memory, initial_alloc_size, PROT_READ | PROT_WRITE) != 0) {
        perror("mprotect");
        exit(1);
    }

#else
#    error "Arena creation not implemented for this OS."
#endif // OS_WINDOWS

    arena->memory            = memory;
    arena->cursor            = 0;
    arena->committed_size    = initial_alloc_size;
    arena->reserved_size     = params.reserved_size;
    arena->alloc_granularity = mem_info.alloc_granularity;
    arena->grow_rate         = params.grow_rate;
}

//------------------------------------------------------------------------------
// arena_done
// Releases resources associated with an Arena
//------------------------------------------------------------------------------

void arena_done(Arena* arena)
{
#if OS_WINDOWS
    VirtualFree(arena->memory, 0, MEM_RELEASE);
#elif OS_POSIX
    munmap(arena->memory, arena->reserved_size);
#else
#    error "Arena destruction not implemented for this OS."
#endif

    memset(arena, 0, sizeof(Arena));
}

//------------------------------------------------------------------------------
// _arena_ensure_room
// Ensures that an arena has enough space for a certain number of bytes
//------------------------------------------------------------------------------

internal void _arena_ensure_room(Arena* arena, usize size)
{
    usize new_cursor = arena->cursor + size;

    if (new_cursor > arena->reserved_size) {
        eprn("Arena overflow: requested %zu bytes, but only %zu bytes "
             "available.",
             new_cursor,
             arena->reserved_size - arena->cursor);
        exit(1);
    }

    if (new_cursor > arena->committed_size) {
        // Need to commit more memory.
        usize commit_size =
            ALIGN_UP(new_cursor - arena->committed_size,
                     arena->alloc_granularity * arena->grow_rate);

#if OS_WINDOWS
        MEM_CHECK(VirtualAlloc(arena->memory + arena->committed_size,
                               commit_size,
                               MEM_COMMIT,
                               PAGE_READWRITE));
#elif OS_POSIX
        if (mprotect(arena->memory + arena->committed_size,
                     commit_size,
                     PROT_READ | PROT_WRITE) != 0) {
            perror("mprotect");
            exit(1);
        }
#else
#    error "Arena memory commit not implemented for this OS."
#endif // OS_WINDOWS

        arena->committed_size += commit_size;
    }
}

//------------------------------------------------------------------------------
// arena_alloc
// Allocates a block of memory from the arena
//------------------------------------------------------------------------------

void* arena_alloc(Arena* arena, usize size)
{
    _arena_ensure_room(arena, size);

    void* ptr = arena->memory + arena->cursor;
    arena->cursor += size;
    return ptr;
}

//------------------------------------------------------------------------------
// arena_align
// Aligns the arena's allocation cursor to the specified alignment
//------------------------------------------------------------------------------

void arena_align(Arena* arena, usize align)
{
    usize aligned_cursor = ALIGN_UP(arena->cursor, align);
    _arena_ensure_room(arena, aligned_cursor - arena->cursor);
    arena->cursor = aligned_cursor;
}

//------------------------------------------------------------------------------
// arena_alloc_align
// Allocates a block of memory from the arena with the specified alignment
//------------------------------------------------------------------------------

void* arena_alloc_align(Arena* arena, usize size, usize align)
{
    arena_align(arena, align);
    return arena_alloc(arena, size);
}

//------------------------------------------------------------------------------
// arena_formatv
// Formats a string into the arena using a va_list
//------------------------------------------------------------------------------

u8* arena_formatv(Arena* arena, cstr fmt, va_list args)
{
    // Get the size of the formatted string.
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    // Allocate space in the arena.
    u8* buffer = (u8*)arena_alloc(arena, (usize)size + 1);

    // Format the string into the buffer.
    vsnprintf((char*)buffer, (usize)size + 1, fmt, args);
    arena->cursor--; // Remove null terminator from arena allocation

    return buffer;
}

//------------------------------------------------------------------------------
// arena_format
// Formats a string into the arena
//------------------------------------------------------------------------------

u8* arena_format(Arena* arena, cstr fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    u8* result = arena_formatv(arena, fmt, args);
    va_end(args);
    return result;
}

//------------------------------------------------------------------------------
// arena_null_terminate
// Appends a null terminator to the arena
//------------------------------------------------------------------------------

void arena_null_terminate(Arena* arena)
{
    u8* ptr = (u8*)arena_alloc(arena, 1);
    *ptr    = '\0';
}

//------------------------------------------------------------------------------
// arena_store
// Returns the current cursor position in the arena as a pointer for storing.
// Pointers are stable in these arenas.
//------------------------------------------------------------------------------

void* arena_store(Arena* arena) { return arena->memory + arena->cursor; }

//------------------------------------------------------------------------------
// arena_restore
// Restores the arena's cursor to a previously stored position
//------------------------------------------------------------------------------

void arena_restore(Arena* arena, void* mark)
{
    usize new_cursor = (usize)((u8*)mark - arena->memory);
    ASSERT(new_cursor <= arena->committed_size,
           "Attempted to restore arena to invalid mark.");
    arena->cursor = new_cursor;
}

//------------------------------------------------------------------------------
// arena_reset
// Resets the arena's cursor to the beginning
//------------------------------------------------------------------------------

void arena_reset(Arena* arena) { arena->cursor = 0; }

//------------------------------------------------------------------------------
// arena_offset
// Returns the offset of a pointer within the arena
//------------------------------------------------------------------------------

usize arena_offset(Arena* arena, void* p)
{
    return (usize)((u8*)p - arena->memory);
}

//------------------------------------------------------------------------------
// End of core/arena.c
//------------------------------------------------------------------------------
