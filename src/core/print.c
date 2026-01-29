//------------------------------------------------------------------------------
// Implementation of the print functions
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

Mutex g_kore_output_mutex;

//------------------------------------------------------------------------------
// _format_output
// General purpose string formatter, which is thread safe.
//------------------------------------------------------------------------------

internal cstr _format_output(cstr format, va_list args, usize* out_size)
{
    thread_local local_persist Array(char) print_buffer = nullptr;

    // First, determine the size needed.
    va_list args_copy;
    va_copy(args_copy, args);
    *out_size = (usize)vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    // Allocate buffer for the formatted string.
    array_requires(print_buffer, *out_size + 1);
    array_leak(print_buffer); // Prevent detection in leaks

    // Now actually format the string.
    vsnprintf(print_buffer, *out_size + 1, format, args);

    return print_buffer;
}

//------------------------------------------------------------------------------
// fprv (POSIX)
// Print a formatted string from a va_list to a specific channel
//------------------------------------------------------------------------------

#if OS_POSIX

internal void fprv(int fd, cstr format, va_list args)
{
    usize size;
    mutex_lock(&g_kore_output_mutex);
    cstr output = _format_output(format, args, &size);
    write(fd, output, size);
    mutex_unlock(&g_kore_output_mutex);
}

void prv(cstr format, va_list args) { fprv(STDOUT_FILENO, format, args); }
void eprv(cstr format, va_list args) { fprv(STDERR_FILENO, format, args); }

//------------------------------------------------------------------------------
// fprv (Windows)
// Print a formatted string from a va_list to a specific channel
//------------------------------------------------------------------------------

#elif OS_WINDOWS

internal void fprv(HANDLE handle, cstr format, va_list args)
{
    usize size;
    mutex_lock(&g_kore_output_mutex);
    cstr output = _format_output(format, args, &size);

    DWORD console_mode;
    BOOL  is_console = GetConsoleMode(handle, &console_mode);

    DWORD written;
    if (is_console) {
        WriteConsoleA(handle, output, (DWORD)size, &written, nullptr);
    } else {
        WriteFile(handle, output, (DWORD)size, &written, nullptr);
    }
    mutex_unlock(&g_kore_output_mutex);
}

void prv(cstr format, va_list args)
{
    fprv(GetStdHandle(STD_OUTPUT_HANDLE), format, args);
}

void eprv(cstr format, va_list args)
{
    fprv(GetStdHandle(STD_ERROR_HANDLE), format, args);
}

//------------------------------------------------------------------------------
// Unknown OS
//------------------------------------------------------------------------------

#else
#    error "Output functions not implemented for this OS."
#endif // OS_POSIX

//------------------------------------------------------------------------------
// Output functions
//------------------------------------------------------------------------------

void pr(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    prv(format, args);
    va_end(args);
}

void prn(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    prv(format, args);
    pr("\n");
    va_end(args);
}

void epr(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    eprv(format, args);
    va_end(args);
}

void eprn(cstr format, ...)
{
    va_list args;
    va_start(args, format);
    eprv(format, args);
    epr("\n");
    va_end(args);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
