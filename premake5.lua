workspace "clox"
	configurations { "debug", "release" }

project "clox"
	kind "ConsoleApp"
	language "C"
	toolset "clang"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin/%{cfg.buildcfg}/obj"
	includedirs { "includes" }

	files { "src/**.c" }

	prebuildcommands { "clang-format --style='{IndentWidth: 4}' -i src/*.c includes/*.h"}

	buildoptions {
		"-Wall", "-Wextra", "-Werror", "-Wno-unused-parameters"
	}

	filter "configurations:debug"
		defines { "DEBUG_TRACE_EXECUTION" }
		buildoptions { "-Wno-unused-functions" }
		symbols "On"
		optimize "Debug"

	filter "configurations:release"
		optimize "Speed"

