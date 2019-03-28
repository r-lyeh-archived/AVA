// based on code by niklas gray (likely public domain)
// - rlyeh, public domain.

#pragma once
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../detect/detect_inline.c"
#include "../memory/memory_realloc.c"
#include "struct_array.c"

typedef struct hashmap_t {
    uint64_t *keys;
    uint32_t *positions;
    unsigned n;
} hashmap_t;

static const uint64_t HASH_UNUSED = (uint64_t)(-1);

static inline void hashmap_create(hashmap_t *h, unsigned max_elems) {
    ++max_elems;
    h->keys = (uint64_t*)REALLOC( 0, sizeof(uint64_t) * max_elems );
    h->positions = (uint32_t*)REALLOC( 0, sizeof(uint32_t) * max_elems );
    h->n = max_elems;

    for( int i = 0; i < h->n; ++i ) {
        h->keys[i] = HASH_UNUSED;
    }
}
static inline void hashmap_destroy(hashmap_t *h) {
    REALLOC(h->keys, 0), h->keys = 0;
    REALLOC(h->positions, 0), h->positions = 0;
    h->n = 0;
}
static inline void hashmap_insert(hashmap_t *h, uint64_t key, uint32_t position) {
    uint32_t i = key % h->n;
    while (h->keys[i] != key && h->keys[i] != HASH_UNUSED) {
        i = (i + 1) % h->n;
    }
    h->keys[i] = key;
    h->positions[i] = position;
}
static inline uint32_t* hashmap_find(const hashmap_t *h, uint64_t key) {
    uint32_t i = key % h->n;
    while (h->keys[i] != key && h->keys[i] != HASH_UNUSED) {
        i = (i + 1) % h->n;
    }
    return h->keys[i] == HASH_UNUSED ? 0 : &h->positions[i];
}


#define hashmap(K,V) \
    struct { hashmap_t base; array(V) values; uint32_t *at; V *ptr, tmp; uint64_t (*hash)(K); /* int (*cmp)(V,V); // @todo*/ }

#define hashmap_create(h, max_elems, hashfn) ( \
    hashmap_create(&(h)->base, max_elems), \
    (h)->values = 0, /*array_resize((h)->values, (max_elems)),*/ \
    (h)->at = 0, \
    (h)->ptr = 0, \
    (h)->hash = hashfn \
    )

#define hashmap_destroy(h) ( \
    hashmap_destroy(&(h)->base), \
    array_free((h)->values) \
    )

#define hashmap_insert(h, key, value) ( \
    (h)->tmp = (value), \
    array_push((h)->values, (h)->tmp), \
    hashmap_insert(&(h)->base, (h)->hash(key), array_count((h)->values) - 1), \
    &(h)->tmp \
    )

#define hashmap_find(h, key) ( \
    ((h)->at = hashmap_find(&(h)->base, (h)->hash(key))), \
    ((h)->ptr = (h)->at ? array_data((h)->values) + *(h)->at : NULL) \
    )


#ifdef HASHMAP_DEMO
#include <assert.h>
#include <stdio.h>
#include <time.h>

uint64_t hash_int( int i) {
    return i;
}

uint64_t hash_str(const char *str) {
    uint64_t hash = 0;
    while( *str ) {
        hash = (hash ^ (unsigned char)*str++) * 131;
    }
    return hash;
}

void benchmark() {
    #ifndef N
    #define N 10000000
    #endif

    hashmap(int,int) m;
    hashmap_create(&m, N, hash_int);

    clock_t t0 = clock();

    for( int i = 0; i < N; ++i ) {
        hashmap_insert(&m, i, i+1);
    }
    for( int i = 0; i < N; ++i ) {
        uint32_t *v = hashmap_find(&m, i);
        assert( v && *v == i + 1 );
    }

    double t = (clock() - t0) / (double)CLOCKS_PER_SEC;

    printf("[0]=%d\n", *hashmap_find(&m, 0));
    printf("[N-1]=%d\n", *hashmap_find(&m, N-1));
    printf("%d ops in %5.3fs = %fM ops/s\n", (N*2), t, (N*2) / (1e6 * t) );

    hashmap_destroy(&m);
}

int main() {
    hashmap(char*,double) m;

    hashmap_create(&m, 4, hash_str);

    hashmap_insert(&m, "hello", 101.1);
    hashmap_insert(&m, "world", 102.2);
    hashmap_insert(&m, "nice", 103.3);
    hashmap_insert(&m, "hash", 104.4);

    assert(!hashmap_find(&m, "random"));

    assert(hashmap_find(&m, "hello"));
    assert(hashmap_find(&m, "world"));
    assert(hashmap_find(&m, "nice") );
    assert(hashmap_find(&m, "hash") );

    assert( 101.1 == *hashmap_find(&m, "hello"));
    assert( 102.2 == *hashmap_find(&m, "world"));
    assert( 103.3 == *hashmap_find(&m, "nice"));
    assert( 104.4 == *hashmap_find(&m, "hash"));

    hashmap_destroy(&m);

    // ---

    benchmark();
}
#endif
