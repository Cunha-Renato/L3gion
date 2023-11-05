#pragma once

#include <filesystem>

namespace L3gion
{
	struct Directory
	{
		std::filesystem::path path;
		std::string pathStr;
		Directory* parent = nullptr;
		std::filesystem::path relativePath;
		std::string relativePathStr;
		bool isDir = false;
		std::vector<Directory> childs;
	};

	class DirectoryTree
	{

	public:
		DirectoryTree() = default;
		DirectoryTree(Directory& root, uint32_t maxDepth = 500)
		{
			cleanAndCreate(root.childs, &root, maxDepth);
		}

		void cleanAndCreate(std::vector<Directory>& target, Directory* relativeTo, uint32_t maxDepth = 500)
		{
			m_DirectoriesPaths.clear();
			m_DirectoriesPaths[relativeTo->path] = relativeTo;
			createDirectoryTree(target, relativeTo, maxDepth);
		}

		uint32_t getSize() { return m_DirectoriesPaths.size(); }

		Directory& operator[] (std::filesystem::path path)
		{
			return get(path);
		}

		Directory& get(std::filesystem::path path)
		{
			if (m_DirectoriesPaths.find(path) != m_DirectoriesPaths.end())
				return *m_DirectoriesPaths[path];

			LG_CORE_ASSERT(false, "No Directory found!");
		}

	private:
		void createDirectoryTree(std::vector<Directory>& target, Directory* relativeTo, uint32_t maxDepth = 500)
		{
			if (target.capacity() <= 1)
				target.reserve(maxDepth);

			target.clear();

			for (auto& directoryEntry : std::filesystem::directory_iterator(relativeTo->path))
			{
				const auto& path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, relativeTo->path);
				std::string filenameStr = relativePath.filename().string();

				Directory directory;
				target.push_back(directory);
				auto dir = &target[target.size() - 1];

				dir->path = path;
				dir->pathStr = path.string();
				dir->relativePath = relativePath;
				dir->relativePathStr = filenameStr;
				dir->isDir = directoryEntry.is_directory();
				dir->parent = relativeTo;
				
				if (m_DirectoriesPaths.find(dir->path) != m_DirectoriesPaths.end())
				{
					LG_CORE_ASSERT(false, "In Directory.h: Dir already exists!");
				}
				else
					m_DirectoriesPaths[dir->path] = dir;

				if (dir->isDir)
					createDirectoryTree(dir->childs, dir, maxDepth);
			}
		}

	private:
		std::unordered_map<std::filesystem::path, Directory*> m_DirectoriesPaths;
	};
}