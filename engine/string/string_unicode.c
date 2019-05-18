API array(uint32_t) string32( const char *utf8 );

#ifdef UNICODE_C
#pragma once

static
uint32_t extract_utf32(const char **p) {
    if( (**p & 0x80) == 0x00 ) {
        int a = *((*p)++);
        return a;
    }
    if( (**p & 0xe0) == 0xc0 ) {
        int a = *((*p)++) & 0x1f;
        int b = *((*p)++) & 0x3f;
        return (a << 6) | b;
    }
    if( (**p & 0xf0) == 0xe0 ) {
        int a = *((*p)++) & 0x0f;
        int b = *((*p)++) & 0x3f;
        int c = *((*p)++) & 0x3f;
        return (a << 12) | (b << 6) | c;
    }
    if( (**p & 0xf8) == 0xf0 ) {
        int a = *((*p)++) & 0x07;
        int b = *((*p)++) & 0x3f;
        int c = *((*p)++) & 0x3f;
        int d = *((*p)++) & 0x3f;
        return (a << 18) | (b << 12) | (c << 8) | d;
    }
    return 0;
}

array(uint32_t) string32( const char *utf8 ) {
    array(uint32_t) out = 0;
    //int worstlen = strlen(utf8) + 1; array_reserve(out, worstlen);
    while( *utf8 ) {
        uint32_t unicode = extract_utf32( &utf8 );
        array_push(out, unicode);
    }
    return out;
}

#endif
