#pragma once

#include "RendererAPI.h"
 
namespace L3gion
{
	class RenderCommand
	{
	public:
		~RenderCommand() 
		{ 
			if(s_RendererAPI)
				delete s_RendererAPI; 
		}
		
		inline static void init()
		{
			s_RendererAPI->init();
		}
		inline static void setClearColor(const glm::vec4& color)
		{
			s_RendererAPI->setClearColor(color);
		}
		inline static void clear()
		{
			s_RendererAPI->clear();
		}
		inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->drawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}