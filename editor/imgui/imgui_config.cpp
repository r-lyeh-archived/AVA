void imgui_config() {
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigInputTextCursorBlink |= 1;
    io.ConfigResizeWindowsFromEdges |= 1;
    io.MouseDrawCursor |= 1;
}
