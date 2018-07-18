// # dll ######################################################################

#ifndef DLL_H
#define DLL_H
#include <stdbool.h>

bool dllopen( int plug_id, const char *filename );
void *dllfind( int plug_id, const char *name );
void dllclose( int plug_id);
void* dllquick( const char *filename, const char *symbol );

#endif

#ifdef DLL_C
#pragma once
#include "../core/system.c"
#include "../core/format.c"
#include "os_exec.c"
#if WIN
#include <io.h>
#else
#include <stdio.h>
#endif

#if WIN
#   include <stdlib.h>
#   include <winsock2.h>
#   define DLLTYPE              HMODULE
#   define dlopen(name,mode)    LoadLibraryA( name )
#   define dlsym(handle,symbol) GetProcAddress( handle, symbol )
#   define dlclose(handle)      0
#else
#   include <dlfcn.h>
#   define DLLTYPE              void*
#endif
#if OSX || IOS
#   include <mach-o/dyld.h>
#endif
bool fexists( const char *filename ) {
#if WIN
    return _access( filename, 0 ) != -1;
#else
    return access( filename, F_OK ) != -1;
#endif
}
static DLLTYPE libraries[256] = {0};
bool dllopen(int plug_id, const char *filename) {
    const char *buf;
    if( fexists(buf = format("%s", filename)) ||
        fexists(buf = format("%s.dll", filename)) ||
        fexists(buf = format("%s.so", filename)) ||
        fexists(buf = format("lib%s.so", filename)) ||
        fexists(buf = format("%s.dylib", filename)) ) {
        filename = buf;
    } else {
        return 0;
    }
#if WIN && !SHIPPING
    // We want this dll likely to be monitored and hot-reloaded, but windows will lock this dll file for writing.
    // Hack: We move the dll to the temp folder and load it from there. This temp dll will be locked instead.
    char outfile[512];
    char *tmpdir = getenv("TMP") ? getenv("TMP") : getenv("TEMP") ? getenv("TEMP") : 0;
    if( tmpdir ) {
        os_exec(format("mkdir %s\\DLL 1> nul 2> nul", tmpdir));
        strcpy(outfile, os_exec(format("echo %s\\DLL\\%%random%%", tmpdir)) );
        os_exec(format("copy /y \"%s\" \"%s.%s\" 1> nul 2> nul", filename, outfile, filename));
        strcpy(outfile, os_exec(format("echo %s.%s", outfile, filename)));
        filename = outfile;
    }
#endif
    libraries[plug_id] = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    return libraries[plug_id] != 0;
}
void *dllfind(int plug_id, const char *name) {
    return dlsym(libraries[plug_id], name);
}
void dllclose(int plug_id) {
    dlclose(libraries[plug_id]);
}
void* dllquick( const char *filename, const char *symbol ) {
    DLLTYPE dll = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
    if( dll != 0 ) {
        return dlsym(dll, symbol);
    }
    return 0;
}
#endif

#ifdef DLL_SRVDEMO
// how-to build a dll (msvc):
// cl.exe /LD <files-to-compile>       , or:
// cl.exe /D_USRDLL /D_WINDLL <files-to-compile> <files-to-link> /link /DLL /OUT:<desired-dll-name>.dll
#include <stdio.h>
__declspec(dllexport)
void hello() {
    puts("Hello from server");
}
#endif

#ifdef DLL_CLIDEMO
#include <stdio.h>
int main() {
    enum {
        MY_SERVER_DLL,
    };
    void (*hello)() = NULL;
    if( dllopen( MY_SERVER_DLL, "server" ) ) { // server.dll, server.so, libserver.so, server.dylib
        hello = dllfind( MY_SERVER_DLL, "hello" );
        if( hello ) {
            hello();
        }
        dllclose( MY_SERVER_DLL );
    }
    puts( hello ? "Ok" : "Error: cannot find any of {server.dll, server.so, libserver.so, server.dylib}");
}
#endif
