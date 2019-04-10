#include "engine.h"

EXPORT
int main(int argc, char **argv) {
    window_create( 25.f /*25%*/, 0 );

    while (window_update()) {
        ddraw_printf("%s", window_timings());
        window_swap( 0 );
    }
}
