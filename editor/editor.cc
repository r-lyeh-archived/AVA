// include opengl, imgui and all widgets.
#include "imgui/imgui.cpp"

#include "core/math/math_linear.c"
#include "core/math/math_transform.c"
#include "core/math/math_utils.c"
#include "input/input.c"

#include <timeapi.h> // timeBeginPeriod, timeEndPeriod
#include <thread>
#include <chrono>
#include <set>

// dir.h
#include <sys/stat.h>
#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif
bool dir_exists( const char *pathfile ) {
    struct stat s;
    return (stat(pathfile, &s) == 0 && S_ISDIR(s.st_mode));
}
// --
#ifdef _WIN32
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif

/*
typedef unsigned char stbi_uc;
extern "C" stbi_uc* stbi_load_from_memory( const stbi_uc *bin, int len, int *x, int *y, int *n, int comp );
extern "C" void stbi_image_free( stbi_uc * );
*/
#define STB_IMAGE_IMPLEMENTATION
#include "3rd/stb_image.h"



#define WITH_MAINMENU 1
#define WITH_DOCKING  1
#define WITH_SCENE    1
#define WITH_TOOLS    1
#define WITH_PANELS   1


#if 1 
#include <Windows.h>
// Oskar Dahlberg
//static NTSTATUS(__stdcall *NtDelayExecution)(BOOL Alertable, PLARGE_INTEGER DelayInterval) = (NTSTATUS(__stdcall*)(BOOL, PLARGE_INTEGER)) GetProcAddress(GetModuleHandle("ntdll.dll"), "NtDelayExecution");
//static NTSTATUS(__stdcall *ZwSetTimerResolution)(IN ULONG RequestedResolution, IN BOOLEAN Set, OUT PULONG ActualResolution) = (NTSTATUS(__stdcall*)(ULONG, BOOLEAN, PULONG)) GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwSetTimerResolution");
static int32_t(*NtDelayExecution)(BOOL Alertable, PLARGE_INTEGER DelayInterval) = (int32_t (__cdecl *)(BOOL,PLARGE_INTEGER))GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtDelayExecution");
static int32_t(*ZwSetTimerResolution)(IN ULONG RequestedResolution, IN BOOLEAN Set, OUT PULONG ActualResolution) = (int32_t (__cdecl *)(ULONG,BOOLEAN,PULONG))GetProcAddress(GetModuleHandleA("ntdll.dll"), "ZwSetTimerResolution");
static void SleepShort(float milliseconds) {
    static bool once = true;
    if (once) {
        ULONG actualResolution;
        ZwSetTimerResolution(1, true, &actualResolution);
        once = false;
    }
    LARGE_INTEGER interval;
    interval.QuadPart = -1 * (int)(milliseconds * 10000.0f);
    NtDelayExecution(false, &interval);
}
// Emil Gustafsson
void usleep(unsigned long usec) {
    struct timeval tv;
    fd_set dummy;
    SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    FD_ZERO(&dummy);
    FD_SET(s, &dummy);
    tv.tv_sec = usec/1000000L;
    tv.tv_usec = usec%1000000L;
    int r = select(0, 0, 0, &dummy, &tv); /* return bool: 0 == select() */
    closesocket(s);
}
// Adi Shavit
void usleep2(__int64 usec) {
    HANDLE timer; 
    LARGE_INTEGER ft; 
    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time
    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}
#endif


GLuint texturegen() {
    GLuint texture_id;
    glGenTextures( 1, &texture_id );
    glBindTexture( GL_TEXTURE_2D, texture_id );
    // clamping (s,t,r)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // border color
    float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // #if OPENGL >= 3
    // glGenerateMipmap(GL_TEXTURE_2D);
    // #endif
#if 1
    // Black/white checkerboard
    float pixels[] = {
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
    };
    int mip = 0, w = 2, h = 2;
    int texture_fmt = GL_RGB; // incl. compressed formats
    int image_fmt = GL_RGB, pixel_fmt = GL_FLOAT;
    glTexImage2D(GL_TEXTURE_2D, mip, texture_fmt, w, h, 0, image_fmt, pixel_fmt, pixels);
#else
    int x,y,n;
    unsigned char *pixels = stbi_load("image.png", &x, &y, &n, 4);
    int mip = 0, w = x, h = y;
    int texture_fmt = GL_RGBA; // incl. compressed formats
    int image_fmt = GL_RGBA, pixel_fmt = GL_UNSIGNED_BYTE;
    if( pixels) {
        glTexImage2D(GL_TEXTURE_2D, mip, texture_fmt, w, h, 0, image_fmt, pixel_fmt, pixels);
        stbi_image_free(pixels);
    }
#endif
    return texture_id;
}

void textureset() {
}




struct Camera2 {
   bool is_perspective;
   float XAngle, YAngle, Distance;
   struct { float fov, ratio, znear, zfar; };
   struct { float width, height; };

   //     OpenGL matrix convention for typical GL software
   //     float transform[16];
   //      
   //     [0] [4] [8 ] [12]
   //     [1] [5] [9 ] [13]
   //     [2] [6] [10] [14]
   //     [3] [7] [11] [15]
   //      
   //     [RIGHT.x] [UP.x] [BK.x] [POS.x]
   //     [RIGHT.y] [UP.y] [BK.y] [POS.y]
   //     [RIGHT.z] [UP.z] [BK.z] [POS.z]
   //     [       ] [    ] [    ] [US   ]
   float transform[16] = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f };

   float projection[16];
};


void imgui_dockspace_begin() {
    // config dockspace
    {
        // https://github.com/ocornut/imgui/issues/2109
        // This is how to create an invisible dockspace covering the whole screen:
        bool open = 1, *p_open = &open;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", p_open, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruDockspace;
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
}

void imgui_dockspace_end() {
    ImGui::End();
}

void profiler2_demo() {
    float msBegin[4] = { 0, 1, 2, 3 };
    float msEnd[4] = { 1, 3, 5, 7 };
    const char *label[4] = { "a", "b", "c", "d" };
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
    for( int i = 0; i < 4; ++i ) {
        float hue = i / 4.f;
        ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue, 0.6f, 0.6f).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue, 0.7f, 0.7f).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue, 0.8f, 0.8f).Value);
        ImGui::SetCursorPosX(msBegin[i] * 40.0f);
        ImGui::Button(label[i], ImVec2((msEnd[i] - msBegin[i]) * 40.0f, 0.0f));
        ImGui::PopStyleColor( 3 );
        ImGui::Separator();
    }
    ImGui::PopStyleVar(2);
}

#if 0
overlay

    bool p_open = 1;
    ImGui::SetNextWindowPos(ImVec2(x + 10, y + 10));
    // Demonstrate creating a simple static window with no decoration.
    ImGui::Begin("Fixed Overlay", &p_open, ImVec2(0,0), 0.90f /*opacity*/, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings);
    ImGui::End();

#endif

void stats_demo() {
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);

    ImGui::Separator();

    {
        static float fps_history[60] = {0};
        static float cpu_history[60] = {0};
        static float mem_history[60] = {0};
        static double left = 0;
        left -= ImGui::GetIO().DeltaTime;
        if( left <= 0 ) { // every sec
            left += 1.0;
            for( int i = 1; i < 60; ++i ) fps_history[i - 1] = fps_history[i];
            fps_history[59] = ImGui::GetIO().Framerate;
            for( int i = 1; i < 60; ++i ) cpu_history[i - 1] = cpu_history[i];
            cpu_history[59] = 1.00; //cpu_usage();
            for( int i = 1; i < 60; ++i ) mem_history[i - 1] = mem_history[i];
            mem_history[59] = 1024; //memory_usage();
        }

        char buf[256]; sprintf(buf, "fps:%6.2f cpu:%6.2f%% mem:%d/%d ##PERF", fps_history[59], cpu_history[59], (int)mem_history[59], (int)mem_history[59] );
        const char* label = buf;
        const char* names[] = { "FPS", "CPU", "MEM" };
        const ImColor colors[] = { PURPLE, GREEN, CYAN }; // also: blue, yellow; do not use { green, orange, red }
        const void * const datas[] = { fps_history, cpu_history, mem_history };
        int num_datas = sizeof(datas)/sizeof(datas[0]);
        int values_count = 60;
        float scale_min = 1;
        float scale_max = 100;
        ImVec2 graph_size( 315, 25 );

        ImGui::PlotMultiLines(
            label,
            num_datas,
            names,
            colors,
            [](const void* data, int idx) -> float { return ((float *)data)[idx]; },
            datas,
            values_count,
            scale_min,
            scale_max,
            graph_size);
    }
}

// -----------------

int osc_socket;

struct remote_render {
    // virtual framebuffer
    char *buffer[2];
    int width, height, frame, must_clear;
    // opengl
    GLuint texture_id;
} R = { {0,0}, 256, 256, 0, 0, 0 };

void editor_init() {
    imgui_config();
    imgui_fonts();

    imgui_theme_cherry();
    imgui_style_framework();

    osc_socket = osc_listen("127.0.0.1", "9000");
}

void editor_tick()
{}

auto is_pow2 = []( uint32_t v ) {
    return !((~(~0U>>1)|v)&v -1); // from SO
};
auto next_pow2 = []( uint32_t v ) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return ++v;
};
int align16(int x) {
    return (x + (16 - 1)) & ~(16 - 1);
}

void editor_draw() {

#if WITH_MAINMENU
    imgui_menubar();
#endif

#if WITH_DOCKING
    imgui_dockspace_begin();
#endif

#if WITH_SCENE

    enum { NETWORK_BUFFER_MAXSIZE = 1920 * 1080 * 4 };

    // update network buffers
    if( !R.buffer[0] ) {
        R.buffer[0] = (char*)calloc( 1, NETWORK_BUFFER_MAXSIZE );
        R.buffer[1] = (char*)calloc( 1, NETWORK_BUFFER_MAXSIZE );
    }
    auto network_update = [&]() {
        // update osc server
        osc_update( osc_socket, -1 ); // 16/*ms*/ );

        // render framebuffer into remotebuffer
        const osc_message *list;
        int i = 0, e = osc_list( &list );
        for( ; i < e; ++i ) {
            const osc_message *msg = list+i;
            //const osc_message *msg = osc_find("/render/");
            //if( msg ) {
                // @todo clear mem when remote viewport changes
                // iii i b: w,h,fmt, y, blob
                int ow = msg->i[0];
                int oh = msg->i[1];
                int format = msg->i[2];
                int y = msg->i[3];
                int size = msg->i[4];
                const char *data = msg->s[4];
                R.must_clear |= (R.width != ow) || (R.height != oh);
                R.width = ow;
                R.height = oh;
                int w = align16(ow); // w = is_pow2(ow) ? ow : next_pow2(ow);
                char *buf = &R.buffer[R.frame][ (0 + y * w) * 3 ];
                if( format == 888 ) memcpy( buf, data, ow*3 );
                if( format == 332 ) for( int x = 0; x < ow; ++x ) {
#if 1
                    unsigned char p = data[x];
                    *buf++ = ((p & 0xE0)     );
                    *buf++ = ((p & 0x1C) << 3);
                    *buf++ = ((p       ) << 6);
#else
                    // From https://github.com/r-lyeh-archived/rgb332
                    // Uniform palette (RGB 8x8x4 cube + a gamma ramp tables per channel + balanced R/G/B + true grays by compensating B channel).
                    // Note: try to dither image when using this palette.
                    // - rlyeh, public domain.
                    const unsigned char rgb332[256*3] = {
                    /*000000*/ 0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x7d,0x00,0x00,0xbb,0x00,0x33,0x00,0x0f,
                    /*000010*/ 0x33,0x3e,0x18,0x33,0x7d,0x1d,0x33,0xbb,0x00,0x56,0x00,0x12,0x56,0x3e,0x1f,0x56,
                    /*000020*/ 0x7d,0x26,0x56,0xbb,0x00,0x76,0x00,0x13,0x76,0x3e,0x21,0x76,0x7d,0x2c,0x76,0xbb,
                    /*000030*/ 0x00,0x94,0x00,0x13,0x94,0x3e,0x23,0x94,0x7d,0x2f,0x94,0xbb,0x00,0xaf,0x00,0x14,
                    /*000040*/ 0xaf,0x3e,0x24,0xaf,0x7d,0x31,0xaf,0xbb,0x00,0xc9,0x00,0x14,0xc9,0x3e,0x24,0xc9,
                    /*000050*/ 0x7d,0x32,0xc9,0xbb,0x00,0xe3,0x00,0x14,0xe3,0x3e,0x25,0xe3,0x7d,0x33,0xe3,0xbb,
                    /*000060*/ 0x25,0x00,0x00,0x25,0x23,0x3e,0x25,0x2e,0x7d,0x25,0x33,0xbb,0x25,0x33,0x05,0x2f,
                    /*000070*/ 0x3e,0x43,0x36,0x45,0x81,0x3a,0x49,0xbf,0x25,0x56,0x06,0x32,0x5d,0x44,0x3c,0x62,
                    /*000080*/ 0x82,0x43,0x65,0xc0,0x25,0x76,0x07,0x33,0x7b,0x45,0x3f,0x7f,0x83,0x48,0x81,0xc1,
                    /*000090*/ 0x25,0x94,0x07,0x35,0x97,0x45,0x41,0x99,0x83,0x4d,0x9c,0xc2,0x25,0xaf,0x07,0x36,
                    /*0000a0*/ 0xb2,0x46,0x42,0xb4,0x84,0x4f,0xb6,0xc2,0x25,0xc9,0x08,0x36,0xcc,0x46,0x43,0xcd,
                    /*0000b0*/ 0x84,0x51,0xcf,0xc2,0x25,0xe3,0x08,0x36,0xe5,0x46,0x44,0xe6,0x84,0x52,0xe8,0xc2,
                    /*0000c0*/ 0x46,0x00,0x00,0x46,0x2a,0x3e,0x46,0x3b,0x7d,0x46,0x46,0xbb,0x46,0x33,0x08,0x4e,
                    /*0000d0*/ 0x44,0x45,0x54,0x50,0x83,0x58,0x59,0xc1,0x46,0x56,0x0b,0x48,0x48,0x48,0x59,0x69,
                    /*0000e0*/ 0x86,0x60,0x70,0xc4,0x46,0x76,0x0c,0x53,0x7e,0x4a,0x5d,0x84,0x88,0x65,0x8a,0xc6,
                    /*0000f0*/ 0x46,0x94,0x0d,0x54,0x99,0x4b,0x5f,0x9f,0x89,0x69,0xa3,0xc7,0x46,0xaf,0x0e,0x55,
                    /*000100*/ 0xb4,0x4c,0x61,0xb8,0x8a,0x6b,0xbc,0xc8,0x46,0xc9,0x0e,0x55,0xcd,0x4c,0x62,0xd1,
                    /*000110*/ 0x8a,0x6d,0xd4,0xc8,0x46,0xe3,0x0f,0x56,0xe6,0x4d,0x63,0xea,0x8b,0x6f,0xec,0xc9,
                    /*000120*/ 0x66,0x00,0x00,0x66,0x2e,0x3e,0x66,0x44,0x7d,0x66,0x52,0xbb,0x66,0x33,0x0a,0x6c,
                    /*000130*/ 0x48,0x47,0x71,0x56,0x85,0x75,0x62,0xc3,0x66,0x56,0x0e,0x6f,0x64,0x4b,0x77,0x6f,
                    /*000140*/ 0x89,0x7d,0x78,0xc7,0x66,0x76,0x11,0x71,0x80,0x4e,0x7a,0x89,0x8c,0x82,0x90,0xc9,
                    /*000150*/ 0x66,0x94,0x12,0x72,0x9c,0x50,0x7d,0xa2,0x8d,0x85,0xa8,0xcb,0x66,0xaf,0x13,0x73,
                    /*000160*/ 0xb6,0x51,0x7e,0xbb,0x8f,0x87,0xc0,0xcc,0x66,0xc9,0x14,0x74,0xcf,0x52,0x80,0xd4,
                    /*000170*/ 0x90,0x89,0xd8,0xcd,0x66,0xe3,0x15,0x74,0xe8,0x53,0x81,0xec,0x90,0x8b,0xf0,0xce,
                    /*000180*/ 0x85,0x00,0x00,0x85,0x2f,0x3e,0x85,0x48,0x7d,0x85,0x5a,0xbb,0x85,0x33,0x0b,0x8a,
                    /*000190*/ 0x4a,0x48,0x8e,0x5c,0x86,0x91,0x69,0xc4,0x85,0x56,0x11,0x8d,0x67,0x4e,0x93,0x74,
                    /*0001a0*/ 0x8b,0x99,0x7f,0xc9,0x85,0x76,0x14,0x8e,0x82,0x51,0x97,0x8d,0x8f,0x9e,0x96,0xcc,
                    /*0001b0*/ 0x85,0x94,0x17,0x90,0x9e,0x54,0x91,0x91,0x91,0xa1,0xae,0xcf,0x85,0xaf,0x18,0x91,
                    /*0001c0*/ 0xb8,0x56,0x9b,0xbf,0x93,0xa4,0xc5,0xd1,0x85,0xc9,0x19,0x91,0xd1,0x57,0x9c,0xd6,
                    /*0001d0*/ 0x94,0xa6,0xdc,0xd2,0x85,0xe3,0x1a,0x92,0xe9,0x58,0x9e,0xee,0x96,0xa8,0xf3,0xd3,
                    /*0001e0*/ 0xa3,0x00,0x00,0xa3,0x30,0x3e,0xa3,0x4b,0x7d,0xa3,0x5f,0xbb,0xa3,0x33,0x0c,0xa8,
                    /*0001f0*/ 0x4d,0x49,0xab,0x60,0x87,0xae,0x6f,0xc5,0xa3,0x56,0x13,0xaa,0x69,0x50,0xb0,0x78,
                    /*000200*/ 0x8d,0xb4,0x84,0xcb,0xa3,0x76,0x17,0xac,0x84,0x54,0xb3,0x90,0x91,0xb9,0x9a,0xcf,
                    /*000210*/ 0xa3,0x94,0x1a,0xad,0x9f,0x57,0xb5,0xa9,0x95,0xbd,0xb2,0xd2,0xa3,0xaf,0x1c,0xae,
                    /*000220*/ 0xb9,0x5a,0xb7,0xc1,0x97,0xc0,0xc9,0xd4,0xa3,0xc9,0x1e,0xaf,0xd2,0x5b,0xb9,0xd9,
                    /*000230*/ 0x99,0xc2,0xdf,0xd6,0xa3,0xe3,0x1f,0xaf,0xea,0x5d,0xba,0xf1,0x9a,0xc3,0xf6,0xd8,
                    /*000240*/ 0xc1,0x00,0x00,0xc1,0x31,0x3e,0xc1,0x4e,0x7d,0xc1,0x63,0xbb,0xc1,0x33,0x0c,0xc4,
                    /*000250*/ 0x4e,0x4a,0xc8,0x63,0x88,0xca,0x74,0xc5,0xc1,0x56,0x14,0xc7,0x6a,0x51,0xcc,0x7b,
                    /*000260*/ 0x8f,0xd0,0x88,0xcc,0xc1,0x76,0x19,0xc9,0x86,0x56,0xd0,0x93,0x94,0xd5,0x9f,0xd1,
                    /*000270*/ 0xc1,0x94,0x1d,0xca,0xa0,0x5a,0xd1,0xab,0x97,0xd9,0xb5,0xd5,0xc1,0xaf,0x20,0xcb,
                    /*000280*/ 0xb9,0x5d,0xd3,0xc4,0x9a,0xdc,0xcc,0xd8,0xc1,0xc9,0x22,0xcc,0xd3,0x5f,0xd5,0xdb,
                    /*000290*/ 0x9d,0xda,0xda,0xda,0xc1,0xe3,0x24,0xcc,0xeb,0x61,0xd6,0xf2,0x9e,0xdf,0xf9,0xdc,
                    /*0002a0*/ 0xde,0x00,0x00,0xde,0x31,0x3e,0xde,0x50,0x7d,0xde,0x66,0xbb,0xde,0x33,0x0d,0xe1,
                    /*0002b0*/ 0x4f,0x4a,0xe4,0x65,0x88,0xe7,0x78,0xc6,0xde,0x56,0x15,0xe4,0x6c,0x53,0xe9,0x7e,
                    /*0002c0*/ 0x90,0xec,0x8c,0xce,0xde,0x76,0x1b,0xe5,0x87,0x58,0xeb,0x96,0x96,0xf1,0xa2,0xd3,
                    /*0002d0*/ 0xde,0x94,0x20,0xe7,0xa1,0x5d,0xed,0xae,0x9a,0xf4,0xb9,0xd7,0xde,0xaf,0x23,0xe8,
                    /*0002e0*/ 0xbb,0x60,0xef,0xc5,0x9d,0xf7,0xcf,0xdb,0xde,0xc9,0x26,0xe8,0xd4,0x63,0xf1,0xdd,
                    /*0002f0*/ 0xa0,0xf9,0xe5,0xdd,0xde,0xe3,0x28,0xe9,0xec,0x65,0xf2,0xf4,0xa2,0xfd,0xfd,0xef
                    };

                    unsigned char p = data[x];
                    *buf++ = rgb332[p*3+0];
                    *buf++ = rgb332[p*3+1];
                    *buf++ = rgb332[p*3+2];
#endif
                }
                if( format == 242 ) for( int x = 0; x < ow; ++x ) {
                    unsigned char p = data[x];
                    *buf++ = ((p & 0xC0)     );
                    *buf++ = ((p & 0x3C) << 2);
                    *buf++ = ((p       ) << 6);
                }
                if( format == 7755 ) for( int x = 0; x < ow / 2; ++x ) {
                    // Y0Y1CoCg 7:7:5:5 (24 bits, 2 pixels)
                    // 76543210 76543210 76543210
                    // YYYYYYYY YYYYYYCC CCCCCCCC
                    // 00000001 111111oo oooggggg
                    const uint8_t *d = (const uint8_t *)(&data[x*3]);
                    uint32_t y0 = ((d[0]>>1));
                    uint32_t y1 = ((d[0]&1)<<6)|(d[1]>>2);
                    uint32_t co = ((d[1]&3)<<3)|(d[2]>>5);
                    uint32_t cg = ((d[2]&31));
                    // dequantize
                    y0 = y0 * 2;
                    y1 = y1 * 2;
                    co = co * 8;
                    cg = cg * 8;
                    // 
                    co += 128;
                    cg += 128;
                    // reconstruct first r,g,b triplet
                    uint32_t r0 = y0 + co - cg;
                    uint32_t g0 = y0      + cg;
                    uint32_t b0 = y0 - co - cg;
                    *buf++ = (uint8_t)r0;
                    *buf++ = (uint8_t)g0;
                    *buf++ = (uint8_t)b0;
                    // reconstruct second r,g,b triplet
                    uint32_t r1 = y1 + co - cg;
                    uint32_t g1 = y1      + cg;
                    uint32_t b1 = y1 - co - cg;
                    *buf++ = (uint8_t)r1;
                    *buf++ = (uint8_t)g1;
                    *buf++ = (uint8_t)b1;
                }
                //++socket_numrecv;
                //++socket_activity;
            //}
        }
        //if(e) printf( "%d,", (int)list[0].i[2] );
        return e;
    };
    static bool threaded = (std::thread( [&]() { for(;;) { while( network_update() > 0 ); Sleep(1); } } ).detach(), 1 );
    // Sleep(0);
    // std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    // timeBeginPeriod(1); Sleep(1); timeEndPeriod(1);
    // usleep(1);
    // usleep2(1);
    // SleepShort(0.5f);

    // update remoteview id texture
    static GLuint remote_id = 0;
    if( !remote_id ) {
        // remote_id = texgen();
        glGenTextures(1, &remote_id);
    }
    if( remote_id ) {
        // texupdate( remote_id, R.buffer, 256, 256 );
        glBindTexture( GL_TEXTURE_2D, remote_id );
        // clamping (s,t,r)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // border color
        float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
        // filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // #if OPENGL >= 3
        // glGenerateMipmap(GL_TEXTURE_2D);
        // #endif
        int texture_fmt = GL_RGB; // incl. compressed formats
        int image_fmt = GL_RGB, pixel_fmt = GL_UNSIGNED_BYTE;
        int mip = 0;
        int w = R.width; w = align16(w); //w = is_pow2(w) ? w : next_pow2(w);
        int h = R.height; h = align16(h); //h = is_pow2(h) ? h : next_pow2(h);
        void *pixels = R.buffer[R.frame];
        if( R.must_clear ) {
            memset( R.buffer[0], 0, 1920*1080*4 );
            memset( R.buffer[1], 0, 1920*1080*4 );
            R.must_clear = 0;
        }
        glTexImage2D(GL_TEXTURE_2D, mip, texture_fmt, w, h, 0, image_fmt, pixel_fmt, pixels);
    }

    // create suru icons
    static GLuint suru_id = 0;
    if( !suru_id ) {
        // suru_id = texgen();
        glGenTextures(1, &suru_id);



        // texupdate( suru_id, R.buffer, 256, 256 );
        glBindTexture( GL_TEXTURE_2D, suru_id );
        // clamping (s,t,r)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // border color
        float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
        // filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // #if OPENGL >= 3
        // glGenerateMipmap(GL_TEXTURE_2D);
        // #endif


        int x,y,n;
        stbi_uc *pixels = stbi_load_from_memory((const stbi_uc*)suru_data, suru_size, &x, &y, &n, 4);
        int mip = 0, w = x, h = y;
        int texture_fmt = GL_RGBA; // incl. compressed formats
        int image_fmt = GL_RGBA, pixel_fmt = GL_UNSIGNED_BYTE;
        if( pixels) {
            glTexImage2D(GL_TEXTURE_2D, mip, texture_fmt, w, h, 0, image_fmt, pixel_fmt, pixels);
            stbi_image_free(pixels);
        }
    }

    float rad2deg = (180.0 / M_PI);
    float deg2rad = (M_PI / 180.0);
    static Camera2 cam, *c = 0;
    if( !c ) {
        c = &cam;
        c->YAngle = 52.f / 180.f * 3.14159f;
        c->XAngle = 165.f / 180.f * 3.14159f;
        c->Distance = 1; //8.f;
        c->is_perspective = 1; // 1
        c->fov = 45, c->znear = 0.001f, c->zfar = 100.f; // for persp
    }
    // for ortho
    ImGuiIO& io = ImGui::GetIO();
    c->width = ImGui::GetContentRegionAvail().x;
    c->height = ImGui::GetContentRegionAvail().y;
    // for persp
    c->ratio = c->width / c->height;

    if( ImGui::Begin("Camera") ) {
        ImGui::SliderFloat("fov", &c->fov, 10, 120 );
    }
    ImGui::End();

    if (c->is_perspective) {
        perspective44( c->projection, c->fov * deg2rad, c->ratio, c->znear, c->zfar );
    } else {
        OrthoGraphic(-c->width, c->width, -c->height, c->height, -c->width, c->width, c->projection);
    }

    // KeyCtrl, KeyShift, KeyAlt, KeySuper, KeysDown
    const float movement_speed = 0.1f;
    float vkw = !!(io.KeysDown[87/*GLFW_KEY_W*/] | io.KeysDown[265/*GLFW_KEY_UP*/]);
    float vka = !!(io.KeysDown[65/*GLFW_KEY_A*/] | io.KeysDown[263/*GLFW_KEY_LEFT*/]);
    float vks = !!(io.KeysDown[83/*GLFW_KEY_S*/] | io.KeysDown[264/*GLFW_KEY_DOWN*/]);
    float vkd = !!(io.KeysDown[68/*GLFW_KEY_D*/] | io.KeysDown[262/*GLFW_KEY_RIGHT*/]);
    float vke = !!(io.KeysDown[69/*GLFW_KEY_E*/] | io.KeysDown[81/*GLFW_KEY_Q*/]);
    float vkc = !!(io.KeysDown[67/*GLFW_KEY_C*/]);

    bool cam_active = ImGui::IsMouseDown(1) && !ImGuizmo::IsUsing();

    static ImVec2 mouse_prev = {}, mouse_now = {}, mouse_diff = {};
    mouse_now = ImGui::GetMousePos();
    mouse_diff = mouse_now - mouse_prev;
    mouse_prev = mouse_now;

    if( cam_active ) {
        //ImGui::CaptureKeyboardFromApp(false);
        //ImGui::SetWindowFocus();
    } else {
        //ImGui::CaptureKeyboardFromApp(true);
        mouse_diff = ImVec2(0,0);
    }

    float mouse_speed = 0.01f; //0.01f; // sensitivity
    if( cam_active ) {
        c->XAngle += mouse_diff.x * mouse_speed;
        c->YAngle += mouse_diff.y * mouse_speed;
    }

#if 1

    if( 1 ) { // if cam active
        static vec3 cam_pos = vec3(10,10,10);
        static vec2 cam_rot = vec2(0,0);
        static quat cam_quat = quat(0.852f,0.147f,-0.354f,0.355f);
        static vec3 cam_scale = vec3(1,1,1);
        static mat44 cam_mat4 = {0};

        float p,r,y;
        toEulerAngle(cam_quat, &p, &r, &y);
        bool vkf1 = (GetAsyncKeyState(VK_F1) & 0x8000), vkf2 = (GetAsyncKeyState(VK_F2) & 0x8000);
        if( vkf1 || vkf2 ) {
            p *= rad2deg;
            r *= rad2deg;
            y *= rad2deg;
            r = (int)r + (vkf1 ? +1 : -1); 
            toQuaternion(cam_quat, p * deg2rad, r * deg2rad, y * deg2rad);
        }

        float delta = 1/60.f;
        float look_mult = 0.1f;
        float move_mult = 1 * delta; // 1 (speed); also (key('LSHF') ? 3 : 1) * delta;
        vec2 look_delta = scale2(vec2(mouse_now.x, mouse_now.y), 1);
        vec3 move_delta = scale3(vec3(vkd-vka,vke-vkc,vkw-vks), move_mult);

        static camera cc, *cam = 0;
        if( !cam ) { cam = &cc; camera_create(cam, 0.25f, false, true ); }
        camera_fps( cam, move_delta, look_delta );
        copy44(c->transform, cam->view);

        PRINTF("Cam: %f %f %f (%ff,%ff,%ff,%ff) (%05.2fº pitch, %05.2fº roll, %05.2fº yaw)\n", cam_pos.x, cam_pos.y, cam_pos.z, cam_quat.x, cam_quat.y, cam_quat.z, cam_quat.w, p*rad2deg,r*rad2deg,y*rad2deg);
     }


     if( ImGui::Begin("Scene") ) {
        ImGui::Text("Scene graph");

        // [see] https://github.com/ocornut/imgui/issues/143

        static int selected = 0;

        auto left = [&]() {
            // User state
            const int COUNT = 5;
            static const char* items_data[COUNT] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
            static int items_list[COUNT] = { 0, 1, 2, 3, 4 };

            // Render + dragging
            for (int n = 0; n < COUNT; n++)
            {
                int item_no = items_list[n];
                ImGui::Selectable(items_data[item_no]);

                if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                {
                    float drag_dy = ImGui::GetMouseDragDelta(0).y;
                    if (drag_dy < 0.0f && n > 0)
                    {
                        // Swap
                        items_list[n] = items_list[n-1];
                        items_list[n-1] = item_no;
                        ImGui::ResetMouseDragDelta();
                    }
                    else if (drag_dy > 0.0f && n < COUNT-1)
                    {
                        items_list[n] = items_list[n+1];
                        items_list[n+1] = item_no;
                        ImGui::ResetMouseDragDelta();
                    }
                }
            }

            struct funcs
            {
                static void ShowDummyObject(const char* prefix, int uid)
                {
                    ImGui::PushID(uid);                // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
                    ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
                    bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
                    if (node_open) {
                        static float dummy_members[8] = { 0.0f,0.0f,1.0f,3.1416f,100.0f,999.0f };
                        for (int i = 0; i < 8; i++)
                        {
                            ImGui::PushID(i); // Use field index as identifier.
                            if (i < 2) {
                                ShowDummyObject("Child", 424242);
                            }
                            else {
                                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                                ImGui::AlignTextToFramePadding();
                                ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet, "Field_%d", i);
                            }
                            ImGui::PopID();
                        }
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
            };

            funcs::ShowDummyObject("Object", 0);

            for (int i = 0; i < 100; i++)
            {
                char label[128];
                sprintf(label, "MyObject %d", i);
                if (ImGui::Selectable(label, selected == i))
                    selected = i;
            }
        };

        auto right = [&]() {
            ImGui::Text("MyObject: %d", selected);
            ImGui::Separator();
            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");

            property_panel_demo();
        };

        // left
        ImGui::BeginChild("left pane", ImVec2(150, 0), true);
        left();
        ImGui::EndChild();
        ImGui::SameLine();

        // right
        ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
                right();
            ImGui::EndChild();
            if (ImGui::Button("Revert")) {}
            ImGui::SameLine();
            if (ImGui::Button("Save")) {}
        ImGui::EndGroup();

     }
     ImGui::End();


#endif

#if 0
    //     OpenGL matrix convention for typical GL software
    //     float transform[16];
    //      
    //     [0] [4] [8 ] [12]
    //     [1] [5] [9 ] [13]
    //     [2] [6] [10] [14]
    //     [3] [7] [11] [15]
    //      
    //     [RIGHT.x] [UP.x] [BK.x] [POS.x]
    //     [RIGHT.y] [UP.y] [BK.y] [POS.y]
    //     [RIGHT.z] [UP.z] [BK.z] [POS.z]
    //     [       ] [    ] [    ] [US   ]
    //

    float yaw = c->XAngle, pitch = c->YAngle, roll = 0;

#if 0
    float pos[3] = { c->transform[12], c->transform[13], c->transform[14] };
    mat4 m;
    RotationPitchYawRoll( m, pitch, yaw, roll );
    memcpy(c->transform, &m[0][0], 16 * sizeof(float) );
    memcpy(&c->transform[12], pos, 3 * sizeof(float) );
    float *right = &c->transform[0];
    float *up = &c->transform[4];
    float *forward = &c->transform[8]; // backward!
#else
    float cy = cos(yaw), sy = sin(yaw);
    float cp = cos(pitch), sp = sin(pitch);
    float cr = cos(roll), sr = sin(roll);
    float right   [] = {  cr * cy,                 sr * cy,               -sy      };
    float upv     [] = { -sr * cp + cr * sy * sp,  cr * cp + sr * sy * sp, cy * sp };
    float forward [] = {  sr * sp + cr * sy * cp, -cr * sp + sr * sy * cp, cy * cp };
    memcpy(&c->transform[0], right, 3 * sizeof(float) );
    memcpy(&c->transform[4], upv, 3 * sizeof(float) );
    memcpy(&c->transform[8], forward, 3 * sizeof(float) );
#endif

    float *position = &c->transform[12];
    //position[0] += (kw - ks) * movement_speed * forward[0] + (kd - ka) * movement_speed * right[0];
    //position[1] += (kw - ks) * movement_speed * forward[1] + (kd - ka) * movement_speed * right[1];
    //position[2] += (kw - ks) * movement_speed * forward[2] + (kd - ka) * movement_speed * right[2];
    printf("%f %f %f (%f %f)\n", position[0], position[1], position[2], c->XAngle, c->YAngle);

#endif

     if( 0 )
     {
        /*
        int tex_px = 64;
        int images_per_line = (ImGui::GetWindowContentRegionWidth() / (tex_px * 1.20));
        ImVec2 zoom( tex_px * 4, tex_px * 4 );
        if( images_per_line ) for( int ID = 1; ID <= texture_count; ++ID ) {
            ImGui::PushID(ID);

            imgui_texture( ID % texture_count, tex_px, tex_px, true );

            if ((ID % images_per_line) < (images_per_line-1)) {
                ImGui::SameLine();
            }
            ImGui::PopID();
        }
        */
     }


    #if 1 
    if( ImGui::Begin("Game") ) {
        ImVec2 cursor_pos = ImGui::GetCursorPos();

        // layer #0 (remoteview)
        ImGui::SetCursorPos(cursor_pos);

        float width = ImGui::GetContentRegionAvail().x, height = ImGui::GetContentRegionAvail().y;
        imgui_texture( remote_id, width, height, false );
    }
    ImGui::End();

    int flags = ImGui::IsMouseDown(0) ? 0 : ImGuiWindowFlags_NoMove;
    if( /*ImGui::IsMouseDown(1) ||*/ (ImGui::IsMouseDown(0) && ImGuizmo::IsOver()) ) ImGui::SetNextWindowFocus();
    if( ImGui::Begin("Viewport", NULL, flags) ) {
        ImVec2 cursor_pos = ImGui::GetCursorPos();

        // layer #0 (remoteview)
        // ImGui::SetCursorPos(cursor_pos);
        // imgui_texture( remote_id, R.width, R.height, false );

#if 0
        // commented out because of m_math.h
        // layer #1 (debugdraw)
        ImGui::SetCursorPos(cursor_pos);
        {
            using namespace im3d;
            static Camera cam = { {0, 0, 90}, {0,0,0} };
            static Viewport vp; vp.yfovdeg = 70;
            static bool rotate[3] = {1,1,0};

            static int selected = 0; // cube
            static Mesh *prims[] = {
                #define SHAPE_NEW_MESH(prim,a,b) new Mesh(#prim,shape_##prim##_face_count,shape_##prim##_vertex_count,shape_##prim##_face,shape_##prim##_vertex),
                SHAPE_XMACRO(SHAPE_NEW_MESH)
            };

            auto xy = ImGui::GetWindowPos(); // ImGui::GetCursorPos(); // 
            auto wh = ImVec2( R.width, R.height ); // ImGui::GetWindowSize(); // 
            vp.x = xy.x, vp.y = xy.y, vp.w = wh.x, vp.h = wh.y;

            Mesh &prim = *prims[selected];
            Render( cam, &vp, 1, &prim );
            vec3_set( prim.Rotation, X(prim.Rotation) + 0.5f * rotate[0], Y(prim.Rotation) + 0.5f * rotate[1], Z(prim.Rotation) + 0.5f * rotate[2] );

            char buf[128];
            sprintf(buf, "%s, vp: %f,%f %f,%f", prim.Name, vp.x, vp.y, vp.w, vp.h);
            ImGui::TextDisabled( buf );
        }
#endif

        // layer #2 (gizmo)
        ImGui::SetCursorPos(cursor_pos);
        ImGuizmo::SetDrawlist();
        gizmo_demo1( c->transform, c->projection, c->is_perspective );
    }
    ImGui::End();
    #endif
#endif

#if WITH_TOOLS
    imgui_pangram();
    imgui_icons();
    sequencer_demo();
    profiler_demo();
    browser_demo();

    static ImGui::Nodes nodes_;
    if( ImGui::Begin("Nodes") ) {
        nodes_.ProcessNodes();
    }
    ImGui::End();

    // alt, monospaced font
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        static int init = (texteditor_demo_init(),1); 
        texteditor_demo_draw();

        static MemoryEditor mem_edit_1;
        char *mem_block = "oh lala"; size_t mem_block_size = strlen(mem_block) + 1;
        mem_edit_1.DrawWindow("Memory Editor", mem_block, mem_block_size, 0x0000); // create a window and draw memory editor (if you already have a window, use DrawContents())
    ImGui::PopFont();

    if( ImGui::Begin("Log") ) {
        console_demo();
    }
    ImGui::End();
#endif

#if WITH_PANELS
// floating content
    if( ImGui::Begin("toolbar demo 1" /*, NULL, ImGuiWindowFlags_NoTitleBar */) ) {
        toolbar_panel_demo();
    }
    ImGui::End();

    if( ImGui::Begin("demo 1") ) {
        PlotVar("fps", ImGui::GetIO().Framerate); // if(t>60s) PlotVarFlushOldEntries(), t = 0;

        stats_demo();

        if( 1/*app('load')*/ ) {
            ImGui::Separator();
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]); // roboto-m-16
            ImGui::TextColored(ImVec4(1.0f,1.0f,1.0f,1.0f),"loading");
            ImGui::PopFont();
            //ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(0/7.0f, 0.5f, 0.5f));
            ImGui::ProgressBar(0.5f, ImVec2(-1/*100%*/,2/*px*/), "loading");
            //ImGui::PopStyleColor();

            ImGui::Separator();
        }

        profiler2_demo();
        spinner_demo();

        static GLuint texture_id = 0;
        if( !texture_id ) {
            texture_id = texturegen();
        }
        if( texture_id ) {
            imgui_texture( texture_id, 2, 2, true );
        /*
            int w = 1024, h = 768, d = 3;
            char *pixels = render_consume();
            if( pixels ) {
                //glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, w, h, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, pixels);
                //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
                glBindTexture( GL_TEXTURE_2D, texture_id );
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
                render_discard(pixels);
            }
        */
        }
    }
    ImGui::End();

    if( ImGui::Begin("demo 2") ) {
        extern bool show_demo_window; ImGui::Checkbox("Demo Window", &show_demo_window);
        static bool rec = 0; if( ImGui::Checkbox("Record", &rec) ) set_render('rec0', (double)!!rec);

        property_panel_demo();

        table_panel_demo();
        group_demo();
        richtext_demo();
    }
    ImGui::End();

#endif

    if( ImGui::Begin("Texture viewer") ) {
        GLuint tc;
        glGenTextures( 1, &tc );
        int texture_count = (int)tc;
        glDeleteTextures( 1, &tc );

        int tex_px = 64;
        int images_per_line = (ImGui::GetWindowContentRegionWidth() / (tex_px * 1.20));
        ImVec2 zoom( tex_px * 4, tex_px * 4 );
        if( images_per_line ) for( int ID = 1; ID <= texture_count; ++ID ) {
            ImGui::PushID(ID);

            imgui_texture( ID % texture_count, tex_px, tex_px, true );

            if ((ID % images_per_line) < (images_per_line-1)) {
                ImGui::SameLine();
            }
            ImGui::PopID();
        }
    }
    ImGui::End();


#if WITH_DOCKING
    // dockspace space
    // ...

    imgui_dockspace_end();
#endif

    // overlay space
    // ...
}

void editor_drop()
{}




int imgui_main(int, char**);

EXPORT
int main(int argc, char **argv) {
    return imgui_main(argc, argv);
}
