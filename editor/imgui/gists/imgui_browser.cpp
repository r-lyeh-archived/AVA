// tiny directory listing
// - rlyeh, public domain | wtrmrkrlyeh
#pragma once
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <dirent.h>
#endif

template<typename FN>
bool tinydir( const char *directory, const FN &yield ) {
    std::string src( directory );
    while( !src.empty() && (src.back() == '/' || src.back() == '\\') ) src.pop_back();
#ifdef _WIN32
    WIN32_FIND_DATAA fdata;
    for( HANDLE h = FindFirstFileA( (src + "/*").c_str(), &fdata ); h != INVALID_HANDLE_VALUE; ) {
        for( bool next = true; next; next = FindNextFileA( h, &fdata ) != 0 ) {
            if( fdata.cFileName[0] != '.' ) {
                yield( (src + "/" + fdata.cFileName).c_str(), (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0 );
            }
        }
        return FindClose( h ), true;
    }
#else
    for( DIR *dir = opendir( (src + "/").c_str() ); dir; ) {
        for( struct dirent *ep; ep = readdir( dir ); ) {
            if( ep->d_name[0] != '.' ) {
                DIR *tmp = opendir( ep->d_name );
                yield( (src + "/" + ep->d_name).c_str(), tmp ? (closedir( tmp ), 1) : 0 );
            }
        }
        return closedir( dir ), true;
    }
#endif
    return false;
}

#ifdef TINYDIR_MAIN
#include <stdio.h>
#include <functional>
void TINYDIR_MAIN() {
    std::function<void(const char *,bool)> callback = [&]( const char *name, bool is_dir ) {
        printf( "%5s %s\n", is_dir ? "<dir>" : "", name );
        //if( is_dir ) tinydir( name, callback ); // <-- uncomment for recursive listing
    };
    return tinydir( "./", callback );
}
#endif



//#include "browser/tinydir.cc"
/*
#include <functional>
int tinydir( const char *path, std::function<void (const char *fname,bool is_dir)> yield ) {
    yield(__FILE__, false);
}
*/

#include <vector>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <functional>
struct filedir {
    bool is_dir;
    std::string name;
    uint64_t    date;
    uint64_t    mode;
    std::string hint;
    uint64_t    size;
    uint64_t    owner;
    uint64_t    color;
};
std::vector<filedir> dir_cache[2];
int bs_viewmode = 0;
bool bs_refreshed = false;
bool bs_strmatch( const char *text, const char *pattern ) {
    if( *pattern=='\0' ) return !*text;
    if( *pattern=='*' )  return bs_strmatch(text, pattern+1) || (*text && bs_strmatch(text+1, pattern));
    if( *pattern=='?' )  return *text && (*text != '.') && bs_strmatch(text+1, pattern+1);
    return (*text == *pattern) && bs_strmatch(text+1, pattern+1);
}
int imgui_browser( char path[256] ) {
    auto update_path = [&]( const char *new_path ) {
        char temp[256];
        strcpy(temp, new_path);
        _fullpath(path, temp, 256);
    };

    if( !bs_refreshed ) {
        bs_refreshed = true;
        //update_path( path[0] ? path : "./" );

        int skip = strlen(path);
        std::function<void(const char *,bool)> callback = [&]( const char *name, bool is_dir ) {
            struct stat t = {0}; stat( name, &t ); 

            filedir f;
            f.is_dir = is_dir;
            f.name = std::string() + &name[skip] + (is_dir ? "/" : "");
            f.date = t.st_mtime; // file last modification date
            f.mode = t.st_mode;  // file type and mode
            f.size = t.st_size;  // file size
            f.owner = t.st_uid;  // user ID of file owner
            f.color = 0;
            const char *ext = strrchr(name, '.');
            while( ext && ext[0] ) {
                f.color = (f.color ^ ((uint8_t)*ext++)) * 131;
            }
            f.color %= editor_palette_max;

            char fdtemp[256];
            sprintf(fdtemp, 
                "Path: %s\n"
                "Type: %lld\n"
                "Size: %lld\n"
                "Owner: %lld\n"
                "Modified: (%lld) %s",
                name, (int64_t)t.st_mode, (int64_t)t.st_size, (int64_t)t.st_uid, (int64_t)t.st_mtime, ctime(&t.st_mtime) /*includes \n*/
            );
            f.hint = fdtemp;

            dir_cache[!is_dir].push_back( f );
        };
        dir_cache[0].clear(); dir_cache[0].push_back( filedir { true, "../" } );
        dir_cache[1].clear();
        tinydir( path, callback );
    }
    auto draw_entry = [&]( const filedir &f, bool is_dir ) -> int {
        ImGui::PushID(&f);
        ImGui::BeginGroup();

        auto lower = []( const char *buf ) -> std::string {
            std::string x = buf;
            for( auto &c : x ) if( c >= 'A' && c <= 'Z' ) c = c - 'A' + 'a';
            return x;
        };

        static std::map< std::string, const icon *> cache, *init = 0;
        if( !init ) {
            init = &cache;
            // array
            for( int i = 0; i < IM_ARRAYSIZE(suru); ++i ) {
                const char *ext = strrchr(suru[i].name, '-');
                cache[ lower( ext ? ext + 1 : suru[i].name).c_str() ] = &suru[i];
            }
            // aliases
            cache[ "ico" ] = cache["image"];
            cache[ "gif" ] = cache["image"];
            cache[ "png" ] = cache["image"];
            cache[ "pkm" ] = cache["image"];
            cache[ "dds" ] = cache["image"];
            cache[ "ktx" ] = cache["image"];
            cache[ "pvr" ] = cache["image"];
            cache[ "jpg" ] = cache["image"];
            cache[ "tga" ] = cache["image"];
            cache[ "bmp" ] = cache["image"];
            cache[ "mp3" ] = cache["audio"];
            cache[ "wav" ] = cache["audio"];
            cache[ "ogg" ] = cache["audio"];
            cache[ "xm"  ] = cache["audio"];
            cache[ "mod" ] = cache["audio"];
            cache[ "mid" ] = cache["audio"];
            cache[ "avi" ] = cache["video"];
            cache[ "mov" ] = cache["video"];
            cache[ "license" ] = cache["copying"];
            cache[ "authors" ] = cache["credits"];
            cache[ "bat" ] = cache["sh"];
            cache[ "htm" ] = cache["html"];
            cache[ "dll" ] = cache["bin"];
            cache[ "cc" ] = cache["cpp"];
            cache[ "hh" ] = cache["hpp"];
            cache[ "inl" ] = cache["cpp"];
        }
        auto find_thumbnail = [&]( const char *fname ) -> const icon * {
            const char *ext = strrchr(fname, '.');
            auto find = cache.find( lower( ext ? ext + 1 : fname).c_str() );
            if( find == cache.end() ) return NULL;
            return find->second;
        };

        ImTextureID thumbnail = (ImTextureID)(uintptr_t)(3); // 3: suru.png texture
        const auto *suru_icon = thumbnail ? find_thumbnail( f.name.c_str() ) : NULL;
        const auto *suru_blank_folder = &suru[79], *suru_blank_file = &suru[3];
        float factor = bs_viewmode == 0 ? 6 : bs_viewmode == 1 ? 2 : 1;
        if( thumbnail != NULL ) {
            if( !suru_icon ) suru_icon = is_dir ? suru_blank_folder : suru_blank_file;
        }

        auto iconify = [&]( float zoomout_factor ) {
            /****/ if( thumbnail != NULL ) {
                ImGui::Image( thumbnail, ImVec2(suru_icon->w/zoomout_factor, suru_icon->h/zoomout_factor),
                    ImVec2( suru_icon->u0, suru_icon->v0 ),
                    ImVec2( suru_icon->u1, suru_icon->v1 ),
                    suru_icon == suru_blank_file ? editor_palette[f.color] : ImVec4(1,1,1,1)
                );
            } else if( is_dir ) {
                ImGui::TextColored( ImVec4(255/255.f,240/255.f,90/255.f,255/255.f), ICON_MD_FOLDER );
            } else {
                ImGui::TextColored( editor_palette[f.color], ICON_MD_INSERT_DRIVE_FILE );
            }

            /****/ if( bs_viewmode == 0 ) {
                ImGui::SameLine(); ImGui::Text( f.name.c_str() );
            } else if( bs_viewmode >= 1 ) {
                ImGui::Text( f.name.c_str() );
            }
        };

        iconify(factor);

        ImGui::EndGroup();
        int clicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);

        if(ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            iconify(2);
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(f.hint.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        // contextual menu
        if (!ImGui::IsPopupOpen("##popup") && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("EDITOR__FILEDROP", &f, sizeof(filedir*));
            iconify(factor);
            ImGui::EndDragDropSource();
        }
        else
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) ImGui::OpenPopup("##popup");

        ImGui::PopID();

        return clicked;
    };


    if(ImGui::Button(ICON_MDI_EYE)) bs_viewmode = (bs_viewmode+1) % 3;

    ImGui::SameLine();

    static char computer_bufname[128], *computer_name = 0;
    if( !computer_name ) {
        computer_name = getenv("COMPUTERNAME") ? getenv("COMPUTERNAME") : "HOME";
        sprintf(computer_bufname, ICON_MDI_HOME " %s", computer_name );
    }
    static char drive_name[128], *drive_buffer = 0;
    if( !drive_buffer ) {
        sprintf(drive_buffer = drive_name, "%s", computer_bufname);
    }
    if( ImGui::Button(drive_name) ) ImGui::OpenPopup("id");
        if (ImGui::BeginPopupContextItem("id")) {
            // --- special folders
            if( ImGui::Selectable(computer_bufname) ) {
                ImGui::CloseCurrentPopup();
                strcpy(drive_name, computer_bufname);
                update_path("./");
                bs_refreshed = false;
            }
            // --- drives
            DWORD d = GetLogicalDrives();
            for( int i = 0; i < 26; i++ ) {
                if( !(d & (1<<i)) ) continue;

                bool is_virtual_drive = false;
                DWORD sectorsPerCluster;
                DWORD bytesPerSector;
                DWORD numberOfFreeClusters;
                DWORD totalNumberOfClusters;
                char Drive[] = "\\\\.\\F:"; Drive[4] = 'A' + i;
                is_virtual_drive = 0 == GetDiskFreeSpaceA(Drive, &sectorsPerCluster, &bytesPerSector,
                    &numberOfFreeClusters, &totalNumberOfClusters);

                char buf[16];
                sprintf(buf, ICON_MDI_FOLDER " %c:/", 'A' + i );
                if( ImGui::Selectable(buf) ) {
                    ImGui::CloseCurrentPopup();
                    strcpy(drive_name, buf);
                    update_path(&buf[4]);
                    bs_refreshed = false;
                    break;
                }
            }
            ImGui::EndPopup();
        }

    ImGui::SameLine();

    static char filterbuf[128] = {0}, filter[128+2];
    ImGui::Text(ICON_MDI_FILTER);
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##ICON_MDI_FILTER", filterbuf, sizeof(filterbuf)-1);
    ImGui::PopItemWidth();
    sprintf(filter, "*%s*", filterbuf);



    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(60/255.f,60/255.f,60/255.f,60/255.f));
        int columns = (int)(ImGui::GetContentRegionAvail().x / (bs_viewmode > 1 ? 128.0f : 96.0f));
        columns = columns < 1 || bs_viewmode == 0 ? 1 : columns;
        ImGui::Columns(columns, false);

        for( auto &f : dir_cache[0] ) {
            if( filter[2] && !bs_strmatch(f.name.c_str(), filter)) continue;
            if( draw_entry(f, 1) ) {
                bs_refreshed = false;
                strcat(path, f.name.c_str() /*MD icon*/ );
                update_path(path);
            }

            ImGui::NextColumn();
        }
        for( auto &f : dir_cache[1] ) {
            if( filter[2] && !bs_strmatch(f.name.c_str(), filter)) continue;
            draw_entry(f, 0);

            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    ImGui::PopStyleColor();

    if (ImGui::BeginPopup("##popup", ImGuiWindowFlags_None)) {
        if (ImGui::MenuItem("Open")) {
        }
        ImGui::EndPopup();
    }

    return 0;
}

void browser_demo() {
    static char path[2048] = "./";
    ImGui::Begin("Browser");
        if( imgui_browser( path ) ) {

        }
    ImGui::End();
}
