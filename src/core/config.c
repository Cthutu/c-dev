//------------------------------------------------------------------------------
// Configuration management
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------

internal void _dump_compiler_config(void)
{
    pr(ANSI_CYAN "Compiler Configuration: " ANSI_YELLOW);

#if COMPILER_GCC
    pr("GCC");
#elif COMPILER_CLANG
    pr("Clang");
#elif COMPILER_MSVC
    pr("MSVC");
#else
    pr("Unknown Compiler");
#endif

    prn(ANSI_RESET);
}

//------------------------------------------------------------------------------

internal void _dump_os_config(void)
{
    pr(ANSI_CYAN "      Operating System: " ANSI_YELLOW);

#if OS_WINDOWS
    pr("Windows");
#elif OS_LINUX
    pr("Linux");
#elif OS_MACOS
    pr("macOS");
#elif OS_BSD
    pr("BSD");
#else
    pr("Unknown OS");
#endif
    prn(ANSI_RESET);

    prn(ANSI_CYAN "       POSIX Compliant: " ANSI_YELLOW "%s" ANSI_RESET,
        OS_POSIX ? "Yes" : "No");
}

//------------------------------------------------------------------------------

internal void _dump_arch_config(void)
{
    pr(ANSI_CYAN "          Architecture: " ANSI_YELLOW);

#if ARCH_X86
    pr("x86");
#elif ARCH_X86_64
    pr("x86_64");
#elif ARCH_ARM
    pr("ARM");
#elif ARCH_ARM64
    pr("ARM64");
#else
    pr("Unknown Architecture");
#endif
    prn(ANSI_RESET);
}

//------------------------------------------------------------------------------

internal void _dump_build_config(void)
{
    pr(ANSI_CYAN "   Build Configuration: " ANSI_YELLOW);

#if BUILD_DEBUG
    pr("Debug");
#elif BUILD_RELEASE
    pr("Release");
#else
    pr("Unknown Build Configuration");
#endif
    prn(ANSI_RESET);
}

//------------------------------------------------------------------------------

void dump_config(void)
{
    _dump_compiler_config();
    _dump_os_config();
    _dump_arch_config();
    _dump_build_config();
}

//------------------------------------------------------------------------------
// End of core/config.c
//------------------------------------------------------------------------------
