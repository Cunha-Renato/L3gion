#pragma once

#include "L3gion/Renderer/SubTexture.h"
#include "Directory.h"

namespace L3gion
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void onImGuiRender();

		void refresh();

	private:
		bool m_SetRefresh = false;
		
		std::filesystem::path m_CurrentDirPath;
		std::filesystem::path m_SelectedDirPath;
		std::filesystem::path m_PayloadPath;

		Directory m_RootDir;
		DirectoryTree tree;

		ref<SubTexture2D> m_FolderIcon;
		ref<SubTexture2D> m_FileIcon;
	};
}