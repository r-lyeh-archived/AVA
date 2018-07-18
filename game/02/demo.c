/*
#include <AVA/ava.h>
#include <AVA/ogl.h>
*/

#include "AVA.h"

#include <stdio.h>

EXPORT
int main(int argc, char **argv) {
    puts(";; hello from game "
        // IF(WIN, "(win)", "(not win)")
        // IF(MSC, "(msc)", "(not msc)")
    );

    window_create(0.85f, 0);

    int w, h;
    void *pixels = 0;
    while( window_update(&w, &h) ) {
        window_swap(&pixels);
    }
}

