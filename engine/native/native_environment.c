#ifndef ENV_H
#define ENV_H
#include <stdint.h>

// environment vars

const char *env_get( const char *key );
const char *env_set( const char *key, const char *value );

// environment folders

char *env_arg0(); // appname()
char *env_curr(); // envwork
char *env_home();
char *env_proc_();
char *env_root(); // envexec, envgame
char *env_save(); // envdata
char *env_temp();
char *env_user();
const char *env_dir( const int folder );
uint64_t env_free();

#endif

// # env ######################################################################
// @ todo: paths must end with slash always

#ifdef ENV_C
#include "engine.h" // platform, memory
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
#include <stdlib.h>
const char *env_set( const char *key, const char *value ) { //$
#if WINDOWS
    char buf[1024];
    sprintf(buf,"%s=%s", key, value ? value : "");
    putenv( buf );
#else
    setenv( key, value ? value : "", 1 );
#endif
    return value;
}
const char *env_get( const char *key ) { //$
    return getenv(key);
}

#ifdef ENVDEMO
#endif

#if !WINDOWS
#include <pwd.h>
#endif

// get process full path
static
char *env_proc_() {
    static char *t = 0;
    if(t) return t;

    char path[PATH_MAX+1] = {0};

#if IOS || OSX
    uint32_t i = sizeof(path);
    if (_NSGetExecutablePath(path, &i) > -1) {
        return t = STRDUP(path);
    }
#elif LINUX
    //if (readlink(strf("/proc/%d/exe", getpid()), path, sizeof(path)) > -1) {
    if (readlink("/proc/self/exe", path, sizeof(path)) > -1) {
        return t = STRDUP(path);
    }
#elif WINDOWS
    if (GetModuleFileNameA(0, path, sizeof(path))) {
        return t = STRDUP(path);
    }
#endif
    //t = STRDUP( __argv[0] );
    //return t = dirabs( &t );
    return "./";
}

// util that ensure paths end with '/' and are normalized as well.
static
char *ENV_FIX(const char *pathfile) {
    char *buf = (char*)MALLOC( PATH_MAX + 1 );
    uri_norm(buf, pathfile);
    int len = strlen(buf);
    if( buf[ len ] != '/' ) {
        buf[ len ] = '/';
    }
    return buf;
}

#if !MSC
extern char **__argv; // implemented in string.option.c
#endif

char *env_arg0() { // appname()
    static char *t = 0;
    if(t) return t;
    t = STRDUP(__argv[0]);
    // remove extension if present (windows)
    strtrimlfe(t, ".exe");
    // remove path if present
    strtrimblf(t, "\\");
    strtrimblf(t,  "/");
    return t;
}
char *env_user() {
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("USER");
    t = t ? t : getenv("USERNAME");
    t = STRDUP( t ? t : "GUEST" );
    return t;
}
char *env_home() {
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("USERPROFILE");
    t = t ? t : getenv("HOME");
    t = ENV_FIX( t ? t : "./" );
    return t;
}
char *env_save() { // envdata
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("APPDATA");
    t = t ? t : getenv("XDG_DATA_HOME");
    t = ENV_FIX( t ? t : "./" );
    return t;
}
char *env_temp() {
    static char *t = 0;
    if(t) return t;
    t = t ? t : getenv("TMPDIR");
    t = t ? t : getenv("TMP");
    t = t ? t : getenv("TEMP");
#if !WINDOWS
    t = t ? t : "/tmp";
#else
    // GetTempPathW(n, buf);
#endif
    t = ENV_FIX( t ? t : "./" );
    return t;
}
static THREAD_LOCAL char cwd[PATH_MAX+1];
char *env_curr() { // envwork
#if WINDOWS
    _getcwd(cwd, sizeof(cwd));
#else
    getcwd(cwd, sizeof(cwd));
#endif
    return uri_norm( cwd, cwd ), cwd[strlen(cwd)] = '/', cwd;
}
uint64_t env_free() {
#if WINDOWS
    DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
    if( GetDiskFreeSpaceA( ".\\", &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters ) ) {
        return ((uint64_t)NumberOfFreeClusters) * SectorsPerCluster * BytesPerSector;
    }
#endif
    return ~0LLU;
}

char *env_root() { // envexec, envgame
    static char *t = 0;
    if( t ) return t;

    t = STRDUP( env_proc_() );
    for( size_t i = strlen(t); i-- > 0; ) {
        if (t[i] == '\\' || t[i] == '/') {
            t[i] = '\0';
            break;
        }
    }
    return uri_norm(t, t), t[strlen(t)] = '/', t;
}

#endif

#ifdef ENV_DEMO
#include <stdio.h>
int main() {
    printf("env_arg0: %s\n", env_arg0() );
    printf("env_curr: %s\n", env_curr() );
    printf("env_home: %s\n", env_home() );
    printf("env_root: %s\n", env_root() );
    printf("env_save: %s\n", env_save() );
    printf("env_temp: %s\n", env_temp() );
    printf("env_user: %s\n", env_user() );

    double GiB = 1024 * 1024 * 1024;
    printf("env_free: %d GiB\n", (int)(env_free() / GiB) );
}
#endif
