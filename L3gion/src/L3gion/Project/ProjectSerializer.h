#pragma once

#include "Project.h"

namespace L3gion
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer(ref<Project> project);

		bool serialize(const std::filesystem::path& path);
		bool deserialize(const std::filesystem::path& path);

	private:
		ref<Project> m_Context;
	};
}