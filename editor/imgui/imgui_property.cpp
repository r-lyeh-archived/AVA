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
    PROPERTY_COLOR3,    // [x] floatx3
    PROPERTY_COLOR4,    // [ ] floatx4
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
    struct { int item, max_items; const char **items; };
    struct { float value, minv, maxv; const char *format; };
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
        char *id = info, type = info[2], *name = info+3;
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
                break; case 'e': data.item = rand() % data.max_items;
                break; case 'f': data.value = (data.maxv - data.minv ) * (rand() / (float)RAND_MAX) + data.minv;
                break; case '4': case '3': case '2': case '1':
                for( int id = 0, end_id = info[2] - '0'; id < end_id; ++id )
                    for( int i = 0; i < 3; ++i ) data.color[id][i] = rand() / (float)RAND_MAX;
            }
        }

        int ret = 0;
        ImGui::PushID(this);
        switch( type ) {
            default:
            break; case 'e': ret = ImGui::Combo(info, &data.item, data.items, data.max_items);
            break; case 'f': ret = ImGui::SliderFloat(info, &data.value, data.minv, data.maxv, data.format);
            break; case '4': case '3': case '2': case '1': {
                int misc_flags = ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_PickerHueWheel;
                for( int id = 0, end_id = type - '0'; id < end_id; ++id ) {
                    ImGui::PushID(id); ret |= !!ImGui::ColorEdit3(info, data.color[id], misc_flags); ImGui::PopID();
                    if( id != end_id - 1 ) ImGui::SameLine();
                }
            }
            break; case 't': { bool prev = data.on; ToggleButton(info, &data.on); ret = prev != data.on; } /*ImGui::Toggle*/
            break; case 'b': ret = ImGui::Button("on");
            break; case 'B': ret = ImGui::Button(data.on ? "on" : "off"); data.on ^= !!ret;
            break; case '>': ImGui::CollapsingHeader(info);
            break; case '-': ImGui::Separator();
            break; case 's': ImGui::InputText(info, data.string, IM_ARRAYSIZE(data.string));
            break; case '*': ImGui::InputText(info, data.string, IM_ARRAYSIZE(data.string), ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);
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
    p.data.minv = FLT_MIN/2;
    p.data.maxv = FLT_MAX/2;
    p.data.format = "%f";
    return p;
}

property property_list( const char *info, const std::vector<const char *> &labels ) {
    property p = MAKE_PROPERTY(info, 'e');
    p.data.max_items = (int)labels.size();
    p.data.items = (const char **)malloc( sizeof(char*) * p.data.max_items ); // LEAK
    for( int i = 0; i < p.data.max_items; ++i ) {
        p.data.items[i] = labels[i];
    }
    return p;
}

property property_color3( const char *info, float r, float g, float b ) {
    property p = MAKE_PROPERTY(info, '1');
    p.data.color[0][0] = r;
    p.data.color[0][1] = g;
    p.data.color[0][2] = b;
    p.data.color[0][3] = 1;
    return p;
}

property property_color4( const char *info, float r, float g, float b, float a ) {
    property p = MAKE_PROPERTY(info, '1');
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

void property_demo() {
    static struct property p0 = property_group( "My widgets" );
    static struct property p1 = property_angle( ICON_MD_KEYBOARD_ARROW_DOWN " My Angle\nThis is PI", 3.14159f );
    static struct property p2 = property_list(  ICON_MD_TIMELAPSE   " My Enum\nThis is ENUM", {"AAAA", "BBBB", "CCCC"} );
    static struct property p3 = property_list(  ICON_MD_FILE_UPLOAD " My Enum\nThis is ENUM", {"AAAA", "BBBB", "CCCC"} );
    static struct property p4 = property_color4( "My Color\nConfigure a color", 0,0,0,0 );
    static struct property p5 = property_separator( "" );
    static struct property p6 = property_toggle( "My Toggle\nConfigure a state", 1 );
    static struct property p7 = property_button( "My Button" );
    static struct property p8 = property_button2( "My Button 2", 1 );
    static struct property p9 = property_delta( "My Delta", 0.5f );
    static struct property pA = property_slider( "My Slider", 0.5f );
    static struct property pB = property_string( "My String", "abc" );
    static struct property pC = property_password( "My Password", "abc" );

    struct property *ps[] = { &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &pA, &pB, &pC };
    for( auto &p : ps ) {
        // p->disabled = 1;
        p->draw();
    }
}
