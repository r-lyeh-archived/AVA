API void network_sendbuf( void *pixels, int w, int h, int comp, int rgb_out );


#ifdef SENDBUF_C
#pragma once
#include "../memory/memory_realloc.c"
#include "network.c"
#include "network_oscpack.c"

void network_sendbuf( void *pixels, int w, int h, int comp, int outfmt ) {
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
    int r_stride = w * comp;
    int w_stride = w * (outfmt == 888 ? 3 : 1);

    line = line_b + r_stride; 
    if( outfmt == 888 ) for( int y = 0; y < h; ++y ) { line -= r_stride;
        int osclen = osc_pack( oscbuf, "/render/", "iiib", w,h,y, w_stride,line);
        bool sent = osc_send( s, oscbuf + 4, osclen - 4 );
        num_sent_packets += sent; sending |= sent;
    }
    if( outfmt == 332 ) for( int y = 0; y < h; ++y ) { line -= r_stride;
        for( int x = 0; x < r_stride; ++x ) {
            unsigned char r = line[x*3+0], g = line[x*3+1], b = line[x*3+2];
            line[x] = (( r >> 5 ) << 5) | (( g >> 5 ) << 2) | (( b >> 6 ) << 0);
        }
        int osclen = osc_pack( oscbuf, "/render/", "iiib", w,h,y, w_stride,line);
        bool sent = osc_send( s, oscbuf + 4, osclen - 4 );
        num_sent_packets += sent; sending |= sent;
    }
    if( outfmt == 242 ) for( int y = 0; y < h; ++y ) { line -= r_stride;
        for( int x = 0; x < r_stride; ++x ) {
            unsigned char r = line[x*3+0], g = line[x*3+1], b = line[x*3+2];
            line[x] = (( r >> 6 ) << 6) | (( g >> 4 ) << 2) | (( b >> 6 ) << 0);
        }
        int osclen = osc_pack( oscbuf, "/render/", "iiib", w,h,y, w_stride,line);
        bool sent = osc_send( s, oscbuf + 4, osclen - 4 );
        num_sent_packets += sent; sending |= sent;
    }

    activity += sending;
    // printf("\r%c netsend %d", "\\|/-"[ activity % 4 ], num_sent_packets);
    // stbi_write_png("out.png", w, h, 3, pixels, w * 3);
}

#endif
