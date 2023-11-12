#pragma once

#include "L3gion/Core/UUID.h"
#include "L3gion/Scene/SceneCamera.h"
//#include "L3gion/Scene/ScriptableEntity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace L3gion
{
	struct IDComponent
	{
		UUID id;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(UUID uuid) : id(uuid) {}
	};

	struct TagComponent
	{
		std::string tag;
	
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& Tag) : tag(Tag) {}
	};
	
	struct TransformComponent
	{
		glm::vec3 translation = glm::vec3(0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& Translation, const glm::vec3& Rotation = glm::vec3(0.0f), const glm::vec3& Scale = glm::vec3(1.0f)) 
			: translation(Translation), rotation(Rotation), scale(Scale) {}

		glm::mat4 getTransform() const
		{
			glm::mat4 Rotation = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* Rotation
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 color{1.0f};
		ref<SubTexture2D> texture = nullptr;
		uint32_t tilingFactor = 1;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& Color) : color(Color) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 color{ 1.0f };
		float thickness = 1.0f;
		float smoothness = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool primary = false;
		bool staticAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	class ScriptableEntity;
	struct ScriptComponent
	{
		std::string name;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};
	struct NativeScriptComponent
	{
		ScriptableEntity* instance = nullptr;

		ScriptableEntity*(*instantiateScript)();
		void (*destroyScript)(NativeScriptComponent*);

		template<typename T>
		void bind()
		{
			instantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			destroyScript = [](NativeScriptComponent* nsc) { delete nsc->instance; nsc->instance = nullptr; };
		}
	};

	// Physics
	struct RigidBody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType type = BodyType::Static;

		bool fixedRotation = false;

		// Storage for runtime
		void* runtimeBody = nullptr;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;
	};

	struct Collider2DComponent
	{
		enum class Type { Box = 0, Circle };

		Type type = Type::Box;

		float angle = 0.0f;

		glm::vec2 offset = { 0.0f, 0.0f };
		glm::vec2 size = { 0.5f, 0.5f };

		// Circle
		float radius = 0.5f;

		float density = 1.0f;
		float friction = 0.5f;
		float restitution = 0.0f;
		float restitutionThreshold = 0.4f;

		// Storage for runtime
		void* runtimeFixture = nullptr;

		Collider2DComponent() = default;
		Collider2DComponent(const Collider2DComponent&) = default;
	};
}