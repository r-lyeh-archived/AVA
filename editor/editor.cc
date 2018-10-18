#include "engine.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "imgui_internal.h"

#include "imgui/imgui_config.cpp"
#include "imgui/imgui_fonts.cpp"
#include "imgui/imgui_palette.cpp"
#include "imgui/imgui_theme.cpp"
#include "imgui/imgui_dsl.cpp"
// --
#include "imgui/imgui_icons.cpp"
#include "imgui/imgui_pangram.cpp"

// menubar
#define render(x) 0
#define set_render(opt_int, dbl_val)
#include "imgui/imgui_utils.cpp"
#include "imgui/imgui_menubar.cpp"

using namespace ImGui;

#include "imgui/gists/bitfield.cpp"
#include "imgui/gists/browse.cpp"
#include "imgui/gists/disabled.cpp"
#include "imgui/gists/combofilter.cpp"
#include "imgui/gists/hexedit.cpp"
#include "imgui/gists/ip_entry.cpp"
#include "imgui/gists/knob.cpp"
#include "imgui/gists/toggle.cpp"

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

void demo_windows() {
    static char path[256] = "./";
    if( ImGui::Begin("Browser") ) {
        if( imgui_browser( path ) ) {

        }
        ImGui::End();
    }

    static MemoryEditor mem_edit_1;                                            // store your state somewhere
    char *mem_block = "oh lala"; size_t mem_block_size = strlen(mem_block);
    mem_edit_1.DrawWindow("Memory Editor", mem_block, mem_block_size, 0x0000); // create a window and draw memory editor (if you already have a window, use DrawContents())
}

void demo_content() {
    //
    ImGui::Begin("Debug");

        static unsigned bits = 0xC1, hoverIndex = 0;
        BitField("label1", &bits, &hoverIndex);

        ImGui::PushDisabled();
        ImGui::Button("Disabled");
        ImGui::PopDisabled();

        static const char *hints[] = { "ABC", "ABCD", "DEF" };
        static ComboFilterState s = {0};
        static char buf[128] = "ABC";
        ComboFilter("my combofilter", buf, IM_ARRAYSIZE(buf), hints, IM_ARRAYSIZE(hints), s);

        static int ip[4] = {127,0,0,1};
        ipentry( ip );

        static float v = 0.5f;
        MyKnob("label2", &v, 0.0f, 1.0f);

        static bool yesno = 0;
        ToggleButton("label3", &yesno);

    ImGui::End();
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

    // tests
    imgui_pangram();
    imgui_icons();

    // window space
    demo_windows();

    // content space
    demo_content();

    // dockspace space
    // ...

    imgui_dockspace_end();
}

void editor_drop()
{}

int imgui_main(int, char**);

EXPORT
int main(int argc, char **argv) {
    return imgui_main(argc, argv);
}
