/* Goxel 3D voxels editor
 *
 * copyright (c) 2015 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Goxel is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * You should have received a copy of the GNU General Public License along with
 * goxel.  If not, see <http://www.gnu.org/licenses/>.
 */

static void stencil_callback(
        const ImDrawList* parent_list, const ImDrawCmd* cmd)
{
    int op = (int)(intptr_t)cmd->UserCallbackData;

    switch (op) {
    case 0: // Reset
        GL(glDisable(GL_STENCIL_TEST));
        GL(glStencilMask(0x00));
        break;
    case 1: // Write
        GL(glEnable(GL_STENCIL_TEST));
        GL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
        GL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
        GL(glStencilMask(0xFF));
        break;
    case 2: // Filter
        GL(glEnable(GL_STENCIL_TEST));
        GL(glStencilFunc(GL_EQUAL, 1, 0xFF));
        GL(glStencilMask(0x00));
        break;
    default:
        assert(false);
        break;
    }
}

void GoxBox2(ImVec2 pos, ImVec2 size, ImVec4 color, bool fill,
             float thickness = 1,
             int rounding_corners_flags = ~0)
{
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    ImGuiWindow* window = GetCurrentWindow();
    float r = style.FrameRounding;
    size.x = size.x ? size.x : ImGui::GetContentRegionAvailWidth();
    if (fill) {
        window->DrawList->AddRectFilled(
                pos, pos + size,
                ImGui::ColorConvertFloat4ToU32(color), r,
                rounding_corners_flags);
    } else {
        window->DrawList->AddRect(
                pos, pos + size,
                ImGui::ColorConvertFloat4ToU32(color), r,
                rounding_corners_flags, thickness);
    }
}

void group_begin(const char *label)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    // if (label) ImGui::Text("%s", label); // @r-lyeh
    ImGui::PushID(label ? label : "group");
    //gui->group++;
    draw_list->ChannelsSplit(2);
    draw_list->ChannelsSetCurrent(1);
    ImGui::BeginGroup();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1)); // @r-lyeh: ImVec2(1,1) before
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
}

void group_end(void)
{
    //gui->group--;
    ImGui::PopID();
    ImGui::PopStyleVar(2);
    ImGui::Dummy(ImVec2(0, 0));
    ImGui::EndGroup();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->ChannelsSetCurrent(0);
    ImVec2 pos = ImGui::GetItemRectMin();
    ImVec2 size = ImGui::GetItemRectMax() - pos;

    ImVec4 color = ImGui::GetStyle().Colors[ImGuiCol_Border]; // = COLOR(WIDGET, OUTLINE, 0)

    // Stencil write.
    draw_list->AddCallback(stencil_callback, (void*)(intptr_t)1);
    GoxBox2(pos + ImVec2(1, 1), size - ImVec2(2, 2),
            color, true);
    // Stencil filter.
    draw_list->AddCallback(stencil_callback, (void*)(intptr_t)2);

    draw_list->ChannelsMerge();
    // Stencil reset.
    draw_list->AddCallback(stencil_callback, (void*)(intptr_t)0);
    GoxBox2(pos, size, color, false);
}


void group_demo() {
    group_begin( "test group" );

    ImGui::Button("hi1");
    ImGui::SameLine();
    ImGui::Button("hi2");
    ImGui::Button("hi3");
    ImGui::SameLine();
    ImGui::Button("hi4");

    group_end();
}