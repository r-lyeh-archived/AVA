// @todo 1: dir_registry as array(char*)
// @todo 2: import(), set unsorted flag = 1
// @todo 3: asset(), if unsorted flag { clear flag && qsort() array of const char *files }
// @todo 4: asset(), bsearch() array of const char *files

#ifndef VFS_H
#define VFS_H

API void            vfs_import( const char *pathmask );
API char *          vfs_read( const char *file );
API const char *    vfs_find( const char *file );
API const char *    vfs_find_fuzzy( const char *file ); // const char ** instead?

#endif

// ----------------------------------------------------------------------------

#ifdef VFS_C
#pragma once
#include "engine.h" // detect, realloc
#include "filesys_dir.c"
typedef struct asset_counter {
    int hits;
    int bytes;
    int timestamp_last_access;
} asset_counter;
#include <stdio.h>
#include <string.h>
static char *dir_registry = 0;
static bool vfs_changed = 0;
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
    vfs_changed = 1;
}
const char *vfs_find( const char *file ) {
    char *found = dir_registry ? strstr(dir_registry, va("%s\n", file)) : 0;
    if( found ) {
        for( char *back = found; back > dir_registry; --back ) {
            if( *back == '\n' ) {
                return va("%.*s", (int)(found + strlen(file) - (back + 1)),back + 1);
            }
        }
    }
    printf("warning: cannot find '%s' in vfs registry\n", file, dir_registry ? dir_registry : "");
    return file;
}
char *vfs_read( const char *file ) {
    return file_read( vfs_find(file) );
}
void vfs_debug() {
    if( dir_registry ) {
        puts( dir_registry );
        puts( "---" );
    }
}

#include "../struct/struct_map.c"
#include "../struct/struct_hash.c"
#include "../struct/struct_cmp.c"

static int vfs_scmp(char *a, char *b) {
    return strcmp((const char *)a, (const char*)b);
}
static uint64_t vfs_shash(char *key) {
    // compute fast hash. faster than fnv64, a few more collisions though.
    const unsigned char *buf = (const unsigned char *)key;
    uint64_t hash = 0;
    while( *buf ) {
        hash = (hash ^ *buf++) * 131;
    }
    return hash;
}

const char *vfs_find_fuzzy( const char *file ) {
    static map_t(char*,char*) init = {0}, *map = 0;
    if( !map ) {
        map = &init;
        map_create(map, vfs_scmp, vfs_shash);
    }

    // rebuild map(string,string) here
    if( vfs_changed ) {
        vfs_changed = 0;

        // clear previous data (if any)
        map_foreach(map,char*,k,char*,v) {
            free(k);
            free(v);
        }
        map_clear(map);

        // iterate registry & insert lookups
        for( char *index = dir_registry, *next = strstr(index, "\n"); index[0]; ) {
            char url[512];
            sprintf(url, "%.*s", next ? (int) (next - index) : (int) strlen(index), index);

            if (url[0] && url[strlen(url)-1] != '/') { // files only
                TEMP char *norm = string_normalize(url);
                char *k = strdup(norm), *v = strdup(url);
                map_insert(map, k, v);
            }

            next = strstr(index = next + 1, "\n");
        }
    }

    // registry search: def_abc.tga -> ./dir/def_abc.tga
    TEMP const char *registry = vfs_find( file );
    if( strcmp(registry, file) ) return file;

    // not found? try normalized search: folder/def_abc.tga -> abc_def_folder
    TEMP char *candidate = string_normalize(file);
    char **found = map_find(map, candidate);
    if( found ) return *found;

    // not found? try fuzzy search: def_abc.tga -> did you mean default_abc.tga ?
    int record = 0;
    map_foreach(map, char*, k, char*, v) {
        int score = string_fuzzy_score(file, v[0] == '.' && v[1] == '/' ? v+2 : v /* skip ./ */);
        // printf("%s vs %s => score %d\n", file, v+2, score);
        if (score > record) {
            record = score;
            candidate = v;
        }
    }
    return record > 0 ? candidate : file;
/*
    static THREAD_LOCAL const char *candidates[16] = {0};
    candidates[0] = found ? *found : 0;
    candidates[1] = 0;
    return (const char **)candidates;
*/
}


#ifdef VFS_DEMO
int main() {
    vfs_import( "**.c" );
    vfs_import( "**.h" );

    vfs_debug();

    puts( vfs_find( "filesys_vfs.c") );
    puts( vfs_find( "non_existing_file.c" ) );

    puts( vfs_find_fuzzy("filesys_vfs3.cpp") );
}
#endif

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

