#pragma once

#include "L3gion/Core/Core.h"
#include "L3gion/Renderer/SubTexture.h"

#include <filesystem>

namespace L3gion
{
	struct MSDFData;
	class Font
	{
	public:
		Font(const std::filesystem::path& filepath);
		~Font();
	
		ref<SubTexture2D> getAtlasTexture() const { return m_AtlasTexture; }
	private:
		MSDFData* m_Data;
		ref<SubTexture2D> m_AtlasTexture;
	};
}