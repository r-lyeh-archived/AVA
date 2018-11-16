/*  // [src] https://github.com/ocornut/imgui/issues/513

    // Usage:
    static const char *headers[] = {
        "Index", "Color", "Flip?", "Filename"
    };
    static float widths[ IM_ARRAYSIZE(headers) ] = {}; 
    if( ImGui::BeginTable("WinTextureContent", headers, widths, IM_ARRAYSIZE(headers)) ) {
        // Draw as many rows as needed
        for( int i = 0; i < 10; ++i ) {
            ImGui::Text("%d", i);                                ImGui::NextColumn();
            ImGui::ColorButton("",ImVec4(0.5f,0.2f,i*0.3f,1.f)); ImGui::NextColumn();
            ImGui::Text("%s", i % 2 ? "yes" : "no");             ImGui::NextColumn();
            ImGui::Text(__FILE__);                               ImGui::NextColumn();
        }
        ImGui::EndTable();
    }
*/

// .h
namespace ImGui {
IMGUI_API int  BeginTable(const char* id, const char** headers, float *widths, int count, bool border=true);
IMGUI_API void EndTable();
}

// .cpp
namespace ImGui {
static inline IMGUI_API
int BeginTable(const char* id, const char** headers, float* widths, int count, bool draw_border)
{
    if(count<=0)
        return 0;

    // Draw column headers
    ImGuiStyle & style = ImGui::GetStyle();

    ImGui::PushID(id);
    ImGui::Columns(count, id, draw_border);

    float offset = 0.0f;
    for(int i=0; i < count; i++)
    {
        ImGui::SetColumnOffset(i, offset);

        if(widths[i] <= 0)
        {
            const ImVec2 textsize = ImGui::CalcTextSize(headers[i], NULL, true);
            const float colSizeX = (textsize.x + 2 * style.ItemSpacing.x);
            widths[i] = colSizeX + 1;
        }

        if(i < (count-1))
        {
            float curOffset = offset;
            offset = ImGui::GetColumnOffset(i+1);
            widths[i] = offset - curOffset + 1;
        }

        ImGui::Text(headers[i]);
        ImGui::NextColumn();
    }

    ImGui::Columns(1);

    // Draw body
    ImGui::PushID("table_next");
    ImGui::Columns(count, id, draw_border);

    offset = 0.0f;
    for(int i=0; i < count; i++)
    {
        ImGui::SetColumnOffset(i, offset);
        offset += widths[i] - 1;
    }

    return 1;
}

static inline IMGUI_API
void EndTable()
{
    ImGui::Columns(1);
    ImGui::PopID();
    ImGui::PopID();
}

}

void table_demo() {
    static const char *headers[] = {
        "Index", "Color", "Flip?", "Filename"
    };
    static float widths[ IM_ARRAYSIZE(headers) ] = {}; 
    if( ImGui::BeginTable("WinTextureContent", headers, widths, IM_ARRAYSIZE(headers)) ) {
        // Draw as many rows as needed
        for( int i = 0; i < 10; ++i ) {
            ImGui::Text("%d", i);                                ImGui::NextColumn();
            ImGui::ColorButton("",ImVec4(0.5f,0.2f,i*0.3f,1.f)); ImGui::NextColumn();
            ImGui::Text("%s", i % 2 ? "yes" : "no");             ImGui::NextColumn();
            ImGui::Text(__FILE__);                               ImGui::NextColumn();
        }
        ImGui::EndTable();
    }
}
