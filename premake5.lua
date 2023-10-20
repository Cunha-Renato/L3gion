workspace "L3gion"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "L3gion/vendor/GLFW/include"
IncludeDir["Glad"] = "L3gion/vendor/Glad/include"
IncludeDir["ImGui"] = "L3gion/vendor/imgui"
IncludeDir["glm"] = "L3gion/vendor/glm"

group "Dependencies"
	include "L3gion/vendor/GLFW"
	include "L3gion/vendor/Glad"
	include "L3gion/vendor/imgui"

group ""

project "L3gion"
	location "L3gion"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin_int/" .. outputdir .. "/%{prj.name}")

	pchheader "lgpch.h"
	pchsource "L3gion/src/lgpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"LG_PLATFORM_WINDOWS",
			"LG_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "LG_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LG_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LG_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin_int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"L3gion/vendor/spdlog/include",
		"L3gion/src",
		"L3gion/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"L3gion"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"LG_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "LG_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LG_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LG_DIST"
		runtime "Release"
		optimize "on"