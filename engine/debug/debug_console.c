// - [ ] quake-style borderless console.
// - [ ] history
// - [ ] completion
// - [ ] arguments

#ifndef CONSOLE_H
#define CONSOLE_H

typedef void(*console_cmd)();

void console_add(const char* name, console_cmd func, void* user_data);
void console_del(const char* name);
void console_run1(const char* name, void *arg1 );
void console_run2(const char* name, void *arg1, void *arg2 );

#endif

// ------------------------------------------------

#ifdef CONSOLE_C
#pragma once

typedef struct console_opcode {
    const char *name;
    console_cmd func;
} console_opcode;

console_opcode console_list_opcode[256] = {0};

void console_add(const char* name, console_cmd func, void* user_data) {
    for( int i = 0; i < 256; ++i ) {
        if( console_list_opcode[i].name == 0 ) {
            console_list_opcode[i].name = name;
            console_list_opcode[i].func = func;
            return;
        }
    }
}

void console_del(const char* name) {
    for( int i = 0; i < 256; ++i ) {
        if( console_list_opcode[i].name != 0 ) {
            if( !strcmp(console_list_opcode[i].name, name) ) {
                console_list_opcode[i].name = 0;
                console_list_opcode[i].func = 0;
            }
        }
    }
}

void console_run1(const char* name, void *arg1 ) {
    for( int i = 0; i < 256; ++i ) {
        if( console_list_opcode[i].name != 0 ) {
            if( !strcmp(console_list_opcode[i].name, name) ) {
                console_list_opcode[i].func( arg1 );
                return;
            }
        }
    }
    // else, strlower? fuzzy search?
}
void console_run2(const char* name, void *arg1, void *arg2 ) {
    for( int i = 0; i < 256; ++i ) {
        if( console_list_opcode[i].name != 0 ) {
            if( !strcmp(console_list_opcode[i].name, name) ) {
                console_list_opcode[i].func( arg1, arg2 );
                return;
            }
        }
    }
    // else, strlower? fuzzy search?
}

#include <stdio.h>
#include <string.h>

char *prompt() {
    static char buffer[512];
    buffer[0] = 0;
    if( !fgets(buffer, 512, stdin) || feof(stdin) ) {
        return 0;
    }
    for( int c = strlen(buffer); c && buffer[c-1] < 32; c = strlen(buffer) ) {
        buffer[c-1] = 0;
    }
    return buffer;
}

void console(void *userdata) {
    for( const char *line = prompt(); line; line = prompt() ) {
        puts(line);
    }
}

#ifdef AUTORUN
    void console_bye(void) {
    #ifdef _WIN32
        // sendEnterToStdin() - original code by Asain Kujovic
        // https://stackoverflow.com/questions/28119770/kill-fgets-thread-in-mingw-and-wine
        INPUT_RECORD ir[2];
        for (int i=0; i<2; i++) {
            KEY_EVENT_RECORD *kev =&ir[i].Event.KeyEvent;
            ir[i].EventType        =KEY_EVENT;
            kev->bKeyDown          = i == 0;    //<-true, than false
            kev->dwControlKeyState = 0;
            kev->wRepeatCount      = 1;
            kev->uChar.UnicodeChar = VK_RETURN;
            kev->wVirtualKeyCode   = VK_RETURN;
            kev->wVirtualScanCode  = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
        }
        DWORD dw; WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ir, 2, &dw);
    #else
        fclose(stdin);
    #endif
    }

    AUTORUN {
        atexit( console_bye );
        detach( console, 0 );
    }
#endif

#endif

// ------------------------------------------------

#ifdef CONSOLE_DEMO
#include <stdio.h>

void my_log(const char* text) {
    puts(text);
}
void my_add(int a, int b) {
    printf("%d\n", a + b );
}

int main() {
    console_add( "log", my_log, 0 );
    console_add( "add", my_add, 0 );

    console_run1( "log", "hello world" );
    console_run2( "add", 2, 3 );
}

#endif
