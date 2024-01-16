workspace "KuchCraft"
    startproject "KuchCraft"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    architecture "x64"
    configurations 
    { 
        "Debug", 
        "Release" 
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
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    location "KuchCraft"
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{wks.location}/KuchCraft/src/**.h",
		"%{wks.location}/KuchCraft/src/**.cpp"
	}

    includedirs
    {
        "%{wks.location}/KuchCraft/src",
        "%{wks.location}/KuchCraft/vendor/glfw/include",
        "%{wks.location}/KuchCraft/vendor/Glad/include"
    }

    links
    {
        "GLFW",
        "Glad",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

    defines
    { 
        "GLFW_INCLUDE_NONE",
        "_CRT_SECURE_NO_WARNINGS"
     }

    filter "configurations:Debug"
	defines "KC_DEBUG"
	runtime "Debug"
	symbols "On"

	filter "configurations:Release"
	defines "KC_RELEASE"
	runtime "Release"
	optimize "on"
	symbols "Off"

