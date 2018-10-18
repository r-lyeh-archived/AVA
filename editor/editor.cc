#include "engine.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "imgui/imgui_config.cpp"
#include "imgui/imgui_fonts.cpp"
#include "imgui/imgui_palette.cpp"
#include "imgui/imgui_theme.cpp"
// --
#include "imgui/imgui_icons.cpp"
#include "imgui/imgui_pangram.cpp"

using namespace ImGui;

void editor_init() {
    imgui_config();
    imgui_fonts();

    theme_cherry();
    style_framework();
}

void editor_tick()
{}

void editor_draw() {
    // tests
    imgui_pangram();
    imgui_icons();
}

void editor_drop()
{}



int imgui_main(int, char**);

EXPORT
int main(int argc, char **argv) {
	return imgui_main(argc, argv);
}
