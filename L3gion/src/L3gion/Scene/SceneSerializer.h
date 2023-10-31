#pragma once

#include "Scene.h"

namespace L3gion
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const ref<Scene>& scene);

		void serialize(const std::string& filepath);
		void serializeBinary(const std::string& filepath);

		bool deserialize(const std::string& filepath);
		bool deserializeBinary(const std::string& filepath);

	private:
		ref<Scene> m_Scene;
	};
}