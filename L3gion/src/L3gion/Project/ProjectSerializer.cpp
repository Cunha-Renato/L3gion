#include "lgpch.h"
#include "ProjectSerializer.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace L3gion
{
	ProjectSerializer::ProjectSerializer(ref<Project> project)
		: m_Context(project){}

	bool ProjectSerializer::serialize(const std::filesystem::path& path)
	{
		const auto& specs = m_Context->getProjectSpecs();

		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Project" << YAML::Value;
			out << YAML::BeginMap;
			{
				out << YAML::Key << "name" << YAML::Value << specs.name;
				out << YAML::Key << "path" << YAML::Value << specs.path.string();
				out << YAML::Key << "assetsDir" << YAML::Value << specs.assetsDir.string();
				out << YAML::Key << "scriptModulePath" << YAML::Value << specs.scriptModulePath.string();
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();

		return true;
	}
	bool ProjectSerializer::deserialize(const std::filesystem::path& path)
	{
		auto& specs = m_Context->getProjectSpecs();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e)
		{
			LG_CORE_ERROR("Failed to load .lgproj file '{0}'\n     {1}", path.string(), e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		specs.name = projectNode["name"].as<std::string>();
		specs.path= projectNode["path"].as<std::string>();
		specs.assetsDir = projectNode["assetsDir"].as<std::string>();
		specs.scriptModulePath= projectNode["scriptModulePath"].as<std::string>();

		return true;
	}
}