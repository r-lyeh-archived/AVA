// generic map<K,V> container.
// ideas from: https://en.wikipedia.org/wiki/Hash_table
// ideas from: https://probablydance.com/2017/02/26/i-wrote-the-fastest-hashtable/
// ideas from: http://www.idryman.org/blog/2017/05/03/writing-a-damn-fast-hash-table-with-tiny-memory-footprints/
// - rlyeh, public domain.

#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <stdbool.h>
#include "struct_array.c"
#include "../memory/memory_realloc.c"

// config
#ifndef MAP_DONT_ERASE
#define MAP_DONT_ERASE 1
#endif

// config
#ifndef MAP_HASHSIZE
#define MAP_HASHSIZE (4096 << 4)
#endif

// internal api

typedef struct pair_t {
    struct pair_t *next;

    uint64_t keyhash;
    void *key;
    void *value;
    void *super;
} pair_t;

typedef struct map_t {
    array(pair_t*) array;
    int (*cmp)(void *, void *);
    uint64_t (*hash)(void *);
} map_t;

API void  map_create(map_t *m);
API void  map_destroy(map_t *m);

API void  map_insert(map_t *m, pair_t *p, void *key, void *value, uint64_t keyhash, void *super);
API void  map_erase(map_t *m, void *key, uint64_t keyhash);
API void* map_find(map_t *m, void *key, uint64_t keyhash);
API int   map_count(map_t *m);
API void  map_gc(map_t *m); // only if using MAP_DONT_ERASE

// ----------------
// public api

#define map_t(K,V) \
    struct { map_t base; struct { pair_t p; K key; V val; } tmp, *ptr; V* tmpval; \
        int (*typed_cmp)(K, K); uint64_t (*typed_hash)(K); }

#define map_create(m, cmpfn, hashfn) ( \
    map_create(&(m)->base), \
    (m)->base.cmp = (int(*)(void*,void*))( (m)->typed_cmp = cmpfn), \
    (m)->base.hash = (uint64_t(*)(void*))( (m)->typed_hash = hashfn ) \
    )

#define map_destroy(m) ( \
    map_destroy(&(m)->base) \
    )

#define map_insert(m, k, v) ( \
    (m)->ptr = map_cast((m)->ptr) REALLOC(0, sizeof((m)->tmp)), \
    (m)->ptr->val = (v), \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    map_insert(&(m)->base, &(m)->ptr->p, &(m)->ptr->key, &(m)->ptr->val, (m)->ptr->p.keyhash, (m)->ptr), \
    &(m)->ptr->val \
    )

#define map_find(m, k) ( \
    (m)->ptr = &(m)->tmp, \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    (m)->ptr = map_cast((m)->ptr) map_find(&(m)->base, &(m)->ptr->key, (m)->ptr->p.keyhash), \
    (m)->ptr ? &(m)->ptr->val : 0 \
    )

#define map_erase(m, k) ( \
    (m)->ptr = &(m)->tmp, \
    (m)->ptr->p.keyhash = (m)->typed_hash((m)->ptr->key = (k)), \
    map_erase(&(m)->base, &(m)->ptr->key, (m)->ptr->p.keyhash) \
    )

#define map_foreach(m,key_t,k,val_t,v) \
    for( int ii = 0; ii < MAP_HASHSIZE; ++ii) \
        for( pair_t *cur = (m)->base.array[ii], *on = cur; cur; on = cur = cur->next ) \
            for( key_t k = *(key_t *)cur->key; on; ) \
                for( val_t v = *(val_t *)cur->value; on; on = 0 )

#define map_clear(m) ( \
    map_clear(&(m)->base) \
    )

#define map_count(m)        map_count(&(m)->base)
#define map_gc(m)           map_gc(&(m)->base)

#ifdef __cplusplus
#define map_cast(t) (decltype(t))
#else
#define map_cast(t) (void *)
#endif

// ----------
// quick helpers

API int      map_strcmp(char *a, char *b);
API uint64_t map_strhash(char *key);

API int      map_intcmp(int a, int b);
API uint64_t map_inthash(int key);

#define map_create_keystr(map) map_create(map, map_strcmp, map_strhash)
#define map_create_keyint(map) map_create(map, map_intcmp, map_inthash)

#endif

// -------------------------------

#if defined MAP_C || defined MAP_DEMO
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

enum { MAP_GC_SLOT = MAP_HASHSIZE };
typedef int map_is_pow2_assert[ !(MAP_HASHSIZE & (MAP_HASHSIZE - 1)) ];

static int map_get_index(uint64_t hkey1) {
    return hkey1 & (MAP_HASHSIZE-1);
}

void (map_create)(map_t* m) {
    map_t c = {0};
    *m = c;

    array_resize(m->array, (MAP_HASHSIZE+1));
    // memset(m->array, 0, (MAP_HASHSIZE+1) * sizeof(m->array[0]) ); // array_resize() just did memset()
}

void (map_insert)(map_t* m, pair_t *p, void *key, void *value, uint64_t keyhash, void *super) {
    p->keyhash = keyhash;
    p->key = key;
    p->value = value;
    p->super = super;

    /* Insert onto the beginning of the list */
    int index = map_get_index(p->keyhash);
    p->next = m->array[index];
    m->array[index] = p;
}

void* (map_find)(map_t* m, void *key, uint64_t keyhash) {
    int index = map_get_index(keyhash);
    for( pair_t *cur = m->array[index]; cur; cur = cur->next ) {
        if( cur->keyhash == keyhash ) {
            char **c = (char **)cur->key;
            char **k = (char **)key;
            if( !m->cmp(c[0], k[0]) ) {
                return cur->super;
            }
        }
    }
    return 0;
}

void (map_erase)(map_t* m, void *key, uint64_t keyhash) {
    int index = map_get_index(keyhash);
    for( pair_t *prev = 0, *cur = m->array[index]; cur; (prev = cur), (cur = cur->next) ) {
        if( cur->keyhash == keyhash ) {
            char **c = (char **)cur->key;
            char **k = (char **)key;
            if( !m->cmp(c[0], k[0]) ) {
                if( prev ) prev->next = cur->next; else m->array[index] = 0;
#if MAP_DONT_ERASE
                /* Insert onto the beginning of the GC list */
                cur->next = m->array[MAP_GC_SLOT];
                m->array[MAP_GC_SLOT] = cur;
#else
                REALLOC(cur,0);
#endif
                return;
            }
        }
    }
}

int (map_count)(map_t* m) {
    int counter = 0;
    for( int i = 0; i < MAP_HASHSIZE; ++i) {
        for( pair_t *cur = m->array[i]; cur; cur = cur->next ) {
            ++counter;
        }
    }
    return counter;
}

void (map_gc)(map_t* m) {
#if MAP_DONT_ERASE
    for( pair_t *next, *cur = m->array[MAP_GC_SLOT]; cur; cur = next ) {
        next = cur->next;
        REALLOC(cur,0);
    }
    m->array[MAP_GC_SLOT] = 0;
#endif
}

void (map_clear)(map_t* m) {
    for( int i = 0; i <= MAP_HASHSIZE; ++i) {
        for( pair_t *next, *cur = m->array[i]; cur; cur = next ) {
            next = cur->next;
            REALLOC(cur,0);
        }
        m->array[i] = 0;
    }
}

void (map_destroy)(map_t* m) {
    (map_clear)(m);

    array_free(m->array);
    m->array = 0;

    map_t c = {0};
    *m = c;
}

int map_strcmp(char *a, char *b) {
#if 1 
    int sa = strlen((const char*)a);
    int sb = strlen((const char*)b);
    return sa<sb ? -1 : sa>sb ? +1 : strncmp((const char*)a, (const char*)b, sa);
#else
    return strcmp((const char*)a, (const char*)b);
#endif
}
uint64_t map_strhash(char *key) { // compute fast hash. faster than fnv64, a few more collisions though.
    const unsigned char *buf = (const unsigned char *)key;
    uint64_t hash = 0;
    while( *buf ) {
        hash = (hash ^ *buf++) * 131;
    }
    return hash;
}

int map_intcmp(int key1, int key2) {
    return key1 - key2;
}
uint64_t map_inthash(int key) { // triple32 hashing https://github.com/skeeto/hash-prospector (unlicensed)
    uint32_t x = (uint32_t)key;
    x ^= x >> 17;
    x *= UINT32_C(0xed5ad4bb);
    x ^= x >> 11;
    x *= UINT32_C(0xac4c1b51);
    x ^= x >> 15;
    x *= UINT32_C(0x31848bab);
    x ^= x >> 14;
    return x;
}

#endif

// -------------------------------

#ifdef MAP_DEMO
#include <stdio.h>
#include <time.h>
#ifdef __cplusplus
#include <unordered_map>
#include <map>
#endif

#ifdef NDEBUG
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#else
#include <assert.h>
#endif

int icmp(int a, int b) {
    return b - a;
}
uint64_t ihash(int k) {
    /* Thomas Wang's 64 bit Mix Function (public domain) http://www.cris.com/~Ttwang/tech/inthash.htm */
    uint64_t key = k;
    key += ~(key << 32);
    key ^=  (key >> 22);
    key += ~(key << 13);
    key ^=  (key >>  8);
    key +=  (key <<  3);
    key ^=  (key >> 15);
    key += ~(key << 27);
    key ^=  (key >> 31);
    return key;
}

int scmp(char *a, char *b) {
    return strcmp((const char *)a, (const char*)b);
}
uint64_t shash(char *key) {
    // compute fast hash. faster than fnv64, a few more collisions though.
    const unsigned char *buf = (const unsigned char *)key;
    uint64_t hash = 0;
    while( *buf ) {
        hash = (hash ^ *buf++) * 131;
    }
    return hash;
}

void benchmark() {
    #ifndef M
    #define M 100
    #endif
    #ifndef N
    #define N 50000
    #endif
    #define BENCH(CREATE,COUNT,INSERT,FIND,ITERATE,ERASE,DESTROY) do { \
        static char **bufs = 0; \
        if(!bufs) { \
            bufs = (char **)REALLOC(0, sizeof(char*) * N ); \
            for( int i = 0; i < N; ++i ) { \
                bufs[i] = (char*)REALLOC(0, 16); \
                sprintf(bufs[i], "%d", i); \
            } \
        } \
        clock_t t0 = clock(); \
        for( int i = 0; i < M; ++i ) { \
            CREATE; \
            if(i==0) printf("CREATE:%d ", (int)COUNT), fflush(stdout); \
            for( int j = 0; j < N; ++j ) { \
                char *buf = bufs[j]; \
                INSERT; \
            } \
            if(i==0) printf("INSERT:%d ", (int)COUNT), fflush(stdout); \
            for( int j = 0; j < N; ++j ) { \
                char *buf = bufs[j]; \
                FIND; \
            } \
            if(i==0) printf("FIND:%d ", (int)COUNT), fflush(stdout); \
            ITERATE; \
            if(i==0) printf("ITERATE:%d ", (int)COUNT), fflush(stdout); \
            for( int j = 0; j < N; ++j ) { \
                char *buf = bufs[j]; \
                ERASE; \
            } \
            if(i==0) printf("REMOVE:%d ", (int)COUNT), fflush(stdout); \
            DESTROY; \
            if(i==0) printf("DESTROY%s", " "); \
        } \
        clock_t t1 = clock(); \
        double t = (t1 - t0) / (double)CLOCKS_PER_SEC; \
        int ops = (M*N)*6; \
        printf("%d ops in %fs = %.2fM ops/s (" #CREATE ")\n", ops, t, ops / (t * 1e6)); \
    } while(0)

    map_t(char*,int) m = {0};
    BENCH(
        map_create(&m, scmp, shash),
        map_count(&m),
        map_insert(&m, buf, i),
        map_find(&m, buf),
        map_foreach(&m,char*,k,int,v) {},
        map_erase(&m, buf),
        map_destroy(&m)
    );

#ifdef __cplusplus
    using std_map = std::map<const char *,int>;
    BENCH(
        std_map v,
        v.size(),
        v.insert(std::make_pair(buf, i)),
        v.find(buf),
        for( auto &kv : v ) {},
        v.erase(buf),
        {}
    );

    using std_unordered_map = std::unordered_map<const char *,int>;
    BENCH(
        std_unordered_map v,
        v.size(),
        v.insert(std::make_pair(buf, i)),
        v.find(buf),
        for( auto &kv : v ) {},
        v.erase(buf),
        {}
    );
#endif
}

void tests() {
    {
        map_t(int,char*) m = {0};
        map_create(&m, icmp, ihash);
            assert( 0 == map_count(&m) );
        map_insert(&m, 123, "123");
        map_insert(&m, 456, "456");
            assert( 2 == map_count(&m) );
            assert( map_find(&m, 123) );
            assert( map_find(&m, 456) );
            assert(!map_find(&m, 789) );
            assert( 0 == strcmp("123", *map_find(&m, 123)) );
            assert( 0 == strcmp("456", *map_find(&m, 456)) );

        map_foreach(&m,const int,k,char*,v) {
            printf("%d->%s\n", k, v);
        }

        map_erase(&m, 123);
            assert(!map_find(&m, 123) );
            assert( 1 == map_count(&m) );
        map_erase(&m, 456);
            assert(!map_find(&m, 456) );
            assert( 0 == map_count(&m) );
        map_destroy(&m);
            assert( puts("ok") >= 0 );
    }

    {
        map_t(char*,int) m = {0};
        map_create(&m, scmp, shash);
            assert( map_count(&m) == 0 );
        map_insert(&m, "123", 123);
        map_insert(&m, "456", 456);
            assert( map_count(&m) == 2 );
            assert( map_find(&m,"123") );
            assert( map_find(&m,"456") );
            assert(!map_find(&m,"789") );

        map_foreach(&m,const char *,k,int,v) {
            printf("%s->%d\n", k, v);
        }

        map_erase(&m, "123");
            assert( 456 == *map_find(&m,"456") );
            assert( map_count(&m) == 1 );
        map_erase(&m, "456");
            assert( map_count(&m) == 0 );
        map_destroy(&m);
            assert( puts("ok") >= 0 );
    }
}

int main() {
    tests();
    puts("---");
    benchmark();
}
#endif
