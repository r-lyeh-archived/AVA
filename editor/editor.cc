// include opengl, imgui and all widgets.
#include "imgui/imgui.cpp"

#include "math/math_linear.c"
#include "math/math_transform.c"
#include "math/math_utils.c"

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


typedef unsigned char stbi_uc;
extern "C" stbi_uc* stbi_load_from_memory( const stbi_uc *bin, int len, int *x, int *y, int *n, int comp );
extern "C" void stbi_image_free( stbi_uc * );



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


// laurent couvidou
float vec3_dot(const vec3 v1, const vec3 v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
void rotate_vector_by_quat(vec3 vprime, const vec3 v, const quat q )
{
    // Extract the vector part of the quaternion
    vec3 u = {q[0], q[1], q[2]};

    // Extract the scalar part of the quaternion
    float s = q[3];

    // Do the math
    /*
    vprime = 2.0f * dot(u, v) * u
          + (s*s - dot(u, u)) * v
          + 2.0f * s * cross(u, v);
    */

    float ss = s * s;
    float s2 = s * 2;

    float duv2 = 2 * vec3_dot(u,v);
    float duuss = ss - vec3_dot(u,u);
    vec3 cuv; vec3_mul_cross(cuv, u, v);

    vprime[0] = duv2 * u[0] + duuss * v[0] + s2 * cuv[0];
    vprime[1] = duv2 * u[1] + duuss * v[1] + s2 * cuv[1];
    vprime[2] = duv2 * u[2] + duuss * v[2] + s2 * cuv[2];
}
void extract_direction( vec3 forward, vec3 right, vec3 up, const quat q ) {
    vec3 u = {0,1,0};
    vec3 f = {0,0,-1};
    vec3 r = {1,0,0};
    rotate_vector_by_quat( up, u, q );
    rotate_vector_by_quat( forward, f, q );
    rotate_vector_by_quat( right, r, q );
}

void GetRotation(float m[16], float& Yaw, float& Pitch, float& Roll) {
    // @test me!
    //     [0] [4] [8 ] [12]
    //     [1] [5] [9 ] [13]
    //     [2] [6] [10] [14]
    //     [3] [7] [11] [15]

    if( m[0] == 1.0f ) {
        Yaw = atan2f( m[2], m[11] );
        Pitch = 0;
        Roll = 0;
    }
    else if( m[0] == -1.0f ) {
        Yaw = atan2f( m[2], m[11] );
        Pitch = 0;
        Roll = 0;
    }
    else {
        Yaw = atan2( -m[8], m[0] );
        Pitch = asin( m[4] );
        Roll = atan2( -m[6], m[5] );
    }
}


// from wikipedia (in radians)
void toQuaternion(float q[4], float pitch, float roll, float yaw) {
    // Abbreviations for the various angular functions
    float cy = cosf(yaw * 0.5);
    float sy = sinf(yaw * 0.5);
    float cr = cosf(roll * 0.5);
    float sr = sinf(roll * 0.5);
    float cp = cosf(pitch * 0.5);
    float sp = sinf(pitch * 0.5);

    q[0] = cy * cr * cp + sy * sr * sp;
    q[1] = cy * sr * cp - sy * cr * sp;
    q[2] = cy * cr * sp + sy * sr * cp;
    q[3] = sy * cr * cp - cy * sr * sp;
}
void toEulerAngle(const float q[4], float *pitch, float *roll, float *yaw) {
    // roll (x-axis rotation)
    float sinr_cosp = +2.0 * (q[0] * q[1] + q[2] * q[3]);
    float cosr_cosp = +1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
    *roll = atan2f(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = +2.0 * (q[0] * q[2] - q[3] * q[1]);
    if (fabs(sinp) >= 1)
        *pitch = copysignf(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        *pitch = asinf(sinp);

    // yaw (z-axis rotation)
    float siny_cosp = +2.0 * (q[0] * q[3] + q[1] * q[2]);
    float cosy_cosp = +1.0 - 2.0 * (q[2] * q[2] + q[3] * q[3]);
    *yaw = atan2f(siny_cosp, cosy_cosp);
}


// https://github.com/procedural/gl_flycamera by @procedural (public domain)
#include <math.h>
static inline void flycamera(
    float * cam_vec3, float * cam_quat, float * cam_m3x3,
    float look_mult, float move_mult,
    float x_delta, float y_delta,
    float key_f, float key_l, float key_b, float key_r, float key_u, float key_d)
{
    // y_quat = quatFromAxisAngle({1, 0, 0}, y_delta * look_mult * to_radians)
    // x_quat = quatFromAxisAngle({0, 1, 0}, x_delta * look_mult * to_radians)

    float ydt = y_delta * look_mult * 0.017453f;
    float xdt = x_delta * look_mult * 0.017453f;
    float yqx = (float)sin(ydt);
    float yqw = (float)cos(ydt);
    float xqy = (float)sin(xdt);
    float xqw = (float)cos(xdt);

    // cam_quat = cam_quat * y_quat
    // cam_quat = x_quat * cam_quat

    float iqx = cam_quat[0];
    float iqy = cam_quat[1];
    float iqz = cam_quat[2];
    float iqw = cam_quat[3];

    float tqx = +iqw * yqx + iqx * yqw;
    float tqy = -iqz * yqx - iqy * yqw;
    float tqz = -iqy * yqx + iqz * yqw;
    float tqw = +iqx * yqx - iqw * yqw;

    float cqx = +tqz * xqy + tqx * xqw;
    float cqy = -tqw * xqy - tqy * xqw;
    float cqz = -tqx * xqy + tqz * xqw;
    float cqw = +tqy * xqy - tqw * xqw;

    // x_axis = rotateVec3ByQuat({-1, 0, 0}, cam_quat)
    // y_axis = rotateVec3ByQuat({ 0, 1, 0}, cam_quat)
    // z_axis = rotateVec3ByQuat({ 0, 0,-1}, cam_quat)

    float qxqx = cqx * cqx;
    float qxqy = cqx * cqy;
    float qxqz = cqx * cqz;
    float qxqw = cqx * cqw;

    float qyqy = cqy * cqy;
    float qyqz = cqy * cqz;
    float qyqw = cqy * cqw;

    float qzqz = cqz * cqz;
    float qzqw = cqz * cqw;

    float xax = (qyqy + qzqz) *  2.f - 1.f;
    float xay = (qxqy + qzqw) * -2.f;
    float xaz = (qyqw - qxqz) *  2.f;

    float yax = (qxqy - qzqw) *  2.f;
    float yay = (qxqx + qzqz) * -2.f + 1.f;
    float yaz = (qxqw + qyqz) *  2.f;

    float zax = (qyqw + qxqz) * -2.f;
    float zay = (qxqw - qyqz) *  2.f;
    float zaz = (qxqx + qyqy) *  2.f - 1.f;

    float xm = key_l - key_r;
    float ym = key_u - key_d;
    float zm = key_f - key_b;

    float mx = xax * xm + yax * ym + zax * zm;
    float my = xay * xm + yay * ym + zay * zm;
    float mz = xaz * xm + yaz * ym + zaz * zm;

    float ln = (float)sqrt(mx * mx + my * my + mz * mz);
    if (ln != 0) {
        mx /= ln;
        my /= ln;
        mz /= ln;
    }

    cam_vec3[0] += mx * move_mult;
    cam_vec3[1] += my * move_mult;
    cam_vec3[2] += mz * move_mult;

    cam_quat[0] =  cqx;
    cam_quat[1] =  cqy;
    cam_quat[2] =  cqz;
    cam_quat[3] =  cqw;

    cam_m3x3[0] = -xax;
    cam_m3x3[1] =  yax;
    cam_m3x3[2] = -zax;

    cam_m3x3[3] = -xay;
    cam_m3x3[4] =  yay;
    cam_m3x3[5] = -zay;

    cam_m3x3[6] = -xaz;
    cam_m3x3[7] =  yaz;
    cam_m3x3[8] = -zaz;
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
    int width, height, frame;
    // opengl
    GLuint texture_id;
} R = { {0,0}, 256, 256, 0, 0 };

void editor_init() {
    imgui_config();
    imgui_fonts();

    imgui_theme_cherry();
    imgui_style_framework();

    osc_socket = osc_listen("127.0.0.1", "9000");
}

void editor_tick()
{}

void editor_draw() {

#if WITH_MAINMENU
    imgui_menubar();
#endif

#if WITH_DOCKING
    imgui_dockspace_begin();
#endif

#if WITH_SCENE

    // update network buffers
    if( !R.buffer[0] ) {
        R.buffer[0] = (char*)calloc( 1, 1920 * 1080 * 4 );
        R.buffer[1] = (char*)calloc( 1, 1920 * 1080 * 4 );
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
                int ow = msg->i[0]; int w = is_pow2(ow) ? ow : next_pow2(ow);
                int oh = msg->i[1]; int h = is_pow2(oh) ? oh : next_pow2(oh);
                int format = msg->i[2];
                int y = msg->i[3];
                int size = msg->i[4];
                const char *data = msg->s[4];
                R.width = w;
                R.height = h;
                char *buf = &R.buffer[R.frame][ (0 + y * w) * 3 ];
                if( format == 888 ) memcpy( &R.buffer[R.frame][ (0 + y * w) * 3 ], data, ow*3 );
                if( format == 332 ) for( int x = 0; x < ow; ++x ) {
                    unsigned char p = data[x];
                    *buf++ = ((p & 0xE0)     );
                    *buf++ = ((p & 0x1C) << 3);
                    *buf++ = ((p       ) << 6);
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
        void *pixels = R.buffer[R.frame];
        int mip = 0, w = R.width, h = R.height;
        int texture_fmt = GL_RGB; // incl. compressed formats
        int image_fmt = GL_RGB, pixel_fmt = GL_UNSIGNED_BYTE;
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
        mat4 m;
        mat4_perspective( m, c->fov * deg2rad, c->ratio, c->znear, c->zfar );
        memcpy( c->projection, m, 16 * sizeof(float));
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

#if 0
    // orbit
    c->Distance = 8;
    float eye[] = { cosf(c->XAngle) * cosf(c->YAngle) * c->Distance, sinf(c->YAngle) * c->Distance, sinf(c->XAngle) * cosf(c->YAngle) * c->Distance };
    float at[] = { 0.f, 0.f, 0.f };
    float up[] = { 0.f, 1.f, 0.f };
    LookAt(eye, at, up, c->transform);
#endif

#if 1
    if( 1 ) { // if cam active
        static float cam_pos[3] = {10,10,10};
        static float cam_quat[4] = {0.852f,0.147f,-0.354f,0.355f};
        static float cam_mat3[9] = {0};
        static float cam_scale[3] = {1,1,1};

        float p,r,y;
        toEulerAngle(cam_quat, &p, &r, &y);
        bool vkf1 = (GetAsyncKeyState(VK_F1) & 0x8000), vkf2 = (GetAsyncKeyState(VK_F2) & 0x8000);
        if( vkf1 || vkf2 ) {
            p *= rad2deg;
            r *= rad2deg;
            y *= rad2deg;
            r = (int)r + (vkf1 ? +1 : -1); 
            float *q = cam_quat;
            toQuaternion(q, p * deg2rad, r * deg2rad, y * deg2rad);
        }

        float delta = 1; // 1/60.f
        float look_mult = 0.1f, move_mult = 0.5f;

        if( 1 ) {
            // inverted!
            flycamera( cam_pos, cam_quat, cam_mat3, look_mult, move_mult * delta, mouse_diff.x, -mouse_diff.y, vks, vkd, vkw, vka, vkc, vke );
            // inverted!
            mat4x4 s, t;
            mat4x4_identity(t);
            mat4x4_from_quat(t, cam_quat);
            mat4x4_scale_aniso(t, t, -cam_scale[0], -cam_scale[1], -cam_scale[2]);
            mat4_invert(s, t);
            mat4x4_translate_in_place(s, -cam_pos[0], -cam_pos[1], -cam_pos[2]);
            memcpy(c->transform, &s[0][0], 16 * sizeof(float));

            p = -p, r = -r, y = -y;
        } else {
            // regular
            flycamera( cam_pos, cam_quat, cam_mat3, look_mult, move_mult * delta, mouse_diff.x, mouse_diff.y, vkw, vka, vks, vkd, vke, vkc );
            // regular
            mat4x4 s;
            mat4x4_identity(s);
            mat4x4_from_quat(s, cam_quat);
            mat4x4_scale_aniso(s, s, cam_scale[0], cam_scale[1], cam_scale[2]);
            mat4x4_translate_in_place(s, cam_pos[0], cam_pos[1], cam_pos[2]);
            memcpy(c->transform, &s[0][0], 16 * sizeof(float));
        }

        PRINTF("Cam: %f %f %f (%ff,%ff,%ff,%ff) (%05.2fº pitch, %05.2fº roll, %05.2fº yaw)\n", cam_pos[0], cam_pos[1], cam_pos[2], cam_quat[0], cam_quat[1], cam_quat[2], cam_quat[3], p*rad2deg,r*rad2deg,y*rad2deg);
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
        imgui_texture( remote_id, ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y, false); // R.width, R.height, false );
    }
    ImGui::End();

    int flags = ImGui::IsMouseDown(0) ? 0 : ImGuiWindowFlags_NoMove;
    if( /*ImGui::IsMouseDown(1) ||*/ (ImGui::IsMouseDown(0) && ImGuizmo::IsOver()) ) ImGui::SetNextWindowFocus();
    if( ImGui::Begin("Viewport", NULL, flags) ) {
        ImVec2 cursor_pos = ImGui::GetCursorPos();

        // layer #0 (remoteview)
        // ImGui::SetCursorPos(cursor_pos);
        // imgui_texture( remote_id, R.width, R.height, false );

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
