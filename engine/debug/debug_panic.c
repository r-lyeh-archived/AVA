// # err ######################################################################

// extern API void (*on_panic)( const char *description );
API void die();
API void error( const char *description );
API void fatal( const char *description );
API void panic( const char *description );

#ifdef PANIC_C
#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

#include "debug_logger.c"
#include "debug_breakpoint.c"
#include "debug_dialog.c"

void die() {
    // flush everything
    fflush(NULL);
    // some rest
#ifdef _WIN32
    Sleep(1000);
#else
    sleep(1);
#endif
    // game over
    exit(-1); // also: _Exit(), WIN: ExitProcess(-1),
}

void error( const char *description ) {
    LOG(ERROR, /*"%s",*/ description);
    fflush(NULL);
}

void fatal( const char *description ) {
    error( description );
    die();
}

void panic( const char *description ) {
    error(description);
    if( on_panic ) {
        on_panic( description );
    }
    die();
}

#ifdef _WIN32

void on_panic_windows( const char *description ) {
#ifdef SHIPPING
    dialog( 1, "!Error", description );
#else
    dialog( 1, "!Error (dev hint: attach debugger now).", description );
    if( debugging() ) {
        // can we invoke debugger?
        breakpoint();
        abort();
    }
#endif
}

void (*on_panic)( const char *description ) = on_panic_windows;

#else

void (*on_panic)( const char *description ) = 0;

#endif

#endif


#ifdef PANIC_DEMO
int main() {
    panic("!this is a panic message");
}
#endif
