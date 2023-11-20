#include "lgpch.h"
#include "Project.h"

#include "ProjectSerializer.h"

namespace L3gion
{
	ref<Project> Project::newProject()
	{
		s_ActiveProject = createRef<Project>();
		return s_ActiveProject;
	}
	ref<Project> Project::openProject(const std::filesystem::path& path)
	{
		auto project = createRef<Project>();
		
		ProjectSerializer serializer(project);
		if (serializer.deserialize(path))
		{
			s_ActiveProject = project;
			return s_ActiveProject;
		}

		return nullptr;
	}
	bool Project::saveProject(const std::filesystem::path& path)
	{
		auto& activePath = s_ActiveProject->getProjectSpecs().path;
		activePath = std::filesystem::relative(path.parent_path(), std::filesystem::current_path());
		ProjectSerializer serializer(s_ActiveProject);
		return serializer.serialize(path);
	}
}