namespace ImGui {
    void PushDisabled( bool disabled = true, float alpha_multiplier = 0.75f ) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disabled);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (disabled ? alpha_multiplier : 1.f) );
    }
    void PopDisabled( int num = 1 ) {
        while( num-- ) {
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
        }
    }
}
