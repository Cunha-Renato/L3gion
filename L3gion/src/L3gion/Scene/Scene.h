#pragma once

#include "L3gion/Core/Timestep.h"
#include "L3gion/Renderer/Renderer2D.h"
#include <entt.hpp>

namespace L3gion
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = std::string());
		void deleteEntity(Entity entity);

		Entity getPrimaryCameraEntity();

		void onUptdate(Timestep ts);
		void onViewportResize(uint32_t width, uint32_t height);

	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		Renderer2D::QuadSpecs m_QuadSpecs;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}