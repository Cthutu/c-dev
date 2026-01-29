//------------------------------------------------------------------------------
// Main entry point
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------

int run(int argc, char** argv);

extern Mutex g_kore_output_mutex;

int main(int argc, char** argv)
{
    //
    // Initialisation
    //

    mutex_init(&g_kore_output_mutex);

#if OS_WINDOWS
    UINT old_cp = GetConsoleCP();
    SetConsoleCP(CP_UTF8);
    UINT old_output_cp = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
#endif // OS_WINDOWS

    //
    // User's program
    //

    int result = run(argc, argv);

    //
    // Clean up
    //

#if OS_WINDOWS
    SetConsoleCP(old_cp);
    SetConsoleOutputCP(old_output_cp);
#endif // OS_WINDOWS

#if DEBUG
    mem_dump_leaks();
#endif // DEBUG

    mutex_done(&g_kore_output_mutex);
    return result;
}
