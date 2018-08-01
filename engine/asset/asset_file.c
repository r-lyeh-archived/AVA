// # binary files #############################################################
// @todo: file_lock, file_unlock
// @todo: fopeni()

#ifndef FILE_H
#define FILE_H
#include <stdbool.h>

API char*       file_map( const char *pathfile, size_t offset, size_t len );
API void        file_unmap( char *buf, size_t len );

API int         file_size( const char *pathfile ); // 4gib limit
API int         file_stamp( const char *pathfile ); // Y2038

API bool        file_exist( const char *pathfile );
API bool        file_isdir( const char *pathfile );
API bool        file_isfile( const char *pathfile );
API bool        file_islink( const char *pathfile );

API char*       file_read( const char *pathfile );
API bool        file_write( const char *pathfile, const void *data, int len );
API bool        file_append( const char *pathfile, const void *data, int len );

API bool        file_copy( const char *srcpath, const char *dstpath );
API bool        file_touch( const char *pathfile );
API bool        file_delete( const char *path );

#endif

#ifdef FILE_C
#pragma once
#include "../detect/detect.c"
#include <stdio.h>
#include <sys/stat.h>

#if WIN
#include <io.h>
#else
#include <unistd.h>
#endif

#define open8(path,mode)  IF(WIN, _wopen(file_widen(path))               ,  open(path, mode) )
#define fopen8(path,mode) IF(WIN, _wfopen(file_widen(path),file_widen(mode)), fopen(path,mode) )
#define remove8(path)     IF(WIN, _wremove(file_widen(path))             ,  remove(path)     )
#define rename8(path)     IF(WIN, _wrename(file_widen(path))             ,  rename(path)     )
#define stat8(path,st)    IF(WIN, _wstat(file_widen(path),st)            ,  stat(path,st)    )
#define stat_             IF(WIN, _stat,                                 ,  stat_t           )

#if WIN
#define O_RDONLY 00
// mmap() replacement for Windows. Placed into the public domain (Mike Frysinger)
enum {  PROT_READ = 0x1, PROT_WRITE = 0x2, PROT_EXEC = 0x4,
        MAP_SHARED = 0x01, MAP_PRIVATE = 0x02, MAP_ANON = 0x20, MAP_ANONYMOUS = MAP_ANON };
#define MAP_FAILED    ((void *) -1)
static void* mmap(void* start, size_t length, int prot, int flags, int fd, size_t offset) {
    DWORD flProtect;
    size_t end = length + offset;
    if( !(prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC)) ) {
        if( ( fd == -1 &&  (flags & MAP_ANON) && !offset ) ||
            ( fd != -1 && !(flags & MAP_ANON)            )) {
                 if( prot & PROT_EXEC  ) flProtect = prot & PROT_READ ? PAGE_EXECUTE_READ : PAGE_EXECUTE_READWRITE;
            else if( prot & PROT_WRITE ) flProtect = PAGE_READWRITE;
            else                         flProtect = PAGE_READONLY;
            HANDLE h = CreateFileMapping( fd == -1 ? INVALID_HANDLE_VALUE : (HANDLE)_get_osfhandle(fd),
                NULL, flProtect, (end >> 31) >> 1, (uint32_t)end, NULL);
            if( h != NULL ) {
                DWORD dwDesiredAccess = 0;
                dwDesiredAccess |= prot & PROT_WRITE ? FILE_MAP_WRITE : FILE_MAP_READ;
                dwDesiredAccess |= prot & PROT_EXEC ? FILE_MAP_EXECUTE : 0;
                dwDesiredAccess |= flags & MAP_PRIVATE ? FILE_MAP_COPY : 0;
                void *ret = MapViewOfFile(h, dwDesiredAccess, (offset >> 31) >> 1, (uint32_t)offset, length);
                CloseHandle(h); // close the Windows Handle here (we handle the file ourselves with fd)
                return ret == NULL ? MAP_FAILED : ret;
            }
        }
    }
    return MAP_FAILED;
}
static void munmap(void* addr, size_t length) {
    UnmapViewOfFile(addr);
}
#endif

#if WIN
#include <winsock2.h>
#include <shlobj.h>
wchar_t *file_widen(const char *utf8) { // wide strings (windows only)
    static __thread wchar_t bufs[4][260];
    static __thread int index = 0;

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

char* file_map( const char *pathfile, size_t offset, size_t len ) {
    int file = open(pathfile, O_RDONLY);
    if( file < 0 ) {
        return 0;
    }
    void *ptr = mmap(0, len, PROT_READ, MAP_PRIVATE, file, 0);
    if( ptr == MAP_FAILED ) {
        ptr = 0;
    }
    close(file); // close file. mapping held until unmapped
    return (char *)ptr;
}
void file_unmap( char *buf, size_t len ) {
    munmap( buf, len );
}

int file_size( const char *pathfile ) {
    struct stat_ st;
    uint64_t s = stat8(pathfile, &st) < 0 ? 0ULL : (uint64_t)st.st_size;
    return (int)s;
}
int file_stamp( const char *pathfile ) {
    struct stat_ st;
    uint64_t t = stat8(pathfile, &st) < 0 ? 0ULL : (uint64_t)st.st_mtime;
    return (int)t;
}

bool file_exist( const char *pathfile ) {
    struct stat_ st;
    return stat8(pathfile, &st) < 0 ? 0 : 1;
    // WIN: return _access( filename, 0 ) != -1;
    // else: return access( filename, F_OK ) != -1;
}
bool file_isdir( const char *pathfile ) {
    struct stat_ st;
    return stat8(pathfile, &st) < 0 ? 0 : S_IFDIR == ( st.st_mode & S_IFMT );
}
bool file_isfile( const char *pathfile ) {
    struct stat_ st;
    return stat8(pathfile, &st) < 0 ? 0 : S_IFREG == ( st.st_mode & S_IFMT );
}
bool file_islink( const char *pathfile ) {
#ifdef S_IFLNK
    struct stat_ st;
    return stat8(pathfile, &st) < 0 ? 0 : S_IFLNK == ( st.st_mode & S_IFMT );
#else
    return 0;
#endif
}

char* file_read(const char *pathfile) {
    uint64_t len = file_size(pathfile);
    char *buf = REALLOC( 0, len + 1 ); buf[len] = 0;
    char *map = file_map( pathfile, 0, len );
    memcpy( buf, map, len );
    file_unmap( map, len );
    return buf;
}
bool file_write(const char *pathfile, const void *data, int len) {
    bool ok = 0;
    FILE *fp = fopen8(pathfile, "wb");
    if( fp ) {
        ok = 1 == fwrite(data, len, 1, fp);
        fclose(fp);
    }
    return ok;
}
bool file_append(const char *pathfile, const void *data, int len) {
    bool ok = 0;
    FILE *fp = fopen8(pathfile, "a+b");
    if( fp ) {
        ok = 1 == fwrite(data, len, 1, fp);
        fclose(fp);
    }
    return ok;
}

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#if MSC
#   include <sys/utime.h>
#else
#   include <utime.h>
#endif
bool file_touch( const char *pathfile ) {
    struct stat_ st;
    if( stat8( pathfile, &st ) < 0 ) {
        return false;
    }
    time_t date = time(0);
    struct utimbuf tb;
    tb.actime = st.st_atime;  /* keep atime unchanged */
    tb.modtime = date;          /* set mtime to given time */
    return utime( pathfile, &tb ) != -1 ? true : false;
}
bool file_copy( const char *srcpath, const char *dstpath ) {
#  if WIN
    return CopyFileW( file_widen(srcpath), file_widen(dstpath), FALSE );
#elif OSX
    return copyfile( srcpath, dstpath, 0, COPYFILE_DATA )>=0;
#else
    bool ok = 0;
    FILE *in = fopen8( srcpath, "rb" );
    if( in ) {
        FILE *out = fopen8( dstpath, "wb" );
        if( out ) {
            char buf[1024];
            while( int n = fread( buf, 1, 1024, in ) ){
                ok = fwrite( buf, 1, n, out ) == n;
                if( !ok ) break;
            }
            fclose( out );
        }
        fclose( in );
    }
    return ok;
#endif
}
bool file_delete( const char *path ) {
    remove8( path );
    return !file_exist(path);
}

#endif

#ifdef FILE_DEMO
#include <stdio.h>
int main() {
    char *readbuf = file_map(__FILE__, 0, file_size(__FILE__));
    printf( "read: [%p]: %s\n", readbuf, readbuf );
    file_unmap( readbuf, file_size(__FILE__) );
}
#endif
