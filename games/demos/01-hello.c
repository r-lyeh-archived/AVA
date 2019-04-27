#include <engine.h>

EXPORT
int main(int argc, char **argv) {
    // 50% sized, no flags
    window_create(0.50f, 0);

    while( window_update() ) {
        ddraw_printf("%s", window_stats());
        ddraw_printf("basic window creation");
        window_swap(NULL);
    }
}
