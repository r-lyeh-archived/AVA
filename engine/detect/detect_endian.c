// endianness

#ifndef ENDIAN_H
#define ENDIAN_H

#if !defined(_WIN32) && !defined(__APPLE__)
#   include <endian.h>
#endif

#if (defined __BYTE_ORDER__ && __BYTE_ORDER__==__BIG_ENDIAN) || defined __BIG_ENDIAN__ || \
    (defined __ARMEB__ || defined __THUMBEB__ || defined __MIPSEB__)
    enum { is_little = 0 };
#else
    enum { is_little = 1 };
#endif

// architecture size

#if INTPTR_MAX >= INT64_MAX
    enum { is_64bit = 1 };
#else
    enum { is_64bit = 0 };
#endif

// conversion utils

API uint16_t swap16( uint16_t x );
API uint32_t swap32( uint32_t x );
API uint64_t swap64( uint64_t x );

API uint16_t swapifle16( uint16_t x );
API uint32_t swapifle32( uint32_t x );
API uint64_t swapifle64( uint64_t x );

API uint16_t swapifbe16( uint16_t x );
API uint32_t swapifbe32( uint32_t x );
API uint64_t swapifbe64( uint64_t x );

#endif

#ifdef ENDIAN_C
#pragma once

#if INTPTR_MAX >= INT64_MAX
typedef char static_assert_64bit[ (sizeof(void *) == 8) ];
#else
typedef char static_assert_32bit[ (sizeof(void *) == 4) ];
#endif

uint16_t swap16( uint16_t x ) { return (x<<8)|(x>>8); }
uint32_t swap32( uint32_t x ) { return (x<<24)|(x>>24)|((x&0xff00)<<8)|((x>>8)&0xff00); }
uint64_t swap64( uint64_t x ) { return (x<<56)|(x>>56)|((x&0xff00)<<40)|((x>>40)&0xff00)|((x&0xff0000)<<24)|((x>>24)&0xff0000)|((x&0xff000000)<<8)|((x>>8)&0xff000000); }

uint16_t swap16ifle( uint16_t x ) { return is_little ? swap16(x) : x; }
uint32_t swap32ifle( uint32_t x ) { return is_little ? swap32(x) : x; }
uint64_t swap64ifle( uint64_t x ) { return is_little ? swap64(x) : x; }

uint16_t swap16ifbe( uint16_t x ) { return !is_little ? swap16(x) : x; }
uint32_t swap32ifbe( uint32_t x ) { return !is_little ? swap32(x) : x; }
uint64_t swap64ifbe( uint64_t x ) { return !is_little ? swap64(x) : x; }

/*
uint16_t ntoh16( uint16_t x ) { return is_little ? swap16(x) : x; }
uint32_t ntoh32( uint32_t x ) { return is_little ? swap32(x) : x; }
uint64_t ntoh64( uint64_t x ) { return is_little ? swap64(x) : x; }

uint16_t hton16( uint16_t x ) { return is_little ? swap16(x) : x; }
uint32_t hton32( uint32_t x ) { return is_little ? swap32(x) : x; }
uint64_t hton64( uint64_t x ) { return is_little ? swap64(x) : x; }
*/

#endif

#ifdef DEMO_ENDIAN
#pragma once
int main() {
    printf("is_little %d\n", is_little);
    printf("is_64bit %d\n", is_64bit);
    printf("%x %x\n", 0x1234, swap16(0x1234), swap16ifle(0x1234), swap16ifbe(0x1234));
}
#endif
