-- Delete existing solution files
    os.rmdir("solution")

workspace "ColorBlindPalettes"
    architecture "x64"
    configurations { "Debug", "Release" }
	 location "solution"

project "ColorBlindPalettes"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
    files { "src/**.cpp", "include/**.h" }
    includedirs { "include" }
    libdirs { "lib" }
    links { "glew32s.lib", "glfw3.lib", "opengl32", "gdi32" }
    defines { "GLEW_STATIC" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "on"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "on"