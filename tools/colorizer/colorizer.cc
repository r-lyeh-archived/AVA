#include <string>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define API
#define TTY_C
#include "../../engine/native/native_tty.c"

struct word {
    const char *text;
    unsigned char beep, r,g,b;
} words[] = { // priority: top list is higher priority.
    { "error",  1,  255,0,0,   },
    { "fail",   1,  255,0,0,   },
    { "warn",   0,  255,255,0, },
    { "info",   0,  0,255,255, },
    /*
    "err", "warn", "log", "info",
    "mesh", "texture", "asset",
    "load", "save", "init",
    "debug", "release"
    */
};

bool parse_line( FILE *fp, std::string &s ) {
    s.clear();
    while( fp ) {
        int ch = fgetc(fp);
        if( ch == EOF ) return false;
        s += ch;
        if( ch == '\r' || ch == '\n' ) return true;
    }
    return false;
}

bool parse_stream( FILE *fp ) {
    std::string line, lower;

    while( parse_line(fp, line) ) {
        lower = line;
        for( auto &ch: lower ) ch = tolower(ch);

        struct word *found = 0;
        for( int i = 0; i < sizeof(words)/sizeof(0[words]) && !found; ++i) {
            if( strstr(lower.c_str(), words[i].text) ) {
                found = &words[i];
                if( found->beep ) putc(0x7, stdout);
            }
        }

        if( found ) tty_color(found->r, found->g, found->b); else tty_reset();
        printf("%s", line.c_str());
    }

    return true;
}

#ifdef _MSC_VER
#pragma comment(lib,"user32.lib")
#define popen _popen
#define pclose _pclose
#endif

int main( int argc, const char **argv ) {
    // SetConsoleMode(_In_  HANDLE hConsoleHandle, ENABLE_EXTENDED_FLAGS );
    if( argc > 1 ) {
        std::string args;
        for( int i = 1; i < argc; ++i ) {
            args += std::string(" ") + argv[i];
        }
        for( FILE *fp = popen( (args + " 2>&1").c_str(), "r" ); fp; (pclose(fp), fp = 0)) {
            if( !parse_stream(fp) ) {
                return false;
            }
        }
        // tty_reset();
    } else {
        printf("error: nothing to pipe. try `%s %s` instead.\n", argv[0], argv[0]);
        puts("example: this is a message");
        puts("example: this is an info message");
        puts("example: this is a warn message");
        puts("example: this is a debug message");
        puts("example: this is an error");
    }
}
