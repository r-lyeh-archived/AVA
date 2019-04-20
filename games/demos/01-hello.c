#include <engine.h>

EXPORT
int main(int argc, char **argv) {
    window_create(0.25f, 0);

    while( window_update() ) {
        ddraw_printf("%s", window_stats());
        ddraw_printf("basic window creation");
        window_swap(NULL);
    }
}
