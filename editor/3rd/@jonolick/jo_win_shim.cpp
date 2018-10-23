#define UNICODE
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <stdio.h>
#include <windows.h>

const int width = 640;
const int height = 480;
char screen[width*height * 4];

void draw_screen() {
	// Draw to the screen!
	int X = 320, Y = 240;
	screen[ (X + Y * 640) * 4 + 0 ] = 000; // B
	screen[ (X + Y * 640) * 4 + 1 ] = 255; // G
	screen[ (X + Y * 640) * 4 + 2 ] = 000; // R
	screen[ (X + Y * 640) * 4 + 3 ] = 255; // A
}

LRESULT WINAPI WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	switch (message) {
	case WM_CHAR: DestroyWindow(window); break;
	case WM_ERASEBKGND: return 1;
	case WM_DESTROY: PostQuitMessage(0); return 0;
	case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC dc = BeginPaint(window, &ps);
			PatBlt(dc, 0, 0, 4096, 4096, BLACKNESS);
			EndPaint(window, &ps);
		}
		return 0;
	}
	return DefWindowProc(window, message, wparam, lparam);
}

//int PASCAL WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR cmd_line, int cmd_show) {
int main() {
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = 0;
	wc.lpszClassName = L"jo_win_shim";
	RegisterClass(&wc);
	HWND window = CreateWindow(L"jo_win_shim", L"jo_win_shim", WS_CAPTION | WS_POPUP | WS_CLIPCHILDREN | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX, 64, 64, 64, 64, 0, 0, 0, 0);

	// Calculate the extra width and height for the window.
	RECT r, c;
	GetWindowRect(window, &r);
	GetClientRect(window, &c);
	int extra_width = (r.right - r.left) - (c.right - c.left);
	int extra_height = (r.bottom - r.top) - (c.bottom - c.top);

	SetWindowPos(window, 0, 0, 0, width + extra_width, height + extra_height, SWP_NOMOVE);
	ShowWindow(window, SW_SHOWDEFAULT);

	while (true) {
		MSG msg;
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				exit(0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			draw_screen();
			BITMAPINFOHEADER bmiHeader;
			HDC dc;
			bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmiHeader.biWidth = width;
			bmiHeader.biHeight = -height;
			bmiHeader.biPlanes = 1;
			bmiHeader.biBitCount = 32;
			bmiHeader.biCompression = BI_RGB;
			dc = GetDC(window);
			GetClientRect(window, &c);
			StretchDIBits(dc, c.left, c.top, c.right, c.bottom, 0, 0, width, height, screen, (BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS, SRCCOPY);
			ReleaseDC(window, dc);
		}
	}
	return 0;
}
