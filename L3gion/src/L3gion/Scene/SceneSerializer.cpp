#include "lgpch.h"
#include "L3gion/Scene/SceneSerializer.h"
#include "L3gion/Scene/Entity.h"
#include "L3gion/Scene/Components.h"

#include <yaml-cpp/yaml.h>
#include <sstream>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& v)
		{
			Node node;
			node.push_back(v.x);
			node.push_back(v.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& v)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			v.x = node[0].as<float>();
			v.y = node[1].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& v)
		{
			Node node;
			node.push_back(v.x);
			node.push_back(v.y);
			node.push_back(v.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& v)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			v.x = node[0].as<float>();
			v.y = node[1].as<float>();
			v.z = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& v)
		{
			Node node;
			node.push_back(v.x);
			node.push_back(v.y);
			node.push_back(v.z);
			node.push_back(v.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& v)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			v.x = node[0].as<float>();
			v.y = node[1].as<float>();
			v.z = node[2].as<float>();
			v.w = node[3].as<float>();
			return true;
		}
	};
}

namespace L3gion
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;

		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		
		return out;
	}
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w <<YAML::EndSeq;

		return out;
	}

	static std::string rigidbody2DtypeToString(RigidBody2DComponent::BodyType type)
	{
		switch (type)
		{
			case RigidBody2DComponent::BodyType::Static: return "Static";
			case RigidBody2DComponent::BodyType::Dynamic: return "Dynamic";
			case RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		LG_CORE_ASSERT(false, "Unknown BodyType!");
		return {};
	}

	static RigidBody2DComponent::BodyType rigidbody2DtypeFromString(std::string type)
	{
		if (type == "Static")
			return RigidBody2DComponent::BodyType::Static;
		else if (type == "Dynamic")
			return RigidBody2DComponent::BodyType::Dynamic;
		else if (type == "Kinematic")
			return RigidBody2DComponent::BodyType::Kinematic;

		LG_CORE_ASSERT(false, "Unknown BodyType!");
		return RigidBody2DComponent::BodyType::Static;
	}

	static std::string colliderTypeToString(Collider2DComponent::Type type)
	{
		switch (type)
		{
			case Collider2DComponent::Type::Box: return "Box";
			case Collider2DComponent::Type::Circle: return "Circle";
		}

		LG_CORE_ASSERT(false, "Unknown Collider type!");
		return {};
	}

	Collider2DComponent::Type colliderTypeFromString(const std::string& type)
	{
		if (type == "Box")
			return Collider2DComponent::Type::Box;
		else if (type == "Circle")
			return Collider2DComponent::Type::Circle;

		LG_CORE_ASSERT(false, "Unknown Collider type!");
		return Collider2DComponent::Type::Box;
	}

	SceneSerializer::SceneSerializer(const ref<Scene>& scene)
		: m_Scene(scene) {}

	template<typename T, typename Func>
	static void serializeComponent(Entity& entity, YAML::Emitter& out, const std::string& componentKey, Func func)
	{
		if (entity.hasComponent<T>())
		{
			out << YAML::Key << componentKey;
			out << YAML::BeginMap;

			func();

			out << YAML::EndMap;
		}
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		LG_CORE_ASSERT(entity.hasComponent<IDComponent>(), " ");

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.getUUID();

		serializeComponent<TagComponent>(entity, out, "TagComponent", [&]()
		{
			auto& tag = entity.getComponent<TagComponent>().tag;
			out << YAML::Key << "tag" << YAML::Value << tag;
		});

		serializeComponent<TransformComponent>(entity, out, "TransformComponent", [&]()
		{
			auto& tc = entity.getComponent<TransformComponent>();

			out << YAML::Key << "translation" << YAML::Value << tc.translation;
			out << YAML::Key << "rotation" << YAML::Value << tc.rotation;
			out << YAML::Key << "scale" << YAML::Value << tc.scale;
		});

		serializeComponent<ScriptComponent>(entity, out, "ScriptComponent", [&]()
		{
			auto& sc = entity.getComponent<ScriptComponent>();

			out << YAML::Key << "script" << YAML::Value << sc.name;
		});

		serializeComponent<CameraComponent>(entity, out, "CameraComponent", [&]()
		{
			auto& cameraComponent = entity.getComponent<CameraComponent>();
			auto& camera = cameraComponent.camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "projectionType" << YAML::Value << (int)camera.getProjectionType();
			out << YAML::Key << "perspectiveVerticalFov" << YAML::Value << camera.getPerspectiveVerticalFov();
			out << YAML::Key << "perspectiveNearClip" << YAML::Value << camera.getPerspectiveNearClip();
			out << YAML::Key << "perspectiveFarClip" << YAML::Value << camera.getPerspectiveFarClip();
			out << YAML::Key << "orthographicSize" << YAML::Value << camera.getOrthographicSize();
			out << YAML::Key << "orthographicNearClip" << YAML::Value << camera.getOrthographicNearClip();
			out << YAML::Key << "orthographicFarClip" << YAML::Value << camera.getOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "primary" << YAML::Value << cameraComponent.primary;
			out << YAML::Key << "staticAspectRatio" << YAML::Value << cameraComponent.staticAspectRatio;
		});

		serializeComponent<SpriteRendererComponent>(entity, out, "SpriteRendererComponent", [&]()
		{
			auto& spr = entity.getComponent<SpriteRendererComponent>();
			out << YAML::Key << "color" << YAML::Value << spr.color;
			if (spr.texture)
				out << YAML::Key << "texturePath" << YAML::Value << spr.texture->getPath();
			out << YAML::Key << "tilingFactor" << YAML::Value << spr.tilingFactor;
		});

		serializeComponent<CircleRendererComponent>(entity, out, "CircleRendererComponent", [&]()
		{
			auto& crc = entity.getComponent<CircleRendererComponent>();
			out << YAML::Key << "color" << YAML::Value << crc.color;
			out << YAML::Key << "thickness" << YAML::Value << crc.thickness;
			out << YAML::Key << "smoothness" << YAML::Value << crc.smoothness;
		});

		serializeComponent<RigidBody2DComponent>(entity, out, "RigidBody2DComponent", [&]()
		{
			auto& rb2d = entity.getComponent<RigidBody2DComponent>();
			out << YAML::Key << "type" << YAML::Value << rigidbody2DtypeToString(rb2d.type);
			out << YAML::Key << "fixedRotation" << YAML::Value << rb2d.fixedRotation;
		});

		serializeComponent<Collider2DComponent>(entity, out, "Collider2DComponent", [&]()
		{
			auto& collider = entity.getComponent<Collider2DComponent>();
			out << YAML::Key << "type" << YAML::Value << colliderTypeToString(collider.type);
			out << YAML::Key << "radius" << YAML::Value << collider.radius;
			out << YAML::Key << "offset" << YAML::Value << collider.offset;
			out << YAML::Key << "size" << YAML::Value << collider.size;
			out << YAML::Key << "density" << YAML::Value << collider.density;
			out << YAML::Key << "friction" << YAML::Value << collider.friction;
			out << YAML::Key << "restitution" << YAML::Value << collider.restitution;
			out << YAML::Key << "restitutionThreshold" << YAML::Value << collider.restitutionThreshold;
		
		});

		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled Scene";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		for (auto [entityID, ref] : m_Scene->m_Registry.storage<TagComponent>().each())
		{
			Entity entity = { entityID, m_Scene.get() };
			if(!entity)
				return;

			SerializeEntity(out, entity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	void SceneSerializer::serializeBinary(const std::string& filepath)
	{
		LG_CORE_ASSERT(false, "Not Implemented!");
	}

	bool SceneSerializer::deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			LG_CORE_ERROR("Failed to load .lg file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->createEntityWithUUID(UUID(uuid), name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = deserializedEntity.getComponent<TransformComponent>();
					tc.translation = transformComponent["translation"].as<glm::vec3>();
					tc.rotation = transformComponent["rotation"].as<glm::vec3>();
					tc.scale = transformComponent["scale"].as<glm::vec3>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.addComponent<ScriptComponent>();
					sc.name = scriptComponent["script"].as<std::string>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.addComponent<CameraComponent>();
					auto cameraProps = cameraComponent["Camera"];

					cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["projectionType"].as<int>());

					cc.camera.setPerspectiveVerticalFov(cameraProps["perspectiveVerticalFov"].as<float>());
					cc.camera.setPerspectiveNearClip(cameraProps["perspectiveNearClip"].as<float>());
					cc.camera.setPerspectiveFarClip(cameraProps["perspectiveFarClip"].as<float>());

					cc.camera.setOrthographicSize(cameraProps["orthographicSize"].as<float>());
					cc.camera.setOrthographicNearClip(cameraProps["orthographicNearClip"].as<float>());
					cc.camera.setOrthographicFarClip(cameraProps["orthographicFarClip"].as<float>());

					cc.primary = cameraComponent["primary"].as<bool>();
					cc.staticAspectRatio = cameraComponent["staticAspectRatio"].as<bool>();
				}

				auto src = entity["SpriteRendererComponent"];
				if (src)
				{
					auto& spriteComponent = deserializedEntity.addComponent<SpriteRendererComponent>();
					spriteComponent.color = src["color"].as<glm::vec4>();
					if (src["texturePath"])
						spriteComponent.texture = SubTexture2D::create(src["texturePath"].as<std::string>());
					spriteComponent.tilingFactor = src["tilingFactor"].as<uint32_t>();
				}
				
				auto crc = entity["CircleRendererComponent"];
				if (crc)
				{
					auto& circleComponent = deserializedEntity.addComponent<CircleRendererComponent>();
					circleComponent.color = crc["color"].as<glm::vec4>();
					circleComponent.thickness = crc["thickness"].as<float>();
					circleComponent.smoothness = crc["smoothness"].as<float>();
				}

				auto rb2dComponent = entity["RigidBody2DComponent"];
				if (rb2dComponent)
				{
					auto& rb2d = deserializedEntity.addComponent<RigidBody2DComponent>();
					rb2d.type = rigidbody2DtypeFromString(rb2dComponent["type"].as<std::string>());
					rb2d.fixedRotation = rb2dComponent["fixedRotation"].as<bool>();
				}

				auto colliderComponent = entity["Collider2DComponent"];
				if (colliderComponent)
				{
					auto& collider = deserializedEntity.addComponent<Collider2DComponent>();
					collider.type = colliderTypeFromString(colliderComponent["type"].as<std::string>());
					collider.radius = colliderComponent["radius"].as<float>();
					collider.offset = colliderComponent["offset"].as<glm::vec2>();
					collider.size = colliderComponent["size"].as<glm::vec2>();
					collider.density = colliderComponent["density"].as<float>();
					collider.friction = colliderComponent["friction"].as<float>();
					collider.restitution = colliderComponent["restitution"].as<float>();
					collider.restitutionThreshold = colliderComponent["restitutionThreshold"].as<float>();
				}
			}
		}

		return true;
	}
	bool SceneSerializer::deserializeBinary(const std::string& filepath)
	{
		LG_CORE_ASSERT(false, "Not Implemented!");
		return false;
	}
}