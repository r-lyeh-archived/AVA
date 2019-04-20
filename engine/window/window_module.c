extern void (*on_init[256])(int argc, char ** argv);
extern void (*on_draw[256])(int w, int h);
extern void (*on_step[256])(int t, int dt);
extern void (*on_swap[256])(void *pixels);
extern void (*on_quit[256])(void);
//extern void (*on_menu[256])();

int start(int argc, char **argv);



#ifdef MODULE_C
#pragma once
#include "window.c"

void (*on_init[256])(int argc, char ** argv) = {0};
void (*on_draw[256])(int w, int h) = {0};
void (*on_step[256])(int d, int dt) = {0};
void (*on_swap[256])(void *) = {0};
void (*on_quit[256])(void) = {0};

int start(int argc, char **argv) {
    window_create(0.85f, 0);

    for( int i = 0; i < 256; ++i ) {
        if( on_init[i] ) {
            on_init[i](argc, argv);
        }
    }

    void *pixels = 0;
    while( window_update() ) {
        vec2 rect = window_size();
        int w = (int)rect.w, h = (int)rect.h;

        for( int i = 0; i < 256; ++i ) {
            if( on_step[i] ) {
                on_step[i](0, 0);
            }
        }
        for( int i = 0; i < 256; ++i ) {
            if( on_draw[i] ) {
                on_draw[i](w, h);
            }
        }
        window_swap( &pixels );
        for( int i = 0; i < 256; ++i ) {
            if( on_swap[i] ) {
                on_swap[i](pixels);
            }
        }
    }

    for( int i = 0; i < 256; ++i ) {
        if( on_quit[i] ) {
            on_quit[i]();
        }
    }

    return 0;
}

#endif
