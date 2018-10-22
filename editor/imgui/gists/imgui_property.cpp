/* todo:
slider (1d), range (2d),
transform (gizmo),
string, passw (string), double, integer, 
percent, delta, angle, 
(progress)bar, 
bitmask, 
color, palette ramp (1d), texture (2d), cubemap, light, 
tracker,
timeline,
profiler,
toolbar,
separator
group
*/


// horizontal widget, composed of:
// [icon][revert][reset][manipulator][text] | with tooltip in whole line

union widget_data {
    bool  on;
    struct { int item; const char **items; int max_items; };
    char  text[64];
    struct { float value, minv, maxv; const char *format; };
    float color[4][4];  // upto 4 colors
    float vector[4][4]; // upto 4 vec4s
    float matrix[16];
};

/*
struct widget_result {
    int dummy;
    int hover;
    int click;
    int shift, alt, ctrl
    float drag[2];
};
*/

struct widget2 {

    const char *name; // ##xName where X=(e)num,(s)lider,(0-3)color/vector; Name may include icons
    const char *help;

    widget_data data;

    bool hidden;
    bool disabled;
    bool randomized;

    void (*on_load)();
    void (*on_save)();
    void (*on_random)();

    bool copied;
    widget_data copy;

    bool draw() {
        if( !copied ) {
            copy = data;
            copied = 1;
        }

        if( hidden ) return false;

        bool shift = !!ImGui::GetIO().KeyShift;

        ImGui::Columns(2);

        //static float initial_spacing = 100.f; if( initial_spacing ) ImGui::SetColumnWidth(0, initial_spacing), initial_spacing = 0;

        ImGui::PushID(this);
            if( ImGui::Button("R") ) { // ICON_MD_UNDO / ICON_MD_SETTINGS_BACKUP_RESTORE
                if( shift ) randomized = 1; else data = copy;  // revert/randomize button
            }
            if( ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("Revert changes. Hold SHIFT to randomize instead.");
                ImGui::EndTooltip();
            }
        ImGui::SameLine();
        ImGui::Text(name+3);
        ImGui::PopID();

        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);

        if( disabled ) ImGui::PushDisabled();

        // (e)num list, (s)lider/(f)loat, vec(1/2/3/4), (a)ngle

        if( randomized ) {
            randomized = 0;
            switch( name[2] ) {
                default:
                break; case 'e': data.item = rand() % data.max_items;
                break; case 's': case 'a': data.value = (data.maxv - data.minv ) * (rand() / (float)RAND_MAX) + data.minv;
                break; case '0': case '1': case '2': case '3':
                for( int id = 0, end_id = name[2] - '0'; id < end_id; ++id )
                    for( int i = 0; i < 3; ++i ) data.color[id][i] = rand() / (float)RAND_MAX;
            }
        }

        int ret = 0;
        ImGui::PushID(this);
        switch( name[2] ) {
            default:
            break; case 'e': ret = ImGui::Combo(name, &data.item, data.items, data.max_items);
            break; case 's': case 'a': ret = ImGui::SliderFloat(name, &data.value, data.minv, data.maxv, data.format);
            break; case '4': case '3': case '2': case '1':;
                int misc_flags = ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_PickerHueWheel;
                for( int id = 0, end_id = name[2] - '0'; id < end_id; ++id ) {
                    ImGui::PushID(id); ret |= !!ImGui::ColorEdit3(name, data.color[id], misc_flags); ImGui::PopID();
                    if( id != end_id - 1 ) ImGui::SameLine();
                }
        }
        ImGui::PopID();

        ImGui::PopItemWidth();

        ImGui::Columns(1);
        if (help && ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(help);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        if( disabled ) ImGui::PopDisabled();

        return ret;
    }
};


widget_data widget_angle( float angle ) {
    widget_data w = {0};
    w.value = angle;
    w.minv = -180;
    w.maxv = 180;
    w.format = "%5.2fº";
    return w;
}

widget_data widget_list( const std::vector<const char *> &labels ) {
    widget_data w = {0};
    w.max_items = (int)labels.size();
    w.items = (const char **)malloc( sizeof(char*) * w.max_items ); // LEAK
    for( int i = 0; i < w.max_items; ++i ) {
        w.items[i] = labels[i];
    }
    return w;
}


void property_demo() {
    static struct widget2 w0 = { "##aMy Angle " ICON_MD_KEYBOARD_ARROW_DOWN, "This is PI", widget_angle( 3.14159f ) };
    static struct widget2 w1 = { "##eMy Enum " ICON_MD_TIMELAPSE, "This is ENUM", widget_list( {"AAAA", "BBBB", "CCCC"} ) };
    static struct widget2 w2 = { "##eMy Enum " ICON_MD_FILE_UPLOAD, "This is ENUM", widget_list( {"AAAA", "BBBB", "CCCC"} ) }; w2.disabled = 1;

    if( ImGui::CollapsingHeader("widgets") ) {
        if( w0.draw() ) printf("PI tweaked: %f\n", w0.data.value);
        if( w1.draw() ) printf("ENUM tweaked: %d\n", w1.data.item);
        if( w2.draw() ) printf("ENUM tweaked: %d\n", w2.data.item);
    }
}
