#ifdef DS_C
#pragma once
#define VREALLOC_C
#define CMP_C
#define QUARK_C
#define STRING_C
#define HASH_C
#endif

#ifndef API
#define API
#endif

#include "ds_array.c"
#include "ds_map.c"

#include "ds_hash.c"
#include "ds_vrealloc.c"
#include "ds_cmp.c"
#include "ds_quark.c"
#include "ds_string.c"
