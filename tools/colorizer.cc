#include <string>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#endif

void tty_reset(void) {
    static int end = (atexit(tty_reset), 1);
#ifdef _WIN32
    static CONSOLE_SCREEN_BUFFER_INFO csbi = {0}, *ever = 0;
    if( !ever ) {
        const HANDLE console = GetStdHandle( STD_OUTPUT_HANDLE );
        if (console == INVALID_HANDLE_VALUE) {
            return;
        }
        if (!GetConsoleScreenBufferInfo(console, &csbi)) {
            return;
        }
        ever = &csbi;
    }
    const HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console != INVALID_HANDLE_VALUE) {
        SetConsoleTextAttribute(console, csbi.wAttributes);
    }
#else
    puts("\x1B[39;49m");
#endif
}

void tty_color(uint8_t r, uint8_t g, uint8_t b) {
    static int end = (tty_reset(), 1);
#ifdef _WIN32
    const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    auto color = ( r > 127 ? FOREGROUND_RED : 0 ) |
                 ( g > 127 ? FOREGROUND_GREEN : 0 ) |
                 ( b > 127 ? FOREGROUND_BLUE : 0 ) |
                 FOREGROUND_INTENSITY;
    SetConsoleTextAttribute(stdout_handle, color);
#else
    // 24-bit console ESC[ … 38;2;<r>;<g>;<b> … m Select RGB foreground color
    // 256-color console ESC[38;5;<fgcode>m
    // 0x00-0x07:  standard colors (as in ESC [ 30..37 m)
    // 0x08-0x0F:  high intensity colors (as in ESC [ 90..97 m)
    // 0x10-0xE7:  6*6*6=216 colors: 16 + 36*r + 6*g + b (0≤r,g,b≤5)
    // 0xE8-0xFF:  grayscale from black to white in 24 steps
    r /= 51, g /= 51, b /= 51; // [0..5]
    printf("\033[38;5;%dm", r*36+g*6+b+16); // "\033[0;3%sm", color_code);
#endif
}

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
