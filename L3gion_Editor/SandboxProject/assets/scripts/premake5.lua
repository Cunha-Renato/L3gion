local rootDir = "../../../.."

include (rootDir .."/vendor/premake/premake_custom/solution_items.lua")

workspace "Sandbox"
	architecture "x86_64"
	startproject "Sandbox"

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

project "Sandbox"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("Binaries")
	objdir ("Intermediates")

	files
	{
		"Source/**cs",
		"Properties/**cs",
	}

	links
	{
		"L3gion_ScriptCore"
	}

	filter "configurations:Debug"	
		optimize "Off"
		symbols "Default"
	
	filter "configurations:Release"	
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"	
		optimize "Full"
		symbols "Off"

group "L3gion"
	include (rootDir .."/L3gion_ScriptCore")
group ""
			
