// https://github.com/ocornut/imgui/issues/388

void ipentry( int octets[4] ) {
    float width = ImGui::CalcItemWidth();
    ImGui::BeginGroup();
    ImGui::PushID("IP");
    ImGui::TextUnformatted("IP");
    ImGui::SameLine();
    for (int i = 0; i < 4; i++) {
        ImGui::PushItemWidth(width / 4.0f);
        ImGui::PushID(i);
        bool invalid_octet = false;
        if (octets[i] > 255) {
            // Make values over 255 red, and when focus is lost reset it to 255.
            octets[i] = 255;
            invalid_octet = true;
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        }
        if (octets[i] < 0) {
            // Make values below 0 yellow, and when focus is lost reset it to 0.
            octets[i] = 0;
            invalid_octet = true;
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        }
        ImGui::InputInt("##v", &octets[i], 0, 0, ImGuiInputTextFlags_CharsDecimal);
        if (invalid_octet) {
            ImGui::PopStyleColor();
        }
        ImGui::SameLine();
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();
    ImGui::EndGroup();
}
