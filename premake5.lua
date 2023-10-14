workspace "L3gion"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directiories relative to root folder (Solution)
IncludeDir = {}
IncludeDir["GLFW"] = "L3gion/vendor/GLFW/include"

LinkDir = {}
LinkDir["GLFW"] = ("L3gion/vendor/GLFW/bin/" .. outputdir .."/GLFW/GLFW.lib")

-- Including the premake file
include "L3gion/vendor/GLFW"

project "L3gion"
	location "L3gion"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "lgpch.h"
	pchsource "L3gion/src/lgpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs 
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib",
		"L3gion/vendor/GLFW/bin/" .. outputdir .."/GLFW/GLFW.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"LG_PLATFORM_WINDOWS",
			"LG_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "LG_DEBUG"
		staticruntime "off"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "LG_RELEASE"
		staticruntime "off"
		runtime "Release"
		optimize"On"

	filter "configurations:Dist"
		defines "LG_DIST"
		staticruntime "off"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"L3gion/vendor/spdlog/include",
		"L3gion/src"
	}

	links
	{
		"L3gion",
		("bin/" .. outputdir .. "/L3gion/L3gion.lib")
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"LG_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "LG_DEBUG"
		staticruntime "off"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "LG_RELEASE"
		staticruntime "off"
		runtime "Release"
		optimize"On"

	filter "configurations:Dist"
		defines "LG_DIST"
		staticruntime "off"
		runtime "Release"
		optimize "On"