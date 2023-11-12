#pragma once

#include "Scene.h"
#include "L3gion/Scene/Components.h"

#include <entt.hpp>

namespace L3gion
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
		~Entity() = default;

		template<typename Component, typename... Args>
		Component& addComponent(Args&&... args)
		{
			LG_CORE_ASSERT(!hasComponent<Component>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<Component>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename Component, typename... Args>
		Component& addOrReplaceComponent(Args&&... args)
		{
			return m_Scene->m_Registry.emplace_or_replace<Component>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename Component>
		Component& getComponent()
		{
			LG_CORE_ASSERT(hasComponent<Component>(), "Entity doesn't have the component!");
			return m_Scene->m_Registry.get<Component>(m_EntityHandle);
		}
		
		template<typename Component>
		void removeComponent()
		{
			LG_CORE_ASSERT(hasComponent<Component>(), "Entity doesn't have the component!");
			m_Scene->m_Registry.remove<Component>(m_EntityHandle);
		}

		template<typename Component>
		bool hasComponent()
		{
			return m_Scene->m_Registry.any_of<Component>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}
		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		UUID getUUID() { return getComponent<IDComponent>().id; }
		const std::string& getName() { return getComponent<TagComponent>().tag; }
	private:
		entt::entity m_EntityHandle = { entt::null };
		Scene* m_Scene = nullptr;
	};
}