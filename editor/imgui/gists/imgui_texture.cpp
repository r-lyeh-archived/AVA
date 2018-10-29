void imgui_texture( intptr_t texture_id, float w, float h, bool zoom = true, bool caption = false ) {
    auto &io = ImGui::GetIO();

    if( caption ) {
        ImGui::Text("%.0fx%.0f", 2, 2);
        ImGui::SameLine();
    }

    ImTextureID id = (void*)texture_id;
#if 0
    float aspect_ratio = h / (float)w;
    my_tex_w = 256;
    my_tex_h = my_tex_w * aspect_ratio;
#endif
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::Image(id, ImVec2(w, h), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));
    if (zoom && ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        float region_sz = 32.0f;
        float region_x = io.MousePos.x - pos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > w - region_sz) region_x = w - region_sz;
        float region_y = io.MousePos.y - pos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > h - region_sz) region_y = h - region_sz;
        float zoom = 4.0f;
        ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
        ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
        ImVec2 uv0 = ImVec2((region_x) / w, (region_y) / h);
        ImVec2 uv1 = ImVec2((region_x + region_sz) / w, (region_y + region_sz) / h);
        ImGui::Image(id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::EndTooltip();
    }
}

void texture_demo() {
    imgui_texture( 1, 64, 64 );
}
