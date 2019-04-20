#include "engine.h"

EXPORT
int main(int argc, char **argv) {
    window_create(0.25f, 0);

    audio_init(0);
    audio bgm = audio_loadstream( vfs_find("wrath_of_the_djinn.xm") );
    audio_play(bgm, 0);

    while( window_update() ) {
        ddraw_printf(window_stats());
        ddraw_printf("audio playing '%s'", "Wrath of the Djinn");
        window_swap(NULL);
    }
}

