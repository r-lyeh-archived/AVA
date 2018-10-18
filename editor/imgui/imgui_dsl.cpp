#pragma once

#include <vector>
#include <string>
std::vector<std::string> notifications;
const char *modal = 0;
int clicked = 0;
int logged = 0;
float topbar_progress = 0;
#include <map>
static std::map<std::string, bool > changed;
static std::map<std::string, std::map<int, bool> >  booleans;
static std::map<std::string, std::map<int, int>  >  integers;
static std::map<std::string, std::map<int, float>>  floats;
extern "C" int editor;

static const char *names[] = { "ABC", "DEF" };
static std::map<int /*name*/, std::vector<int /*mask*/>> groups;


#   define ___ \
    ImGui::Separator();
#   define THEN \
    ImGui::SameLine();
#   define HINT(x) \
if( ImGui::IsItemHovered()) ImGui::SetTooltip(x);
#   define MENU(x, ...) \
if( ImGui::BeginMenu( ("" #__VA_ARGS__)[0] == '\0' ? ICON_MD_EXPAND_MORE "  " x : __VA_ARGS__ "  " x ) )
#   define ENDMENU \
    ImGui::EndMenu();
#   define ITEM3(x,y,z) \
    clicked = ImGui::MenuItem(x "  " y, z);
#   define ITEM2(x,y) \
    ITEM3(x,y,"")
#   define ITEM1(x) \
    ITEM3("      " x,"","")
#   define GRAY3(x,y,z) \
    clicked = ImGui::MenuItem(x "  " y, z, false, false);
#   define GRAY2(x,y) \
    GRAY3(x,y,"")
#   define GRAY1(x) \
    GRAY3(x,"","")
#   define BUTTON(x) \
    clicked = ImGui::TinyButton(x);
#   define ICON(x) \
    clicked = ImGui::TinyButton(x, 1);
#   define CHECKEX(x,icon0,icon1) \
    clicked = ImGui::MyCheckbox( &booleans[x][__COUNTER__], icon0, icon0##icon1 ); \
    THEN ImGui::MenuItemCheckbox(x, &booleans[x][__COUNTER__ - 1]);
#   define CHECK(x) \
    clicked = ImGui::CheckBoxFont(x, &booleans[x][__COUNTER__], ICON_MD_CHECK_BOX, ICON_MD_CHECK_BOX_OUTLINE_BLANK);
#   define SELECT(x) \
    clicked = ImGui::Selectable(x, &booleans[x][__COUNTER__], ImGuiSelectableFlags_DontClosePopups);
#   define SUBMENU(x, ...) \
if( ImGui::BeginMenu( ("" #__VA_ARGS__)[0] == '\0' ? x : __VA_ARGS__ "  " x ) )
#   define ENDSUBMENU \
    ENDMENU // /**/
#   define S7R(x) #x
#   define STR(x) S7R(x)
#   define HIGHLIGHT(x) \
    clicked = ImGui::Selectable(x, 1, ImGuiSelectableFlags_DontClosePopups);
#   define NOTIFY(x) \
    notifications.push_back( x );
#   define MODAL(title) \
    modal = title;
#   define ON_MODAL(title, text, left, right, leftcb, rightcb) \
    if (ImGui::BeginPopupModal(title, NULL)) { \
        ImGui::Text(text); \
        ImGui::Separator(); \
        float w = ImGui::GetContentRegionAvailWidth(); \
        if (ImGui::Button(left, ImVec2(w*0.5,0))) { leftcb(); modal = 0; ImGui::CloseCurrentPopup(); } \
        ImGui::SameLine(); \
        if (ImGui::Button(right, ImVec2(-1,0))) { rightcb(); modal = 0; ImGui::CloseCurrentPopup(); } \
        ImGui::EndPopup(); \
    }
#   define POPUP(x, ...) do { \
    ImGui::OpenPopup(x); \
    if (ImGui::BeginPopup(x)) { \
        __VA_ARGS__; \
        ImGui::EndPopup(); \
    } } while(0)
#   define ON_CLICK \
    if ( clicked || ImGui::IsItemClicked(0) ) // IsItemHovered() && IsMouseClicked(int button, bool repeat)
#   define GROUP(stage,mask) { \
        ImGui::Columns(4, NULL, false); \
        for (int i = 0; i < IM_ARRAYSIZE(names); i++) { \
            bool v = groups[stage][i] == /*&*/ mask; \
            ImGui::Selectable(names[i], &v, ImGuiSelectableFlags_DontClosePopups); \
            if( ImGui::IsItemClicked(0) ) groups[stage][i] = v ? /*(groups[i] & ~mask)*/ 0 : /*(groups[i] | mask)*/ mask; \
            ImGui::NextColumn(); \
        } \
        ImGui::Columns(1); \
    }
//  GETTERS
#   define OPTION(x) (booleans[x].size() && booleans[x].begin()->second)

#   define ONCE(...) do { static bool once = 1; if( once ) { once = 0; __VA_ARGS__; } } while(0)



#if 1

#   define TOGGLE2(icon0,icon1) \
    do { auto &v = booleans[""][__COUNTER__]; \
    clicked = (v ^= ImGui::TinyButton(v ? icon0 : icon1, 1 )); } while(0);
#   define TOGGLE(icon0,icon1) \
    TOGGLE2(icon0, icon0##icon1)

#else

#define TOGGLEEX(status, on, off ) \
        do { status ^= ImGui::SmallButton( status ? on : off ); } while(0)
#define TOGGLE2(on, off) \
        do { static bool b = 0; TOGGLEEX(b, on, off); } while(0)
#define TOGGLE(on, off) \
        TOGGLE2(on, on##off);

#endif
