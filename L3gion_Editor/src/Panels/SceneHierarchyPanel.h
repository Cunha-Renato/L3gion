#pragma once

#include "L3gion/Core/Core.h"
#include "L3gion/Core/Log.h"
#include "L3gion/Scene/Scene.h"
#include "L3gion/Scene/Entity.h"

namespace L3gion
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const ref<Scene>& scene);

		void setContext(const ref<Scene>& scene);

		void onImGuiRender();

		void setSelectedEntity(const Entity& entity) { m_SelectionContext = entity; }
		Entity getSelectedEntity() const { return m_SelectionContext; }

	private:
		void drawEntityNode(Entity entity);
		void drawComponents(Entity entity);

	private:
		ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}