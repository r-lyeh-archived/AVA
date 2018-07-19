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
	defines {"LINKAGE=EXPORT", "AVA_C"}

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
	--defines {"LINKAGE=IMPORT"}

	files {"../../editor/**.c*", "../../editor/**.h*"}
	includedirs {"../../editor/", "../../engine/"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "game01"
	kind "SharedLib"
	location "../../.project"
	language "C++"
	targetdir "../../.build/%{cfg.buildcfg}"

	files { "../../game/01/**.c", "../../game/01/**.cpp", "../../game/01/**.cc", "../../game/01/**.cxx", "../../game/01/**.h", "../../game/01/**.hpp", "../../game/01/**.inl"}
	--removefiles { "../../app/**" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine"}
	defines {"LINKAGE=IMPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "game02"
	kind "SharedLib"
	location "../../.project"
	language "C++"
	targetdir "../../.build/%{cfg.buildcfg}"

	files { "../../game/02/**.c", "../../game/02/**.cpp", "../../game/02/**.cc", "../../game/02/**.cxx", "../../game/02/**.h", "../../game/02/**.hpp", "../../game/02/**.inl"}
	--removefiles { "../../app/**" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine"}
	defines {"LINKAGE=IMPORT"}

	filter "configurations:debug"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:release"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:shipping"
		defines {"NDEBUG", "SHIPPING"}
		optimize "On"

project "game03"
	kind "SharedLib"
	location "../../.project"
	language "C++"
	targetdir "../../.build/%{cfg.buildcfg}"

	files { "../../game/03/**.c", "../../game/03/**.cpp", "../../game/03/**.cc", "../../game/03/**.cxx", "../../game/03/**.h", "../../game/03/**.hpp", "../../game/03/**.inl"}
	--removefiles { "../../app/**" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine"}
	defines {"LINKAGE=IMPORT"}

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

project "launch"
	kind "ConsoleApp" --"WindowedApp"
	location "../../.project"
	language "C++"
	targetdir "../../.build/%{cfg.buildcfg}"

	files {"../../engine/app/launch/**.c*", "../../engine/app/launch/**.h*" } -- "../../engine/app/launcher/dialog.rc" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine", "editor", "game01", "game02", "game03" }
	--defines {"LINKAGE=IMPORT"}

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

