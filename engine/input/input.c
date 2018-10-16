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
    mb[0] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    mb[1] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
    mb[2] = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    glfwGetCursorPos(window, &mx, &my);
    // show or hide+lock
    if( mcursor ) {
        if( mhas_imgui ) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        //int width, height;
        //glfwGetFramebufferSize(window, &width, &height);
        //glfwSetCursorPos(window, width/2, height/2);
        // or ...
        //if(mx > width) set mx -= width; else if(mx < 0) set mx += width;
        //if(MY > height) set MY -= height; else if(MY < 0) set MY += height;
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
        break; case    '*': key = GLFW_KEY_KP_MULTIPLY;
        break; case    '+': key = GLFW_KEY_KP_ADD;
        break; case    ',': key = GLFW_KEY_COMMA;
        break; case    '-': case 'L-': key = GLFW_KEY_MINUS;
        break; case    '.': case 'L.': key = GLFW_KEY_PERIOD;
        break; case    '/': case 'L/': key = GLFW_KEY_SLASH;
        break; case    ';': key = GLFW_KEY_SEMICOLON;
        break; case    '=': case 'L=': key = GLFW_KEY_EQUAL;
        break; case    '[': key = GLFW_KEY_LEFT_BRACKET;
        break; case    ']': key = GLFW_KEY_RIGHT_BRACKET;
        break; case    '`': case 'BTCK': key = GLFW_KEY_GRAVE_ACCENT;
        break; case   'R-': key = GLFW_KEY_KP_SUBTRACT;
        break; case   'R.': key = GLFW_KEY_KP_DECIMAL;
        break; case   'R/': key = GLFW_KEY_KP_DIVIDE;
        break; case   'R=': key = GLFW_KEY_KP_EQUAL;
        break; case   '\\': key = GLFW_KEY_BACKSLASH;
        break; case 'LALT': key = GLFW_KEY_LEFT_ALT;
        break; case 'RALT': key = GLFW_KEY_RIGHT_ALT;
        break; case 'BACK': case '\b': key = GLFW_KEY_BACKSPACE;
        break; case 'CAPS': key = GLFW_KEY_CAPS_LOCK;
        break; case 'LCTR': key = GLFW_KEY_LEFT_CONTROL;
        break; case 'RCTR': key = GLFW_KEY_RIGHT_CONTROL;
        break; case 'DEL' : key = GLFW_KEY_DELETE;
        break; case 'DOWN': key = GLFW_KEY_DOWN;
        break; case 'END' : key = GLFW_KEY_END;
        break; case 'LENT': case '\n': case '\r': key = GLFW_KEY_ENTER;
        break; case 'RENT': key = GLFW_KEY_KP_ENTER;
        break; case 'ESC' : case '\x1b': key = GLFW_KEY_ESCAPE;
        break; case 'HOME': key = GLFW_KEY_HOME;
        break; case 'INS' : key = GLFW_KEY_INSERT;
        break; case 'LEFT': key = GLFW_KEY_LEFT;
        break; case 'MENU': key = GLFW_KEY_MENU;
        break; case 'NUML': key = GLFW_KEY_NUM_LOCK;
        break; case 'PAUS': key = GLFW_KEY_PAUSE;
        break; case 'PGDN': key = GLFW_KEY_PAGE_DOWN;
        break; case 'PGUP': key = GLFW_KEY_PAGE_UP;
        break; case 'PRNT': key = GLFW_KEY_PRINT_SCREEN;
        break; case 'RGHT': key = GLFW_KEY_RIGHT;
        break; case 'SCRL': key = GLFW_KEY_SCROLL_LOCK;
        break; case 'LSHF': key = GLFW_KEY_LEFT_SHIFT;
        break; case 'RSHF': key = GLFW_KEY_RIGHT_SHIFT;
        break; case 'SPC' : case  ' ': key = GLFW_KEY_SPACE;
        break; case 'LSUP': case 'LWIN': key = GLFW_KEY_LEFT_SUPER;
        break; case 'RSUP': case 'RWIN': key = GLFW_KEY_RIGHT_SUPER;
        break; case 'TAB' : case '\t': key = GLFW_KEY_TAB;
        break; case 'TICK': case '\'': key = GLFW_KEY_APOSTROPHE;
        break; case 'UP'  : key = GLFW_KEY_UP;
        default:
        /**/ if( key >=    '0' && key <=    '9' ) key +=-   '0' + GLFW_KEY_0;
        else if( key >=    'a' && key <=    'z' ) key +=-   'a' + GLFW_KEY_A;
        else if( key >=    'A' && key <=    'Z' ) key +=-   'A' + GLFW_KEY_A;
        else if( key >=   'F1' && key <=   'F9' ) key +=-  'F1' + GLFW_KEY_F1;
        else if( key >=  'F10' && key <=  'F19' ) key +=- 'F10' + GLFW_KEY_F10;
        else if( key >=  'F20' && key <=  'F25' ) key +=- 'F20' + GLFW_KEY_F20;
        else if( key >= 'PAD0' && key <= 'PAD9' ) key +=-'PAD0' + GLFW_KEY_KP_0;
        else return 0;
    }
/*
    GLFW_KEY_WORLD_1            161 // non-US #1
    GLFW_KEY_WORLD_2            162 // non-US #2
*/
    return !!glfwGetKey(window, key);
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
