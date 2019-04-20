#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef STRUCT_C
#pragma once
#define ARRAY_C
#define CMP_C
#define HASH_C
#define HASHMAP_C
#define MAP_C
#endif

#ifndef API
#define API
#endif

#include "struct_array.c"
#include "struct_hash.c"
#include "struct_hashmap.c"
#include "struct_cmp.c"
#include "struct_map.c"
