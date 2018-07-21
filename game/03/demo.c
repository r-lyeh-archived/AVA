/*
#include <AVA/ava.h>
#include <AVA/ogl.h>
*/

#include "AVA.h"

#include <stdio.h>

EXPORT
int main(int argc, char **argv) {
    puts(";; hello from game 03");

    window_create(0.25f, 0);

    audio_init(0);
    audio bgm = audio_loadstream( asset("wrath_of_the_djinn.xm") );
    audio_play(bgm, 0);

    int w, h;
    void *pixels = 0;
    while( window_update(&w, &h) ) {
        window_swap(&pixels);
    }

    audio_drop();
    window_destroy();
}

