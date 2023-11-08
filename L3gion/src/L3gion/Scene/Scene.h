#pragma once

#include "L3gion/Core/UUID.h"
#include "L3gion/Core/Timestep.h"
#include "L3gion/Renderer/Renderer2D.h"
#include "L3gion/Renderer/EditorCamera.h"
#include <entt.hpp>

class b2World;

namespace L3gion
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		static ref<Scene> copy(ref<Scene> other);

		Entity createEntity(const std::string& name = std::string());
		Entity createEntityWithUUID(UUID id, const std::string& name = std::string());
		void deleteEntity(Entity entity);

		void onRuntimeStart();
		void onRuntimeStop();

		Entity getPrimaryCameraEntity();

		void onUptdateRuntime(Timestep ts);
		void onUptdateEditor(Timestep ts, EditorCamera& editorCamera);
		void onViewportResize(uint32_t width, uint32_t height);
	
		void duplicateEntity(Entity entity);

		template<typename... Components>
		auto getAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		void updatePhysics(Timestep ts);
	
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		Renderer2D::QuadSpecs m_QuadSpecs;
		Renderer2D::CircleSpecs m_CircleSpecs;

		b2World* m_World = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}