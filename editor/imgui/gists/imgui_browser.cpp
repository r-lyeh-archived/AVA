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
    if( !bs_refreshed ) {
        bs_refreshed = true;

        char temp[256];
        strcpy(temp, path[0] ? path : "./");
        _fullpath(path, temp, 256);

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
                f.color ^= ((uint8_t)*ext++) * 131;
            }
            f.color %= editor_palette_max;

            char fdtemp[256];
            sprintf(fdtemp, 
                "Name: %s\n"
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

        ImTextureID thumbnail = (ImTextureID)(uintptr_t)0; // find_thumbnail(f.name);
        int thumb_size = bs_viewmode == 1 ? 64 : 128;
        ImVec2 sz(thumb_size - 20, thumb_size - 16);

        auto iconify = [&]() {
            if( thumbnail != NULL )
                ImGui::ImageButton( thumbnail, sz );
            else if( is_dir )
                ImGui::TextColored( ImVec4(255/255.f,240/255.f,90/255.f,255/255.f), ICON_MD_FOLDER );
            else
                ImGui::TextColored( editor_palette[f.color], ICON_MD_INSERT_DRIVE_FILE );

            if( bs_viewmode <= 1 ) {
                ImGui::SameLine();
            }
            ImGui::Text( f.name.c_str() );
        };

        iconify();
        int clicked = ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0);

        HINT( f.hint.c_str() )

        // contextual menu
        if (!ImGui::IsPopupOpen("##popup") && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("EDITOR__FILEDROP", &f, sizeof(filedir*));
            iconify();
            ImGui::EndDragDropSource();
        }
        else
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) ImGui::OpenPopup("##popup");

        ImGui::EndGroup();
        ImGui::PopID();

        return clicked;
    };

    static char computer_bufname[128], *computer_name = 0;
    if( !computer_name ) {
        computer_name = getenv("COMPUTERNAME") ? getenv("COMPUTERNAME") : "PC";
        sprintf(computer_bufname, ICON_MDI_HOME " %s", computer_name );
    }

    ImGui::Button(computer_bufname);
    ImGui::SameLine();
    ImGui::Button(ICON_MDI_FOLDER " C:");
    ImGui::SameLine();

    if(ImGui::Button(ICON_MDI_EYE)) bs_viewmode = (bs_viewmode+1) % 3;
    ImGui::SameLine();
    static char filterbuf[128] = {0}, filter[128+2];
    ImGui::InputText(ICON_MDI_FILTER, filterbuf, sizeof(filterbuf)-1);
    sprintf(filter, "*%s*", filterbuf);

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(60/255.f,60/255.f,60/255.f,60/255.f));
        float itemSize = bs_viewmode > 1 ? 48 : 128;
        int columns = (int)(ImGui::GetContentRegionAvail().x / (bs_viewmode > 1 ? 256.0f : 128.0f));
        columns = columns < 1 || bs_viewmode == 0 ? 1 : columns;
        ImGui::Columns(columns, false);

        for( auto &f : dir_cache[0] ) {
            if( filter[2] && !bs_strmatch(f.name.c_str(), filter)) continue;
            if( draw_entry(f, 1) ) {
                bs_refreshed = false;
                strcat(path, f.name.c_str() /*MD icon*/ );
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
    static char path[256] = "./";
    ImGui::Begin("Browser");
        if( imgui_browser( path ) ) {

        }
    ImGui::End();
}
