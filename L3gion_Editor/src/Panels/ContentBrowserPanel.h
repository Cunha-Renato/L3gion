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

	private:

		bool m_SetRefresh = false;
		
		std::filesystem::path m_CurrentDirPath;

		Directory m_RootDir;
		DirectoryTree tree;

		ref<SubTexture2D> m_FolderIcon;
		ref<SubTexture2D> m_FileIcon;
	
	private:
		void refresh();
	};
}