workspace "clox"
	configurations { "debug", "release" }

project "clox"
	kind "ConsoleApp"
	language "C"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin/%{cfg.buildcfg}/obj"
	includedirs { "includes" }

	files { "src/**.c" }

	filter {"action:gmake*", "toolset:clang or toolset:gcc"}
		buildoptions {
			"-Wall", "-Wextra", "-Wno-unused-parameter", "-Wno-unused-label"
		}

	filter "configurations:debug"
		defines { "DEBUG" }
		symbols "On"
		optimize "Debug"

	filter "configurations:release"
		optimize "Speed"

