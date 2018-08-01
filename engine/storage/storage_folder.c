// # folder ###################################################################
// @ todo: paths must end with slash always

#ifndef FOLDER_H
#define FOLDER_H
#include <stdint.h>

API char *folder_appname();
API char *folder_username();
API char *folder_gamedir();
API char *folder_datadir();
API char *folder_tempdir();
API char *folder_workdir();
API uint64_t folder_freespace();

#endif

#ifdef FOLDER_C
#pragma once
#include "../detect/detect_threadls.c"
#include "../asset/asset_dir.c"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef PATH_MAX
#ifdef MAX_PATH
#define PATH_MAX MAX_PATH
#elif defined _MAX_PATH
#define PATH_MAX _MAX_PATH
#else
#define PATH_MAX 260
#endif
#endif

static
char *folder__trimr( char *str, char *sub ) {
    sub = strstr(str, sub);
    if( sub ) *sub = 0;
    return str;
}

static
char *folder__triml( char *str, char *sub ) {
    char *found = strstr(str, sub);
    if( found ) str = memmove(str, found + strlen(sub), strlen(found) + 1);
    return str;
}

static
char *folder_appfull() {
    static char *t = 0;
    if(t) return t;

    char path[PATH_MAX+1] = {0};

#if defined(__APPLE__)
    uint32_t i = sizeof(path);
    if (_NSGetExecutablePath(path, &i) > -1) {
        return t = strdup(path);
    }
#elif defined(__linux__)
    if (readlink(va("/proc/%d/exe", getpid()), path, sizeof(path)) > -1) {
        return t = strdup(path);
    }
#elif defined(_WIN32)
    if (GetModuleFileNameA(0, path, sizeof(path))) {
        return t = strdup(path);
    }
#endif

    char absolute[PATH_MAX+1] = {0};
#ifdef _WIN32
    _fullpath(absolute, __argv[0], PATH_MAX);
#else
    realpath(__argv[0], absolute);
#endif

    for( int i = 0; absolute[i]; ++i ) {
        path[i] = absolute[i] == '\\' ? '/' : absolute[i];
        path[i+1] = 0;
    }
    return t = strdup( path );
}
char *folder_appname() {
    static char *t = 0;
    if(t) return t;
    t = strdup(__argv[0]);
    // remove extension if present (windows)
    folder__trimr(t, ".exe");
    // remove path if present
    folder__triml(t, "\\");
    folder__triml(t,  "/");
    return t;
}
char *folder_username() {
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("USER");
    t = t ? t : getenv("USERNAME");
    t = strdup( t ? t : "GUEST" );
    return t;
}
char *folder_gamedir() {
    static char *t = 0;
    if( t ) return t;

    t = strdup( folder_appfull() );
    for( size_t i = strlen(t); i-- > 0; ) {
        if (t[i] == '\\' || t[i] == '/') {
            t[i] = '\0';
            break;
        }
    }
    return t;
}
char *folder_datadir() {
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("APPDATA");
    t = t ? t : getenv("HOME");
    t = strdup( t ? t : "./" );
    return t;
}
char *folder_tempdir() {
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("XDG_RUNTIME_DIR");
    t = t ? t : getenv("TMPDIR");
    t = t ? t : getenv("TMP");
    t = t ? t : getenv("TEMP");
#ifndef _WIN32
    t = t ? t : "/tmp";
#endif
    t = strdup( t ? t : "./" );
    return t;
}
char *folder_workdir() {
    static __thread char cwd[PATH_MAX+1] = {0};
    getcwd(cwd, sizeof(cwd));
    return cwd;
}
uint64_t folder_freespace() {
#ifdef _WIN32
    DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
    if( GetDiskFreeSpaceA( ".\\", &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters ) ) {
        return ((uint64_t)NumberOfFreeClusters) * SectorsPerCluster * BytesPerSector;
    }
#endif
    return ~0LLU;
}

#endif

#ifdef FOLDER_DEMO
#include <stdio.h>
int main() {
    printf("folder_appname: %s\n", folder_appname() );
    printf("folder_username: %s\n", folder_username() );
    printf("folder_gamedir: %s\n", folder_gamedir() );
    printf("folder_datadir: %s\n", folder_datadir() );
    printf("folder_tempdir: %s\n", folder_tempdir() );
    printf("folder_workdir: %s\n", folder_workdir() );
    printf("folder_freespace: %llu bytes free\n", folder_freespace());
}
#endif
