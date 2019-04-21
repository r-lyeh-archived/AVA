// Based on code from MinimalSystray project (unlicensed)
// - rlyeh, public domain.

API void window_tray( const char *tray_name, const char *tray_icon_pathfile );

// ----------------------------------------------------------------------------

#ifdef TRAY_C
#pragma once

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
static volatile HWND tray_window = 0;
static const char *TRAY_ICON_PATHFILE = 0;
static const char *TRAY_NAME = "";
static LRESULT CALLBACK SystrayThreadProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    //printf("debug msg %d\n", Msg);
    enum EButtons { IDM_NONE, IDM_EXIT };
    static UINT s_WM_TASKBARRESTART;
    if (Msg == WM_COMMAND && wParam == IDM_EXIT) { NOTIFYICONDATAA i; ZeroMemory(&i, sizeof(i)); i.cbSize = sizeof(i); i.hWnd = hWnd; Shell_NotifyIconA(NIM_DELETE, &i); ExitProcess(0); }
    if (Msg == WM_USER && (LOWORD(lParam) == WM_LBUTTONUP || LOWORD(lParam) == WM_RBUTTONUP)) {
        HMENU hPopMenu = CreatePopupMenu();
        InsertMenuA(hPopMenu,0xFFFFFFFF,MF_STRING|MF_GRAYED,IDM_NONE, TRAY_NAME);
        InsertMenuA(hPopMenu,0xFFFFFFFF,MF_SEPARATOR,IDM_NONE,NULL);
        InsertMenuA(hPopMenu,0xFFFFFFFF,MF_STRING,IDM_EXIT,"Exit");
        SetForegroundWindow(hWnd); //cause the popup to be focused
        POINT lpClickPoint;
        GetCursorPos(&lpClickPoint);
        TrackPopupMenu(hPopMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y,0,hWnd,NULL);
    }
    if (Msg == WM_CREATE || Msg == s_WM_TASKBARRESTART) {
        if (Msg == WM_CREATE) s_WM_TASKBARRESTART = RegisterWindowMessageA(TRAY_NAME);
        NOTIFYICONDATAA nid;
        ZeroMemory(&nid, sizeof(nid));
        nid.cbSize = sizeof(nid); 
        nid.hWnd = hWnd;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
        nid.hIcon = 0;
        if( TRAY_ICON_PATHFILE ) {
            nid.hIcon = window_load_ico(TRAY_ICON_PATHFILE);
        } else {
            //GetIconFromParentProcess();
            //GetIconFromDLL( "imageres.dll", 19 );
            //LoadIconA(GetModuleHandleA(NULL), "ICN");
            //etc; 
        }
        nid.uCallbackMessage = WM_USER; 
        strcpy_s(nid.szTip, sizeof(nid.szTip), TRAY_NAME);
        Shell_NotifyIconA(NIM_ADD, &nid);
    }
    return DefWindowProcA(hWnd, Msg, wParam, lParam);
}
static DWORD CALLBACK SystrayThread(LPVOID arg) {
    MSG Msg;
    WNDCLASSA c;
    ZeroMemory(&c, sizeof(c));
    c.lpfnWndProc = SystrayThreadProc;
    c.hInstance = GetModuleHandleA(NULL);
    c.lpszClassName = TRAY_NAME;
    if (!RegisterClassA(&c)) return 1;
    tray_window = CreateWindowA(c.lpszClassName, 0, 0, 0, 0, 0, 0, 0, 0, c.hInstance, 0);
    if( !tray_window ) return 1;
    while (GetMessageA(&Msg, 0, 0, 0) > 0) { TranslateMessage(&Msg); DispatchMessageA(&Msg); }
    tray_window = 0;
    return 0;
}
// install a systray icon that:
// - uses __argv[0]+.ico
// - has context popup menu with "title, about, exit" options
void window_tray( const char *tray_name, const char *tray_icon_pathfile ) {
    static int ever = 0;
    if( !ever ) {
        ever = 1;
        TRAY_NAME = strdup(tray_name);
        TRAY_ICON_PATHFILE = strdup(tray_icon_pathfile);
        CreateThread(NULL, 0, SystrayThread, NULL, 0, NULL);
        while(!tray_window) Sleep(10);
    }
}
#else
void window_tray( const char *, const char * )
{}
#endif

#if defined TRAY_C && defined TRAY_DEMO

int main() {
    window_tray( "my demo", "demo.ico" );
    system("pause");
}

#endif

#endif