workspace "L3gion"
	architecture "x86_64"
	startproject "L3gion_Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "L3gion/vendor/GLFW/include"
IncludeDir["Glad"] = "L3gion/vendor/Glad/include"
IncludeDir["ImGui"] = "L3gion/vendor/imgui"
IncludeDir["glm"] = "L3gion/vendor/glm"
IncludeDir["stb_image"] = "L3gion/vendor/stb_image"
IncludeDir["EnTT"] = "L3gion/vendor/EnTT/include"

group "Dependencies"
	include "L3gion/vendor/GLFW"
	include "L3gion/vendor/Glad"
	include "L3gion/vendor/imgui"

group ""

project "L3gion"
	location "L3gion"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
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
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.EnTT}"
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
	cppdialect "C++20"
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
		"%{IncludeDir.glm}",
		"%{IncludeDir.EnTT}"
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

project "L3gion_Editor"
	location "L3gion_Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
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
		"%{IncludeDir.glm}",
		"%{IncludeDir.EnTT}"
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