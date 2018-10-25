
#include "imgui/gists/bitfield.cpp"
#include "imgui/gists/hexedit.cpp"
#include "imgui/gists/ip_entry.cpp"
#include "imgui/gists/knob.cpp" // fresh ideas here: https://www.g200kg.com/en/webknobman/gallery.php
#include "imgui/gists/toggle.cpp"

#include "imgui/gists/imgui_browser.cpp"
#include "imgui/gists/imgui_disabled.cpp"
#include "imgui/gists/imgui_combofilter.cpp"
#include "imgui/gists/imgui_bezier.cpp"
#include "imgui/gists/imgui_curve.cpp"

#include "imgui/gists/spinner.cpp"
#include "imgui/gists/table.cpp"
#include "imgui/gists/multiplot.cpp"
#include "imgui/gists/plotvar.cpp"

#include "imgui/gists/gizmo.cpp"

#include "imgui/gists/sequencer.h"
#include "imgui/gists/sequencer.cpp"

#ifdef GLSL
#undef GLSL
#endif
#include "imgui/gists/textedit.h"
#include "imgui/gists/textedit.cpp"

#ifdef UNREFERENCED_PARAMETER
#undef UNREFERENCED_PARAMETER
#endif
#include "imgui/gists/nodes.h" //v1
#include "imgui/gists/nodes.cpp" //v1

#define YAP_ENABLE
#define YAP_IMPL
#define YAP_IMGUI
#include "imgui/gists/profiler.cpp"

#include "imgui/gists/orient.cpp"

#include "imgui/gists/imgui_im3d.cpp"

#include "imgui/gists/richtext.cpp"

#include "imgui/gists/console.cpp"
