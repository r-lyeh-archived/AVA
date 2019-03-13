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

-- games

project "game01"
    language "C++"
    kind "WindowedApp" links {"engine"} defines {"LINKAGE=IMPORT"} -- kind "SharedLib" links {"engine"}
    files { "../../games/01/**.c", "../../games/01/**.cc", "../../games/01/**.cpp", "../../games/01/**.cxx", "../../games/01/**.h", "../../games/01/**.hh", "../../games/01/**.hpp", "../../games/01/**.inl"}
    includedirs {"../../engine/", "../../editor/"}

project "game02"
    language "C++"
    kind "WindowedApp" links {"engine"} defines {"LINKAGE=IMPORT"} -- kind "SharedLib" links {"engine"}
    files { "../../games/02/**.c", "../../games/02/**.cc", "../../games/02/**.cpp", "../../games/02/**.cxx", "../../games/02/**.h", "../../games/02/**.hh", "../../games/02/**.hpp", "../../games/02/**.inl"}
    includedirs {"../../engine/", "../../editor/"}

project "game03"
    language "C++"
    kind "WindowedApp" links {"engine"} defines {"LINKAGE=IMPORT"} -- kind "SharedLib" links {"engine"}
    files { "../../games/03/**.c", "../../games/03/**.cc", "../../games/03/**.cpp", "../../games/03/**.cxx", "../../games/03/**.h", "../../games/03/**.hh", "../../games/03/**.hpp", "../../games/03/**.inl"}
    includedirs {"../../engine/", "../../editor/"}

project "game04"
    language "C++"
    kind "WindowedApp" links {"engine"} defines {"LINKAGE=IMPORT"} -- kind "SharedLib" links {"engine"}
    files { "../../games/04/**.c", "../../games/04/**.cc", "../../games/04/**.cpp", "../../games/04/**.cxx", "../../games/04/**.h", "../../games/04/**.hh", "../../games/04/**.hpp", "../../games/04/**.inl"}
    includedirs {"../../engine/", "../../editor/"}

project "game05"
    language "C++"
    kind "WindowedApp" links {"engine"} defines {"LINKAGE=IMPORT"} -- kind "SharedLib" links {"engine"}
    files { "../../games/05b/05_sponza.c"}
    includedirs {"../../engine/", "../../editor/"}

-- copy a file from the objects directory to the target directory
-- postbuildcommands {
-- '{COPY} "%{cfg.targetdir}/../engine/3rd/SDL2.dll" "%{cfg.targetdir}"',
-- }
