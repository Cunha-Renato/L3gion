#include "lgpch.h"
#include "ContentBrowserPanel.h"

#include <ImGui/imgui.h>

namespace L3gion
{
	ContentBrowserPanel::ContentBrowserPanel()
	{	
		m_FolderIcon = SubTexture2D::create("resources/icons/ContentBrowserPanel/FolderIcon.png");
		m_FileIcon = SubTexture2D::create("resources/icons/ContentBrowserPanel/FileIcon.png");

		m_RootDir.path = "assets";
		m_RootDir.isDir = true;
		m_RootDir.filenameStr = "assets";

		m_CurrentDirPath = m_RootDir.path;
		refresh();
	}

	void ContentBrowserPanel::onImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (ImGui::BeginTable("ContentBrowserFilesistem", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableSetupColumn(
				"A0",
				ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed,
				300.0f);

			// Filesystem
			ImGui::TableNextColumn();


			// ContentBrowser
			ImGui::TableNextColumn();
			if (m_CurrentDirPath != m_RootDir.path)
			{
				ImGui::Button("<--");
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					m_CurrentDirPath = tree[m_CurrentDirPath].parent->path;
			}
			ImGui::SameLine();
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
					ref<SubTexture2D> icon = dir.isDir ? m_FolderIcon : m_FileIcon;

					ImGui::ImageButton((ImTextureID)icon->getTexture()->getRendererID(), { thumbNailSize, thumbNailSize }, { 0, 1 }, { 1, 0 });
					if (dir.isDir)
					{
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							m_CurrentDirPath = dir.path;
					}
					ImGui::TextWrapped(dir.filenameStr.c_str());
					ImGui::TableNextColumn();
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