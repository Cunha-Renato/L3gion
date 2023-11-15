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

		bool isRunning() { return m_IsRunning; }

		static ref<Scene> copy(ref<Scene> other);

		Entity createEntity(const std::string& name = std::string());
		Entity createEntityWithUUID(UUID id, const std::string& name = std::string());
		void deleteEntity(Entity entity);

		void onRuntimeStart();
		void onRuntimeStop();
		void onSimulationStart();
		void onSimulationStop();

		Entity getEntityByUUID(UUID id);
		Entity getEntityByName(const std::string_view& name);
		Entity getPrimaryCameraEntity();

		void onUptdateRuntime(Timestep ts);
		void onUpdateSimulation(Timestep, EditorCamera& editorCamera);
		void onUptdateEditor(Timestep ts, EditorCamera& editorCamera);
		void onViewportResize(uint32_t width, uint32_t height);
	
		void refreshScripts();
		Entity duplicateEntity(Entity entity);
		Entity duplicateEntity(UUID entityID);

		template<typename... Components>
		auto getAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		void renderEditorScene(EditorCamera& camera);
		
		void initializePhysics();
		void stopPhysics();
		void updatePhysics(Timestep ts);
	
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		Renderer2D::QuadSpecs m_QuadSpecs;
		Renderer2D::CircleSpecs m_CircleSpecs;

		bool m_IsRunning = false;

		b2World* m_World = nullptr;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}