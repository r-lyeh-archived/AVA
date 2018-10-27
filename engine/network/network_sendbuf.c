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
    int stride = w * comp;
#if 0
    line = line_a - stride; for( int y = 0; y < h; ++y ) { line += stride;
#else
    line = line_b + stride; for( int y = 0; y < h; ++y ) { line -= stride;
#endif
        {
            int mtu = stride; // > 512 ? 512 : stride;

            int osclen = osc_pack( oscbuf, "/render/", "ib", y, mtu,line);
            bool sent = osc_send( s, oscbuf + 4, osclen - 4 );
            num_sent_packets += sent;

            sending |= sent;
        }
    }

    activity += sending;
    // printf("\r%c netsend %d", "\\|/-"[ activity % 4 ], num_sent_packets);
    // stbi_write_png("out.png", w, h, 3, pixels, w * 3);
}

#endif
