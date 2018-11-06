require "ninja"

-- sln

solution "Project"
    location "../../_project"
    configurations {"debug", "debugopt", "release"}
    platforms { "x64" }

-- dlls

project "engine"
    language "C++"
    kind "SharedLib"
    location "../../_project"
    targetdir "../../_%{cfg.buildcfg}"
--  files {"../../#sdk/*.c", "../../#sdk/*.cpp", "../../#sdk/*.h", "../../#sdk/*.inl"}
--  files {"../../src/**.c", "../../src/**.cpp", "../../src/**.h", "../../src/**.inl"}
    files {"../../engine/engine.c"}
    includedirs {"../../engine/"}
    defines {"LINKAGE=EXPORT", "ENGINE_C"}
    filter "configurations:debug"
        symbols "On"
    filter "configurations:debugopt"
        symbols "On"
        optimize "On"
    filter "configurations:release"
        defines {"NDEBUG"}
        symbols "Off"
        optimize "On"

project "editor"
    language "C++"
    kind "SharedLib"
    location "../../_project"
    targetdir "../../_%{cfg.buildcfg}"
    files {
    "../../editor/**.h*",
    "../../editor/*.c*",
    "../../editor/3rd/@ocornut/*.cpp",
    "../../editor/3rd/@ocornut/examples/imgui_impl_glfw.cpp",
    "../../editor/3rd/@ocornut/examples/imgui_impl_opengl3.cpp",
    "../../editor/3rd/@ocornut/examples/libs/gl3w/GL/gl3w.c",
    "../../engine/engine.c",
    }
    includedirs {
    "../../engine/",
    "../../engine/app/3rd/",
    "../../editor/",
    "../../editor/imgui/",
    "../../editor/3rd/@ocornut/",
    "../../editor/3rd/@ocornut/examples/",
    "../../editor/3rd/@ocornut/examples/libs/gl3w/",
    "../../editor/3rd/@ocornut/examples/example_glfw_opengl3/",
    }
    -- links {"engine"}
    defines {"ENGINE_C"} -- "LINKAGE=STATIC",
    configuration "windows"
        links { "user32", "gdi32" }
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

project "game01"
    language "C++"
    kind "SharedLib"
    location "../../_project"
    targetdir "../../_%{cfg.buildcfg}"
    files { "../../games/01/**.c", "../../games/01/**.cpp", "../../games/01/**.cc", "../../games/01/**.cxx", "../../games/01/**.h", "../../games/01/**.hpp", "../../games/01/**.inl"}
    includedirs {"../../editor/", "../../engine/"}
    links {"engine"}
    defines {"LINKAGE=IMPORT"}
    filter "configurations:debug"
        symbols "On"
    filter "configurations:debugopt"
        symbols "On"
        optimize "On"
    filter "configurations:release"
        defines {"NDEBUG"}
        symbols "Off"
        optimize "On"

project "game02"
    language "C++"
    kind "SharedLib"
    location "../../_project"
    targetdir "../../_%{cfg.buildcfg}"
    files { "../../games/02/**.c", "../../games/02/**.cpp", "../../games/02/**.cc", "../../games/02/**.cxx", "../../games/02/**.h", "../../games/02/**.hpp", "../../games/02/**.inl"}
    includedirs {"../../editor/", "../../engine/"}
    links {"engine"}
    defines {"LINKAGE=IMPORT"}
    filter "configurations:debug"
        symbols "On"
    filter "configurations:debugopt"
        symbols "On"
        optimize "On"
    filter "configurations:release"
        defines {"NDEBUG"}
        symbols "Off"
        optimize "On"

project "game03"
    language "C++"
    kind "SharedLib"
    location "../../_project"
    targetdir "../../_%{cfg.buildcfg}"
    files { "../../games/03/**.c", "../../games/03/**.cpp", "../../games/03/**.cc", "../../games/03/**.cxx", "../../games/03/**.h", "../../games/03/**.hpp", "../../games/03/**.inl"}
    includedirs {"../../editor/", "../../engine/"}
    links {"engine"}
    defines {"LINKAGE=IMPORT"}
    filter "configurations:debug"
        symbols "On"
    filter "configurations:debugopt"
        symbols "On"
        optimize "On"
    filter "configurations:release"
        defines {"NDEBUG"}
        symbols "Off"
        optimize "On"

project "game04"
    language "C++"
    kind "SharedLib"
    location "../../_project"
    targetdir "../../_%{cfg.buildcfg}"
    files { "../../games/04/**.c", "../../games/04/**.cpp", "../../games/04/**.cc", "../../games/04/**.cxx", "../../games/04/**.h", "../../games/04/**.hpp", "../../games/04/**.inl"}
    includedirs {"../../editor/", "../../engine/"}
    links {"engine"}
    defines {"LINKAGE=IMPORT"}
    filter "configurations:debug"
        symbols "On"
    filter "configurations:debugopt"
        symbols "On"
        optimize "On"
    filter "configurations:release"
        defines {"NDEBUG"}
        symbols "Off"
        optimize "On"

-- exes

project "launch"
    language "C++"
    kind "ConsoleApp" --"WindowedApp"
    location "../../_project"
    targetdir "../../_%{cfg.buildcfg}"
    files {"../../editor/app/launch/**.c*", "../../editor/app/launch/**.h*" } -- "../../editor/app/launch/dialog.rc" }
    includedirs {"../../engine/"}
    links {"engine", "editor", "game01", "game02", "game03", "game04" }
    configuration "windows"
        links { "user32", "gdi32" }
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
