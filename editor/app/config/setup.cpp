#include "setup.h"
#include "resource.h"

typedef struct {
  int w,h;
} RES;

// if your windows supports more than 4000 reolutions, well, we're screwed :)
#define MAXNUMRES 4096
static int nRes = 0;
static RES ress[MAXNUMRES];
static const char *caption = "AVA"; // 0;
static const char *title = "AVA"; // 0;
static const char *icon = "AVA.ico"; // 0;
static fwzSettings * setupcfg;

#include <string>

static
bool get_app_folder( std::string &path, std::string &name ) {
#   ifdef _WIN32
    char s_path[MAX_PATH] = {0};
    const size_t len = MAX_PATH;

    HMODULE mod = 0;
    ::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&get_app_folder, &mod);
    DWORD size = ::GetModuleFileNameA(mod, s_path, (DWORD)len);

    while( size > 0 ) {
        if( s_path[size] == '\\' ) {
            name = &s_path[size+1];
            s_path[size+1] = '\0';
            path = s_path;
            return 1;
        }
        --size;
    }
    return 0;
#   else
    char arg1[20];
    char exepath[PATH_MAX + 1] = {0};

    sprintf( arg1, "/proc/%d/exe", getpid() );
    readlink( arg1, exepath, 1024 );
    path = exepath;
    name = std::string();
    return 1;
#   endif
}

static
void set_app_icon( const char *pathfileico, HWND *hWnd ) {
    HINSTANCE hInstance = GetModuleHandle(0);
    HANDLE hIcon = LoadImageA( hInstance, pathfileico, IMAGE_ICON, 32, 32, LR_LOADFROMFILE );
    SendMessage( hWnd ? (HWND)*hWnd : (HWND)GetActiveWindow(), (UINT)WM_SETICON, ICON_BIG, (LPARAM)hIcon );
}



INT_PTR CALLBACK DlgFunc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
  switch ( uMsg ) {
    case WM_INITDIALOG: {
      char buf[500];
      int i=0;

      // set window caption
      if( caption ) SetWindowText(hWnd, caption);
      // set title
      if( title ) SetDlgItemText(hWnd, IDC_TITLE, title);
      // set pathfile icon
      if( icon ) {
          set_app_icon(icon, &hWnd);
      }

      DEVMODE d;
      EnumDisplaySettings(NULL,0,&d);
      while(1) {
        BOOL h = EnumDisplaySettings(NULL,i++,&d);
        if (!h || nRes>MAXNUMRES) break;

/*** You can use this following line to select only certain aspect ratios ***/
//        if ((d.dmPelsWidth * 3) / 4 != d.dmPelsHeight) continue;

/*** You can use this following line to avoid "rotated" pictures on e.g. laptops ***/
//        if (d.dmDisplayOrientation != DMDO_DEFAULT) continue;

        if (d.dmBitsPerPel != setupcfg->scrBPP) continue;
        if (!nRes || ress[nRes-1].w != d.dmPelsWidth || ress[nRes-1].h != d.dmPelsHeight) {
          ress[nRes].w = d.dmPelsWidth;
          ress[nRes].h = d.dmPelsHeight;
          nRes++;
          _snprintf(buf,500,"%d * %d",d.dmPelsWidth,d.dmPelsHeight);
          SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)buf);
        }
      }

      int s = nRes - 1;
      for (i=0; i<nRes; i++)
        if (ress[i].w==setupcfg->scrWidth && ress[i].h==setupcfg->scrHeight)
          s = i;
      SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_SETCURSEL, s, 0);

      if (!setupcfg->nWindowed) {
        SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_SETCHECK, 1, 1);
        EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),0);
      }
      if (setupcfg->nVsync)
        SendDlgItemMessage(hWnd, IDC_VSYNC, BM_SETCHECK, 1, 1);
      if (setupcfg->nMultisample)
        SendDlgItemMessage(hWnd, IDC_MULTI, BM_SETCHECK, 1, 1);

    } break;
    case WM_COMMAND:
    switch( LOWORD(wParam) ) {
      case IDOK: {
        setupcfg->scrWidth = ress[SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0)].w;
        setupcfg->scrHeight= ress[SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_GETCURSEL, 0, 0)].h;
        setupcfg->nWindowed    =!SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0);
        setupcfg->nAlwaysOnTop = SendDlgItemMessage(hWnd, IDC_ONTOP,      BM_GETCHECK , 0, 0);
        setupcfg->nVsync       = SendDlgItemMessage(hWnd, IDC_VSYNC,      BM_GETCHECK , 0, 0);
        setupcfg->nMultisample = SendDlgItemMessage(hWnd, IDC_MULTI,      BM_GETCHECK, 0, 0);
        EndDialog (hWnd, TRUE);
      } break;
      case IDCANCEL: {
        EndDialog (hWnd, FALSE);
      } break;
      case IDC_FULLSCREEN: {
        /* cake. */
        if (SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0)) {
          SendDlgItemMessage(hWnd, IDC_ONTOP, BM_SETCHECK, 0, 0);
          EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),0);
        } else {
          EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),1);
        }
      } break;
    } break;
  }
  return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}

int OpenSetupDialog(fwzSettings * s) {
  setupcfg = s;

  std::string base, name, ico;
  if( get_app_folder(base, name) ) {
      name.resize( name.size() - 4 ); // remove ".exe"
      ico = name + ".ico";
      title = caption = name.c_str();
      icon = ico.c_str();
  }

  return DialogBox(s->hInstance,MAKEINTRESOURCE(IDD_SETUP),GetForegroundWindow(),DlgFunc);
}
