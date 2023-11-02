#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "L3gion/Scene/SceneCamera.h"
#include "L3gion/Scene/ScriptableEntity.h"

namespace L3gion
{
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

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& Color) : color(Color) {}
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool primary = false;
		bool staticAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
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
}