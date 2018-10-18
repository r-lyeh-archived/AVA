#if 1

#define IMGUI_CUSTOM_CLOSE_BUTTON \
    static const char *buf_display[] = { "\xEE\x97\x8D", "\xEE\xA2\x88" }; /* ICON_MD_CLOSE and ICON_MD_HIGHTLIGHT_OFF*/ \
    window->DrawList->AddText( g.Font, g.FontSize, center - ImVec2(ImMax(2.0f, radius),ImMax(2.0f, radius)), GetColorU32(ImGuiCol_Text), buf_display[hovered], buf_display[hovered] + 3, 0.0f, NULL);

    IMGUI_CUSTOM_CLOSE_BUTTON
    return pressed;

#endif
