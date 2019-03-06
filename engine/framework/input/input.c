// @todo: delta(mouse('X')), repeat(mouse('X')), click(mouse('L'), click2(mouse('L')
// @todo: for all inputs!
// @todo: bind(key, 'X', 'Z');    // redefine
// @todo: bind(mouse, 'Y', '-Y'); // invert Y
// @todo: bind(mouse, 'Y', 0.01); // sensivity
// @todo: repeat(key, 'X', 10), hit(key, 'X'), drop(key, 'X')
// @todo: if(click2(mouse, 'X'))
// instead of 'DX', 'RX', 'CL', 'LL' etc...

void mouse_update();
double mouse(int key);   // 'X', 'Y', 'L', 'M', 'R', 'show', 'hide',
void set_mouse(int key); // 'show', 'hide'

/*
   '*',    '+',    ',',    '-',    '.',    '/',    ';',    '=',    '[',    ']',    '`',
   'R-',   'R.',   'R/',   'R=',   '\\', 'LALT', 'RALT', 'BACK', 'CAPS', 'LCTR', 'RCTR',
   'DEL' , 'DOWN', 'END' , 'LENT', 'RENT', 'ESC' , 'HOME', 'INS' , 'LEFT', 'MENU', 'NUML',
   'PAUS', 'PGDN', 'PGUP', 'PRNT', 'RGHT', 'SCRL', 'LSHF', 'RSHF', 'SPC' , 'LSUP', 'RSUP',
   'TAB' , 'TICK', 'UP'  , 
*/
int key( int key );




#ifdef INPUT_C
#pragma once

#include "../app/window.c"

static double mx, my, mb[3], mcursor = 1;
static int mhas_imgui = 0;


//static 
void mouse_update() { // $
    int mxi, myi;
    uint32_t state = SDL_GetMouseState(&mxi, &myi);
    mx = mxi; my = myi;

    mb[0] = state & SDL_BUTTON(SDL_BUTTON_LEFT);
    mb[1] = state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    mb[2] = state & SDL_BUTTON(SDL_BUTTON_RIGHT);
    // show or hide+lock
    if( mcursor ) {
        if( mhas_imgui ) {
            SDL_ShowCursor(SDL_DISABLE);
        } else {
            SDL_ShowCursor(SDL_ENABLE);
        }
    } else {
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}
// @todo: delta(mouse('X')), repeat(mouse('X')), click(mouse('L'), click2(mouse('L')
// @todo: for all inputs!
// @todo: bind(key, 'X', 'Z');    // redefine
// @todo: bind(mouse, 'Y', '-Y'); // invert Y
// @todo: bind(mouse, 'Y', 0.01); // sensivity
// @todo: repeat(key, 'X', 10), hit(key, 'X'), drop(key, 'X')
// @todo: if(click2(mouse, 'X'))
// instead of 'DX', 'RX', 'CL', 'LL' etc...
double mouse(int key) { // $
    switch(key) {
        default: return 0;
        break; case 'X': return mx;
        break; case 'Y': return my;
        break; case 'L': return mb[0];
        break; case 'M': return mb[1];
        break; case 'R': return mb[2];
        break; case 'show': return  mcursor;
        break; case 'hide': return !mcursor;
    }
}
void set_mouse(int key) { // $
    switch(key) {
        default: return;
        break; case 'show': mcursor = 1;
        break; case 'hide':
            if( mcursor ) {
                mcursor = 0, mouse_update(), mouse_update();
            }
    }
}

int key( int key ) { // $
    switch(key) {
        break; case    '*': key = SDL_SCANCODE_KP_MULTIPLY;
        break; case    '+': key = SDL_SCANCODE_KP_PLUS;
        break; case    ',': key = SDL_SCANCODE_COMMA;
        break; case    '-': case 'L-': key = SDL_SCANCODE_MINUS;
        break; case    '.': case 'L.': key = SDL_SCANCODE_PERIOD;
        break; case    '/': case 'L/': key = SDL_SCANCODE_SLASH;
        break; case    ';': key = SDL_SCANCODE_SEMICOLON;
        break; case    '=': case 'L=': key = SDL_SCANCODE_EQUALS;
        break; case    '[': key = SDL_SCANCODE_LEFTBRACKET;
        break; case    ']': key = SDL_SCANCODE_RIGHTBRACKET;
        break; case    '`': case 'BTCK': key = SDL_SCANCODE_NONUSBACKSLASH;
        break; case   'R-': key = SDL_SCANCODE_KP_MINUS;
        break; case   'R.': key = SDL_SCANCODE_KP_DECIMAL;
        break; case   'R/': key = SDL_SCANCODE_KP_DIVIDE;
        break; case   'R=': key = SDL_SCANCODE_KP_EQUALS;
        break; case   '\\': key = SDL_SCANCODE_BACKSLASH;
        break; case 'LALT': key = SDL_SCANCODE_LALT;
        break; case 'RALT': key = SDL_SCANCODE_RALT;
        break; case 'BACK': case '\b': key = SDL_SCANCODE_BACKSPACE;
        break; case 'CAPS': key = SDL_SCANCODE_CAPSLOCK;
        break; case 'LCTR': key = SDL_SCANCODE_LCTRL;
        break; case 'RCTR': key = SDL_SCANCODE_RCTRL;
        break; case 'DEL' : key = SDL_SCANCODE_DELETE;
        break; case 'DOWN': key = SDL_SCANCODE_DOWN;
        break; case 'END' : key = SDL_SCANCODE_END;
        break; case 'LENT': case '\n': case '\r': key = SDL_SCANCODE_RETURN;
        break; case 'RENT': key = SDL_SCANCODE_KP_ENTER;
        break; case 'ESC' : case '\x1b': key = SDL_SCANCODE_ESCAPE;
        break; case 'HOME': key = SDL_SCANCODE_HOME;
        break; case 'INS' : key = SDL_SCANCODE_INSERT;
        break; case 'LEFT': key = SDL_SCANCODE_LEFT;
        break; case 'MENU': key = SDL_SCANCODE_MENU;
        break; case 'NUML': key = SDL_SCANCODE_NUMLOCKCLEAR;
        break; case 'PAUS': key = SDL_SCANCODE_PAUSE;
        break; case 'PGDN': key = SDL_SCANCODE_PAGEDOWN;
        break; case 'PGUP': key = SDL_SCANCODE_PAGEUP;
        break; case 'PRNT': key = SDL_SCANCODE_PRINTSCREEN;
        break; case 'RGHT': key = SDL_SCANCODE_RIGHT;
        break; case 'SCRL': key = SDL_SCANCODE_SCROLLLOCK;
        break; case 'LSHF': key = SDL_SCANCODE_LSHIFT;
        break; case 'RSHF': key = SDL_SCANCODE_RSHIFT;
        break; case 'SPC' : case  ' ': key = SDL_SCANCODE_SPACE;
        break; case 'LSUP': case 'LWIN': key = SDL_SCANCODE_LGUI;
        break; case 'RSUP': case 'RWIN': key = SDL_SCANCODE_RGUI;
        break; case 'TAB' : case '\t': key = SDL_SCANCODE_TAB;
        break; case 'TICK': case '\'': key = SDL_SCANCODE_APOSTROPHE;
        break; case 'UP'  : key = SDL_SCANCODE_UP;
        default:
        /**/ if( key >=    '0' && key <=    '9' ) key +=-   '0' + SDL_SCANCODE_0;
        else if( key >=    'a' && key <=    'z' ) key +=-   'a' + SDL_SCANCODE_A;
        else if( key >=    'A' && key <=    'Z' ) key +=-   'A' + SDL_SCANCODE_A;
        else if( key >=   'F1' && key <=   'F9' ) key +=-  'F1' + SDL_SCANCODE_F1;
        else if( key >=  'F10' && key <=  'F19' ) key +=- 'F10' + SDL_SCANCODE_F10;
        else if( key >=  'F20' && key <=  'F25' ) key +=- 'F20' + SDL_SCANCODE_F20;
        else if( key >= 'PAD0' && key <= 'PAD9' ) key +=-'PAD0' + SDL_SCANCODE_KP_0;
        else return 0;
    }
/*
    GLFW_KEY_WORLD_1            161 // non-US #1
    GLFW_KEY_WORLD_2            162 // non-US #2
*/
    return !!SDL_GetKeyboardState(NULL)[key];
}

// ----------------------------------------------------------------------------

#if 0

#include <windows.h>

enum {
    INPUT_REPEAT,
    INPUT_TRIGGER,
    INPUT_HELD,
};

int key2( int vk, int mode ) {
    static int prev[256] = {0};

    if( vk == 'UP' ) vk = VK_UP;
    else if( vk == 'DOWN' ) vk = VK_DOWN;
    else if( vk == '\n' ) vk = VK_RETURN;
    else return 0;

    int now = (GetAsyncKeyState(vk) & 0x8000) > 0, then = prev[vk];
    int held = now & ~then;
    prev[vk] = now;

    return then < now; // trigger
    return then > now; // release
    return  (then+now); // held
    return !(then+now); // idle
}

#endif

#endif
