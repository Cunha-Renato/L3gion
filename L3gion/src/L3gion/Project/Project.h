#pragma once

#include "L3gion/Core/Core.h"

#include <string>
#include <filesystem>

namespace L3gion
{
	struct ProjectSpecs
	{
		std::string name = "Untitled";

		std::filesystem::path path;
		std::filesystem::path assetsDir = "assets";
		std::filesystem::path scriptModulePath;
	};

	class Project
	{
	public:
		static ref<Project> newProject();
		static ref<Project> openProject(const std::filesystem::path& path);
		static bool saveProject(const std::filesystem::path& path);

		ProjectSpecs& getProjectSpecs()
		{
			return m_Specs;
		}

		static std::filesystem::path getAssetsDir()
		{
			LG_CORE_ASSERT(s_ActiveProject, "");
			return s_ActiveProject->m_Specs.assetsDir;
		}
		static std::filesystem::path getRootDir()
		{
			LG_CORE_ASSERT(s_ActiveProject, "");
			return s_ActiveProject->m_Specs.path;
		}
		static std::filesystem::path getScriptModulePath()
		{
			LG_CORE_ASSERT(s_ActiveProject, "");
			return s_ActiveProject->m_Specs.scriptModulePath;
		}
		static ref<Project> getActive() { return s_ActiveProject; }

	private:
		ProjectSpecs m_Specs;

		inline static ref<Project> s_ActiveProject = nullptr;
	};
}

