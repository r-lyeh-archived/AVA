// flat ui colors
#define FLATUI_PALETTE(X) \
    X(FLATUI_CYAN        ,  26, 188, 156 ) /* turquoise */ \
    X(FLATUI_GREEN       ,  46, 204, 113 ) /* emerland */ \
    X(FLATUI_BLUE        ,  52, 152, 219 ) /* peterriver */ \
    X(FLATUI_PURPLE      , 155,  89, 182 ) /* amethyst */ \
    X(FLATUI_NAVY        ,  52,  73,  94 ) /* wetasphalt */ \
    X(FLATUI_DARKCYAN    ,  22, 160, 133 ) /* greensea */ \
    X(FLATUI_DARKGREEN   ,  39, 174,  96 ) /* nephritis */ \
    X(FLATUI_DARKBLUE    ,  41, 128, 185 ) /* belizehole */ \
    X(FLATUI_DARKPURPLE  , 142,  68, 173 ) /* wisteria */ \
    X(FLATUI_DARKNAVY    ,  44,  62,  80 ) /* midnightblue */ \
    X(FLATUI_YELLOW      , 241, 196,  15 ) /* sunflower */ \
    X(FLATUI_ORANGE      , 230, 126,  34 ) /* carrot */ \
    X(FLATUI_RED         , 231,  76,  60 ) /* alizarin */ \
    X(FLATUI_DARKYELLOW  , 243, 156,  18 ) /* orange */ \
    X(FLATUI_DARKORANGE  , 211,  84,   0 ) /* pumpkin */ \
    X(FLATUI_DARKRED     , 192,  57,  43 ) /* pomegranate */ \

#define FLATUI_PALETTE_FULL(X) \
    X(FLATUI_DARKWHITE   , 189, 195, 199 ) /* silver */ \
    X(FLATUI_DARKGRAY    , 127, 140, 141 ) /* asbestos */ \
    X(FLATUI_WHITE       , 236, 240, 241 ) /* clouds */ \
    X(FLATUI_GRAY        , 149, 165, 166 ) /* concrete */ \
    FLATUI_PALETTE(X)


#define COLOR_MACRO(NAME,R,G,B) ImVec4(R/255.f,G/255.f,B/255.f,255/255.f) ,
const ImVec4 editor_palette[] = {
    FLATUI_PALETTE(COLOR_MACRO)
    // AVA_PALETTE(COLOR_MACRO)
    // //    ARNE_PALETTE(COLOR_MACRO)
    // BASE16_PALETTE(COLOR_MACRO)
};
const int editor_palette_max = sizeof(editor_palette) / sizeof(0[editor_palette]);
