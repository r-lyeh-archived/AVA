// Function to normalize resource identificators.
// - rlyeh, public domain.
//
// Basically `strnorm(string)` does a string transformation from given string
// to a deterministic pattern, whose hash is guaranteed to remain inmutable (on a
// high degree) even if name of physical source is altered externally.
// 
// Also, normalizing a normalized uri will return the very same value as well.
//
// You usually want to normalize all your asset URIs before hashing the paths
// and storing them in a virtual filesystem.
//
// ## Features
// - Normalize folder/asset separators.
// - Normalize English singular/plurals.
// - Normalize AoS (OO) and SoA (ECS) disk layouts.
// - Normalize absolute, relative, virtual and remote paths.
// - Normalize uppercases, lowercases, whitespaces and hyphens.
// - Normalize extensions, double extensions and double punctuations.
// - Normalize SOV, SVO, VSO, VOS, OVS, OSV subject/verb/object language topologies.
// - Normalize folder/file tagging (useful when globbing and deploying files and/or directories).
// - Normalize previously unified ids (lossless process).
//
// ## Todo
// - Normalize typos on unicode diacritics.
//

TEMP char *string_normalize( const char *uri );


#ifdef NORMALIZE_C
#pragma once

#include "engine.h"
#include <ctype.h>

static
char *strlower( char *s ) {
    for( char *c = s; *c; ++c ) {
        *c = tolower(*c);
    }
    return s;
}

static
char *strremap( char *s, const char *src, const char *dst ) {
    for( char *c = s; *c; ++c ) {
        char *found = strchr(src, *c);
        if( found ) {
            *c = dst[ found - src ];
        }
    }
    return s;
}

static
int string_normalize_strcmp(const void* a, const void* b) {
    return strcmp(*(char* const*)a, *(char* const*)b);
}

static THREAD_LOCAL int   string_normalize_buflevel = 0;
static THREAD_LOCAL char *string_normalize_buf[16] = {0};

TEMP char *string_normalize( const char *uri ) {

#   define with (string_normalize_buflevel[string_normalize_buf])
    string_normalize_buflevel = (string_normalize_buflevel+1) % 16;

    // 0. @todo: unescape url here
    // 1. @todo: convert diacritics into latin characters here (romanization)
    // 2. camelCase to lisp-case

    int i = 0;
    char lbuf[511+1] = { 0 };
    for( char *p = lbuf; *uri && i < 511; ++uri ) {
        if( islower(*uri) ) {
            p[i++] = *uri;
        } else {
            p[i++] = '-';
            p[i++] = tolower(*uri);
        }
    }
    char *lisp = ( lbuf[i] = 0, &lbuf[ lbuf[0] == '-' ] );

    // 3. lowercase
    TEMP char *buf = strlower( strcpyf(&with, "%s", lisp) );

    // 4. remove url options at eos (if any)
    strtrimffe( buf, "?" );

    // 5. split path "\\/" up to 2nd level only
    HEAP char **tokens = strsplit( buf, "\\/" );
    int n = 0; while( tokens[n] ) n++;
    char *file = tokens[ n - 1 ];
    char *path = n >= 2 ? tokens[ n - 2 ] : "";

    // 6. strip all #tags and trim extensions
    strtrimffe(file, ".");
    strtrimffe(file, "#");
    strtrimffe(path, ".");
    strtrimffe(path, "#");
    strcpyf( &with, "%s_%s", path, file );
    FREE( tokens );

    // 7. convert separators to underscores
    strremap( with, " -,|;:()[]", "__________" );

    // 8. split stems
    HEAP char **words = strsplit(with, "_");

    // 9. trim aos/soa plurals
    int w = 0;
    while( words[w] ) {
        int L = strlen(words[w]);
        if( words[w][L-1] == 's' ) words[w][L-1] = 0;
        ++w;
    }

    // 10. sort and join stems
    qsort(words, w, sizeof(char*), string_normalize_strcmp);
    strjoin(&with, words, "_");
    FREE(words);

    return &with[0];
#   undef with
}

#ifdef NORMALIZE_DEMO
#include <assert.h>
#include <stdio.h>
int main() {
    const char *should_be;
    #define test(expr) printf( "[%s] %s\n", !strcmp(string_normalize(expr),should_be) ? " OK ":"FAIL", string_normalize(expr) )
    if( "test absolute, relative, virtual and remote paths" ) {
        should_be = "folder_logo_main";
        test( "~home/game/folder/main logo.jpg" );
        test( "~user/game1/folder/main logo.jpg" );
        test( "~mark/game2/folder/main logo.jpg" );
        test( "~john/game3/data/folder/main logo.jpg" );
        test( "../folder/main logo.jpg" );
        test( "C:\\data\\folder\\main logo.jpg" );
        test( "C:/game/data/folder/main logo.jpg" );
        test( "data.zip/data/folder/main logo.jpg" );
        test( "virtual.rar/folder/main logo.jpg" );
        test( "http://web.domain.com%20/folder/main logo.jpg?blabla=123&abc=123#qwe" );
    }
    if( "test uppercases, lowercases, whitespaces and hyphens" ) {
        should_be = "folder_logo_main";
        test( "folder/main-logo" );
        test( "folder/main_logo" );
        test( "folder/Main logo" );
        test( "folder / Main  logo " );
    }
    if( "test camelCase" ) {
        should_be = "folder_logo_main";
        test( "folderMainLogo" );
        test( "FolderMainLogo" );
        test( "folder/MainLogo" );
        test( "folder/MainLogo" );
    }
    if( "test folder/asset separators" ) {
        should_be = "folder_logo_main";
        test( "folder/main-logo" );
        test( "folder\\main-logo" );
        test( "folder-main-logo" );
        test( "folder_main-logo" );
        test( "folder|main-logo" );
        test( "folder:main-logo" );
        test( "folder;main-logo" );
        test( "folder,main-logo" );
        test( "[folder]main-logo" );
        test( "main-logo(folder)" );
    }
    if( "test extensions" ) {
        should_be = "folder_logo_main";
        test( "folder/main_logo.jpg" );
        test( "folder/main_logo.png" );
        test( "folder/main_logo.webp" );
    }
    if( "test double extensions and double punctuations" ) {
        should_be = "folder_logo_main";
        test( "folder/main logo.bmp.png" );
        test( "folder/main  logo..png" );
    }
    if( "test diacritics" ) {
        // @todo: diacritrics need additional utf8 pass. might be much slower though.
        // should_be = "animation_walk";
        // test( "âñimátïón/wàlk" );
    }
    if( "test AoS (OO) and SoA (ECS) disk layouts" ) {
        should_be = "logo_purple";
        test( "logos/purple" );
        test( "purple/logo" );
        should_be = "kid_sprite";
        test( "sprites/kid" );
        test( "kid/sprite" );
    }
    if( "test SOV, SVO, VSO, VOS, OVS, OSV subject/verb/object language topologies"
        "test English plurals as well" ) {
        should_be = "join_player_scene";
        test( "player-joins-scene.intro" );
        test( "player-scene-join.intro" );
        test( "join-player-scene.intro" );
        test( "join-scene-player.intro" );
        test( "scene-join-player.intro" );
        test( "scene-player-join.intro" );
    }
    if( "test folder/file tagging" ) {
        should_be = "logo_splash";
        test( "splash/logo" );
        test( "/splash #win32/logo" );
        test( "splash #mobile/logo #win32=always.png" );
    }
    if( "test re-normalization consistency" ) {
        should_be = "folder_logo_main";
        test( string_normalize("main/folder-logo") );
        test( string_normalize(string_normalize("main/folder-logo")) );
    }
    assert(~puts("Ok"));
}

#endif
#endif
