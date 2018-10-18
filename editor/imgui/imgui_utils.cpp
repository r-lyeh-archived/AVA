#pragma once

#ifndef IM_ARRAYSIZE
#define IM_ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

namespace ImGui {
    // @dougbinks
    bool FullWidthButton(const char* l) {
        return ImGui::Selectable(l, false);
    }
    bool CheckBoxFont( const char* name_, bool* pB_, const char* pOn_ = "[X]", const char* pOff_="[  ]" ) {
        if( *pB_ ) {
            ImGui::Text(pOn_);
        } else {
            ImGui::Text(pOff_);
        }
        bool bHover = false;
        bHover = bHover || ImGui::IsItemHovered();
        ImGui::SameLine();
        ImGui::Text( name_ );
        bHover = bHover || ImGui::IsItemHovered();
        if( bHover && ImGui::IsMouseClicked(0) ) {
            *pB_ = ! *pB_;
            return true;
        }
        return false;
    }
    // @r-lyeh
    // Small buttons fits within text without additional vertical and horizontal spacing.
    bool TinyButton(const char* label, bool transparent = 0, ImVec2 padding = ImVec2(0,0) ) {
        ImGuiContext& g = *GImGui;
        float backup_padding_x = g.Style.FramePadding.x;
        float backup_padding_y = g.Style.FramePadding.y;
        g.Style.FramePadding.x = padding.x;
        g.Style.FramePadding.y = padding.y;
        int flags = transparent ? 1<<15 : 0; // custom handled
        bool pressed = ButtonEx(label, ImVec2(0,0), ImGuiButtonFlags_AlignTextBaseLine | flags);
        g.Style.FramePadding.y = backup_padding_y;
        g.Style.FramePadding.x = backup_padding_x;
        return pressed;
    }
    // @r-lyeh
    bool MyCheckbox(bool *mode, const char *off, const char *on) {
        //ImGui::LabelText( *mode ? off : (on ? on : off), "" );
        ImGui::Text( *mode ? off : (on ? on : off) );
        bool ret = *mode ^= ImGui::IsItemClicked(0);
        return ret;
    }
    bool MyButton(bool *mode, const char *off, const char *on) {
        ImGui::TinyButton( *mode ? off : (on ? on : off), 1 );
        return *mode ^= ImGui::IsItemClicked(0);
    }

    bool CheckBoxTick( const char* name_, bool* pB_ ) {
        return CheckBoxFont( name_, pB_, ICON_MD_CHECK_BOX, ICON_MD_CHECK_BOX_OUTLINE_BLANK );
    }
    bool MenuItemCheckbox( const char* name_, bool* pB_ ) {
        bool retval = ImGui::MenuItem( name_ );
        ImGui::SameLine();
        if( *pB_ ) {
            ImGui::Text(ICON_MD_CHECK_BOX);
        } else {
            ImGui::Text(ICON_MD_CHECK_BOX_OUTLINE_BLANK);
        }
        if( retval ) {
            *pB_ = ! *pB_;
        }
        return retval;
    }
}

