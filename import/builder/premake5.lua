require "ninja"

-- sln

solution "Project"
	location "../../_project"
	configurations {"debug", "debugopt", "release"}
	platforms { "x64" } -- "native", "universal64" }

-- dlls

project "engine"
	kind "SharedLib"
	location "../../_project"
	language "C++"
	targetdir "../../_build/%{cfg.buildcfg}"

--	files {"../../#sdk/*.c", "../../#sdk/*.cpp", "../../#sdk/*.h", "../../#sdk/*.inl"}
--	files {"../../src/**.c", "../../src/**.cpp", "../../src/**.h", "../../src/**.inl"}
	files {"../../engine/engine.c"}
	includedirs {"../../engine/"}
	defines {"LINKAGE=EXPORT", "ENGINE_C"}

	filter "configurations:debug"
		symbols "On"

	filter "configurations:debugopt"
		symbols "On"
		optimize "On"

	filter "configurations:release"
		symbols "Off"
		optimize "On"

project "editor"
	kind "SharedLib"
	location "../../_project"
	language "C++"
	targetdir "../../_build/%{cfg.buildcfg}"
	--defines {"LINKAGE=IMPORT"}

	files {"../../editor/**.c*", "../../editor/**.h*"}
	includedirs {"../../editor/", "../../engine/"}
	links {"engine"}

	filter "configurations:debug"
		symbols "On"

	filter "configurations:debugopt"
		symbols "On"
		optimize "On"

	filter "configurations:release"
		defines {"NDEBUG", "release"}
		optimize "On"

project "game01"
	kind "SharedLib"
	location "../../_project"
	language "C++"
	targetdir "../../_build/%{cfg.buildcfg}"

	files { "../../games/01/**.c", "../../games/01/**.cpp", "../../games/01/**.cc", "../../games/01/**.cxx", "../../games/01/**.h", "../../games/01/**.hpp", "../../games/01/**.inl"}
	--removefiles { "../../app/**" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine"}
	defines {"LINKAGE=IMPORT"}

	filter "configurations:debug"
		symbols "On"

	filter "configurations:debugopt"
		symbols "On"
		optimize "On"

	filter "configurations:release"
		symbols "Off"
		optimize "On"

project "game02"
	kind "SharedLib"
	location "../../_project"
	language "C++"
	targetdir "../../_build/%{cfg.buildcfg}"

	files { "../../games/02/**.c", "../../games/02/**.cpp", "../../games/02/**.cc", "../../games/02/**.cxx", "../../games/02/**.h", "../../games/02/**.hpp", "../../games/02/**.inl"}
	--removefiles { "../../app/**" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine"}
	defines {"LINKAGE=IMPORT"}

	filter "configurations:debug"
		symbols "On"

	filter "configurations:debugopt"
		symbols "On"
		optimize "On"

	filter "configurations:release"
		symbols "Off"
		optimize "On"

project "game03"
	kind "SharedLib"
	location "../../_project"
	language "C++"
	targetdir "../../_build/%{cfg.buildcfg}"

	files { "../../games/03/**.c", "../../games/03/**.cpp", "../../games/03/**.cc", "../../games/03/**.cxx", "../../games/03/**.h", "../../games/03/**.hpp", "../../games/03/**.inl"}
	--removefiles { "../../app/**" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine"}
	defines {"LINKAGE=IMPORT"}

	filter "configurations:debug"
		symbols "On"

	filter "configurations:debugopt"
		symbols "On"
		optimize "On"

	filter "configurations:release"
		symbols "Off"
		optimize "On"

-- app

project "launch"
	kind "ConsoleApp" --"WindowedApp"
	location "../../_project"
	language "C++"
	targetdir "../../_build/%{cfg.buildcfg}"

	files {"../../editor/app/launch/**.c*", "../../editor/app/launch/**.h*" } -- "../../engine/app/launcher/dialog.rc" }
	includedirs {"../../editor/", "../../engine/"}
	links {"engine", "editor", "game01", "game02", "game03" }
	--defines {"LINKAGE=IMPORT"}

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
		symbols "Off"
		optimize "On"

--[[
	kind "WindowedApp"
	filter "system:windows"
		flags {"WinMain"}
--]]

