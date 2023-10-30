#pragma once

#include "lgpch.h"
#include "Entity.h"

namespace L3gion
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene){}
}