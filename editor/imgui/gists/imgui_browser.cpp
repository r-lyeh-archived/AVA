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
bool needs_refresh = true;
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
int imgui_browser( char path[256] ) {
    if( needs_refresh ) {
        needs_refresh = false;

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
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(60/255.f,60/255.f,60/255.f,60/255.f));
    for( auto &f : dir_cache[0] ) {
        ImGui::TextColored( ImVec4(255/255.f,240/255.f,90/255.f,255/255.f), ICON_MD_FOLDER );
        ImGui::SameLine();
        if( ImGui::MenuItem( f.name.c_str() ) ) {
            needs_refresh = true;
            strcat(path, f.name.c_str() /*MD icon*/ );
        }
        HINT( f.hint.c_str() )
    }
    for( auto &f : dir_cache[1] ) {
        ImGui::TextColored( editor_palette[f.color], ICON_MD_INSERT_DRIVE_FILE );
        ImGui::SameLine();
        if( ImGui::MenuItem( f.name.c_str() ) ) {
        }
        HINT( f.hint.c_str() )
    }

    ImGui::PopStyleColor();
    return 0;
}

void browser_demo() {
    static char path[256] = "./";
    ImGui::Begin("Browser");
        if( imgui_browser( path ) ) {

        }
    ImGui::End();
}
