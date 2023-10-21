#pragma once

#include <string>

namespace L3gion
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static ref<Shader> create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}
