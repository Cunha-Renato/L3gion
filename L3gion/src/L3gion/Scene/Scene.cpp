#include "lgpch.h"
#include "Scene.h"

#include "L3gion/Scene/Entity.h"
#include "L3gion/Scene/ScriptableEntity.h"
#include "L3gion/Scene/Components.h"
#include "L3gion/Scripting/ScriptEngine.h"

#include <glm/glm.hpp>

// box2d
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

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

	template<typename... TComponent>
	static void copyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
		{
			auto view = src.view<TComponent>();
			for (auto srcEntity : view)
			{
				entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).id);

				auto& srcComponent = src.get<TComponent>(srcEntity);
				dst.emplace_or_replace<TComponent>(dstEntity, srcComponent);
			}
		}(), ...);
	}

	template<typename... TComponent>
	static void copyComponent(ComponentGroup<TComponent...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		copyComponent<TComponent...>(dst, src, enttMap);
	}

	template<typename... TComponent>
	static void copyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		{
			if (src.hasComponent<TComponent>())
				dst.addOrReplaceComponent<TComponent>(src.getComponent<TComponent>());
		}(), ...);
	}
	template<typename... TComponent>
	static void copyComponentIfExists(ComponentGroup<TComponent...>, Entity dst, Entity src)
	{
		copyComponentIfExists<TComponent...>(dst, src);
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
		copyComponent(AllComponents{}, destRegistry, srcRegistry, enttMap);
	
		return newScene;
	}

	void Scene::refreshScripts()
	{
		// Reload all entity Scripts
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::loadEntity(entity);
		}
	}
	void Scene::resetScriptFields()
	{
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ref<ScriptInstance> scriptInstance = ScriptEngine::getEntityScriptInstance(entity.getUUID());
			if (scriptInstance)
			{
				const auto& fields = scriptInstance->getScriptClass()->getFields();

				for (const auto& [name, field] : fields)
				{
					if (field.type == ScriptFieldType::Float)
					{
						float data = scriptInstance->getFieldValue<float>(name, true);
						scriptInstance->setFieldValue(name, data, true);
					}
				}
			}
		}
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

		m_EntityMap[id] = entity;

		return entity;
	}
	Entity Scene::duplicateEntity(UUID entityID)
	{
		Entity entity = getEntityByUUID(entityID);
		return duplicateEntity(entity);
	}
	Entity Scene::duplicateEntity(Entity entity)
	{
		Entity newEntity = createEntity(entity.getName());
		copyComponentIfExists(AllComponents{}, newEntity, entity);

		return newEntity;
	}

	void Scene::deleteEntity(Entity entity)
	{
		m_EntityMap.erase(entity.getUUID());
		m_Registry.destroy(entity);
	}

	void Scene::onRuntimeStart()
	{
		m_IsRunning = true;
		initializePhysics();

		// Instantiate all ScriptEntities
		refreshScripts();
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::onCreateEntity(entity);
		}
	}
	void Scene::onRuntimeStop()
	{
		m_IsRunning = false;
		stopPhysics();

		resetScriptFields();
		ScriptEngine::onRuntimeStop();
	}

	void Scene::onSimulationStart()
	{
		m_IsRunning = true;
		initializePhysics();
	}
	void Scene::onSimulationStop()
	{
		m_IsRunning = false;
		stopPhysics();
	}

	void Scene::initializePhysics()
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

			if (entity.hasComponent<Collider2DComponent>())
			{
				b2FixtureDef fixtureDef;
				b2PolygonShape boxShape;
				b2CircleShape circleShape;

				auto& colider2D = entity.getComponent<Collider2DComponent>();
				if (colider2D.type == Collider2DComponent::Type::Box)
				{
					boxShape.SetAsBox(colider2D.size.x * transform.scale.x, colider2D.size.y * transform.scale.y, { colider2D.offset.x, colider2D.offset.y }, 0.0f);

					fixtureDef.shape = &boxShape;
				}
				else
				{
					circleShape.m_p.Set(colider2D.offset.x, colider2D.offset.y);
					circleShape.m_radius = colider2D.radius * transform.scale.x;

					fixtureDef.shape = &circleShape;
				}

				fixtureDef.density = colider2D.density;
				fixtureDef.friction = colider2D.friction;
				fixtureDef.restitution = colider2D.restitution;
				fixtureDef.restitutionThreshold = colider2D.restitutionThreshold;

				b2Body* body = m_World->CreateBody(&bodyDef);
				body->SetFixedRotation(rb2d.fixedRotation);
				rb2d.runtimeBody = body;
				body->CreateFixture(&fixtureDef);

				colider2D.angle = body->GetAngle();
			}
		}
	}
	void Scene::stopPhysics()
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

			if (entity.hasComponent<Collider2DComponent>())
			{
				auto& bc = entity.getComponent<Collider2DComponent>();

				auto fixture = body->GetFixtureList();
				body->DestroyFixture(fixture);
				
				b2FixtureDef newFixture;
				b2PolygonShape boxShape;
				b2CircleShape circleShape;

				auto& colider2D = entity.getComponent<Collider2DComponent>();
				if (colider2D.type == Collider2DComponent::Type::Box)
				{
					boxShape.SetAsBox(colider2D.size.x * transform.scale.x, colider2D.size.y * transform.scale.y, { colider2D.offset.x, colider2D.offset.y }, 0.0f);

					newFixture.shape = &boxShape;
				}
				else
				{
					circleShape.m_p.Set(colider2D.offset.x, colider2D.offset.y);
					circleShape.m_radius = colider2D.radius * transform.scale.x;

					newFixture.shape = &circleShape;
				}

				newFixture.density = bc.density;
				newFixture.friction = bc.friction;
				newFixture.restitution = bc.restitution;
				newFixture.restitutionThreshold = bc.restitutionThreshold;
				body->CreateFixture(&newFixture);

				const auto& position = body->GetPosition();
				transform.translation.x = position.x;
				transform.translation.y = position.y;
				transform.rotation.z = body->GetAngle();

				colider2D.angle = body->GetAngle();
			}
		}
	}

	void Scene::renderEditorScene(EditorCamera& camera)
	{
		LG_PROFILE_FUNCTION();
		Renderer2D::beginScene(camera);

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
				Renderer2D::draw(m_QuadSpecs);
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
				Renderer2D::draw(m_CircleSpecs);
			}
		}

		Renderer2D::endScene();
	}

	void Scene::onUptdateRuntime(Timestep ts)
	{
		LG_PROFILE_FUNCTION();

		//Update Scripts
		{
			// C# Scripting
			auto view = m_Registry.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptEngine::onUpdateEntity(entity, ts);
			}

			// Native Scripting
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
		}

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
					Renderer2D::draw(m_QuadSpecs);
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
					Renderer2D::draw(m_CircleSpecs);
				}
			}

			Renderer2D::endScene();
		}
	}
	void Scene::onUptdateEditor(Timestep ts, EditorCamera& editorCamera)
	{
		// Render
		renderEditorScene(editorCamera);
	}
	void Scene::onUpdateSimulation(Timestep ts, EditorCamera& editorCamera)
	{
		updatePhysics(ts);

		// Render
		renderEditorScene(editorCamera);
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

	Entity Scene::getEntityByUUID(UUID id)
	{
		// TODO: ASSERT MAYBE
		if (m_EntityMap.find(id) != m_EntityMap.end())
			return { m_EntityMap[id], this};

		return {};
	}
	Entity Scene::getEntityByName(const std::string_view& name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.tag == name)
				return Entity{ entity, this };
		}

		return {};
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