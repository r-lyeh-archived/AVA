#ifndef VFS_H
#define VFS_H

API void            import( const char *pathmask );
API const char *    asset( const char *file );

#endif

#ifdef VFS_C
#pragma once
#include "../ava.h"
#include "asset_dir.c"
struct asset_counter {
    int hits;
    int timestamp_last_access;
};
// @todo 1: dir_registry as array(char*)
// @todo 2: import(), set unsorted flag = 1
// @todo 3: asset(), if unsorted flag { clear flag && qsort() array of const char *files }
// @todo 4: asset(), bsearch() array of const char *files
#include <stdio.h>
#include <string.h>
static char *dir_registry = 0;
static bool append( const char *str ) {
    str = strchr(str, ' ') + 1;
    int slen = strlen(str);
    int olen = (dir_registry ? strlen(dir_registry) : 0);
    dir_registry = REALLOC( dir_registry, olen + slen + 2 );
    strcpy( &dir_registry[olen], str );
    dir_registry[olen + slen] = '\n';
    dir_registry[olen + slen + 1] = 0;
    return true;
}
void import( const char *pathmask ) {
    dir_ls( pathmask, append );
}
const char *asset( const char *file ) {
    char *found = strstr(dir_registry, va("%s\n", file));
    if( found ) {
        for( char *back = found; back > dir_registry; --back ) {
            if( *back == '\n' ) {
                return va("%.*s", (int)(found + strlen(file) - (back + 1)),back + 1);
            }
        }
    }
    return file;
}
/*
const char**assets( const char *fuzzyname ) {
    static builtin(thread) char *candidates[16] = { 0 };
    candidates[0] = (char*)fuzzyname;
    candidates[1] = 0;
    return (const char **)candidates;
}
*/
#endif

#ifdef VFS_DEMO
#include <stdio.h>
int main() {
    import( "**.c" );
    import( "**.h" );
    /* debug
    if( dir_registry ) {
        puts( dir_registry );
        puts( "---" );
    }
    */
    puts( asset( "asset_vfs.c") );
    puts( asset( "non_existing_file.c" ) );
}
#endif
