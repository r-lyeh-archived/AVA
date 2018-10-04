// @todo 1: dir_registry as array(char*)
// @todo 2: import(), set unsorted flag = 1
// @todo 3: asset(), if unsorted flag { clear flag && qsort() array of const char *files }
// @todo 4: asset(), bsearch() array of const char *files

#ifndef VFS_H
#define VFS_H

API void            vfs_import( const char *pathmask );
API const char *    vfs_read( const char *file );

#endif

// ----------------------------------------------------------------------------

#ifdef VFS_C
#pragma once
#include "ava.h" // detect, realloc
#include "filesys_dir.c"
struct asset_counter {
    int hits;
    int timestamp_last_access;
};
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
void vfs_import( const char *pathmask ) {
    dir_ls( pathmask, append );
}
const char *vfs_read( const char *file ) {
    char *found = dir_registry ? strstr(dir_registry, va("%s\n", file)) : 0;
    if( found ) {
        for( char *back = found; back > dir_registry; --back ) {
            if( *back == '\n' ) {
                return va("%.*s", (int)(found + strlen(file) - (back + 1)),back + 1);
            }
        }
    }
    return file;
}
#endif

#if 0

/*
@todo:
    import(**.pathmask);, (for modules? instead)
or
    include(**.pathmask);, (only for files?)

and

    asset(x)[...] = { xx,xx, 0 };
or,
    file(x)[...] instead?
*/

extern "C" {
char *dirlsi(const char *path);
int  strchop(const char *string, const char *delimiters, int avail, const char *tokens[]);
}

const char**assets( const char *fuzzyname ) {
    static builtin(thread) char *candidates[16] = { 0 };
    candidates[0] = (char*)fuzzyname;
    candidates[1] = 0;
    return (const char **)candidates;
}

void import( const char *pathmask ) {
    char *list = dirlsi(pathmask);
    const char *tokens[128] = {0};
    if( strchop( pathmask, "\r\n", 128, tokens ) ) {
        for( int i = 0; tokens[i]; i += 2 ) {
            printf("%*.s\n", (int)tokens[i*2+0], tokens[i*2+1]);
        }
    }
}

const char**asset( const char *fuzzyname ) {
    static char *candidates[16] = { 0 };
    candidates[0] = (char*)fuzzyname;
    candidates[1] = 0;
    return (const char **)candidates;
}

#endif

#ifdef VFS_DEMO
#include <stdio.h>
int main() {
    vfs_import( "**.c" );
    vfs_import( "**.h" );
    /* debug
    if( dir_registry ) {
        puts( dir_registry );
        puts( "---" );
    }
    */
    puts( vfs_read( "filesys_vfs.c") );
    puts( vfs_read( "non_existing_file.c" ) );
}
#endif
