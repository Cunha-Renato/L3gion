#pragma once

#include "L3gion/Renderer/RendererAPI.h"
 
namespace L3gion
{
	class RenderCommand
	{
	public:
		~RenderCommand() 
		{ 
		}
		
		inline static void init()
		{
			s_RendererAPI->init();
		}
		inline static void setViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->setViewPort(x, y, width, height);
		}
		inline static void setClearColor(const glm::vec4& color)
		{
			s_RendererAPI->setClearColor(color);
		}
		inline static void clear()
		{
			s_RendererAPI->clear();
		}
		inline static void drawIndexed(const ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->drawIndexed(vertexArray, indexCount);
		}
	private:
		static scope<RendererAPI> s_RendererAPI;
	};
}