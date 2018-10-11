#pragma once
#include <stdint.h>

#if INTPTR_MAX >= INT64_MAX // __x86_64__ _M_X64 __ia64__ __powerpc64__ __s390__ __s390x__ __alpha__
#define BITS 64
#else
#define BITS 32
#endif

typedef char static_assert_bits[ BITS == (8 * sizeof(void *)) ];

/*
int main() {
    printf("%d\n", BITS);
}
*/
