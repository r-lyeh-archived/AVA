#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <algorithm>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

/*
#include "engine.h"
*/
#define SHAPE_C
#include "render/render_shape.c"
#define EXPORT __declspec(dllexport)
#include <assert.h>
#define ASSERT assert
#include <stdint.h>
#define GL(...) __VA_ARGS__


#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "imgui_internal.h"

// incl render/set_render
#include "imgui/imgui_videorec.cpp"

// legacy api stub
#define IsHovered(rect, id) \
    !!(ImGui::IsMouseHoveringRect(rect.Min, rect.Max) && ImGui::ItemHoverable(rect, id))

using namespace ImGui;

#include "imgui/imgui_config.cpp"
#include "imgui/imgui_fonts.cpp"
#include "imgui/imgui_palette.cpp"
#include "imgui/imgui_theme.cpp"
#include "imgui/imgui_dsl.cpp"
// --
#include "imgui/imgui_icons.cpp"
#include "imgui/imgui_pangram.cpp"
#include "imgui/imgui_utils.cpp"    // for menubar below
#include "imgui/imgui_menubar.cpp"

#include "imgui/imgui_widgets.cpp" // must be first. widgets, all of them.
#include "imgui/imgui_property.cpp" // must be last

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

void editor_init() {
    imgui_config();
    imgui_fonts();

    imgui_theme_cherry();
    imgui_style_framework();
}

void editor_tick()
{}

void editor_draw() {
    // menu bar
    imgui_menubar();

    // dockspace
    imgui_dockspace();

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

    // windowed content
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

            static MemoryEditor mem_edit_1;                                            // store your state somewhere
            char *mem_block = "oh lala"; size_t mem_block_size = strlen(mem_block) + 1;
            mem_edit_1.DrawWindow("Memory Editor", mem_block, mem_block_size, 0x0000); // create a window and draw memory editor (if you already have a window, use DrawContents())
        ImGui::PopFont();

    // floating content
        ImGui::Begin("demo 1");
            extern bool show_demo_window; ImGui::Checkbox("Demo Window", &show_demo_window);
            static bool rec = 0; if( ImGui::Checkbox("Record", &rec) ) set_render('rec0', (double)!!rec);
            property_demo();


            static GLuint texture_id = 0;
            if( !texture_id ) {
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
            }

            if( texture_id ) {
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
                auto &io = ImGui::GetIO();
                ImTextureID my_tex_id = (void*)(intptr_t)texture_id;
                float my_tex_w = 2, my_tex_h = 2;
    #if 0
                float aspect_ratio = my_tex_h / my_tex_w;
                my_tex_w = 256;
                my_tex_h = my_tex_w * aspect_ratio;
    #endif
                ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    float region_sz = 32.0f;
                    float region_x = io.MousePos.x - pos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > my_tex_w - region_sz) region_x = my_tex_w - region_sz;
                    float region_y = io.MousePos.y - pos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > my_tex_h - region_sz) region_y = my_tex_h - region_sz;
                    float zoom = 4.0f;
                    ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
                    ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
                    ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
                    ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
                    ImGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
                    ImGui::EndTooltip();
                }
            }


            richtext_demo();

        ImGui::End();


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

                float tex_w = tex_px, tex_h = tex_px;
                ImTextureID tex_id = (ImTextureID)(int64_t)(ID % (texture_count));
                ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
                ImGui::Image(tex_id, ImVec2(tex_w, tex_h), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    float focus_sz = 32.0f;
                    float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > tex_w - focus_sz) focus_x = tex_w - focus_sz;
                    float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > tex_h - focus_sz) focus_y = tex_h - focus_sz;
                    ImGui::Text("Texture #%d", ID);
                    ImGui::Text("%.0fx%.0f", tex_w, tex_h);
                    ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
                    ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
                    ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
                    ImVec2 uv1 = ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
                    ImGui::Image(tex_id, zoom, uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
                    ImGui::EndTooltip();
                }

                if ((ID % images_per_line) < (images_per_line-1)) ImGui::SameLine();
                ImGui::PopID();
            }
        }
        ImGui::End();

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

        ImGui::Begin("Log");

            console_demo();

        ImGui::End();

    // dockspace space
    // ...

    imgui_dockspace_end();

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
