int show_about_window = 0;

void imgui_menubar() {
    changed.clear();

    if (ImGui::BeginMainMenuBar())
    {
        TOGGLE(ICON_MD_3D_ROTATION, )
            ON_CLICK POPUP("3D Options", {
                ImGui::Text("3D Options");
                ___
                CHECK("Show wireframe") ON_CLICK { set_render('wire', 1 - *render('wire') ); }
                CHECK("Show backfaces")
                CHECK("Show ground plane")
                CHECK("Show bones")
                CHECK("Show skeleton")
                CHECK("Show normals")
                CHECK("Show nodes")
                CHECK("Show shadows")
                CHECK("Show textures") ON_CLICK { set_render('texs', 1 - *render('texs') ); }
                CHECK("Show framebuffers")
                ___
                CHECK("Elevation")
                CHECK("Plan")
                CHECK("Perspective")
                CHECK("Dimetric");
                CHECK("Isometric");
                CHECK("Trimetric");
                CHECK("Oblique");
                CHECK("Cavalier");
                CHECK("Top-down");
                ___
                CHECK("Textureless")
                CHECK("Quantization p,r,s 6,1,2")
                CHECK("Stats fps");
                CHECK("Clear color")
                ___
                CHECK("120 fps")
                CHECK("60 fps")
                CHECK("30 fps")
                CHECK("VR")

                // Animation
                // "space - play/pause animation " + std::string(playing ? "(on)":"(off)"))
                // "[ and ] - change animation playback speed " + std::to_string(animfps))
                // ", and . - single step animation " + std::to_string(animtick))
                // "cursors,shift,z,x,c - anim controller (biped)")
            });

        THEN

        MENU("Editor") {
            CHECK("Work offline")
            CHECK("Configure network")
            ___
            CHECK("Admin users")
            CHECK("Configure local database")
            CHECK("Configure remote database")
            CHECK("Configure versioning system")
            ___
            ITEM2(ICON_MD_LAUNCH, "Instance")
            ___
            SUBMENU("Versioning", ICON_MD_EVENT_NOTE) {
                ITEM2(ICON_MD_EVENT_NOTE, "History")

                ITEM2(ICON_MD_TIMELAPSE, "Revision")
                    THEN BUTTON(ICON_MD_FAST_REWIND);
                    THEN BUTTON(ICON_MD_SKIP_PREVIOUS);
                    THEN BUTTON(ICON_MD_ARROW_DOWNWARD);
                    THEN BUTTON(ICON_MD_SKIP_NEXT);
                    THEN BUTTON(ICON_MD_FAST_FORWARD);

                ITEM2(ICON_MD_UNDO, "Rollback to this revision") // warn: 14 changes from revX (date) to revY (date) will be lost. Continue?
                ITEM2(ICON_MD_SEARCH, "Diff changes")
                ITEM2(ICON_MD_REFRESH, "Update") // warn: you will lose any changes. Continue?
                ITEM2(ICON_MD_CALL_MERGE, "Merge") // warn: cannot merge. keep mine or use theirs?
                ___
                ITEM2(ICON_MD_DONE, "Push")
                ITEM2(ICON_MD_DONE_ALL, "Commit")
                ITEM2(ICON_MD_HIGHLIGHT_OFF, "Revert")
                ENDSUBMENU
            }
            ___
            SUBMENU("Maintain database", ICON_MD_FILE_DOWNLOAD) {
                ITEM2(ICON_MD_FILE_DOWNLOAD, "Backup database")
                ITEM2(ICON_MD_FILE_UPLOAD, "Restore database")
                ___
                ITEM2(ICON_MD_ARCHIVE, "Compact database")
                ENDSUBMENU
            }
            ITEM2(ICON_MD_SHOW_CHART, "Metrics/Budget")
                ON_CLICK NOTIFY( ICON_MD_CANCEL "  THIS IS A NOTIFICATION!  " ICON_MD_WARNING );
            ___
            CHECK("Preferences")
            ___
            ITEM2(ICON_MD_COPYRIGHT, "Licenses")
            CHECK("About")
                ON_CLICK show_about_window = 1;
            ITEM2(ICON_MD_CANCEL, "Quit")
                ON_CLICK MODAL("Quit?")
            ENDMENU
        }

        MENU("Project") {
            ITEM2(ICON_MD_CREATE_NEW_FOLDER, "New project")
            ITEM2(ICON_MD_FOLDER_OPEN, "Load project")
            ITEM2(ICON_MD_CREATE, "Edit project")
            ITEM2(ICON_MD_CLOSE, "Close")
            ___
            CHECK("1.") THEN SUBMENU("Cook") {
                CHECK("Binary-data")
                CHECK("Apply optimizations")
                ___
                GROUP(1,1)
                ENDSUBMENU
            }

            CHECK("2.") THEN SUBMENU("Streams:") {
                SUBMENU("LZMA") {
                    GROUP(2,1)
                    ENDSUBMENU
                }
                SUBMENU("ZIP") {
                    GROUP(2,2)
                    ENDSUBMENU
                }
                SUBMENU("ZSTD") {
                    GROUP(2,3)
                    ENDSUBMENU
                }
                SUBMENU("LZ4") {
                    GROUP(2,4)
                    ENDSUBMENU
                }
                ENDSUBMENU
            }

            CHECK("3.") THEN SUBMENU("Compress:") {
                static const char *compressors[] = {"         ", "Seekable", "LZMA", "ZIP", "ZSTD", "LZ4"};
                static float widths[IM_ARRAYSIZE(compressors)] = {};
#if 1
                int stage = 2;
                ImGui::Columns(IM_ARRAYSIZE(compressors), NULL, 1);
                for( int i = 0; i <= IM_ARRAYSIZE(compressors); i++) ImGui::SetColumnOffset(i, i * 100);
                for (int i = 0; i < IM_ARRAYSIZE(compressors); i++) {
                    bool v = 0;
                    ImGui::Selectable(compressors[i], &v, ImGuiSelectableFlags_DontClosePopups);
                    if( ImGui::IsItemClicked(0) ) for(auto &x : groups[stage]) x = i;
                    ImGui::NextColumn();
                }
                for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                    ImGui::Text(names[i]);
                    ImGui::NextColumn();
                    for( int j = 1; j < IM_ARRAYSIZE(compressors); ++j) {
                        bool v = groups[stage][i] == j;
                        ImGui::Selectable( v ? ICON_MD_CHECK_BOX : ICON_MD_CHECK_BOX_OUTLINE_BLANK, &v, ImGuiSelectableFlags_DontClosePopups);
                        if( ImGui::IsItemClicked(0) ) groups[stage][i] = v ? /*(groups[i] & ~mask)*/ 0 : /*(groups[i] | mask)*/ j;
                        ImGui::NextColumn();
                    }
                }
                ImGui::Columns(1);
#else
                if( ImGui::BeginTable("compressors", compressors, widths, IM_ARRAYSIZE(compressors))) {
                    for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                        ImGui::Text(names[i]);
                        ImGui::NextColumn();
                        for( int j = 1; j < IM_ARRAYSIZE(compressors); ++j) {
                            int stage = 2;
                            bool v = groups[stage][i] == j;
                            ImGui::Selectable( v ? ICON_MD_CHECK : "             ", &v, ImGuiSelectableFlags_DontClosePopups);
                            if( ImGui::IsItemClicked(0) ) groups[stage][i] = v ? /*(groups[i] & ~mask)*/ 0 : /*(groups[i] | mask)*/ j;
                            ImGui::NextColumn();
                        }
                    }
                    ImGui::EndTable();
                }
#endif
                ENDSUBMENU
            }

            CHECK("4.") THEN SUBMENU("Package:") {
                CHECK("PAK")
                CHECK("ZIP/0")
                CHECK("TAR")
                CHECK("ISO")
                ENDSUBMENU
            }

            CHECK("5.") THEN SUBMENU("Deploy:") {
                CHECK("Local")
                CHECK("Internal")
                CHECK("Public")
                ENDSUBMENU
            }

            #   define ICON_MD_EXPAND_MOREICON_MD_EXPAND_LESS ICON_MD_EXPAND_LESS
            #   define ICON_MD_EXPAND_LESSICON_MD_EXPAND_MORE ICON_MD_EXPAND_MORE
            CHECK("6.") THEN SUBMENU("Publish:") { // THEN TOGGLE(ICON_MD_EXPAND_LESS,ICON_MD_EXPAND_MORE)
                ENDSUBMENU
            }

            ITEM2(ICON_MD_BUILD, "Build")
                ON_CLICK topbar_progress += 0.1f;

            SUBMENU("Warnings", ICON_MD_WARNING) {
                ENDSUBMENU
            }
            ___
            ITEM2(ICON_MD_PLAY_ARROW, "Play")
            ITEM2(ICON_MD_LAUNCH, "Launch")
            ITEM2(ICON_MD_LAUNCH, "Instance")
            ENDMENU
        }

        MENU("Level") {
            ENDMENU
        }

        MENU("Asset") {
            ITEM2(ICON_MD_CREATE_NEW_FOLDER, "Create placeholder")
            ITEM2(ICON_MD_FOLDER_SPECIAL,    "Import URL")
            ITEM2(ICON_MD_FOLDER_OPEN,       "Import file")
            ITEM2(ICON_MD_CLOSE, "Close")
            ___
            GRAY2(ICON_MD_SAVE, "Export as...")
            ___
            ITEM2(ICON_MD_SYSTEM_UPDATE, "Upgrade") HINT("(from original)")
            CHECKEX("Hot-reload", ICON_MD_SYNC, _DISABLED )
            ___
            MENU("Versioning", ICON_MD_EVENT_NOTE) {
                ITEM2(ICON_MD_EVENT_NOTE, "History")

                ITEM2(ICON_MD_TIMELAPSE, "Revision")
                    THEN BUTTON(ICON_MD_FAST_REWIND);
                    THEN BUTTON(ICON_MD_SKIP_PREVIOUS);
                    THEN BUTTON(ICON_MD_ARROW_DOWNWARD);
                    THEN BUTTON(ICON_MD_SKIP_NEXT);
                    THEN BUTTON(ICON_MD_FAST_FORWARD);

                ITEM2(ICON_MD_UNDO, "Rollback to this revision") // warn: 14 changes from revX (date) to revY (date) will be lost. Continue?
                ITEM2(ICON_MD_SEARCH, "Diff changes")
                ITEM2(ICON_MD_REFRESH, "Update") // warn: you will lose any changes. Continue?
                ITEM2(ICON_MD_CALL_MERGE, "Merge") // warn: cannot merge. keep mine or use theirs?
                ___
                ITEM2(ICON_MD_DONE, "Push")
                ITEM2(ICON_MD_DONE_ALL, "Commit")
                ITEM2(ICON_MD_HIGHLIGHT_OFF, "Revert")
                ENDMENU
            }
            ___
            ITEM2(ICON_MD_TEXT_FORMAT, "Rename")
            ITEM2(ICON_MD_REPEAT, "Replace")
            ITEM2(ICON_MD_DELETE, "Delete")
                ON_CLICK MODAL("Delete?")
            ___
            ITEM2(ICON_MD_SHOW_CHART, "Metrics/Budget")
            ITEM2(ICON_MD_COPYRIGHT, "Licenses")
            ___
            CHECKEX("Bookmark", ICON_MD_STAR, _BORDER);
            ENDMENU
        }

        MENU("Edit") {
            GRAY3(ICON_MD_UNDO, "Undo", "CTRL+Z")
            GRAY3(ICON_MD_REDO, "Redo", "CTRL+Y")
            ___
            ITEM3(ICON_MD_CONTENT_CUT, "Cut", "CTRL+X")
            ITEM3(ICON_MD_CONTENT_COPY, "Copy", "CTRL+C")
            ITEM3(ICON_MD_CONTENT_PASTE, "Paste", "CTRL+V")
            ___
            ITEM2(ICON_MD_CLASS, "Type")
            ITEM2(ICON_MD_TITLE, "Name")
            ITEM2(ICON_MD_FINGERPRINT, "ID")
            ___
            ITEM3(ICON_MD_VPN_KEY, "Key", "Value")
            ITEM3(ICON_MD_VPN_KEY, "Key", "Value")
            ENDMENU
        }

        MENU("Layers") {
            ITEM2(ICON_MD_CLEAR, "Clear defaults")
            ___
            ITEM2(ICON_MD_ADD, "Add layer")
            ___
            CHECKEX("Master layer", ICON_MD_VISIBILITY, _OFF)
            CHECKEX("Platform: PS4", ICON_MD_VISIBILITY, _OFF)
            CHECKEX("I18N: es-es, es-us, it, jp, en-uk, en-us", ICON_MD_G_TRANSLATE, )
            CHECK("Include preview")
            CHECK("Include thumbnail")
            ___
            CHECKEX("Lossless", ICON_MD_HD, )
            static float f0 = 33.33f;
            static float f1 = 100 - f0;
            if( ImGui::SliderFloat("Compression", &f0, 0, 100, "%5.2f%%") ) f1 = 100 - f0;
            if( ImGui::IsItemActive() )                                     f1 = 100 - f0;
            if( ImGui::SliderFloat("Speed",       &f1, 0, 100, "%5.2f%%") ) f0 = 100 - f1;
            if( ImGui::IsItemActive() )                                     f0 = 100 - f1;
            ENDMENU
        }

        MENU("Properties") {
            ENDMENU
        }

        MENU("Game") {
            ENDMENU
        }

        MENU("Events") {
            BUTTON("Play")
            BUTTON("Frame")
            ___
            BUTTON("Step")
            BUTTON("Draw")
            ___
            BUTTON("Load")
            BUTTON("Save")
            BUTTON("Tilt")
            ___
            BUTTON("Hibernate")
            BUTTON("Resume")
            BUTTON("Orientation")
            BUTTON("Resize")
            BUTTON("Low mem")
            BUTTON("Fatal")
            ENDMENU
        }

        //uint64_t tm = 0; //av_now(0);
        //int dow, TTTT, YY, MM, DD, hh = 21, mm = 15, ss, mss;
        //date_extract( &dow, &TTTT, &YY, &MM, &DD, &hh, &mm, &ss, &mss, tm );

if( !notifications.empty() ) {
    HIGHLIGHT(notifications.back().c_str())
        ON_CLICK notifications.pop_back();
    ImGui::EndMainMenuBar();
    return;
}

        enum { TOP_RIGHT_WIDTH = 320 - 32 };

        ImGui::Dummy( ImVec2(64,0) );
        ImGui::SameLine();



    if( topbar_progress ) {
        // align to right, reserve TOP_RIGHT_WIDTH pixels to the right
        int canceled = 0;
        TOGGLE( ICON_MD_CANCEL, ) ON_CLICK canceled = 1;
        THEN
        ImGui::ProgressBar( topbar_progress+=0.001, ImVec2(-TOP_RIGHT_WIDTH,0), "progress" );
        if( canceled ) topbar_progress = 0;
    } else {
        // search bar
    }

        // systray bar

auto systray = [&]() {
#define CONSOLE_PROMPT "Type command or search here..."
#if 0
        // Show filter bar
        {
        static char InputBuf[256] = ICON_MD_SEARCH "  ";
        // Command-line
        ImGui::PushItemWidth( -1 );
        if (ImGui::InputText("", InputBuf, sizeof(InputBuf)/sizeof(InputBuf[0]), ImGuiInputTextFlags_EnterReturnsTrue /*|ImGuiInputTextFlags_AutoSelectAll*/ ))
        {
            char* input_end = InputBuf+strlen(InputBuf);
            while (input_end > InputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;
            puts(InputBuf);
            strcpy(InputBuf, ICON_MD_SEARCH "  ");
        }
        HINT(CONSOLE_PROMPT)
        /* Demonstrate keeping auto focus on the input box
        if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget */
        ImGui::PopItemWidth();
        }

#else

        // search/console bar
        {
        #define DEFAULT_PROMPT ""
        static char cmd[256] = DEFAULT_PROMPT;
        TOGGLE(ICON_MD_SEARCH,);
        THEN
        ImGui::PushItemWidth( -TOP_RIGHT_WIDTH );
        if( ImGui::InputText("", cmd, sizeof(cmd)/sizeof(cmd[0]), ImGuiInputTextFlags_EnterReturnsTrue )) {
            char* input_end = cmd+strlen(cmd);
            while (input_end > cmd && input_end[-1] == ' ') input_end--; *input_end = 0;
            bool cmd_changed = cmd[0] && strcmp(cmd, DEFAULT_PROMPT);
            if( cmd_changed ) {
                //apprompt(cmd);
            }
            strcpy(cmd, DEFAULT_PROMPT);
        }
        HINT(CONSOLE_PROMPT)
        // Demonstrate keeping auto focus on the input box
        if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget */
        ImGui::PopItemWidth();
        }
#endif

        // systray
        const char *icons[] = {
            ICON_MD_VIEW_QUILT, ICON_MD_VIEW_QUILT,
            ICON_MD_FULLSCREEN, ICON_MD_FULLSCREEN_EXIT,
            ICON_MD_FIBER_MANUAL_RECORD, ICON_MD_FIBER_SMART_RECORD,
            ICON_MD_VIDEOCAM, ICON_MD_VIDEOCAM_OFF,
            ICON_MD_PHOTO_CAMERA, ICON_MD_CAMERA,
            ICON_MD_POWER, ICON_MD_BATTERY_FULL,
            ICON_MD_NOTIFICATIONS, ICON_MD_NOTIFICATIONS_OFF, // status emoji { do not disturb } || gray -> server off
            ICON_MD_FACE, ICON_MD_PERSON, 
            ICON_MD_CHAT_BUBBLE, ICON_MD_CHAT_BUBBLE, 
            0
        };
        static bool icon_states[] = {
            1, //ICON_MD_VIEW_QUILT, ICON_MD_VIEW_QUILT,
            1, //ICON_MD_FULLSCREEN, ICON_MD_FULLSCREEN_EXIT,
            1, //ICON_MD_FIBER_MANUAL_RECORD, ICON_MD_FIBER_SMART_RECORD,
            1, //ICON_MD_VIDEOCAM, ICON_MD_VIDEOCAM_OFF,
            1, //ICON_MD_PHOTO_CAMERA, ICON_MD_CAMERA,
            1, //ICON_MD_POWER, ICON_MD_BATTERY_FULL,
            1, //ICON_MD_NOTIFICATIONS, ICON_MD_NOTIFICATIONS_OFF, // status emoji { do not disturb } || gray -> server off
            1, //ICON_MD_FACE, ICON_MD_PERSON, 
            1, //ICON_MD_CHAT_BUBBLE, ICON_MD_CHAT_BUBBLE, 
        };
        for( int i = 0; icons[i]; i += 2 ) {
            const char *on = icons[i];
            const char *off = icons[i+1];
            icon_states[i] ^= ImGui::MenuItem( icon_states[i] ? on : off );
        }

        THEN ImGui::Text( " 00:00 " );
};

systray();


        if(0)
        if (ImGui::BeginMenu("Monitor")) {
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    {
        bool open = !!modal;
        if( open ) ImGui::OpenPopup(modal);
        ON_MODAL("Quit?", "All those moments will be lost in time, like tears in rain.", "Yes", "No", []{}, []{} )
        ON_MODAL("Delete?", "All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n", "Yes", "No", []{}, []{})
        //if( open ) ImGui::EndPopup();
    }
}
