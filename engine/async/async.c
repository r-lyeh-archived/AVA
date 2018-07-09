#if ASYNC_C
#pragma once
#define ATOMIC_C 1
#define BUILTIN_C 1
#define CHANNEL_C 1
#define CONDV_C 1
#define LFQ_C 1
#define MUTEX_C 1
#define REALLOC_C 1
#define SEMAPHORE_C 1
#define SLEEP_C 1
#define SYSTEM_C 1
#define THREAD_C 1
#define TLS_C 1
#endif

#include "builtin.c"
#include "realloc.c"
#include "system.c"

#include "async_atomic.c"
#include "async_channel.c"
#include "async_condv.c"
#include "async_lfq.c"
#include "async_mutex.c"
#include "async_semaphore.c"
#include "async_sleep.c"
#include "async_thread.c"
#include "async_tls.c"
