int   file_size_(const char* name);
char* file_read_(const char* name);
char* file_find(const char* name);

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

#ifdef FILESYS_C
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int file_size_(const char *name) {
    struct stat st;
    return stat(name, &st) >= 0 ? (int)st.st_size : 0;
}

char* file_read_(const char* name) {
    char *buf;
    size_t len;

    FILE *fp = fopen(name, "rb");
    if( fp ) {
        len = file_size_(name);
        buf = malloc(len + 1);
        len = fread(buf, 1, len, fp);
        fclose(fp);
    } else {
        buf = calloc(1,4);
        len = 0;
    }

    buf[len] = 0;
    return buf;
}

#endif

#if 0

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
