#include "lgpch.h"
#include "ScriptGlue.h"

#include "L3gion/Scripting/ScriptEngine.h"
#include "L3gion/Core/Input.h"
#include "L3gion/Scene/Scene.h"

#include <mono/metadata/object.h>
#include <glm/glm.hpp>

namespace L3gion
{
	#define LG_ADD_INTERNAL_CALL(name) mono_add_internal_call("L3gion.InternalCalls::"#name, name);

	static void Entity_GetTranslation(UUID entityUUID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		Entity entity = scene->getEntityByUUID(entityUUID);
		
		*outTranslation = entity.getComponent<TransformComponent>().translation;
	}
	static void Entity_SetTranslation(UUID entityUUID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::getSceneContext();
		Entity entity = scene->getEntityByUUID(entityUUID);

		entity.getComponent<TransformComponent>().translation = *translation;
	}

	static bool Input_IskeyDown(LgKey keycode)
	{
		return Input::isKeyPressed(keycode);
	}

	void ScriptGlue::registerFunctions()
	{
		LG_ADD_INTERNAL_CALL(Entity_GetTranslation);
		LG_ADD_INTERNAL_CALL(Entity_SetTranslation);
		LG_ADD_INTERNAL_CALL(Input_IskeyDown);
	}

}