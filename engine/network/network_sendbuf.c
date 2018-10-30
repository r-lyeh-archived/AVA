API void network_sendbuf( void *pixels, int w, int h, int comp );


#ifdef SENDBUF_C
#pragma once
#include "../memory/memory_realloc.c"
#include "network.c"
#include "network_oscpack.c"

void network_sendbuf( void *pixels, int w, int h, int comp ) {
    static int s, *init = 0;
    if( !init ) { init = &s; s = osc_open("127.0.0.1", "9000"); }
    if( s <= 0 ) return;

    if( !pixels ) return;
    if( (w * h * comp) <= 0 ) return;

    unsigned char* line_a = (unsigned char *)pixels;
    unsigned char* line_b = (unsigned char *)pixels + (0 + (h - 1) * w) * comp;
    unsigned char* line;

    static unsigned char activity = 0;
    static unsigned num_sent_packets = 0;

    static char *oscbuf = 0;
    if( !oscbuf ) oscbuf = (char*)MALLOC( 8192 * 4 + 64 ); // 8K RGBA32 max + some room for osc headers

    bool sending = 0;

#if 0 // RGB888
    int stride = w * comp;
    line = line_b + stride; for( int y = 0; y < h; ++y ) { line -= stride;
        int osclen = osc_pack( oscbuf, "/render/", "iiib", w,h,y, stride,line);
        bool sent = osc_send( s, oscbuf + 4, osclen - 4 );
        num_sent_packets += sent;

        sending |= sent;
    }
#else // RGB332
    int r_stride = w * 3;
    int w_stride = w * 1;
    line = line_b + r_stride; for( int y = 0; y < h; ++y ) { line -= r_stride;
        for( int x = 0; x < r_stride; ++x ) {
            unsigned char r = line[x*3+0];
            unsigned char g = line[x*3+1];
            unsigned char b = line[x*3+2];
            line[x] = (( r >> 5 ) << 5) | (( g >> 5 ) << 2) | (( b >> 6 ) << 0);
        }
        int osclen = osc_pack( oscbuf, "/render/", "iiib", w,h,y, w_stride,line);
        bool sent = osc_send( s, oscbuf + 4, osclen - 4 );
        num_sent_packets += sent;

        sending |= sent;
    }
#endif

    activity += sending;
    // printf("\r%c netsend %d", "\\|/-"[ activity % 4 ], num_sent_packets);
    // stbi_write_png("out.png", w, h, 3, pixels, w * 3);
}

#endif
