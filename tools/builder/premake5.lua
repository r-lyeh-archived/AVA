require "ninja"

-- sln

solution "Project"
	location "../../.project"
	configurations {"debug", "release", "shipping"}
	platforms { "x64" } -- "native", "universal64" }

-- dlls

project "engine"
	kind "SharedLib"
	location "../../.project"
	language "C++"
	targetdir "../../.build/%{cfg.buildcfg}"

--	files {"../../#sdk/*.c", "../../#sdk/*.cpp", "../../#sdk/*.h", "../../#sdk/*.inl"}
--	files {"../../src/**.c", "../../src/**.cpp", "../../src/**.h", "../../src/**.inl"}
	files {"../../engine/ava.c"}
	includedirs {"../../engine/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "editor"
	kind "SharedLib"
	location "../../.project"
	language "C++"
	targetdir "../../.build/%{cfg.buildcfg}"

	files {"../../editor/**.c*", "../../editor/**.h*"}
	includedirs {"../../editor/", "../../engine/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "game"
	kind "SharedLib"
	location "../../.project"
	language "C++"
	targetdir "../../.build/%{cfg.buildcfg}"

	files { "../../game/**.c", "../../game/**.cpp", "../../game/**.cc", "../../game/**.cxx", "../../game/**.h", "../../game/**.hpp", "../../game/**.inl"}
	--removefiles { "../../app/**" }
	includedirs {"../../editor/", "../../engine/"}
	defines {"DLL_EXPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

-- app

project "launcher"
	kind "ConsoleApp" --"WindowedApp"
	location "../../.project"
	language "C++"
	targetdir "../../.build/%{cfg.buildcfg}"

	files {"../../engine/app/launcher/**.c*", "../../engine/app/launcher/**.h*" } -- "../../engine/app/launcher/dialog.rc" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine", "editor", "game" }

	configuration "windows"
		links { "user32", "gdi32" }

	configuration "linux"
		links { "pthread" }

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

--[[
	kind "WindowedApp"
	filter "system:windows"
		flags {"WinMain"}
--]]

