#include "engine.h"

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

    // tests
    imgui_pangram();
    imgui_icons();
}

void editor_drop()
{}


// data breakpoints
void (*editor_onFileSaved)(const char *name);
void (*editor_onFileLoaded)(const char *name);
void (*editor_onFileChanged)(const char *name);
void (*editor_onPropertySaved)(const char *name);
void (*editor_onPropertyLoaded)(const char *name);
void (*editor_onPropertyChanged)(const char *name);



int imgui_main(int, char**);

EXPORT
int main(int argc, char **argv) {
    return imgui_main(argc, argv);
}
