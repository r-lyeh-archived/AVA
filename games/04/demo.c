#include <engine.h>

#define NK_ASSERT ASSERT
#include <time.h>
#include "3rd/nuklear.h"

int main() {
    // SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    window_create(50, 0);
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //|SDL_WINDOW_ALLOW_HIGHDPI);

    extern struct nk_context *ui_ctx;

    while (window_update()) {
        ui_demo();

        // swap & capture
        static void *pixels = 0;
        window_swap( &pixels );

        // send capture to remote view
        network_sendbuf( pixels, window_width(), window_height(), 3, 7755 );
    }
}

