#include "lgpch.h"
#include "Scene.h"

#include "L3gion/Scene/Entity.h"
#include "L3gion/Scene/ScriptableEntity.h"
#include "L3gion/Scene/Components.h"

#include <glm/glm.hpp>

// box2d
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

namespace L3gion
{
	static b2BodyType rigidbody2DTypeToBox2D(RigidBody2DComponent::BodyType type)
	{
		switch (type)
		{
			case L3gion::RigidBody2DComponent::BodyType::Static: return b2BodyType::b2_staticBody;
			case L3gion::RigidBody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
			case L3gion::RigidBody2DComponent::BodyType::Kinematic: return b2BodyType::b2_kinematicBody;
		}

		LG_CORE_ASSERT(false, "Unknown Type!");
		return b2BodyType::b2_staticBody;
	}
	Scene::Scene()
	{

	}
	Scene::~Scene()
	{

	}

	template<typename Component>
	static void copyComponent(entt::registry& dest, const entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).id;
			LG_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), " ");
			entt::entity destEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dest.emplace_or_replace<Component>(destEnttID, component);
		}
	}

	template<typename Component>
	static void copyComponentIfExists(Entity dest, Entity src)
	{
		if (src.hasComponent<Component>())
			dest.addOrReplaceComponent<Component>(src.getComponent<Component>());
	}

	ref<Scene> Scene::copy(ref<Scene> other)
	{
		ref<Scene> newScene = createRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcRegistry = other->m_Registry;
		auto& destRegistry = newScene->m_Registry;

		auto idView = srcRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcRegistry.get<IDComponent>(e).id;
			const auto& name = srcRegistry.get<TagComponent>(e).tag;

			Entity newEntity = newScene->createEntityWithUUID(uuid, name);
			enttMap[uuid] = newEntity;
		}

		// Copy components
		copyComponent<TransformComponent>(destRegistry, srcRegistry, enttMap);
		copyComponent<SpriteRendererComponent>(destRegistry, srcRegistry, enttMap);
		copyComponent<CircleRendererComponent>(destRegistry, srcRegistry, enttMap);
		copyComponent<CameraComponent>(destRegistry, srcRegistry, enttMap);
		copyComponent<NativeScriptComponent>(destRegistry, srcRegistry, enttMap);
		copyComponent<RigidBody2DComponent>(destRegistry, srcRegistry, enttMap);
		copyComponent<BoxColliderComponent>(destRegistry, srcRegistry, enttMap);
	
		return newScene;
	}

	Entity Scene::createEntity(const std::string& name)
	{
		return createEntityWithUUID(UUID(), name);
	}
	Entity Scene::createEntityWithUUID(UUID id, const std::string& name)
	{
		Entity entity = { m_Registry.create() , this };
		entity.addComponent<IDComponent>(id);
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();

		tag.tag = name.empty() ? "Not named Entity" : name;


		return entity;
	}

	void Scene::deleteEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::onRuntimeStart()
	{
		m_World = new b2World({ 0.0f, -9.8 });
		auto view = m_Registry.view<RigidBody2DComponent>();

		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.getComponent<TransformComponent>();
			auto& rb2d = entity.getComponent<RigidBody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = rigidbody2DTypeToBox2D(rb2d.type);
			bodyDef.position.Set(transform.translation.x, transform.translation.y);
			bodyDef.angle = transform.rotation.z;

			b2Body* body = m_World->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.fixedRotation);
			rb2d.runtimeBody = body;

			if (entity.hasComponent<BoxColliderComponent>())
			{
				auto& bc = entity.getComponent<BoxColliderComponent>();

				b2PolygonShape polyShape;
				glm::vec2 boxSize = { bc.size.x * transform.scale.x, bc.size.y * transform.scale.y };
				polyShape.SetAsBox(boxSize.x, boxSize.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &polyShape;
				fixtureDef.density = bc.density;
				fixtureDef.friction = bc.friction;
				fixtureDef.restitution = bc.restitution;
				fixtureDef.restitutionThreshold = bc.restitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}
		}
	}
	void Scene::onRuntimeStop()
	{
		delete m_World;
		m_World = nullptr;
	}

	void Scene::updatePhysics(Timestep ts)
	{
		LG_PROFILE_FUNCTION();

		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;

		m_World->Step(ts, velocityIterations, positionIterations);

		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& rb2d = entity.getComponent<RigidBody2DComponent>();
			auto& transform = entity.getComponent<TransformComponent>();

			b2Body* body = (b2Body*)rb2d.runtimeBody;
			body->SetType(rigidbody2DTypeToBox2D(rb2d.type));

			if (entity.hasComponent<BoxColliderComponent>())
			{
				auto& bc = entity.getComponent<BoxColliderComponent>();

				auto fixture = body->GetFixtureList();
				fixture->SetDensity(bc.density);
				fixture->SetFriction(bc.friction);
				fixture->SetRestitution(bc.restitution);
				fixture->SetRestitutionThreshold(bc.restitutionThreshold);

				const auto& position = body->GetPosition();
				transform.translation.x = position.x;
				transform.translation.y = position.y;
				transform.rotation.z = body->GetAngle();
			}
		}
	}

	void Scene::onUptdateRuntime(Timestep ts)
	{
		LG_PROFILE_FUNCTION();
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

		// Physics
		updatePhysics(ts);

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

			// Sprites
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

					m_QuadSpecs.transform = transform.getTransform();
					m_QuadSpecs.color = sprite.color;
					m_QuadSpecs.id = (int)entity;
					m_QuadSpecs.tiling = sprite.tilingFactor;
					m_QuadSpecs.subTexture = sprite.texture;
					Renderer2D::drawQuad(m_QuadSpecs);
				}
			}

			// Circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto& transform = view.get<TransformComponent>(entity);
					auto& circleComponent = view.get<CircleRendererComponent>(entity);

					m_CircleSpecs.transform = transform.getTransform();
					m_CircleSpecs.color = circleComponent.color;
					m_CircleSpecs.thickness = circleComponent.thickness;
					m_CircleSpecs.smoothness = circleComponent.smoothness;
					m_CircleSpecs.id = (int)entity;
					Renderer2D::drawCircle(m_CircleSpecs);
				}
			}

			Renderer2D::endScene();
		}
	}
	void Scene::onUptdateEditor(Timestep ts, EditorCamera& editorCamera)
	{
		LG_PROFILE_FUNCTION();
		Renderer2D::beginScene(editorCamera);

		// Sprites
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				m_QuadSpecs.transform = transform.getTransform();
				m_QuadSpecs.color = sprite.color;
				m_QuadSpecs.id = (int)entity;
				m_QuadSpecs.tiling = sprite.tilingFactor;
				m_QuadSpecs.subTexture = sprite.texture;
				Renderer2D::drawQuad(m_QuadSpecs);
			}
		}

		// Circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto& transform = view.get<TransformComponent>(entity);
				auto& circleComponent = view.get<CircleRendererComponent>(entity);

				m_CircleSpecs.transform = transform.getTransform();
				m_CircleSpecs.color = circleComponent.color;
				m_CircleSpecs.thickness = circleComponent.thickness;
				m_CircleSpecs.smoothness = circleComponent.smoothness;
				m_CircleSpecs.id = (int)entity;
				Renderer2D::drawCircle(m_CircleSpecs);
			}
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
	
	void Scene::duplicateEntity(Entity entity)
	{
		std::string name = entity.getName();
		Entity newEntity = createEntity(name);

		// Copy components
		copyComponentIfExists<TransformComponent>(newEntity, entity);
		copyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		copyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		copyComponentIfExists<CameraComponent>(newEntity, entity);
		copyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		copyComponentIfExists<RigidBody2DComponent>(newEntity, entity);
		copyComponentIfExists<BoxColliderComponent>(newEntity, entity);
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