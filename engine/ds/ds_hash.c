#ifndef HASH_H
#define HASH_H

API uint64_t    crc64       (const void *ptr, size_t len);
API uint64_t    ptr64       (const void *addr);
API uint64_t    str64       (const char *str);

API uint64_t crc64(const void *ptr, size_t size);
API uint64_t dbl64(double dbl);
API uint64_t num64(uint64_t key);
API uint64_t ptr64(const void *ptr);
API uint64_t str64(const char* str);
API uint64_t vec64(int pt[3]);

#endif

#ifdef HASH_C
#pragma once

uint64_t crc64(const void *ptr, size_t size) {
    // crc64-jones based on public domain code by Lasse Collin
    // use poly64 0xC96C5795D7870F42 for crc64-ecma
    static uint64_t crc64_table[256];
    static uint64_t poly64 = UINT64_C(0x95AC9329AC4BC9B5);
    if( poly64 ) {
        for( int b = 0; b < 256; ++b ) {
            uint64_t r = b;
            for( int i = 0; i < 8; ++i ) {
                r = r & 1 ? (r >> 1) ^ poly64 : r >> 1;
            }
            crc64_table[ b ] = r;
            //printf("%016llx\n", crc64_table[b]);
        }
        poly64 = 0;
    }
    const uint8_t *buf = (const uint8_t *)ptr;
    uint64_t crc = ~0ull; // ~crc;
    while( size != 0 ) {
        crc = crc64_table[(uint8_t)crc ^ *buf++] ^ (crc >> 8);
        --size;
    }
    return ~crc;
}

uint64_t str64(const char* str) {
   uint64_t hash = 0; // fnv1a: 14695981039346656037ULL;
   while( *str ) {
        hash = ( *str++ ^ hash ) * 131; // fnv1a: 0x100000001b3ULL;
   }
   return hash;
}

uint64_t num64(uint64_t key) {
    /* @todo: 
    // check this hash http://web.archive.org/web/20071223173210/http://www.concentric.net/~Ttwang/tech/inthash.htm
    key = (~key) + (key << 21); // key = (key << 21) - key - 1;
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8); // key * 265
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4); // key * 21
    key = key ^ (key >> 28);
    key = key + (key << 31); */
    /* also,
    key += ~(key << 34);
    key ^=  (key >> 29);
    key += ~(key << 11);
    key ^=  (key >> 14);
    key += ~(key <<  7);
    key ^=  (key >> 28);
    key += ~(key << 26); */
    /* Thomas Wang's 64 bit Mix Function (public domain) http://www.cris.com/~Ttwang/tech/inthash.htm */
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

uint64_t dbl64(double dbl) {
    union { uint64_t i; double d; } u; u.d = dbl;
    return num64( u.i );
}

uint64_t vec64(int pt[3]) {
    uint64_t x = num64(pt[0]);
    uint64_t y = num64(pt[1]);
    uint64_t z = num64(pt[2]);
    return x ^ y ^ z;
}

uint64_t ptr64(const void *ptr) {
    uint64_t key;
    memcpy(&key, ptr, sizeof(uint64_t) );
    return num64( key );
}

#endif
