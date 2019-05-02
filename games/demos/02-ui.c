#include <engine.h>

int main() {
    // 50% sized, no flags
    window_create(50, 0);

    // some vars
    int integer = 0;
    bool boolean = 0;
    float floating = 0;
    float rgb[3] = {1,0,0};
    float rgba[4] = {0,1,0};
    float slider = 0.5f;
    char string[64] = {0};
    int item = 0; const char *list[] = {"one","two","three"};
    //bool show_popup = 0;

    while (window_update()) {

        if( ui_begin("about...", 0)) {
            if( ui_label("label with tooltip@compiled on " __DATE__ " " __TIME__)) {};
            //if( ui_button("show popup") ) show_popup = 1;
            //ui_popup("my popup", &show_popup);
            ui_end();
        }

        if( ui_begin("my tools", 0)) {
            if( ui_bool("my bool", &boolean) ) puts("boolean changed");
            if( ui_int("my int", &integer) ) puts("integer changed");
            if( ui_float("my float", &floating) ) puts("floating changed");
            if( ui_string("my string", string, 64) ) puts("string changed");
            if( ui_separator() ) {}
            if( ui_button("my button") ) puts("button clicked");
            if( ui_color3("my color3", rgb) ) puts("color3 changed");
            if( ui_color4("my color4@this is a tooltip", rgba) ) puts("color4 changed");
            if( ui_combo("my list", &item, list, countof(list) ) ) puts("combo changed");
            if( ui_slider("my slider", &slider)) puts("slider changed");
            ui_end();
        }

        ui_demo();

        //
        ddraw_printf(window_stats());
        ddraw_printf("devtools UI");

        // swap & capture
        static void *pixels = 0;
        window_swap( &pixels );

        // send capture to remote view (crashes while enlarging)
        // network_sendbuf(pixels, window_width(), window_height(), 3, 7755);
    }
}

