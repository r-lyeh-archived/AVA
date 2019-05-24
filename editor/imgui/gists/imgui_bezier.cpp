// ImGui Bezier widget. @r-lyeh, public domain
// v1.03: improve grabbing, confine grabbers to area option, adaptive size, presets, preview.
// v1.02: add BezierValue(); comments; usage
// v1.01: out-of-bounds coord snapping; custom border width; spacing; cosmetics
// v1.00: initial version
//
// [ref] http://robnapier.net/faster-bezier
// [ref] http://easings.net/es#easeInSine
//
// Usage:
// {  static float v[5] = { 0.390f, 0.575f, 0.565f, 1.000f }; 
//    ImGui::Bezier( "easeOutSine", v );       // draw
//    float y = ImGui::BezierValue( 0.5f, v ); // x delta in [0..1] range
// }

//#define IMGUI_DEFINE_MATH_OPERATORS
//#include <imgui.h>
//#include <imgui_internal.h>
#include <time.h>

namespace ImGui
{
    template<int steps>
    void bezier_table(ImVec2 P[4], ImVec2 results[steps + 1]) {
        static float C[(steps + 1) * 4], *K = 0;
        if (!K) {
            K = C;
            for (unsigned step = 0; step <= steps; ++step) {
                float t = (float) step / (float) steps;
                C[step * 4 + 0] = (1 - t)*(1 - t)*(1 - t);   // * P0
                C[step * 4 + 1] = 3 * (1 - t)*(1 - t) * t; // * P1
                C[step * 4 + 2] = 3 * (1 - t) * t*t;     // * P2
                C[step * 4 + 3] = t * t*t;               // * P3
            }
        }
        for (unsigned step = 0; step <= steps; ++step) {
            ImVec2 point = {
                K[step * 4 + 0] * P[0].x + K[step * 4 + 1] * P[1].x + K[step * 4 + 2] * P[2].x + K[step * 4 + 3] * P[3].x,
                K[step * 4 + 0] * P[0].y + K[step * 4 + 1] * P[1].y + K[step * 4 + 2] * P[2].y + K[step * 4 + 3] * P[3].y
            };
            results[step] = point;
        }
    }

    float BezierValue(float dt01, float P[4]) {
        enum { STEPS = 256 };
        ImVec2 Q[4] = { { 0, 0 }, { P[0], P[1] }, { P[2], P[3] }, { 1, 1 } };
        ImVec2 results[STEPS + 1];
        bezier_table<STEPS>(Q, results);
        return results[(int) ((dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01) * STEPS)].y;
    }

    int Bezier(const char *label, float P[5]) {
        // visuals
        enum { SMOOTHNESS = 64 }; // curve smoothness: the higher number of segments, the smoother curve
        enum { CURVE_WIDTH = 4 }; // main curved line width
        enum { LINE_WIDTH = 1 }; // handlers: small lines width
        enum { GRAB_RADIUS = 8 }; // handlers: circle radius
        enum { GRAB_BORDER = 2 }; // handlers: circle border width
        enum { AREA_CONSTRAINED = true }; // should grabbers be constrained to grid area?
        enum { AREA_WIDTH = 128 }; // area width in pixels. 0 for adaptive size (will use max avail width)

        // curve presets
        static struct { const char *name; float points[4]; } presets [] = {
            { "Linear", 0.000f, 0.000f, 1.000f, 1.000f },

            { "In Sine", 0.470f, 0.000f, 0.745f, 0.715f },
            { "In Quad", 0.550f, 0.085f, 0.680f, 0.530f },
            { "In Cubic", 0.550f, 0.055f, 0.675f, 0.190f },
            { "In Quart", 0.895f, 0.030f, 0.685f, 0.220f },
            { "In Quint", 0.755f, 0.050f, 0.855f, 0.060f },
            { "In Expo", 0.950f, 0.050f, 0.795f, 0.035f },
            { "In Circ", 0.600f, 0.040f, 0.980f, 0.335f },
            { "In Back", 0.600f, -0.28f, 0.735f, 0.045f },

            { "Out Sine", 0.390f, 0.575f, 0.565f, 1.000f },
            { "Out Quad", 0.250f, 0.460f, 0.450f, 0.940f },
            { "Out Cubic", 0.215f, 0.610f, 0.355f, 1.000f },
            { "Out Quart", 0.165f, 0.840f, 0.440f, 1.000f },
            { "Out Quint", 0.230f, 1.000f, 0.320f, 1.000f },
            { "Out Expo", 0.190f, 1.000f, 0.220f, 1.000f },
            { "Out Circ", 0.075f, 0.820f, 0.165f, 1.000f },
            { "Out Back", 0.175f, 0.885f, 0.320f, 1.275f },

            { "InOut Sine", 0.445f, 0.050f, 0.550f, 0.950f },
            { "InOut Quad", 0.455f, 0.030f, 0.515f, 0.955f },
            { "InOut Cubic", 0.645f, 0.045f, 0.355f, 1.000f },
            { "InOut Quart", 0.770f, 0.000f, 0.175f, 1.000f },
            { "InOut Quint", 0.860f, 0.000f, 0.070f, 1.000f },
            { "InOut Expo", 1.000f, 0.000f, 0.000f, 1.000f },
            { "InOut Circ", 0.785f, 0.135f, 0.150f, 0.860f },
            { "InOut Back", 0.680f, -0.55f, 0.265f, 1.550f },

            // easeInElastic: not a bezier
            // easeOutElastic: not a bezier
            // easeInOutElastic: not a bezier
            // easeInBounce: not a bezier
            // easeOutBounce: not a bezier
            // easeInOutBounce: not a bezier
        };


        // preset selector

        bool reload = 0;
        ImGui::PushID(label);
            if (ImGui::ArrowButton("##lt", ImGuiDir_Left)) { // ImGui::ArrowButton(ImGui::GetCurrentWindow()->GetID("##lt"), ImGuiDir_Left, ImVec2(0, 0), 0)
                if (--P[4] >= 0) reload = 1; else ++P[4];
            }
            ImGui::SameLine();

            if (ImGui::Button("Presets")) {
                ImGui::OpenPopup("!Presets");
            }
            if (ImGui::BeginPopup("!Presets")) {
                for (int i = 0; i < IM_ARRAYSIZE(presets); ++i) {
                    if( i == 1 || i == 9 || i == 17 ) ImGui::Separator();
                    if (ImGui::MenuItem(presets[i].name, NULL, P[4] == i)) {
                        P[4] = (float)i;
                        reload = 1;
                    }
                }
                ImGui::EndPopup();
            }
            ImGui::SameLine();

            if (ImGui::ArrowButton("##rt", ImGuiDir_Right)) { // ImGui::ArrowButton(ImGui::GetCurrentWindow()->GetID("##rt"), ImGuiDir_Right, ImVec2(0, 0), 0)
                if (++P[4] < IM_ARRAYSIZE(presets)) reload = 1; else --P[4];
            }
            ImGui::SameLine();

            if (reload) {
                memcpy(P, presets[(int) P[4]].points, sizeof(float) * 4);
            }
        ImGui::PopID();

        // bezier widget

        const ImGuiStyle& Style = GetStyle();
        const ImGuiIO& IO = GetIO();
        ImDrawList* DrawList = GetWindowDrawList();
        ImGuiWindow* Window = GetCurrentWindow();
        if (Window->SkipItems)
            return false;

        // header and spacing
        int changed = SliderFloat4(label, P, 0, 1, "%.3f", 1.0f);
        int hovered = IsItemActive() || IsItemHovered(); // IsItemDragged() ?
        Dummy(ImVec2(0, 3));

        // prepare canvas
        const float avail = GetContentRegionAvailWidth();
        const float dim = AREA_WIDTH > 0 ? AREA_WIDTH : avail;
        ImVec2 Canvas(dim, dim);

        ImRect bb(Window->DC.CursorPos, Window->DC.CursorPos + Canvas);
        ItemSize(bb);
        if (!ItemAdd(bb, NULL))
            return changed;

        const ImGuiID id = Window->GetID(label);
        hovered |= 0 != ItemHoverable(ImRect(bb.Min, bb.Min + ImVec2(avail, dim)), id);

        RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, Style.FrameRounding);

        // background grid
        for (int i = 0; i <= Canvas.x; i += (int)(Canvas.x / 4)) {
            DrawList->AddLine(
                ImVec2(bb.Min.x + i, bb.Min.y),
                ImVec2(bb.Min.x + i, bb.Max.y),
                GetColorU32(ImGuiCol_TextDisabled));
        }
        for (int i = 0; i <= Canvas.y; i += (int)(Canvas.y / 4)) {
            DrawList->AddLine(
                ImVec2(bb.Min.x, bb.Min.y + i),
                ImVec2(bb.Max.x, bb.Min.y + i),
                GetColorU32(ImGuiCol_TextDisabled));
        }

        // eval curve
        ImVec2 Q[4] = { { 0, 0 }, { P[0], P[1] }, { P[2], P[3] }, { 1, 1 } };
        ImVec2 results[SMOOTHNESS + 1];
        bezier_table<SMOOTHNESS>(Q, results);

        // control points: 2 lines and 2 circles
        {
            // handle grabbers
            ImVec2 mouse = GetIO().MousePos, pos[2];
            float distance[2];

            for (int i = 0; i < 2; ++i) {
                pos[i] = ImVec2(P[i * 2 + 0], 1 - P[i * 2 + 1]) * (bb.Max - bb.Min) + bb.Min;
                distance[i] = (pos[i].x - mouse.x) * (pos[i].x - mouse.x) + (pos[i].y - mouse.y) * (pos[i].y - mouse.y);
            }

            int selected = distance[0] < distance[1] ? 0 : 1;
            if( distance[selected] < (4*GRAB_RADIUS * 4*GRAB_RADIUS) )
            {
                SetTooltip("(%4.3f, %4.3f)", P[selected * 2 + 0], P[selected * 2 + 1]);

                if (/*hovered &&*/ (IsMouseClicked(0) || IsMouseDragging(0))) {
                    float &px = (P[selected * 2 + 0] += GetIO().MouseDelta.x / Canvas.x);
                    float &py = (P[selected * 2 + 1] -= GetIO().MouseDelta.y / Canvas.y);

                    if (AREA_CONSTRAINED) {
                        px = (px < 0 ? 0 : (px > 1 ? 1 : px));
                        py = (py < 0 ? 0 : (py > 1 ? 1 : py));
                    }

                    changed = true;
                }
            }
        }

        // if (hovered || changed) DrawList->PushClipRectFullScreen();

        // draw curve
        {
            ImColor color(GetStyle().Colors[ImGuiCol_PlotLines]);
            for (int i = 0; i < SMOOTHNESS; ++i) {
                ImVec2 p = { results[i + 0].x, 1 - results[i + 0].y };
                ImVec2 q = { results[i + 1].x, 1 - results[i + 1].y };
                ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
                ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
                DrawList->AddLine(r, s, color, CURVE_WIDTH);
            }
        }

        // draw preview (cycles every 1s)
        static clock_t epoch = clock();
        ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
        for (int i = 0; i < 3; ++i) {
            double now = ((clock() - epoch) / (double)CLOCKS_PER_SEC);
            float delta = ((int) (now * 1000) % 1000) / 1000.f; delta += i / 3.f; if (delta > 1) delta -= 1;
            int idx = (int) (delta * SMOOTHNESS);
            float evalx = results[idx].x; // 
            float evaly = results[idx].y; // ImGui::BezierValue( delta, P );
            ImVec2 p0 = ImVec2(evalx, 1 - 0) * (bb.Max - bb.Min) + bb.Min;
            ImVec2 p1 = ImVec2(0, 1 - evaly) * (bb.Max - bb.Min) + bb.Min;
            ImVec2 p2 = ImVec2(evalx, 1 - evaly) * (bb.Max - bb.Min) + bb.Min;
            DrawList->AddCircleFilled(p0, GRAB_RADIUS / 2, ImColor(white));
            DrawList->AddCircleFilled(p1, GRAB_RADIUS / 2, ImColor(white));
            DrawList->AddCircleFilled(p2, GRAB_RADIUS / 2, ImColor(white));
        }

        // draw lines and grabbers
        float luma = IsItemActive() || IsItemHovered() ? 0.5f : 1.0f;
        ImVec4 pink(1.00f, 0.00f, 0.75f, luma), cyan(0.00f, 0.75f, 1.00f, luma);
        ImVec2 p1 = ImVec2(P[0], 1 - P[1]) * (bb.Max - bb.Min) + bb.Min;
        ImVec2 p2 = ImVec2(P[2], 1 - P[3]) * (bb.Max - bb.Min) + bb.Min;
        DrawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), LINE_WIDTH);
        DrawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), LINE_WIDTH);
        DrawList->AddCircleFilled(p1, GRAB_RADIUS, ImColor(white));
        DrawList->AddCircleFilled(p1, GRAB_RADIUS - GRAB_BORDER, ImColor(pink));
        DrawList->AddCircleFilled(p2, GRAB_RADIUS, ImColor(white));
        DrawList->AddCircleFilled(p2, GRAB_RADIUS - GRAB_BORDER, ImColor(cyan));

        // if (hovered || changed) DrawList->PopClipRect();

        return changed;
    }

    void ShowBezierDemo() {
        { static float v[5] = { 0.950f, 0.050f, 0.795f, 0.035f }; Bezier("easeInExpo", v); }
    }
}
