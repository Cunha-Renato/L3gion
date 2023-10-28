#pragma once

#include "L3gion/Core/Core.h"

namespace L3gion
{
	struct FrameBufferSpecs
	{
		uint32_t width, height;
		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t getColorAttachmentRendererID() const = 0;

		virtual const FrameBufferSpecs getSpecification() const = 0;

		static ref<FrameBuffer> create(const FrameBufferSpecs& specs);
	};

}