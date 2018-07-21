// # dir ######################################################################
// @todo: all paths that start with '/' are absolute (relative otherwise).
// @todo: all paths that end with '/' are directories (file otherwise).
// @todo: dir_rmrf() ? dirchi() ?

#ifndef DIR_H
#define DIR_H
#include <stdbool.h>

API bool dir_ch( const char *pathdir );
API bool dir_mk( const char *pathdir );
API bool dir_mv( const char *pathdirsrc, const char *pathdirdst );
API bool dir_rm( const char *pathdir );

API bool dir_isabs(const char *pathfile);
API char *dir_abs( const char *pathfile);

API char *dir_fix( const char *pathfile);
API char *dir_path( const char *pathfile );
API char *dir_base( const char *pathfile );
API char *dir_name( const char *pathfile );
API char *dir_ext( const char *pathfile );
API char *dir_up( const char *pathfile );

API int dir_ls( const char *pathmask, bool (*yield)(const char *name) );
API int dircmp( const char *pathfile1, const char *pathfile2 );

#endif

#ifdef DIR_C
#pragma once
#include "../core/detect.c"
#include "../core/builtin.c"
#include "../core/realloc.c"
#include "../ava.h"
//char *va( const char *, ... );
//#include <inttypes.h>
#include <errno.h>    // EEXIST
#include <stdio.h>    // sprintf
#include <stdlib.h>   // realpath
#include <string.h>   // strrchr, strstr
#if WIN
#include <winsock2.h> // FindFirstFileA
//#include <direct.h>
#else
#include <dirent.h>   // opendir
#include <unistd.h>
#endif

#ifdef _MAX_PATH
#  define DIR_MAX _MAX_PATH
#elif defined MAX_PATH
#  define DIR_MAX MAX_PATH
#else
#  define DIR_MAX 260
#endif

#define chdir8(path)               IF(WIN, _wchdir(dir_widen(path))                  , chdir(path) )
#define mkdir8(path, mode)         IF(WIN, _wmkdir(dir_widen(path))                  , mkdir(path, mode) )
#define rmdir8(path)               IF(WIN, _wrmdir(dir_widen(path))                  , rmdir(path)       )
//#define realpath8(path,mp,buf,p) IF(WIN, GetFullPathNameW(strwiden(path),mp,buf,p), realpath(*pathfile, absolute) )

#if WIN
#include <winsock2.h>
#include <shlobj.h>
wchar_t *dir_widen(const char *utf8) { // wide strings (windows only)
    static builtin(thread) wchar_t bufs[4][260];
    static builtin(thread) int index = 0;

    int sz = (int)sizeof(bufs[0]);
    wchar_t *buf = bufs[(++index) % 4];

    int needed = 2 * MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    buf[0] = 0;
    if( needed < sz ) {
        MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (void*)buf, needed);
    }
    return (wchar_t *)buf;
}
#endif

bool dir_ch( const char *pathdir ) {
    return chdir8(pathdir) >= 0;
}
bool dir_mv( const char *pathdirsrc, const char *pathdirdst ) {
    return rename( pathdirsrc, pathdirdst ) >= 0;
}
bool dir_mk( const char *pathdir ) {
    int result = mkdir8( pathdir, 0755 );
    return result == 0 || errno == EEXIST ? true : false;
}
bool dir_rm( const char *pathdir ) {
    return rmdir8( pathdir ) >= 0;
}

char *dir_fix(const char *pathfile) {
    char *out = va("%s", pathfile);
    // fix slash
    for( char *p = out; *p; ++p ) if(*p == '\\') *p = '/';
    // fix cases
    for( char *p = out; *p; ++p ) if(*p >= 'A' && *p <= 'Z') *p = *p - 'A' + 'a';
    // fix sandbox
    // SANDBOX: if( out[0] && out[1] == ':' ) out[1] = out[0], out[0] = '/';
    return out;
}

char *dir_path( const char *pathfile ) {
    char *dir = dir_fix( pathfile );
    char *s = strrchr( dir, '/' );
    return s ? (s[1] = '\0', dir) : (dir[0] = '\0', dir);
}
char *dir_base( const char *pathfile ) {
    char *dir = dir_fix( pathfile );
    char *s = strrchr( dir, '/' );
    return s ? s+1 : dir;
}
char *dir_name( const char *pathfile ) {
    char *dir = dir_base( pathfile );
    char *t = strstr( dir, "." );
    if( t ) *t = 0;
    return dir;
}
char *dir_ext( const char *pathfile ) {
    char *dir = dir_base( pathfile );
    char *t = strstr( dir, "." );
    return t ? t : dir;
}

char *dir_abs( const char *pathfile ) {
    char out[DIR_MAX+1] = {0};
#if WIN
    _fullpath(out, pathfile, DIR_MAX);
#else
    realpath(pathfile, out);
#endif
    return dir_fix(out);
}

int dircmp( const char *pathfile1, const char *pathfile2 ) {
    char *p1 = dir_abs(pathfile1);
    char *p2 = dir_abs(pathfile2);
    return strcmp( p1, p2 );
}

bool dir_isabs(const char *pathfile) {
    // SANDBOX: return pathfile[0] == '/';
    if( pathfile[0] == '/' ) return 1;
    if( pathfile[0] && pathfile[1] == ':' ) return 1;
    return 0;
}

char *dir_up(const char *pathfile) {
    char *r = dir_abs( va("%s/../", dir_path(dir_abs(pathfile))) );
    return r;
}


// ---
#include <string.h> // strrchr, strstr
#include <stdio.h>  // sprintf
#include <stdlib.h> // realloc
#ifdef _WIN32
#include <winsock2.h>
#else
#include <dirent.h>
#endif
static char *ls_strcat(char **buf, const char *a, const char *b, const char *c) {
    int len = strlen(a) + (b ? strlen(b) : 0) + (c ? strlen(c) : 0);
    sprintf(*buf = (char *)REALLOC( *buf, len + 1 ), "%s%s%s", a, b ? b : "", c ? c : "");
    return *buf;
}
static int ls_strmatch( const char *text, const char *pattern ) {
    if( *pattern=='\0' ) return !*text;
    if( *pattern=='*' )  return ls_strmatch(text, pattern+1) || (*text && ls_strmatch(text+1, pattern));
    if( *pattern=='?' )  return *text && (*text != '.') && ls_strmatch(text+1, pattern+1);
    return (*text == *pattern) && ls_strmatch(text+1, pattern+1);
}
// requires: source folder to be a path; ie, must end with '/'
// requires: match pattern to be '*' at least
// yields: matching entry, ends with '/' if dir.
// returns: number of matching entries
static int ls_recurse( int recurse, const char *src, const char *pattern, bool (*yield)(const char *name) ) {
    char *dir = 0;
    int count = 0;
#ifdef _WIN32
    WIN32_FIND_DATAA fdata;
    for( HANDLE h = FindFirstFileA( ls_strcat(&dir, src, "*", 0), &fdata ); h != INVALID_HANDLE_VALUE; FindClose( h ), h = INVALID_HANDLE_VALUE ) {
        for( int next = 1; next; next = FindNextFileA( h, &fdata ) != 0 ) {
            if( fdata.cFileName[0] != '.' ) {
                int is_dir = (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
                ls_strcat(&dir, src, fdata.cFileName, is_dir ? "/" : "");
                if( ls_strmatch( fdata.cFileName, pattern ) ) {
                    if( yield( dir ) <= 0 ) return FREE(dir), count;
                }
                count += recurse && is_dir ? ls_recurse( recurse, dir, pattern, yield ) : 1;
            }
        }
    }
#else
    for( DIR *tmp, *odir = opendir( src ); odir; closedir( odir ), odir = 0 ) {
        for( struct dirent *ep; ep = readdir( odir ); ) {
            if( ep->d_name[0] != '.' ) {
                int is_dir = 0 != (tmp = opendir( ep->d_name)) ? (closedir( tmp ), 1) : 0;
                ls_strcat(&dir, src, fdata.cFileName, is_dir ? "/" : "");
                if( ls_strmatch( ep->d_name, pattern ) ) {
                    if( yield( dir ) <= 0 ) return FREE(dir), count;
                }
                count += recurse && is_dir ? ls_recurse( recurse, dir, pattern, yield ) : 1;
            }
        }
    }
#endif
    return FREE(dir), count;
}
int dir_ls(const char *pathmask, bool (*yield)(const char *name) ) {
    if( pathmask[0] == '/' ) return 0; // sandboxed: deny absolute paths
    if( pathmask[1] == ':' ) return 0; // sandboxed: deny absolute paths

    char path_[256] = "./", mask_[256];
    const char *slash = strrchr(pathmask, '/');
    if( slash ) sprintf(path_, "%.*s/", (int)(slash - pathmask), pathmask);
    sprintf(mask_, "%s", slash ? slash + 1 : pathmask);
    if(!mask_[0]) sprintf(mask_, "%s", "*");
    //printf("path=%s mask=%s\n", path_, mask_);

    return ls_recurse( !!strstr(pathmask, "**"), path_, mask_, yield );
}
#endif


#ifdef DIR_DEMO
#include <assert.h>
#include <stdio.h>
bool bputs( const char *file ) {
    return puts(file) < 0 ? false : true;
}
int main() {
    assert( dir_mk( "hello" ) );
    assert( dir_mv( "hello", "hello2" ) );
    assert( dir_rm( "hello2" ) );

    dir_mk("dirtest");
    assert( 0 != dircmp("./1.txt", "./2.txt") );
    assert( 0 == dircmp("./1.txt", "./dirtest/../1.txt") );

    dir_ch("dirtest");
    dir_ls( "../**", bputs );
    puts("---");

    const char *test;

    test = "..\\";
    printf("dir: %s (isabs:%d)\n", test, dir_isabs(test));
    printf("abs: %s (isabs:%d)\n", dir_abs(test), dir_isabs(dir_abs(test)));
    printf("path: %s\n", dir_path(dir_abs(test)));
    printf("base: %s\n", dir_base(dir_abs(test)));
    printf("name: %s\n", dir_name(dir_abs(test)));
    printf("ext: %s\n", dir_ext(dir_abs(test)));
    puts("---");

    test = __FILE__;
    printf("dir: %s (isabs:%d)\n", test, dir_isabs(test));
    printf("abs: %s (isabs:%d)\n", dir_abs(test), dir_isabs(dir_abs(test)));
    printf("path: %s\n", dir_path(dir_abs(test)));
    printf("base: %s\n", dir_base(dir_abs(test)));
    printf("name: %s\n", dir_name(dir_abs(test)));
    printf("ext: %s\n", dir_ext(dir_abs(test)));

}
#endif
