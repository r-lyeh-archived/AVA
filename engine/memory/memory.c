#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef MEMORY_C
#pragma once
#define WATCH_C
#define REALLOC_C
#define MALLOCA_C
#define VREALLOC_C
#endif

#include "memory_watch.c"
#include "memory_realloc.c"
#include "memory_malloca.c"
#include "memory_vrealloc.c"
