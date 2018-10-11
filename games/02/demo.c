#include "engine.h"
#include <stdio.h>

EXPORT
int main(int argc, char **argv) {
    puts(";; hello from game 02");

    window_create(0.25f, 0);

    int w, h;
    void *pixels = 0;
    while( window_update(&w, &h) ) {
        window_swap(&pixels);
    }

    window_destroy();
}
