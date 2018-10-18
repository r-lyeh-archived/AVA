namespace ImGui {
    void PushDisabled( bool disabled = true ) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.75f);
    }
    void PopDisabled( int num = 1 ) {
        while( num-- ) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
    }
}
