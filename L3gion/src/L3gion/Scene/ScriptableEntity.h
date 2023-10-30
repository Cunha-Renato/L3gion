#pragma once

#include "L3gion/Scene/Entity.h"

namespace L3gion
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;

		template<typename T>
		T& getComponent()
		{
			return m_Entity.getComponent<T>();
		}

	protected:
		virtual void onCreate() {}
		virtual void onDestroy() {}
		virtual void onUpdate(Timestep ts) {}

	private:
		L3gion::Entity m_Entity;
		friend class Scene;
	};
}