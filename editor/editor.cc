// include opengl, imgui and all widgets.
#include "imgui/imgui.cpp"

#include <thread>

#define WITH_MAINMENU 1
#define WITH_DOCKING  1
#define WITH_SCENE3D  1
#define WITH_TOOLS    1
#define WITH_PANELS   1


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


void imgui_dockspace() {
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

struct remote_render {
    // virtual framebuffer
    char *buffer[2];
    int width, height, frame;
    // opengl
    GLuint texture_id;
} R = { {0,0}, 256, 256, 0, 0 };


int osc_socket;

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

    if( !R.buffer[0] ) {
        R.buffer[0] = (char*)calloc( 1, 1920 * 1080 * 4 );
        R.buffer[1] = (char*)calloc( 1, 1920 * 1080 * 4 );
    }
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
    auto network_update = [&]() {
        // @todo clear mem when remote viewport changes
        // update osc server
        osc_update( osc_socket );
        // render framebuffer into remotebuffer
        const osc_message *list;
        int i = 0, e = osc_list( &list );
        for( ; i < e; ++i ) {
            const osc_message *msg = list+i;
            //const osc_message *msg = osc_find("/render/");
            //if( msg ) {
                int w = msg->i[0]; w = is_pow2(w) ? w : next_pow2(w);
                int h = msg->i[1]; h = is_pow2(h) ? h : next_pow2(h);
                int y = msg->i[2];
                int size = msg->i[3];
                const char *data = msg->s[3];
                R.width = w;
                R.height = h;
                memcpy( &R.buffer[R.frame][ (0 + y * w) * 3 ], data, size );
                //++socket_numrecv;
                //++socket_activity;
            //}
        }
    };
    static bool threaded = 0; if( !threaded ) { threaded = 1;
        std::thread( [&]() {
            for(;;) {
                network_update();
                Sleep(0); // (0);
            }
        } ).detach();
    }


#if WITH_MAINMENU
    imgui_menubar();
#endif

#if WITH_DOCKING
    imgui_dockspace();
#endif

#if WITH_SCENE3D
    #if 1
    int flags = ImGui::IsMouseDown(0) ? 0 : ImGuiWindowFlags_NoMove;
    ImGui::Begin("3d", NULL, flags);
    ImGuizmo::SetDrawlist();
    //    ImGuizmo::BeginFrame(0,0,300,300);
    gizmo_demo2();
    ImGui::End();
    #else
    gizmo_demo();
    #endif
#endif

#if WITH_TOOLS
    imgui_pangram();
    imgui_icons();
    sequencer_demo();
    profiler_demo();
    im3d_demo();
    browser_demo();

    static ImGui::Nodes nodes_;
    ImGui::Begin("Nodes");
    nodes_.ProcessNodes();
    ImGui::End();

    // alt, monospaced font
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        static int init = (texteditor_demo_init(),1); 
        texteditor_demo_draw();

        static MemoryEditor mem_edit_1;
        char *mem_block = "oh lala"; size_t mem_block_size = strlen(mem_block) + 1;
        mem_edit_1.DrawWindow("Memory Editor", mem_block, mem_block_size, 0x0000); // create a window and draw memory editor (if you already have a window, use DrawContents())
    ImGui::PopFont();

    ImGui::Begin("Log");
        console_demo();
    ImGui::End();
#endif

#if WITH_PANELS
    ImGui::Begin("demo2");

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

        curve_demo();
        table_demo();

    ImGui::End();

// floating content
    ImGui::Begin("remote view");
        // remote id viewer
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
        imgui_texture( remote_id, R.width, R.height, false );
    ImGui::End();

    ImGui::Begin("demo 1");
        extern bool show_demo_window; ImGui::Checkbox("Demo Window", &show_demo_window);
        static bool rec = 0; if( ImGui::Checkbox("Record", &rec) ) set_render('rec0', (double)!!rec);
        property_demo();

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

        richtext_demo();

    ImGui::End();
#endif

    ImGui::Begin("Texture viewer");
    {
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
