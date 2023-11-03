#pragma once

#include "L3gion/Core/Core.h"

namespace L3gion
{
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}
		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static ref<UniformBuffer> create(uint32_t size, uint32_t binding);
	};
}

