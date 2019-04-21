API void tty_color(uint8_t r, uint8_t g, uint8_t b);
API void tty_reset(void);

// ----------------------------------------------------------------------------

#ifdef TTY_C
#pragma once

void tty_reset(void) {
    static int end = 0; if( !end ) { end = 1; atexit(tty_reset); }
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
    static int end = 0; if( !end ) { end = 1; atexit(tty_reset); }
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

#endif
