project "L3gion_Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin_int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/L3gion/vendor/spdlog/include",
		"%{wks.location}/L3gion/src",
		"%{wks.location}/L3gion/vendor",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.EnTT}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"L3gion",
		"Comdlg32.lib"
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