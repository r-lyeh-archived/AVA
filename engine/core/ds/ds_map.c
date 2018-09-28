// based on code by niklas gray (likely public domain)
// warn: avail capacity must be at least >= 1 slot always

#pragma once
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../detect/detect_inline.c"
#include "../object/object_realloc.c"

static const uint64_t HASH_UNUSED = 0xffffffffffffffffULL;
typedef struct map {
    uint64_t *keys;
    uint32_t *values;
    unsigned n;
} map;
static __inline void map_clear(map *h) {
    memset(h->keys, 0xff, sizeof(*h->keys) * h->n);
}
static __inline void map_new(map *h, unsigned elems) {
    h->keys = (uint64_t*)MALLOC( sizeof(uint64_t) * elems );
    h->values = (uint32_t*)MALLOC( sizeof(uint32_t) * elems );
    h->n = elems;
    map_clear(h);
}
static __inline void map_destroy(map *h) {
    FREE(h->keys), h->keys = 0;
    FREE(h->values), h->values = 0;
    h->n = 0;
}
static __inline void map_set(map *h, uint64_t key, uint32_t value) {
    uint32_t i = key % h->n;
    while (h->keys[i] != key && h->keys[i] != HASH_UNUSED) {
        i = (i + 1) % h->n;
    }
    h->keys[i] = key;
    h->values[i] = value;
}
static __inline uint32_t* map_find(const map *h, uint64_t key) {
    uint32_t i = key % h->n;
    while (h->keys[i] != key && h->keys[i] != HASH_UNUSED) {
        i = (i + 1) % h->n;
    }
    return h->keys[i] == HASH_UNUSED ? 0 : &h->values[i];
}


#ifdef MAP_BENCH
#include <stdio.h>
#include <assert.h>
#include <omp.h>
int main() {
    #ifndef N
    #define N 10000000
    #endif

    double t = -omp_get_wtime();

    map m;
    map_new(&m, N);
    for( int i = 0; i < N; ++i ) {
        map_set(&m, i, i+1);
    }
    for( int i = 0; i < N; ++i ) {
        uint32_t *v = map_find(&m, i);
        assert( v && *v == i + 1 );
    }

    t += omp_get_wtime();

    printf("[0]=%d\n", *map_find(&m, 0));
    printf("[N-1]=%d\n", *map_find(&m, N-1));

    printf("%d ops in %5.3fs = %fM ops/s\n", (N*2), t, ((N*2) / 1000000.0) / t );
}
#endif

#ifdef MAP_DEMO
#include <stdio.h>
static uint64_t hash(const char *str) {
    uint64_t hash = 0;
    while( *str ) {
        hash = (hash ^ (unsigned char)*str++) * 131;
    }
    return hash;
}
int main() {
    map m;
    map_new(&m, 5);

    map_set(&m, hash("hello"), 101);
    map_set(&m, hash("world"), 102);
    map_set(&m, hash("nice"), 103);
    map_set(&m, hash("hash"), 104);

    printf( "%p\n", map_find(&m, hash("hello")), *map_find(&m, hash("hello")) );
    printf( "%p\n", map_find(&m, hash("world")), *map_find(&m, hash("world")) );
    printf( "%p\n", map_find(&m, hash("nice")), *map_find(&m, hash("nice")) );
    printf( "%p\n", map_find(&m, hash("hash")), *map_find(&m, hash("hash")) );

    printf( "%p %p\n", map_find(&m, 0), map_find(&m, hash("undefined")) );
}
#endif
