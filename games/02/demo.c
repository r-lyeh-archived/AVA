#include "engine.h"
#include <stdio.h>

EXPORT
int main(int argc, char **argv) {
    puts(";; hello from game 02");

    window_create(0.25f, 0);

    while( window_update() ) {
        window_swap(NULL);
    }

    window_destroy();
}
