#include "lgpch.h"
#include "ContentBrowserPanel.h"
#include "L3gion/Core/Timer.h"

#include "L3gion/Core/Input.h"

#include <ImGui/imgui.h>

namespace L3gion
{
	static void drawTree(DirectoryTree& tree, std::filesystem::path& currentPath, std::filesystem::path& selected)
	{
		auto& selectedDir = tree[currentPath];
		ImGuiTreeNodeFlags flags;

		for (auto& dir : selectedDir.childs)
		{
			if (dir.isDir)
				flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
			else
				flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

			if (ImGui::TreeNodeEx(dir.relativePathStr.c_str(), flags, dir.relativePathStr.c_str()))
			{
				drawTree(tree, dir.path, selected);
				ImGui::TreePop();
			}

			if (Input::isKeyPressed(LgKeys::LG_KEY_LEFT_CONTROL) && Input::isMouseButtonPressed(LG_MOUSE_BUTTON_LEFT) && ImGui::IsItemHovered())
			{
				if (dir.isDir)
					selected = dir.path;
				else
					selected = dir.parent->path;
			}
		}
	}

	ContentBrowserPanel::ContentBrowserPanel()
	{	
		m_FolderIcon = SubTexture2D::create("resources/icons/ContentBrowserPanel/FolderIcon.png");
		m_FileIcon = SubTexture2D::create("resources/icons/ContentBrowserPanel/FileIcon.png");

		m_RootDir.path = "assets";
		m_RootDir.isDir = true;
		m_RootDir.relativePathStr = "assets";

		m_CurrentDirPath = m_RootDir.path;
		m_SelectedDirPath = m_RootDir.path;

		refresh();
	}

	static Timer timer;

	void ContentBrowserPanel::onImGuiRender()
	{
		int timeElapsed = (int)timer.elapsedMillis();

		if (timeElapsed % 3000 == 0 && timeElapsed > 0)
		{
			refresh();
			timer.reset();
		}

		ImGui::Begin("Content Browser");

		if (ImGui::BeginTable("ContentBrowserFilesistem", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableSetupColumn(
				"A0",
				ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed,
				300.0f);

			// Filesystem
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			if (ImGui::BeginTable("fileBrowser", 1, ImGuiTableFlags_ScrollY))
			{
				ImGui::TableNextColumn();
				drawTree(tree, m_RootDir.path, m_CurrentDirPath);
				ImGui::EndTable();
			}

			// ContentBrowser
			ImGui::TableNextColumn();
			if (m_CurrentDirPath != m_RootDir.path)
			{
				ImGui::Button("<--");
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					m_CurrentDirPath = tree[m_CurrentDirPath].parent->path;

				ImGui::SameLine();
				ImGui::Button("Root");
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					m_CurrentDirPath = m_RootDir.path;
				ImGui::SameLine();
			}
			if (ImGui::Button("Refresh"))
				m_SetRefresh = true;

			ImGui::SameLine();
			ImGui::Text("Items: %d", tree[m_CurrentDirPath].childs.size());
			ImGui::SameLine();
			ImGui::Text("Tree Size: %d", tree.getSize());

			static float padding = 4.5f;
			static float thumbNailSize = 110.0f;
			float cellSize = thumbNailSize + padding;

			float panelWidth = ImGui::GetColumnWidth();
			int columCount = (int)(panelWidth / cellSize);

			if (columCount < 1)
				columCount = 1;

			if (ImGui::BeginTable("ContentBrowser", columCount, ImGuiTableFlags_ScrollY))
			{
				ImGui::TableNextColumn();

				auto& currentDir = tree[m_CurrentDirPath].childs;

				for (auto& dir : currentDir)
				{
					ImGui::PushID(dir.relativePathStr.c_str());

					ref<SubTexture2D> icon = dir.isDir ? m_FolderIcon : m_FileIcon;

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::ImageButton((ImTextureID)icon->getTexture()->getRendererID(), { thumbNailSize, thumbNailSize }, { 0, 1 }, { 1, 0 });
					
					if (ImGui::BeginDragDropSource())
					{
						m_PayloadPath = dir.path;
						ImGui::SetDragDropPayload(
							"CONTENT_BROWSER_ITEM",
							(const void*)&m_PayloadPath, sizeof(m_PayloadPath),
							ImGuiCond_Once
						);

						ImGui::EndDragDropSource();
					}
					
					if (dir.isDir)
					{
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							m_CurrentDirPath = dir.path;
					}
					ImGui::PopStyleColor();
					ImGui::TextWrapped(dir.relativePathStr.c_str());
					ImGui::TableNextColumn();

					ImGui::PopID();
				}
			
				ImGui::EndTable();
			}

			//ImGui::SliderFloat("Thumbnail Size", &thumbNailSize, 16, 512);
			//ImGui::SliderFloat("Padding Size", &padding, 0, 32);

			ImGui::EndTable();
		}
		ImGui::End();
		
		if (m_SetRefresh)
			refresh();
	}

	void ContentBrowserPanel::refresh()
	{
		LG_PROFILE_FUNCTION();
		m_SetRefresh = false;

		tree.cleanAndCreate(m_RootDir.childs, &m_RootDir, 800);
	}
}