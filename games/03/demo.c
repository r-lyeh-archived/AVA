#include "engine.h"

#include <stdio.h>

EXPORT
int main(int argc, char **argv) {
    puts(";; hello from game 03");

    window_create(0.25f, 0);

    audio_init(0);
    audio bgm = audio_loadstream( vfs_read("wrath_of_the_djinn.xm") );
    audio_play(bgm, 0);

    while( window_update() ) {
        window_swap(NULL);
    }

    audio_drop();
    window_destroy();
}

