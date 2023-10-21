#pragma once

#include "L3gion/Renderer/RendererAPI.h"

namespace L3gion
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void drawIndexed(const ref<VertexArray>& vertexArray) override;
	};
}