workspace "KuchCraft"
    startproject "KuchCraft"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    architecture "x64"
    configurations 
    { 
        "Release",
        "Dist"
    }
    flags
    { 
        "MultiProcessorCompile" 
    }

group "Dependencies"
	include "KuchCraft/vendor/glfw"
    include "KuchCraft/vendor/Glad"
group ""

project "KuchCraft"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    location "KuchCraft"
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "kcpch.h"
    pchsource "%{wks.location}/KuchCraft/src/kcpch.cpp"

    files
    {
        "%{wks.location}/KuchCraft/src/**.h",
		"%{wks.location}/KuchCraft/src/**.cpp",
        "%{wks.location}/KuchCraft/vendor/glm/glm/**.hpp",
        "%{wks.location}/KuchCraft/vendor/glm/glm/**.inl",
        "%{wks.location}/KuchCraft/vendor/stb_image/**.h",
		"%{wks.location}/KuchCraft/vendor/stb_image/**.cpp",
        "%{wks.location}/KuchCraft/vendor/perlin_noise/**.hpp",
        "%{wks.location}/KuchCraft/vendor/magic_enum/**.hpp",
	}

    includedirs
    {
        "%{wks.location}/KuchCraft/src",
        "%{wks.location}/KuchCraft/vendor",
        "%{wks.location}/KuchCraft/vendor/glfw/include",
        "%{wks.location}/KuchCraft/vendor/Glad/include",
        "%{wks.location}/KuchCraft/vendor/glm",
        "%{wks.location}/KuchCraft/vendor/perlin_noise",
        "%{wks.location}/KuchCraft/vendor/stb_image",
        "%{wks.location}/KuchCraft/vendor/spdlog/include",
        "%{wks.location}/KuchCraft/vendor/freetype/include",
        "%{wks.location}/KuchCraft/vendor/freetype"
    }

    libdirs 
	{
		"%{wks.location}/KuchCraft/vendor/freetype/lib"
	}

    links
    {
        "GLFW",
        "Glad",
        "opengl32.lib",
        "freetype.lib"
    }

    filter "system:windows"
        systemversion "latest"
        links { "kernel32", "user32" }
        entrypoint "WinMainCRTStartup"

    defines
    { 
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS"
     }

	filter "configurations:Release"
	defines "KC_RELEASE"
	runtime "Release"
	optimize "on"
	symbols "On"

    filter "configurations:Dist"
	defines "KC_DIST"
	runtime "Release"
	optimize "on"
	symbols "Off"

