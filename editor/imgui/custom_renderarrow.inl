#if 1

// our custom gui overrides
#define IMGUI_CUSTOM_MENU_ICON(pos,dir) \
    static const char *chevron_r = "\xEE\x97\x8C"; \
    static const char *chevron_d = "\xEE\x97\x8F";/*ICON_MD_EXPAND_MORE?*/ \
    static const char *chevron_l = "\xEE\x97\x8B"; \
    static const char *chevron_u = "\xEE\x97\x8E"; /*ICON_MD_EXPAND_LESS?*/ \
    const char *begin = dir == 0 ? chevron_l : dir == 1 ? chevron_r : dir == 2 ? "ch2" : chevron_d; \
    ImGui::RenderText(pos, begin, begin+3, false);

    IMGUI_CUSTOM_MENU_ICON(center - ImVec2(g.FontSize * 0.50f, g.FontSize * 0.50f), dir);
    return;
#endif