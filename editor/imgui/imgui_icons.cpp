#pragma once

#define UNSORTED_ICONS(X) \
    X(ICON_MD_SAVE) \
    X(ICON_MD_UNDO) \
    X(ICON_MD_REDO) \
    X(ICON_MD_SEND) \
    X(ICON_MD_DONE) \
    X(ICON_MD_DONE_ALL) \
    X(ICON_MD_PAUSE) \
    X(ICON_MD_PLAY_ARROW) \
    X(ICON_MD_BRIGHTNESS_5) \
    X(ICON_MD_MENU) \
    X(ICON_MD_FORWARD) \
    X(ICON_MD_ADD_CIRCLE) \
    X(ICON_MD_LIBRARY_ADD) \
    X(ICON_MD_ATTACH_FILE) \
    X(ICON_MD_REMOVE_RED_EYE) \
    X(ICON_MD_VIEW_LIST) \
    X(ICON_MD_SORT) \
    X(ICON_MD_SETTINGS_INPUT_SVIDEO) \
    X(ICON_MD_SETTINGS_INPUT_COMPONENT) \
    X(ICON_MD_BLUR_ON) \
    X(ICON_MD_HELP_OUTLINE) \
    X(ICON_MD_SEARCH) \
    X(ICON_MD_3D_ROTATION) \
    X(ICON_MD_LAUNCH) \
    X(ICON_MD_EVENT_NOTE) \
    X(ICON_MD_TIMELAPSE) \
    X(ICON_MD_FAST_REWIND) \
    X(ICON_MD_SKIP_PREVIOUS) \
    X(ICON_MD_ARROW_DOWNWARD) \
    X(ICON_MD_SKIP_NEXT) \
    X(ICON_MD_FAST_FORWARD) \
    X(ICON_MD_REFRESH) \
    X(ICON_MD_CALL_MERGE) \
    X(ICON_MD_HIGHLIGHT_OFF) \
    X(ICON_MD_FILE_DOWNLOAD) \
    X(ICON_MD_FILE_UPLOAD) \
    X(ICON_MD_ARCHIVE) \
    X(ICON_MD_SHOW_CHART) \
    X(ICON_MD_CANCEL) \
    X(ICON_MD_WARNING) \
    X(ICON_MD_COPYRIGHT) \
    X(ICON_MD_CREATE) \
    X(ICON_MD_CHECK_BOX) \
    X(ICON_MD_CHECK_BOX_OUTLINE_BLANK) \
    X(ICON_MD_CHECK) \
    X(ICON_MD_EXPAND_MORE) \
    X(ICON_MD_EXPAND_LESS) \
    X(ICON_MD_BUILD) \
    X(ICON_MD_CREATE_NEW_FOLDER) \
    X(ICON_MD_FOLDER_OPEN) \
    X(ICON_MD_FOLDER) \
    X(ICON_MD_FOLDER_SPECIAL) \
    X(ICON_MD_SYSTEM_UPDATE) \
    X(ICON_MD_SYNC) \
    X(ICON_MD_TEXT_FORMAT) \
    X(ICON_MD_REPEAT) \
    X(ICON_MD_DELETE) \
    X(ICON_MD_STAR) \
    X(ICON_MD_CONTENT_CUT) \
    X(ICON_MD_CONTENT_COPY) \
    X(ICON_MD_CONTENT_PASTE) \
    X(ICON_MD_CLASS) \
    X(ICON_MD_TITLE) \
    X(ICON_MD_FINGERPRINT) \
    X(ICON_MD_VPN_KEY) \
    X(ICON_MD_CLEAR) \
    X(ICON_MD_ADD) \
    X(ICON_MD_VISIBILITY) \
    X(ICON_MD_G_TRANSLATE) \
    X(ICON_MD_HD)

#define ICONS_GLYPHS(i) i ,
const char *unsorted_icons[] = {
    UNSORTED_ICONS(ICONS_GLYPHS) 0
};

#define ICONS_LABELS(i) #i ,
const char *unsorted_icons_str[] = {
    UNSORTED_ICONS(ICONS_LABELS) 0
};

void imgui_icons() {
    ImGui::Begin("icon viewer");
    for( int i = 0; unsorted_icons[i]; ++i ) {
        if( (i) % 19 ) THEN
         ImGui::SmallButton( unsorted_icons[i] );
         HINT( unsorted_icons_str[i] );
    }
    ImGui::End();
}
