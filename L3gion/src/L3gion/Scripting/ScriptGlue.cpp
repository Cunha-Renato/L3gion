#include "lgpch.h"
#include "ScriptGlue.h"
#include "L3gion/Scripting/ScriptEngine.h"

#include "L3gion/Core/Input.h"
#include "L3gion/Scene/Scene.h"

#include <glm/glm.hpp>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <box2d/b2_body.h>

namespace L3gion
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFunctions;

	#define LG_ADD_INTERNAL_CALL(name) mono_add_internal_call("L3gion.InternalCalls::"#name, name);

// ENTITY_COMPONENT
	static UUID Entity_CreateEntity()
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");

		auto entity = scene->createEntity("New Entity");
		return entity.getUUID();
	}
	static void Entity_DuplicateEntity(UUID entityUUID, glm::vec3* worldPosition)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		entity = scene->duplicateEntity(entity);
		LG_CORE_ASSERT(entity, "Entity is NULL!");
		entity.getUUID();
		ScriptEngine::onCreateEntity(entity);
	}
	
	static MonoObject* GetScriptInstance(UUID entityUUID)
	{
		return ScriptEngine::getManagedInstance(entityUUID);
	}
	
	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "");
		Entity entity = scene->getEntityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;

		return entity.getUUID();
	}
	static bool Entity_HasComponent(UUID entityUUID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		LG_CORE_ASSERT(s_EntityHasComponentFunctions.find(managedType) != s_EntityHasComponentFunctions.end(), "Couldn't find the Component!");
		return s_EntityHasComponentFunctions.at(managedType)(entity);
	}
	
// TRANSFORM_COMPONENT
	static void TransformComponent_GetTranslation(UUID entityUUID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");
		
		*outTranslation = entity.getComponent<TransformComponent>().translation;
	}
	static void TransformComponent_SetTranslation(UUID entityUUID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		entity.getComponent<TransformComponent>().translation = *translation;
	}
	
// RIGIDBODY_2D_COMPONENT
	static void RigidBody2DComponent_SetDynamic(UUID entityUUID, bool isDynamic)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		auto& rb2d = entity.getComponent<RigidBody2DComponent>();

		if (isDynamic)
			rb2d.type = RigidBody2DComponent::BodyType::Dynamic;
		else
			rb2d.type = RigidBody2DComponent::BodyType::Static;
	}
	static bool RigidBody2DComponent_IsDynamic(UUID entityUUID)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		return entity.getComponent<RigidBody2DComponent>().type == RigidBody2DComponent::BodyType::Dynamic;
	}
	static void RigidBody2DComponent_SetFixedRotation(UUID entityUUID, bool fixedRotation)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		entity.getComponent<RigidBody2DComponent>().fixedRotation = fixedRotation;
	}
	static bool RigidBody2DComponent_HasFixedRotation(UUID entityUUID)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		return entity.getComponent<RigidBody2DComponent>().fixedRotation;
	}
	static void RigidBody2DComponent_SetTransform(UUID entityUUID, glm::vec2* translation, float angle)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		auto& rb2d = entity.getComponent<RigidBody2DComponent>();
		((b2Body*)rb2d.runtimeBody)->SetTransform({ translation->x, translation->y }, angle);
	}
	static void RigidBody2DComponent_GetLinearVelocity(UUID entityUUID, glm::vec2* outLinearVelocity)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		auto& rb2d = entity.getComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtimeBody; // Abstract this latter

		auto const& linearVelocity = body->GetLinearVelocity();
		*outLinearVelocity = glm::vec2(linearVelocity.x, linearVelocity.y);
	}
	static void RigidBody2DComponent_ApplyLinearImpulse(UUID entityUUID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		auto& rb2d = entity.getComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtimeBody; // Abstract this latter

		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}
	static void RigidBody2DComponent_ApplyLinearImpulseToCenter(UUID entityUUID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::getSceneContext().get();
		LG_CORE_ASSERT(scene, "Scene is NULL!");
		Entity entity = scene->getEntityByUUID(entityUUID);
		LG_CORE_ASSERT(entity, "Entity is NULL!");

		auto& rb2d = entity.getComponent<RigidBody2DComponent>();
		b2Body* body = (b2Body*)rb2d.runtimeBody; // Abstract this latter

		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}


	static bool Input_IskeyDown(LgKey keycode)
	{
		return Input::isKeyPressed(keycode);
	}

	template<typename... TComponent>
	static void registerComponent()
	{
		([&]() 
		{
			std::string_view fullName = typeid(TComponent).name();
			size_t position = fullName.find_last_of(':');
			std::string_view typeName = fullName.substr(position+1);
			std::string managedName = fmt::format("L3gion.{}", typeName);

			MonoType* managedType = mono_reflection_type_from_name(managedName.data(), ScriptEngine::getCoreAssemblyImage());
			
			if (!managedType)
			{
				LG_CORE_ERROR("Could not find component type {}!", managedName.c_str());
				return;
			}
			s_EntityHasComponentFunctions[managedType] = [](Entity entity) { return entity.hasComponent<TComponent>(); };
		}(), ...);
	}
	template<typename... TComponent>
	static void registerComponent(ComponentGroup<TComponent...>)
	{
		registerComponent<TComponent...>();
	}

	void ScriptGlue::registerComponents()
	{
		s_EntityHasComponentFunctions.clear();
		registerComponent(AllComponents{});
	}

	void ScriptGlue::registerFunctions()
	{
		LG_ADD_INTERNAL_CALL(GetScriptInstance);
		
		LG_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		LG_ADD_INTERNAL_CALL(Entity_CreateEntity);
		LG_ADD_INTERNAL_CALL(Entity_DuplicateEntity);
		LG_ADD_INTERNAL_CALL(Entity_HasComponent);

		LG_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		LG_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		LG_ADD_INTERNAL_CALL(RigidBody2DComponent_SetDynamic);
		LG_ADD_INTERNAL_CALL(RigidBody2DComponent_IsDynamic);
		LG_ADD_INTERNAL_CALL(RigidBody2DComponent_SetFixedRotation);
		LG_ADD_INTERNAL_CALL(RigidBody2DComponent_HasFixedRotation);
		LG_ADD_INTERNAL_CALL(RigidBody2DComponent_SetTransform);
		LG_ADD_INTERNAL_CALL(RigidBody2DComponent_GetLinearVelocity);
		LG_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		LG_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCenter);

		LG_ADD_INTERNAL_CALL(Input_IskeyDown);
	}
}