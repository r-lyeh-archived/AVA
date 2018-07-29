#ifndef HEXDUMP_H
#define HEXDUMP_H

API void hexdump( const void *ptr, int len );

#endif

#ifdef HEXDUMP_C
#pragma once
#include <stdio.h>
#include "debug_logger.c"

void hexdump( const void *ptr, int len ) {
    int width = 16;
    char hexbuf[256], strbuf[256], *data = (char*)ptr;
    for( int jt = 0; jt < len; jt += width ) {
        char *hex = hexbuf, *str = strbuf;
        for( int it = jt, next = it + width; it < len && it < next; ++it, ++data ) {
            hex += sprintf( hex, "%02x ", /*"0x%02x,",*/ (unsigned char)*data);
            str += sprintf( str, "%c", *data >= 32 && *data != '\\' ? *data : '.' );
        }
        LOG( HEX|DUMP, "%s%*.s// %06x %s", hexbuf, (int)(width * 3 /*5*/ - (hex - hexbuf)), hexbuf, jt, strbuf );
    }
}

#endif

#ifdef HEXDUMP_DEMO
int main() {
    // log_add(stdout,16384);
    hexdump( "hello world", sizeof("hello world"));
    hexdump( "there's a lady who's sure all that glitters is gold...", sizeof("there's a lady who's sure all that glitters is gold..."));
}
#endif
