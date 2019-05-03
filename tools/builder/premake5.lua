require "ninja"

-- workspace

workspace "project"
    configurations { "debugopt" } -- "debug", "release" }
    location "../../_project"
    targetdir "../../_%{cfg.buildcfg}"
    configuration "windows"
        platforms { "x64" }
        links { "user32", "gdi32", "../../engine/3rd/SDL2.lib" }
    configuration "linux"
        links { "pthread" }
    filter "configurations:debug"
        symbols "On"
    filter "configurations:debugopt"
        symbols "On"
        optimize "On"
    filter "configurations:release"
        defines {"NDEBUG"}
        symbols "Off"
        optimize "On"

-- dlls

project "engine"
    language "C++"
    kind "SharedLib"
    files {"../../engine/engine.c"}
    includedirs {"../../engine/"}
    defines {"LINKAGE=EXPORT", "ENGINE_C"}

-- exes

project "editor"
    language "C++"
    kind "WindowedApp" links {"engine"} defines {"LINKAGE=IMPORT"}
    files {
    "../../editor/**.h*",
    "../../editor/*.c*",
    "../../editor/3rd/@ocornut/*.cpp",
    "../../editor/3rd/@ocornut/examples/imgui_impl_sdl.cpp",
    "../../editor/3rd/@ocornut/examples/imgui_impl_opengl3.cpp",
    "../../editor/3rd/@ocornut/examples/libs/gl3w/GL/gl3w.c",
    "../../editor/3rd/@ocornut/misc/freetype/*.cpp",
    "../../editor/3rd/@vinniefalco/*freetype*.c",
    }
    includedirs {
    "../../engine/",
    "../../engine/3rd/SDL2/",
    "../../editor/",
    "../../editor/imgui/",
    "../../editor/3rd/@ocornut/",
    "../../editor/3rd/@ocornut/examples/",
    "../../editor/3rd/@ocornut/examples/libs/gl3w/",
    "../../editor/3rd/@ocornut/examples/example_sdl_opengl3/",
    }

project "launch"
    language "C++"
    kind "WindowedApp" -- "ConsoleApp"
    files {"../../editor/app/launch/**.c*", "../../editor/app/launch/**.h*" } -- "../../editor/app/launch/dialog.rc" }
    includedirs {"../../engine/"}
    links {"engine"}

-- demos

function demos(...)
    for _, name in ipairs({...}) do
        project (name)
            uuid (os.uuid(name))
            language "C++"
            kind "WindowedApp" -- "ConsoleApp"
            includedirs {"../../engine/"}
            links {"engine"} defines {"LINKAGE=IMPORT"} -- kind "SharedLib" links {"engine"}
            includedirs {"../../engine/", "../../editor/"}
            files {
                "../../games/demos/*"..name.."*.h*",
                "../../games/demos/*"..name.."*.c*",
                "../../games/demos/*"..name.."*.inl",
            }
        -- defaults()
    end
end

demos(
    "00-build","01-hello","02-ui","03-debugdraw","04-collision",
    "05-audio","06-triangle","07-mesh","08-postfx","09-frustum",
    "10-cubemap","11-pbr"
)

-- games

function games(...)
    for _, name in ipairs({...}) do
        project ("game-" .. name)
            uuid (os.uuid("game-" .. name))
            language "C++"
            kind "WindowedApp" -- "ConsoleApp"
            includedirs {"../../engine/"}
            links {"engine"} defines {"LINKAGE=IMPORT"} -- kind "SharedLib" links {"engine"}
            includedirs {"../../engine/", "../../editor/"}
            files {
                "../../games/"..name.."/**.h*",
                "../../games/"..name.."/**.c*",
                "../../games/"..name.."/**.inl",
            }
        -- defaults()
    end
end

games("untitled")
