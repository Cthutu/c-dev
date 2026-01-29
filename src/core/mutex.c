//------------------------------------------------------------------------------
// Mutex implementation
//
// Copyright (C)2026 Matt Davies, all rights reserved
//------------------------------------------------------------------------------

#include <core/core.h>

//------------------------------------------------------------------------------
// Windows implementation
//------------------------------------------------------------------------------

#if OS_WINDOWS

void mutex_init(Mutex* mutex) { InitializeCriticalSection(mutex); }

void mutex_done(Mutex* mutex) { DeleteCriticalSection(mutex); }

void mutex_lock(Mutex* mutex) { EnterCriticalSection(mutex); }

void mutex_unlock(Mutex* mutex) { LeaveCriticalSection(mutex); }

//------------------------------------------------------------------------------
// POSIX implementation
//------------------------------------------------------------------------------

#elif OS_POSIX

void mutex_init(Mutex* mutex) { pthread_mutex_init(mutex, nullptr); }

void mutex_done(Mutex* mutex) { pthread_mutex_destroy(mutex); }

void mutex_lock(Mutex* mutex) { pthread_mutex_lock(mutex); }

void mutex_unlock(Mutex* mutex) { pthread_mutex_unlock(mutex); }

//------------------------------------------------------------------------------
// Unsupported OS
//------------------------------------------------------------------------------

#else
#    error "Mutex not implemented for this OS."
#endif // OS_WINDOWS

//------------------------------------------------------------------------------
// End of core/mutex.c
//------------------------------------------------------------------------------
