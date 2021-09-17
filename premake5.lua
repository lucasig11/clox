
local header_files = "include/*.h"
local source_files = "src/*.c"

workspace "clox"
	configurations { "Debug", "Release" }


project "clox"
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"

	files { header_files, source_files}
	
	prebuildcommands { "clang-format --style='{IndentWidth: 4}' -i "..source_files.." "..header_files} 

	filter "configurations:Debug"
		defines { "DEBUG_TRACE_EXECUTION" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
