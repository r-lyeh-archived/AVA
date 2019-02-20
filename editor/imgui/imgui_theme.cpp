#pragma once

void imgui_theme_cinder();
void imgui_theme_cherry();
void imgui_theme_framework();

void imgui_style_framework();


void imgui_theme_framework()
{
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4 black(0,0,0,0);
    ImVec4 white(1,1,1,1);
    ImVec4 color1(0.86f, 0.93f, 0.89f, 1); // text ->
    ImVec4 color2(0.20f, 0.22f, 0.27f, 1); // blur
    ImVec4 color3(0.92f, 0.18f, 0.29f, 1); // active
    ImVec4 color4(0.47f, 0.77f, 0.83f, 1); // slider ->
    ImVec4 color5(0.13f, 0.14f, 0.17f, 1); // windowbg
    ImVec4 color6(0.31f, 0.31f, 1.00f, 1); // border, unused -> black
    ImVec4 color7(0.09f, 0.15f, 0.16f, 1); // ScrollbarGrab
    ImVec4 color8(0.71f, 0.22f, 0.27f, 1); // CheckMark
    ImVec4 color9(0.14f, 0.16f, 0.19f, 1); // Column

    auto mul = []( ImVec4 c, float t ) -> ImVec4 { return ImVec4(c.x * t,c.y * t,c.z * t,c.w); };
    auto lum = []( ImVec4 c, float x ) -> ImVec4 { return ImVec4(c.x,c.y,c.z,c.w * x); };

#if 0
    ImVec4 dutch_teal(22/255.f,147/255.f,165/255.f,1); // skyblue2u http://www.colourlovers.com/palette/437077/gemtone_sea_shore
    ImVec4 evening_at_sea(2/255.f,170/255.f,176/255.f,1); // skyblue2u
    ImVec4 maybegreen(0/255.f,205/255.f,172/255.f,1); // skyblue2u
    ImVec4 a_lert(127/255.f,255/255.f,36/255.f,1); // skyblue2u
    ImVec4 certain_frogs(195/255.f,255/255.f,104/255.f,1); // skyblue2u

    color9 = dutch_teal;
    color8 = evening_at_sea;
    color7 = maybegreen;
    color2 = a_lert;
    color3 = certain_frogs;
#endif

    /* auto ImVec4 = [&]( float r, float g, float b, float a ) {
        float h,s,v;
        ImGui::ColorConvertRGBtoHSV(r, g, b, h, s, v);
        //h += 0.8125f; h = h > 1 ? h - 1 : h; h = h < 0 ? h + 1 : h;
        //v -= 0.0500f; v = v > 1 ? v - 1 : v; v = v < 0 ? v + 1 : v;
        ImGui::ColorConvertHSVtoRGB( h, s, v, r, g, b);
        return ::ImVec4( r, g, b, a );
    }; */

    style.Colors[ImGuiCol_Text]                  = ImVec4(255/255.f,255/255.f,240/255.f,169/255.f); //lum(color1, 0.63f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(181/255.f,194/255.f,182/255.f,114/255.f); //lum(color1, 0.63f);
    style.Colors[ImGuiCol_WindowBg]              = lum(color5, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg]         = lum(color2, 0.58f);
    style.Colors[ImGuiCol_Border]                = ImVec4(181/255.f,194/255.f,182/255.f, 42/255.f); //lum(color6, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = black;
    style.Colors[ImGuiCol_FrameBg]               = lum(color2, 170/255.f);
    style.Colors[ImGuiCol_FrameBgHovered]        = lum(color3, 64/255.f);  //0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = lum(color3, 128/255.f); //1.00f);
    style.Colors[ImGuiCol_TitleBg]               = lum(color2, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = lum(color2, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]         = lum(color3, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = lum(color2, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = lum(color2, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = lum(color7, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = lum(color3, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = lum(color3, 1.00f);
//    style.Colors[ImGuiCol_ComboBg]               = lum(color2, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = lum(color8, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = lum(color4, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = lum(color3, 1.00f);
    style.Colors[ImGuiCol_Button]                = lum(color4, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(101/255.f,128/255.f,144/255.f,219/255.f); //lum(color3, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = lum(color3, 1.00f);
    style.Colors[ImGuiCol_Header]                = lum(color3, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = lum(color3, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = lum(color3, 1.00f);
    style.Colors[ImGuiCol_Column]                = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_ColumnHovered]         = lum(color3, 0.78f);
    style.Colors[ImGuiCol_ColumnActive]          = lum(color3, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = lum(color4, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = lum(color3, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = lum(color3, 1.00f);
//    style.Colors[ImGuiCol_CloseButton]           = lum(color1, 0.63f);
//    style.Colors[ImGuiCol_CloseButtonHovered]    = lum(color1, 0.63f);
//    style.Colors[ImGuiCol_CloseButtonActive]     = lum(color1, 0.63f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(96/255.f, 192/255.f, 255/255.f, 86/255.f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(96/255.f, 192/255.f, 255/255.f, 128/255.f);
    style.Colors[ImGuiCol_PlotHistogram]         = lum(color1, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = lum(color3, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = lum(color3, 0.43f);
    style.Colors[ImGuiCol_PopupBg] = style.Colors[ImGuiCol_WindowBg];
    style.Colors[ImGuiCol_ModalWindowDarkening]  = lum(color2, 0.73f);

    style.Colors[ImGuiCol_CheckMark]  = white;

    // use complementary colors: (blue:18,113,155) or (yellow:214,208,68)
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(18/255.f,113/255.f,155/255.f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(18/255.f,113/255.f,155/255.f, 0.76f);
}

void imgui_theme_cinder() { // Geilfus'
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text]                  = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.20f, 0.22f, 0.27f, 0.58f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
//    style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Header]                = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ColumnActive]          = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
//    style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.86f, 0.93f, 0.89f, 0.16f);
//    style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.86f, 0.93f, 0.89f, 0.39f);
//    style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
    style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
}

void imgui_theme_cherry() {
    auto &style = ImGui::GetStyle();

//      cherry colors
#       define HI(v)      ImVec4(0.502f, 0.075f, 0.256f, v)
#       define MED(v)     ImVec4(0.455f, 0.198f, 0.301f, v)
#       define LOW(v)     ImVec4(0.232f, 0.201f, 0.271f, v)
//      backgrounds (@todo: complete with BG_MED, BG_LOW)
#       define BG(v)      ImVec4(0.20f, 0.22f, 0.27f, v)
//      text colors (@todo: complete with TEXT_LOW)
#       define TEXT_HI(v) ImVec4(0.86f, 0.93f, 0.89f, v)

    style.Colors[ImGuiCol_Text]                  = TEXT_HI(0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = TEXT_HI(0.28f);
    style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildWindowBg]         = BG( 0.58f);
    style.Colors[ImGuiCol_PopupBg]               = BG( 0.9f);
    style.Colors[ImGuiCol_Border]                = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = BG( 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = MED( 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = MED( 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = LOW( 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = HI( 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = BG( 0.75f);
    style.Colors[ImGuiCol_MenuBarBg]             = BG( 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = BG( 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = MED( 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = MED( 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button]                = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_Header]                = MED( 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = HI( 1.00f);
    style.Colors[ImGuiCol_Column]                = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_ColumnHovered]         = MED( 0.78f);
    style.Colors[ImGuiCol_ColumnActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = MED( 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = TEXT_HI(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = TEXT_HI(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = MED( 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = MED( 0.43f);
    // [...]
    style.Colors[ImGuiCol_ModalWindowDarkening]  = BG( 0.73f);

    return;
    style.Colors[ImGuiCol_Tab]  = ImVec4(0.455,0.198,0.301,0.862);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.455,0.198,0.301,0.500);

    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.204,0.478,0.741,1.000);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.204,0.478,0.741,1.000);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.204,0.478,0.741,0.500);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.204,0.478,0.741,1.000);
}

void imgui_style_framework() {
    ImGuiStyle& style = ImGui::GetStyle();

#if 0 // fwk
    style.WindowPadding            = ImVec2(6, 4);
    style.WindowRounding           = 6.0f;
//    style.ChildWindowRounding      = 0.0f;
    style.FramePadding             = ImVec2(7, 2);
    style.FrameRounding            = 4.0f;
    style.ItemInnerSpacing         = ImVec2(1, 2);
    style.TouchExtraPadding        = ImVec2(0, 0);
    style.ScrollbarSize            = 12.0f;
    style.ScrollbarRounding        = 16.0f;
    style.GrabMinSize              = 14.0f;
    style.GrabRounding             = 4.0f;

    style.WindowTitleAlign.x = 0.50f;
#endif

#if 1 // cherry
    style.WindowPadding            = ImVec2(6, 4);
    style.WindowRounding           = 0.0f;
    style.FramePadding             = ImVec2(5/*7*/, 2);
    style.FrameRounding            = 3.0f;
    style.ItemSpacing              = ImVec2(7, 1);
    style.ItemInnerSpacing         = ImVec2(1, 1);
    style.TouchExtraPadding        = ImVec2(0, 0);
    style.IndentSpacing            = 6.0f;
    style.ScrollbarSize            = 12.0f;
    style.ScrollbarRounding        = 16.0f;
    style.GrabMinSize              = 20.0f;
    style.GrabRounding             = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
#endif

    style.ItemSpacing              = ImVec2(6, 2);
    style.IndentSpacing            = 18.0f;
}
