#pragma once

#include "L3gion/Renderer/RendererAPI.h"

namespace L3gion
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		~OpenGLRendererAPI() = default;
		virtual void init() override;
		virtual void setViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void drawIndexed(const ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void drawLines(const ref<VertexArray>& vertexArray, uint32_t vertexCount) override;
		virtual void setLineThickness(float thickness) override;
	};
}