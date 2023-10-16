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

-- Include directiories relative to root folder (Solution)
IncludeDir = {}
IncludeDir["GLFW"] = "L3gion/vendor/GLFW/include"
IncludeDir["Glad"] = "L3gion/vendor/Glad/include"
IncludeDir["ImGui"] = "L3gion/vendor/ImGui"
IncludeDir["glm"] = "L3gion/vendor/glm"


-- Including the premake file
include "L3gion/vendor/GLFW/premake5.lua"
include "L3gion/vendor/Glad/premake5.lua"
include "L3gion/vendor/ImGui/premake5.lua"

project "L3gion"
	location "L3gion"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin_int/" .. outputdir .. "/%{prj.name}")

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
		-- "L3gion/vendor/GLFW/bin/" .. outputdir .."/GLFW/GLFW.lib",
		-- "L3gion/vendor/Glad/bin/" .. outputdir .."/Glad/Glad.lib",
		-- "L3gion/vendor/ImGui/bin/" .. outputdir .."/ImGui/ImGui.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"LG_PLATFORM_WINDOWS",
			"LG_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
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
	objdir ("bin_int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"L3gion/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"L3gion/vendor/spdlog/include"
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