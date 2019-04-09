// @todo: delta(mouse('X')), repeat(mouse('X')), click(mouse('L'), click2(mouse('L')
// @todo: for all inputs!
// @todo: bind(key, 'X', 'Z');    // redefine
// @todo: bind(mouse, 'Y', '-Y'); // invert Y
// @todo: bind(mouse, 'Y', 0.01); // sensivity
// @todo: repeat(key, 'X', 10), hit(key, 'X'), drop(key, 'X')
// @todo: if(click2(mouse, 'X'))
// instead of 'DX', 'RX', 'CL', 'LL' etc...

API void mouse_update();
API float mouse(int key);   // 'X', 'Y', 'L', 'M', 'R', 'show', 'hide',
API void set_mouse(int key); // 'show', 'hide'
API vec2 mouse_xy();

/*
   '*',    '+',    ',',    '-',    '.',    '/',    ';',    '=',    '[',    ']',    '`',
   'R-',   'R.',   'R/',   'R=',   '\\', 'LALT', 'RALT', 'BACK', 'CAPS', 'LCTR', 'RCTR',
   'DEL' , 'DOWN', 'END' , 'LENT', 'RENT', 'ESC' , 'HOME', 'INS' , 'LEFT', 'MENU', 'NUML',
   'PAUS', 'PGDN', 'PGUP', 'PRNT', 'RGHT', 'SCRL', 'LSHF', 'RSHF', 'SPC' , 'LSUP', 'RSUP',
   'TAB' , 'TICK', 'UP'  , 
*/
API float key( int key );
API float key_up( int key );
API float key_down( int key );
API vec3  key_wasdec();


#ifdef INPUT_C
#pragma once

#include "../window/window_app.c"

static float mx, my, mb[3], mcursor = 1;
static int mhas_imgui = 0;


//static 
void mouse_update() { // $
    int mxi, myi;
    uint32_t state;

    if( SDL_GetRelativeMouseMode() ) {
        state = SDL_GetRelativeMouseState(&mxi, &myi);
        mx += mxi; my += myi;
    } else {
        state = SDL_GetMouseState(&mxi, &myi);
        mx = mxi; my = myi;
    }

    mb[0] = state & SDL_BUTTON(SDL_BUTTON_LEFT);
    mb[1] = state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    mb[2] = state & SDL_BUTTON(SDL_BUTTON_RIGHT);
}
// @todo: delta(mouse('X')), repeat(mouse('X')), click(mouse('L'), click2(mouse('L')
// @todo: for all inputs!
// @todo: bind(key, 'X', 'Z');    // redefine
// @todo: bind(mouse, 'Y', '-Y'); // invert Y
// @todo: bind(mouse, 'Y', 0.01); // sensivity
// @todo: repeat(key, 'X', 10), hit(key, 'X'), drop(key, 'X')
// @todo: if(click2(mouse, 'X'))
// instead of 'DX', 'RX', 'CL', 'LL' etc...
float mouse(int key) { // $
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
        break; case 'show': {
            if( !mcursor ) {
                mcursor = 1;

                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_DISABLE);
            }
        }
        break; case 'hide':
            if( mcursor ) {
                mcursor = 0;

                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetRelativeMouseMode(SDL_ENABLE);

                // consume relative, so next call (mouse_update) is nearly 0
                int mxi, myi;
                SDL_GetRelativeMouseState(&mxi, &myi);
            }
    }
}

uint8_t scancodes0[SDL_NUM_SCANCODES] = {0};
uint8_t scancodes1[SDL_NUM_SCANCODES] = {0};
uint8_t *scancodes_now = scancodes0, *scancodes_old = scancodes1;

float key_( uint8_t *scancodes, int key ) { // $
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
        /**/ if( key >=    '1' && key <=    '9' ) key = key -   '1' + SDL_SCANCODE_0;
        else if( key >=    'a' && key <=    'z' ) key = key -   'a' + SDL_SCANCODE_A;
        else if( key >=    'A' && key <=    'Z' ) key = key -   'A' + SDL_SCANCODE_A;
        else if( key >=   'F1' && key <=   'F9' ) key = key -  'F1' + SDL_SCANCODE_F1;
        else if( key >=  'F10' && key <=  'F19' ) key = key - 'F10' + SDL_SCANCODE_F10;
        else if( key >=  'F20' && key <=  'F25' ) key = key - 'F20' + SDL_SCANCODE_F20;
        else if( key >= 'PAD1' && key <= 'PAD9' ) key = key -'PAD1' + SDL_SCANCODE_KP_0;
        else if( key == '0' ) key = SDL_SCANCODE_0;
        else if( key == 'PAD0') key = SDL_SCANCODE_KP_0;
        else return 0;
    }
/*
    GLFW_KEY_WORLD_1            161 // non-US #1
    GLFW_KEY_WORLD_2            162 // non-US #2
*/
    return !!scancodes[key]; // !!SDL_GetKeyboardState(NULL)[key];
}

float key( int key ) { // $
    return key_(scancodes_now, key);
}

float key_up( int key ) { // $
    return !key_(scancodes_now, key) && !!key_(scancodes_old, key);
}

float key_down( int key ) { // $
    return key_(scancodes_now, key) && !key_(scancodes_old, key);
}

// ----------------------------------------------------------------------------

vec2 mouse_xy() { return vec2(mouse('X'), mouse('Y')); }
vec3 key_wasdec() { return vec3(key('D')-key('A'), key('E')-key('C'), key('W')-key('S')); } // vec3(right,up,forward)

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
