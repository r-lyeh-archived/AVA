// horizontal widget, composed of:
// [revert/random][icon][manipulator][text] | with tooltip in whole line

enum {
    PROPERTY_GROUP,     // [x] >v
    PROPERTY_SEPARATOR, // [x] ---
    PROPERTY_BUTTON,    // [x] button
    PROPERTY_BUTTON2,   // [x] button (on/off state) @todo: shading & offset
    PROPERTY_TOGGLE,    // [x] bool
    PROPERTY_SLIDERI,   // [ ] int
    PROPERTY_SLIDERF,   // [x] float
    PROPERTY_RANGEI,    // [ ] int[2]
    PROPERTY_RANGEF,    // [ ] float[2]
    PROPERTY_STRING,    // [x] string box
    PROPERTY_TEXT,      // [ ] text box
    PROPERTY_TRANSFORM, // [ ] gizmo
    PROPERTY_PASSWORD,  // [x] hidden string box ***
    PROPERTY_DELTA,     // [x] float [0..1]
    PROPERTY_PERCENT,   // [ ] float [0..100]
    PROPERTY_ANGLE,     // [x] float [-180..180]
    PROPERTY_BITMASK,   // [ ] int [0..64]
    PROPERTY_PROGRESS,  // [ ] float [0..100]
    PROPERTY_COLOR3,    // [x] color3
    PROPERTY_COLOR4,    // [x] color4
    PROPERTY_VECTOR3,   // [x] vector3
    PROPERTY_VECTOR4,   // [x] vector4

/*
    PROPERTY_TEXTURE1D,
    PROPERTY_TEXTURE2D,
    PROPERTY_TEXTURE3D,
    PROPERTY_CUBEMAP,
    PROPERTY_LIGHT,
    PROPERTY_TRACKER,
    PROPERTY_TIMELINE,
    PROPERTY_PROFILER,
    PROPERTY_TOOLBAR,
*/
};

union property_data {
    bool    on;
    char    string[64];
    float   color[4][4];  // upto 4 colors
    float   vector[4][4]; // upto 4 vec4s
    float   matrix[16];
    struct { float value, minv, maxv; const char *format; };
    struct { unsigned bits, hoverIndex; }; // bitmask
    int     ip[4];
    struct { 
        int max_items; const char **items;
        union {
            int item;
            struct { ComboFilterState cfs; char filtbuf[32]; };
        };
    };
};

struct property_result { // @todo
    int dummy;
    int hover;
    int click;
    int shift, alt, ctrl;
    float drag[2];
};

struct property {

    char *info; // ##xName where X=(e)num,(s)lider,(0-3)color/vector; Name may include icons
    char *help;

    property_data data;

    bool hidden;
    bool disabled;
    bool randomized;

    void (*on_load)();
    void (*on_save)();
    void (*on_random)();

    bool copied;
    property_data copy;

    bool draw() {
        auto show_tooltip = [&]() {
            if (help && ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(help);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        };

        if( !copied ) {
            copy = data;
            copied = 1;
        }

        if( hidden ) return false;

        char *id = info, type = info[2], *name = info+3;
        if( type == '>' || type == '-' ) {
            ImGui::PushID(this);
            if( type == '>' ) {
                ImGui::CollapsingHeader(info);
            } else {
                ImGui::Separator();
            }
            ImGui::PopID();
            return false;
        }

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
        if( disabled ) ImGui::TextDisabled(name); else ImGui::Text(name);
        show_tooltip();
        ImGui::PopID();

        ImGui::NextColumn();

        ImGui::PushItemWidth(-1);

        if( disabled ) ImGui::PushDisabled();

        // (e)num list, (s)lider/(f)loat, vec(1/2/3/4), (a)ngle

        if( randomized ) {
            randomized = 0;
            switch( info[2] ) {
                default:
                break; case 'l': case 'L': data.item = rand() % data.max_items;
                break; case 'f': case 'F': data.value = (data.maxv - data.minv ) * (rand() / (float)RAND_MAX) + data.minv;
                break; case '4': case '3': case '2': case '1':
                for( int id = 0, end_id = info[2] - '0'; id < end_id; ++id )
                    for( int i = 0; i < 3; ++i ) data.color[id][i] = rand() / (float)RAND_MAX;
            }
        }

        int ret = 0;
        ImGui::PushID(this);
        switch( type ) {
            default:
            break; case 'l': ret = ImGui::Combo(info, &data.item, data.items, data.max_items);
            break; case 'L': ret = ComboFilter(info, data.filtbuf, 32, data.items, data.max_items, data.cfs);
            break; case 'f': ret = ImGui::SliderFloat(info, &data.value, data.minv, data.maxv, data.format);
            break; case 'F': ret = ImGui::InputFloat(info, &data.value, 0.01f, 1.0f);
            break; case 'v': ret = ImGui::InputFloat3(info, &data.vector[0][0]);
            break; case 'V': ret = ImGui::InputFloat4(info, &data.vector[0][0]);
            break; case 'c': ret = ImGui::ColorEdit3(info, data.color[0], ImGuiColorEditFlags_PickerHueWheel);
            break; case 'C': ret = ImGui::ColorEdit4(info, data.color[0], ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaPreview);
            break; case 't': ret = ImGui::Checkbox(info, &data.on); // { bool prev = data.on; ToggleButton(info, &data.on); ret = prev != data.on; } /*ImGui::Toggle*/
            break; case 'b': ret = ImGui::Button("click");
            break; case 'B': {
                int i = 0;
                if( data.on ) {
                    const ImU32 col = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
                    ImGui::PushStyleColor(ImGuiCol_Button, col); //(ImVec4)ImColor::HSV(i/7.0f, 0.6f, 0.6f));
                    ret = ImGui::Button("click"); data.on ^= !!ret;
                    ImGui::PopStyleColor(1);
                } else {
                    ret = ImGui::Button("click"); data.on ^= !!ret;
                }
//                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i/7.0f, 0.7f, 0.7f));
//                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i/7.0f, 0.8f, 0.8f));
            }
            break; case '>': ImGui::CollapsingHeader(info);
            break; case '-': ImGui::Separator();
            break; case 's': ImGui::InputText(info, data.string, IM_ARRAYSIZE(data.string));
            break; case '*': ImGui::InputText(info, data.string, IM_ARRAYSIZE(data.string), ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);
            break; case 'm': BitField(info, &data.bits, &data.hoverIndex); // bitmask
            break; case 'i': ipentry(data.ip);
            break; case 'k': MyKnob(""/*info*/, &data.value, 0.0f, 1.0f);
        }
        ImGui::PopID();

        ImGui::PopItemWidth();

        ImGui::Columns(1);
        show_tooltip();

        if( disabled ) ImGui::PopDisabled();

        return ret;
    }
};

property MAKE_PROPERTY(const char *info, char type) {
    property p = {0};
    p.info = (char*)malloc( strlen(info) + 3 + 1 ); // LEAK
    sprintf(p.info, "##%c%s", type, info );
    p.help = strchr(p.info, '\n');
    if( p.help ) (++p.help)[-1] = 0;
    return p;
}

property property_group( const char *info ) {
    property p = MAKE_PROPERTY(info, '>');
    return p;
}

property property_separator( const char *info ) {
    property p = MAKE_PROPERTY("hidden", '-');
    return p;
}

property property_angle( const char *info, float default_angle ) {
    property p = MAKE_PROPERTY(info, 'f');
    p.data.value = default_angle;
    p.data.minv = -180;
    p.data.maxv = 180;
    p.data.format = "%5.2fº";
    return p;
}

property property_delta( const char *info, float default_value ) {
    property p = MAKE_PROPERTY(info, 'f');
    p.data.value = default_value;
    p.data.minv = 0;
    p.data.maxv = 1;
    p.data.format = "%5.2f";
    return p;
}

property property_slider( const char *info, float default_value ) {
    property p = MAKE_PROPERTY(info, 'f');
    p.data.value = default_value;
    p.data.minv = -FLT_MAX/2;
    p.data.maxv = +FLT_MAX/2;
    p.data.format = "%f";
    return p;
}

property property_float( const char *info, float default_value ) {
    property p = MAKE_PROPERTY(info, 'F');
    p.data.value = default_value;
    p.data.minv = -FLT_MAX;
    p.data.maxv = +FLT_MAX;
    p.data.format = "%f";
    return p;
}

property property_list( const char *info, const std::vector<const char *> &labels ) {
    property p = MAKE_PROPERTY(info, 'l');
    p.data.max_items = (int)labels.size();
    p.data.items = (const char **)malloc( sizeof(char*) * p.data.max_items ); // LEAK
    for( int i = 0; i < p.data.max_items; ++i ) {
        p.data.items[i] = labels[i];
    }
    return p;
}

property property_listfilt( const char *info, const std::vector<const char *> &labels ) {
    property p = MAKE_PROPERTY(info, 'L');
    p.data.max_items = (int)labels.size();
    p.data.items = (const char **)malloc( sizeof(char*) * p.data.max_items ); // LEAK
    for( int i = 0; i < p.data.max_items; ++i ) {
        p.data.items[i] = labels[i];
    }
    snprintf(p.data.filtbuf, 32, "%s", p.data.items[0]);
    return p;
}

property property_vector3( const char *info, float r, float g, float b ) {
    property p = MAKE_PROPERTY(info, 'v');
    p.data.vector[0][0] = r;
    p.data.vector[0][1] = g;
    p.data.vector[0][2] = b;
    return p;
}

property property_vector4( const char *info, float r, float g, float b, float a ) {
    property p = MAKE_PROPERTY(info, 'V');
    p.data.vector[0][0] = r;
    p.data.vector[0][1] = g;
    p.data.vector[0][2] = b;
    p.data.vector[0][3] = a;
    return p;
}

property property_color3( const char *info, float r, float g, float b ) {
    property p = MAKE_PROPERTY(info, 'c');
    p.data.color[0][0] = r;
    p.data.color[0][1] = g;
    p.data.color[0][2] = b;
    p.data.color[0][3] = 1;
    return p;
}

property property_color4( const char *info, float r, float g, float b, float a ) {
    property p = MAKE_PROPERTY(info, 'C');
    p.data.color[0][0] = r;
    p.data.color[0][1] = g;
    p.data.color[0][2] = b;
    p.data.color[0][3] = a;
    return p;
}

property property_toggle( const char *info, bool on ) {
    property p = MAKE_PROPERTY(info, 't');
    p.data.on = on;
    return p;
}

property property_button( const char *info ) {
    property p = MAKE_PROPERTY(info, 'b');
    return p;
}

property property_button2( const char *info, bool on ) {
    property p = MAKE_PROPERTY(info, 'B');
    p.data.on = on;
    return p;
}

property property_string( const char *info, const char *default_string ) {
    property p = MAKE_PROPERTY(info, 's');
    snprintf( p.data.string, sizeof(p.data.string), "%s", default_string );
    return p;
}

property property_password( const char *info, const char *default_string ) {
    property p = MAKE_PROPERTY(info, '*');
    snprintf( p.data.string, sizeof(p.data.string), "%s", default_string );
    return p;
}

property property_bitmask( const char *info, unsigned default_mask ) {
    property p = MAKE_PROPERTY(info, 'm');
    p.data.bits = default_mask;
    return p;
}

property property_ip( const char *info, int ip0, int ip1, int ip2, int ip3 ) {
    property p = MAKE_PROPERTY(info, 'i');
    p.data.ip[0] = ip0;
    p.data.ip[1] = ip1;
    p.data.ip[2] = ip2;
    p.data.ip[3] = ip3;
    return p;
}

property property_knob( const char *info, float v ) {
    property p = MAKE_PROPERTY(info, 'k');
    p.data.value = v;
    return p;
}




void property_demo() {
    static struct property ps[] = {
        property_group( "My widgets" ),
        property_angle( ICON_MD_KEYBOARD_ARROW_DOWN " My Angle\nThis is PI", 3.14159f ),
        property_list( ICON_MD_TIMELAPSE " My Enum\nThis is ENUM", {"AAAA", "BBBB", "CCCC"} ),
        property_listfilt( ICON_MD_FILE_UPLOAD " My Enum\nThis is ENUM", {"AAAA", "BBBB", "CCCC"} ),
        property_color3( "My Color3\nConfigure a color", 0,0,0 ),
        property_color4( "My Color4\nConfigure a color", 0,0,0,0 ),
        property_separator( "" ),
        property_toggle( "My Toggle\nConfigure a state", 1 ),
        property_button( "My Button" ),
        property_button2( "My Button 2", 1 ),
        property_delta( "My Delta", 0.5f ),
        property_slider( "My Slider", 0.5f ),
        property_float( "My Float", 0.5f ),
        property_string( "My String", "abc" ),
        property_password( "My Password", "abc" ),
        property_vector3( "My Vector3", 0,1,2 ),
        property_vector4( "My Vector4", 0,1,2,3 ),
        property_bitmask( "My Bitmask", 0xCCCC ),
        property_ip( "My IP", 127,0,0,1 ),
        property_knob( "My Knob", 0.5f ),
    };
    for( auto &p : ps ) {
        // p.disabled = 1;
        p.draw();
    }
}
