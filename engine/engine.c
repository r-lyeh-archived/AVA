#include "engine.h"

#ifdef ENGINE_C
#pragma once
#define CORE_C
#define FRAMEWORK_C
#define SERVER_C
#endif

#include "core/core.c"
#include "framework.c"
#include "server.c"

#ifdef _MSC_VER
//#pragma comment(lib, "3rd/SDL2.lib")
//#pragma comment(lib, "3rd/SDL2/lib/x64/SDL2.lib") // _WIN64
//#pragma comment(lib, "3rd/SDL2/lib/x86/SDL2.lib") // _WIN32
#endif


API int init();

static void os_exec_bg( void *cmdline ) {
    os_exec( (const char*)cmdline );
}

static int run( const char *name, const char *proc, bool threaded ) {
    char *dllname = va("%s.dll", name);
    if( file_exist(dllname) ) {
        puts(dllname);
        void (*dllfunc)(void *) = dllquick(dllname, proc);
        if( threaded ) {
            return (detach( dllfunc, 0 ), 0);
        } else {
            return (dllfunc( 0 ), 0);
        }
    }
    char *exename = va("%s.exe", name);
    if( file_exist(exename) ) {
        puts(exename);
        if( threaded ) {
            return (detach( os_exec_bg, (void *)exename ), 0);
        } else {
            return !!os_exec( exename );
        }
    }
    return -1;
}

int init() {

    // log early
    puts("; engine begin. last rebuilt: " __DATE__ " " __TIME__);

    // cwd
    // if( exist("../debug") && exist("../../games/") ) cwd("../../games/");

    // icon

    // vfs
    vfs_import("data/**");
    vfs_import("game/**");
    vfs_import("games/**");
    vfs_import("assets/**");

    // base path
#ifdef _MSC_VER
    _chdir( SDL_GetBasePath() );
#else
    chdir( SDL_GetBasePath() );
#endif

/*
    // optional game launch
    run("game", "main", 0);

    // optional game launch (ranges #0-128, #00-128, #000-128)
    for( int i = 0; i < 128; ++i ) {
        run(va("game%d", i), "main", 0);
        run(va("game%02d", i), "main", 0);
        run(va("game%03d", i), "main", 0);
    }
*/

    // editor launch
    run("editor", "main", 0);

    // quit
    puts("; engine end");
    return 0;
}

// #ifdef _MSC_VER
// #define __thread __declspec(thread)
// #else
// #define __inline inline
// #endif
#include <stdarg.h>
#include <assert.h>
static THREAD_LOCAL char vl_buf[2048];
static THREAD_LOCAL int  vl_idx = 0;
char *vl( const char *fmt, va_list v ) {
    int l = VSNPRINTF(0, 0, fmt, v );
    assert(l >= 0);
    assert(l+1 <= 2048);
    char *dst = vl_buf + (vl_idx + l < 2048 ? vl_idx : 0);
    vl_idx += VSNPRINTF( dst, 2048, fmt, v ) + 1;
    return dst;
}
char *va( const char *fmt, ... ) {
    va_list v;
    va_start(v, fmt);
    char *dst = vl( fmt, v );
    va_end(v);
    return dst;
}
