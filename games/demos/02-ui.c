#include <engine.h>

int main() {
    // 50% sized, no flags
    window_create(50, 0);

    while (window_update()) {
        // placeholder, upcoming ui demo
        ui_demo();

        //
        ddraw_printf(window_stats());
        ddraw_printf("in-game UI widgets");

        // swap & capture
        static void *pixels = 0;
        window_swap( &pixels );

        // send capture to remote view
        network_sendbuf( pixels, window_width(), window_height(), 3, 7755 );
    }
}

