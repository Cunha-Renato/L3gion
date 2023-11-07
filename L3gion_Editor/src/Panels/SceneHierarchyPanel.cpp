#include "lgpch.h"

#include "SceneHierarchyPanel.h"

#include "L3gion/Scene/Components.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace L3gion
{
	SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& scene)
	{
		setContext(scene);
	}

	void SceneHierarchyPanel::setContext(const ref<Scene>& scene)
	{
		m_Context = scene;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy", nullptr, ImGuiWindowFlags_NoNav);

		if (m_Context)
		{
			// Only on empty spaces
			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("New Empty Entity"))
					m_Context->createEntity("Empty Entity");

				ImGui::EndPopup();
			}

			for (auto [entityID, ref] : m_Context->m_Registry.storage<TagComponent>().each())
			{
				Entity entity{ entityID, m_Context.get() };
				drawEntityNode(entity);
			}
		}
		ImGui::End();

		ImGui::Begin("Components");
		if (m_SelectionContext)
			drawComponents(m_SelectionContext);
		
		ImGui::End();
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity)
	{
		auto& tag = entity.getComponent<TagComponent>().tag;
		bool selected = (m_SelectionContext == entity) ? true : false;

		ImGuiTreeNodeFlags flags = (selected ? ImGuiTreeNodeFlags_Selected : 0) |
		ImGuiTreeNodeFlags_OpenOnArrow | 
		ImGuiTreeNodeFlags_OpenOnDoubleClick | 
		ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_FramePadding | 
		ImGuiTreeNodeFlags_SpanAvailWidth;

		// A little hack to make the color of the active entity visible
		auto& colors = ImGui::GetStyle().Colors;
		if (selected) 
			colors[ImGuiCol_Header] = ImVec4{ 0.8f, 0.4, 0.21f, 1.0f };
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str()); // Drawing the TreeNode
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.18f, 0.18f, 1.0f };

		if (ImGui::IsItemClicked())
			m_SelectionContext = entity;

		bool deleteEntity = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				deleteEntity = true;

			ImGui::EndPopup();
		}

		if (opened)
			ImGui::TreePop();

		if (deleteEntity)
		{
			m_Context->deleteEntity(entity);
			if (entity == m_SelectionContext)
				m_SelectionContext = { };
		}
	}

	static void drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 2.0f });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 2.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.7f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.8f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.6f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.7f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.6f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.7f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.7f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
	
	template<typename T, typename Func>
	static void drawComponent(Entity entity, const std::string& name, Func func)
	{
		if (entity.hasComponent<T>())
		{
			auto flags = ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_AllowItemOverlap | 
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_FramePadding;

			auto& component = entity.getComponent<T>();
			ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });			
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				func(component);

				ImGui::TreePop();
			}

			if (removeComponent)
				entity.removeComponent<T>();
		}
	}
	
	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		ImGui::PushItemWidth(220.0f);
		if (entity.hasComponent<TagComponent>())
		{
			auto& tag = entity.getComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add +"))
			ImGui::OpenPopup("NewComponent");

		if (ImGui::BeginPopup("NewComponent"))
		{
			if (!entity.hasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.addComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!entity.hasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectionContext.addComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!entity.hasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					m_SelectionContext.addComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!entity.hasComponent<RigidBody2DComponent>())
			{
				if (ImGui::MenuItem("Rigidbody 2D"))
				{
					m_SelectionContext.addComponent<RigidBody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!entity.hasComponent<BoxColliderComponent>())
			{
				if (ImGui::MenuItem("Box Collider"))
				{
					m_SelectionContext.addComponent<BoxColliderComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		drawComponent<TransformComponent>(entity, "Transform", [](auto& tc)
		{			
			if (ImGui::Button("Reset"))
			{
				tc.translation = glm::vec3(0.0f);
				tc.rotation = glm::vec3(0.0f);
				tc.scale = glm::vec3(1.0f);
			}

			drawVec3Control("Translation", tc.translation);
			glm::vec3 rotation = glm::degrees(tc.rotation);
			drawVec3Control("Rotation", rotation);
			tc.rotation = glm::radians(rotation);
			drawVec3Control("Scale", tc.scale, 1.0f);
			ImGui::Spacing();
		});

		drawComponent<CameraComponent>(entity, "Camera", [](auto& cameraComponent)
		{
			auto& camera = cameraComponent.camera;

			ImGui::Checkbox("Primary", &cameraComponent.primary);

			const char* projectionTypeStr[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeStr = projectionTypeStr[(int)camera.getProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeStr))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeStr == projectionTypeStr[i];

					if (ImGui::Selectable(projectionTypeStr[i], isSelected))
					{
						currentProjectionTypeStr = projectionTypeStr[i];
						camera.setProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.getOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize, 0.1f))
					camera.setOrthographicSize(orthoSize);

				float orthoNear = camera.getOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear, 0.1f))
					camera.setOrthographicNearClip(orthoNear);

				float orthoFar = camera.getOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar, 0.1f))
					camera.setOrthographicFarClip(orthoFar);

			}

			if (camera.getProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float verticalFov = glm::degrees(camera.getPerspectiveVerticalFov());
				if (ImGui::DragFloat("Vertical Fov", &verticalFov, 0.1f))
					camera.setPerspectiveVerticalFov(glm::radians(verticalFov));

				float perspectiveNear = camera.getPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear, 0.1f))
					camera.setPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.getPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar, 0.1f))
					camera.setPerspectiveFarClip(perspectiveFar);
			}
		});
	
		drawComponent<SpriteRendererComponent>(entity, "Sprite Renderer", [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));

			ImGui::Button("Texture", {100, 0});
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					std::string path = *(std::string*)payload->Data;
					component.texture = SubTexture2D::create(path);
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::SliderInt("Tiling Factor", (int*)&component.tilingFactor, 1, 100);
		});

		drawComponent<CircleRendererComponent>(entity, "Circle Renderer", [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
			ImGui::DragFloat("Thickness", &component.thickness, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Smoothness", &component.smoothness, 0.0001f, 0.0f, 1.0f);
		});

		drawComponent<RigidBody2DComponent>(entity, "Rigidbody 2D", [](auto& component)
		{
			const char* bodyTypeStr[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeStr = bodyTypeStr[(int)component.type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeStr))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeStr == bodyTypeStr[i];

					if (ImGui::Selectable(bodyTypeStr[i], isSelected))
					{
						currentBodyTypeStr = bodyTypeStr[i];
						component.type = (RigidBody2DComponent::BodyType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
		});

		drawComponent<BoxColliderComponent>(entity, "Box Collider", [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.size));
			ImGui::DragFloat("Density", &component.density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.restitutionThreshold, 0.1f, 0.0f, 1.0f);
		});
	}
}