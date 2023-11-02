#include "lgpch.h"
#include "Scene.h"

#include "L3gion/Scene/Components.h"
#include <glm/glm.hpp>

#include "L3gion/Scene/Entity.h"

namespace L3gion
{
	Scene::Scene()
	{

	}
	Scene::~Scene()
	{

	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create() , this };
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		
		tag.tag = name.empty() ? "Not named Entity" : name;
			

		return entity;
	}
	void Scene::deleteEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::onUptdateRuntime(Timestep ts)
	{
		// Update Scripts
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (!nsc.instance)
			{
				nsc.instance = nsc.instantiateScript();
				nsc.instance->m_Entity = Entity{ entity, this };
				nsc.instance->onCreate();
			}

			nsc.instance->onUpdate(ts);
		});
		

		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.primary)
				{
					mainCamera = &camera.camera;
					cameraTransform = transform.getTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::beginScene(*mainCamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				m_QuadSpecs.transform = transform.getTransform();
				m_QuadSpecs.color = sprite.color;
				Renderer2D::drawQuad(m_QuadSpecs);
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUptdateEditor(Timestep ts, EditorCamera& editorCamera)
	{
		Renderer2D::beginScene(editorCamera);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			m_QuadSpecs.transform = transform.getTransform();
			m_QuadSpecs.color = sprite.color;
			m_QuadSpecs.id = (int)entity;
			Renderer2D::drawQuad(m_QuadSpecs);
		}

		Renderer2D::endScene();
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize non static aspect ratio
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camera = view.get<CameraComponent>(entity);

			if (!camera.staticAspectRatio && camera.primary)
				camera.camera.setViewportSize(width, height);
		}
	}
	
	Entity Scene::getPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.primary)
				return Entity{ entity, this };
		}
		return {};
	}
}