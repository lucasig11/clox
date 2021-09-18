workspace "clox"
	configurations { "debug", "release" }

project "clox"
	kind "ConsoleApp"
	language "C"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin/%{cfg.buildcfg}/obj"

	includedirs { "includes" }

	files { "src/**.c" }

	prebuildcommands { "clang-format --style='{IndentWidth: 4}' -i src/*.c includes/*.h"}

	filter "configurations:debug"
		defines { "DEBUG_TRACE_EXECUTION" }
		symbols "On"

	filter "configurations:release"
		optimize "On"
