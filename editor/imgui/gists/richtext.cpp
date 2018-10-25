// [src] https://github.com/ocornut/imgui/issues/902

enum CUiTextFlags
{
    UiTextFlags_Left = 1 << 0,
    UiTextFlags_Center = 1 << 1,
    UiTextFlags_Right = 1 << 2,
    UiTextFlags_VTop = 1 << 3,
    UiTextFlags_VCenter = 1 << 4,
    UiTextFlags_VBottom = 1 << 5,
    UiTextFlags_Default_ = UiTextFlags_Left | UiTextFlags_VTop,
    UiTextFlags_Outline = 1 << 6,
    //UiTextFlags_Rich = 1 << 7,
    //UiTextFlags_Monospace = 1 << 8, // FIXME-TODO
};

struct CRichTextStyle
{
    const ImFont*   Font;
    float           FontSize;
    float           Spacing;        // % of FontSize
    ImU32           Color;
    ImU32           ColorOutline;

    CRichTextStyle() { Font = NULL; FontSize = 0.0f; Spacing = 1.0f; Color = IM_COL32_WHITE; }
    void Set(const ImFont* font, float font_size, ImU32 col, ImU32 col_outline) { Font = font; FontSize = font_size; Color = col; ColorOutline = col_outline; }
};

enum CRichTextStyleType
{
    RichTextStyle_Normal,
    RichTextStyle_Bold,
    RichTextStyle_H1,
    RichTextStyle_H2,
    RichTextStyle_H3,
    RichTextStyle_H4,
    RichTextStyle_Count_,
};

struct CRichTextDrawConfig
{
    CRichTextStyle      Styles[RichTextStyle_Count_];
    float               MaxWidth;           // Stop drawing
    float               WrapWidth;
    const ImVec4*       ClipRect;
    bool                ClipCpuFine;
    char                ImagePathPrefix[128];

    CRichTextDrawConfig()
    {
        MaxWidth = FLT_MAX;
        WrapWidth = 0.0f;
        ClipRect = NULL;
        ClipCpuFine = false;
    }
};

typedef ImU32 CRichTag;

struct CRichTagBackup
{
    CRichTag                Tag;
    CRichTextStyleType      Backup;
    const char*             ContentBegin;

    CRichTagBackup() { Tag = 0; Backup = RichTextStyle_Count_; ContentBegin = NULL; }
};

struct CRichTextState
{
    std::vector<CRichTagBackup>     TagStack;
    bool                            InhibitOne;
    const CRichTextDrawConfig*      Config;
    ImDrawList*                     DrawList;
    bool                            HideChars;
    bool                            Error;
    ImVec2                          StartPos;
    ImVec2                          CurrentPos;
    CRichTextStyleType              TextConfigMode;

    const ImFont*                   Font;
    float                           FontSize;
    float                           FontScale;
    float                           LineHeight;
    ImU32                           Color, ColorOutline;

    CRichTextState(const CRichTextDrawConfig* config)
    {
        TagStack.reserve(8);
        InhibitOne = false;
        Config = config;
        HideChars = false;
        DrawList = NULL;
        Error = false;
        SetTextStyle(RichTextStyle_Normal);
    }

    void SetError()
    {
        Error = true;
    }

    void SetTextStyle(CRichTextStyleType v)
    {
        const CRichTextStyle& style = Config->Styles[v];
        TextConfigMode = v;

        Font = style.Font;
        FontSize = style.FontSize;
        FontScale = FontSize / Font->FontSize;
        LineHeight = FontSize * style.Spacing;
        Color = style.Color;
        ColorOutline = style.ColorOutline;
    }
};

// Functions
ImVec2      RichTextCalcSize(const ImFont* font, float font_size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining = NULL);
ImVec2      RichTextCalcSizeEx(const CRichTextDrawConfig& config, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL);
void        RichTextDraw(ImDrawList* draw_list, const CRichTextDrawConfig& config, CRichTextState& state, CUiTextFlags flags, ImVec2 pos, const char* text_begin, const char* text_end = NULL, const char* text_display_end = NULL);


// cpp

//#include "texture.h"
int GetTexture(const char *asset) { return 1; }
ImVec2 GetTextureRect(int handle) { return ImVec2(4,4); }

//-------------------------------------------------------------------------
// DATA
//-------------------------------------------------------------------------

#define TAG32C(abcd) (0[abcd]<<0)|(1[abcd]<<8)|(2[abcd]<<16)|(3[abcd]<<24)
const CRichTag TAG_BOLD = TAG32C("B\0\0\0");   // [b] [/b]
const CRichTag TAG_H1 = TAG32C("H1\0\0");    // [h1] [/h1]
const CRichTag TAG_H2 = TAG32C("H2\0\0");    // [h2] [/h2]
const CRichTag TAG_H3 = TAG32C("H3\0\0");    // [h3] [/h3]
const CRichTag TAG_H4 = TAG32C("H4\0\0");    // [h4] [/h4]
const CRichTag TAG_IMG = TAG32C("IMG\0");    // [img][/img]

//-------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------
    
enum CUiRichTextParseResult
{
    // >0 : regular character
    UiRichTextParseResult_End = 0,
    UiRichTextParseResult_Continue = -1,
};

static int UiRichParseChar(CRichTextState& state, const char*& src, const char* text_end)
{
    // Decode and advance source
    unsigned int c = (unsigned int)*src;
    if (c < 0x80)
    {
        src += 1;
    }
    else
    {
        src += ImTextCharFromUtf8(&c, src, text_end);
        if (c == 0)
            return UiRichTextParseResult_End;
    }

    if (!state.InhibitOne)
    {
        if (c == '\\')
        {
            state.InhibitOne = true;
            return UiRichTextParseResult_Continue;
        }
        else if (c == '[')
        {
            const bool pop = (src[0] == '/');
            if (pop)
                src++;
            const char* content_end = pop ? src - 2 : NULL;

            CRichTag tag = 0;
            const char* tag_begin = src;
            const char* tag_end = src;
            while (tag_end < text_end && tag_end[0] != ']')
            {
                if (tag_end - tag_begin < 4)
                    tag |= toupper(tag_end[0]) << ((tag_end - tag_begin) * 8);
                tag_end++;
            }
            src = tag_end + 1;

            CRichTagBackup b;
            b.Tag = tag;
            if (!pop)
            {
                // Push
                b.Backup = state.TextConfigMode;
                b.ContentBegin = src;
                if (tag == TAG_BOLD)
                    state.SetTextStyle(RichTextStyle_Bold);
                else if (tag == TAG_H1)
                    state.SetTextStyle(RichTextStyle_H1);
                else if (tag == TAG_H2)
                    state.SetTextStyle(RichTextStyle_H2);
                else if (tag == TAG_H3)
                    state.SetTextStyle(RichTextStyle_H3);
                else if (tag == TAG_H4)
                    state.SetTextStyle(RichTextStyle_H4);
                else if (tag == TAG_IMG)
                    state.HideChars = true;
                else
                    state.SetError();
                state.TagStack.push_back(b);
            }
            else
            {
                // Pop
                if (state.TagStack.empty())
                    return UiRichTextParseResult_Continue;
                CRichTagBackup b = state.TagStack.back();
                if (b.Tag != tag)
                {
                    state.SetError();
                    return UiRichTextParseResult_Continue;       // Parse error
                }

                if (tag == TAG_IMG)
                {
                    char f[128];
                    snprintf(f, 128, "%s%.*s", state.Config->ImagePathPrefix, (int)(content_end - b.ContentBegin), b.ContentBegin);
                    int tex = GetTexture(f);
                    float tex_scale = 1.0f;
                    ImVec2 tex_size = GetTextureRect(tex) * tex_scale;
                    if (state.DrawList)
                    {
                        ImVec2 img_pos(state.StartPos.x, state.CurrentPos.y + state.FontSize * 0.4f);
                        //tex->Draw(state.DrawList, img_pos, IM_COL32_WHITE, ImVec2(tex_scale, tex_scale), 0.0f, ImVec2(0.5f,0.0f)); 
                    }
                    state.CurrentPos.y += (state.FontSize * 0.4f) + tex_size.y;
                    state.HideChars = false;
                }

                if (tag == TAG_BOLD || tag == TAG_H1 || tag == TAG_H2 || tag == TAG_H3 || tag == TAG_H4)
                {
                    state.SetTextStyle(b.Backup);
                }
            }
            return UiRichTextParseResult_Continue;
        }
    }

    state.InhibitOne = false;
    return c;
}


ImVec2 RichTextCalcSize(const ImFont* font, float font_size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining)
{
    CRichTextDrawConfig config;
    config.Styles[RichTextStyle_Normal].Set(font, font_size, 0, 0);
    config.WrapWidth = wrap_width;
    return RichTextCalcSizeEx(config, text_begin, text_end, remaining);
}

ImVec2 RichTextCalcSizeEx(const CRichTextDrawConfig& config, const char* text_begin, const char* text_end, const char** remaining)
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

    ImVec2 text_size = ImVec2(0, 0);
    float line_width = 0.0f;

    const bool word_wrap_enabled = (config.WrapWidth > 0.0f);
    const char* word_wrap_eol = NULL;

    CRichTextState state(&config);

    const char* s = text_begin;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            ASSERT(0);
#if 0
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = UiRichCalcWordWrapPosition(font, scale, s, text_end, wrap_width - line_width);
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                if (text_size.x < line_width)
                    text_size.x = line_width;
                text_size.y += line_height;
                line_width = 0.0f;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsSpace(c)) { s++; }
                    else if (c == '\n') { s++; break; }
                    else { break; }
                }
                continue;
            }
#endif
        }

        const char* prev_s = s;

        state.StartPos = state.CurrentPos = ImVec2(0, 0);
        const int c = UiRichParseChar(state, s, text_end);
        line_width += state.CurrentPos.x;
        text_size.y += state.CurrentPos.y;

        if (c == UiRichTextParseResult_End)
            break;
        if (c == UiRichTextParseResult_Continue || c == '\r')
            continue;
        if (c == '\n')
        {
            text_size.x = ImMax(text_size.x, line_width);
            text_size.y += state.LineHeight;
            line_width = 0.0f;
            continue;
        }

        const ImFont* font = state.Font;
        const float char_width = ((int)c < font->IndexAdvanceX.Size ? font->IndexAdvanceX[(int)c] : font->FallbackAdvanceX) * state.FontScale;
        if (line_width + char_width >= config.MaxWidth)
        {
            s = prev_s;
            break;
        }

        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += state.LineHeight;

    if (remaining)
        *remaining = s;

    return text_size;
}

void RichTextDraw(ImDrawList* draw_list, const CRichTextDrawConfig& config, CRichTextState& state, CUiTextFlags flags, ImVec2 text_pos, const char* text_begin, const char* text_end, const char* text_display_end)
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin);

    const ImVec4 clip_rect = config.ClipRect ? *config.ClipRect : draw_list->_ClipRectStack.back();

    if (flags & (UiTextFlags_VCenter | UiTextFlags_VBottom))
    {
        const ImVec2 text_size = RichTextCalcSizeEx(config, text_begin, text_end);
        if (flags & UiTextFlags_VCenter)
            text_pos.y -= text_size.y * 0.5f;
        else if (flags & UiTextFlags_VBottom)
            text_pos.y -= text_size.y;
    }

    // Align to be pixel perfect
    text_pos.x = (float)(int)text_pos.x + state.Font->DisplayOffset.x;
    text_pos.y = (float)(int)text_pos.y + state.Font->DisplayOffset.y;
    state.StartPos = text_pos;
    state.CurrentPos = text_pos;
    if (state.CurrentPos.y > clip_rect.w)
        return;
    state.DrawList = draw_list;

    const bool word_wrap_enabled = (config.WrapWidth > 0.0f);
    const char* word_wrap_eol = NULL;

    const char* s = text_begin;

    // Skip non-visible lines
    //if (!word_wrap_enabled && state.CurrentPos.y + state.LineHeight < clip_rect.y)
    //    while (s < text_end && *s != '\n')  // Fast-forward to next line
    //        s++;

    draw_list->PushTextureID(state.Font->ContainerAtlas->TexID);

    for (const char* line_begin = s; line_begin < text_end; )
    {
        if (word_wrap_enabled)
        {
            ASSERT(0);
#if 0
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = UiRichCalcWordWrapPosition(font, scale, s, text_end, wrap_width - (x - pos.x));
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsSpace(c)) { s++; }
                    else if (c == '\n') { s++; break; }
                    else { break; }
                }
                continue;
            }
#endif
        }

        const char* line_end = strchr(line_begin, '\n');
        if (!line_end)
            line_end = text_end;

        state.CurrentPos.x = state.StartPos.x;
        if (flags & (UiTextFlags_Center | UiTextFlags_Right))
        {
            ImVec2 line_size = RichTextCalcSizeEx(config, line_begin, line_end);
            if (flags & UiTextFlags_Center)
                state.CurrentPos.x -= line_size.x * 0.5f;
            if (flags & UiTextFlags_Right)
                state.CurrentPos.x -= line_size.x;
        }

        // Render one line of text
        CRichTextState state_backup = state;
        for (int render_pass = (flags & UiTextFlags_Outline) ? 0 : 1; render_pass < 2; render_pass++)
        {
            if ((flags & UiTextFlags_Outline) && render_pass == 0)
                state.CurrentPos -= ImVec2(2,2);
            if ((flags & UiTextFlags_Outline) && render_pass > 0)
                state = state_backup;

            const char* s = line_begin;
            while (s < line_end)
            {
                const int c = UiRichParseChar(state, s, line_end);
                if (c == UiRichTextParseResult_End)
                    break;
                if (c == UiRichTextParseResult_Continue || c == '\r')
                    continue;
                if (state.HideChars)
                    continue;
                ASSERT(c != '\n');

                const ImFont* font = state.Font;
                const ImU32 char_col = (render_pass == 0) ? state.ColorOutline : state.Color;
                font->RenderChar(draw_list, state.FontSize, state.CurrentPos, char_col, (unsigned short)c);
                const float char_width = ((int)c < font->IndexAdvanceX.Size ? font->IndexAdvanceX[(int)c] : font->FallbackAdvanceX) * state.FontScale;
                state.CurrentPos.x += char_width;
            }
        }

        state.CurrentPos.x = state.StartPos.x;
        state.CurrentPos.y += state.LineHeight;

        if (state.CurrentPos.y > clip_rect.w)
            break;

        line_begin = line_end + 1;
        //if (!word_wrap_enabled && state.CurrentPos.y + state.LineHeight < clip_rect.y)
        //    while (line_begin < text_end && *line_begin != '\n')  // Fast-forward to next line
        //        line_begin++;
    }

    draw_list->PopTextureID();
    state.DrawList = NULL;
}

void richtext_demo() {
    static CRichTextDrawConfig config, *ever = 0;
    if( !ever ) { ever = &config;
        config.Styles[RichTextStyle_Normal].Font = ImGui::GetIO().Fonts->Fonts[0];
        config.Styles[RichTextStyle_Normal].FontSize = 16;
        config.Styles[RichTextStyle_Bold].Font = ImGui::GetIO().Fonts->Fonts[0];
        config.Styles[RichTextStyle_Bold].FontSize = 16;
        config.Styles[RichTextStyle_H1].Font = ImGui::GetIO().Fonts->Fonts[0];
        config.Styles[RichTextStyle_H1].FontSize = 16;
        config.Styles[RichTextStyle_H2].Font = ImGui::GetIO().Fonts->Fonts[0];
        config.Styles[RichTextStyle_H2].FontSize = 16;
        config.Styles[RichTextStyle_H3].Font = ImGui::GetIO().Fonts->Fonts[0];
        config.Styles[RichTextStyle_H3].FontSize = 16;
        config.Styles[RichTextStyle_H4].Font = ImGui::GetIO().Fonts->Fonts[0];
        config.Styles[RichTextStyle_H4].FontSize = 16;
    }
    static CRichTextState state( &config );
    ImVec2 pos(0,0);
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    RichTextDraw(window->DrawList, config, state, UiTextFlags_Default_, pos, "this is a test" );
}