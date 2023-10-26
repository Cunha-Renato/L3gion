#include "lgpch.h"
#include "L3gion/Renderer/Renderer2D.h"

#include "L3gion/Renderer/VertexArray.h"
#include "L3gion/Renderer/Shader.h"
#include "L3gion/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace L3gion
{
	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float tilingFactor;
	};

	struct Renderer2DData
	{
		const uint32_t maxQuads = 40'001;
		const uint32_t maxVertices = maxQuads * 4;
		const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxtextureSlots = 20;

		ref<VertexArray> quadVertexArray;
		ref<VertexBuffer> quadVertexBuffer;
		ref<Shader> textureShader;
		ref<Texture2D> whiteTexture;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr; // TODO: Realy dont like this raw pointer
		QuadVertex* quadVertexBufferPtr = nullptr;  // TODO: Realy dont like this raw pointer

		std::array<ref<Texture2D>, maxtextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1;

		~Renderer2DData()
		{
			if(quadVertexBufferBase)
				delete[] quadVertexBufferBase;
		}
	};

	static Renderer2DData s_Data;

	void Renderer2D::init()
	{
		LG_PROFILE_FUNCTION();

		s_Data.textureShader = Shader::create("../Sandbox/assets/shaders/Texture.glsl");
		s_Data.textureShader->bind();

		s_Data.quadVertexArray = VertexArray::create();
		s_Data.quadVertexBuffer = VertexBuffer::create(s_Data.maxVertices * sizeof(QuadVertex));

		s_Data.quadVertexBuffer->setLayout({
			{ShaderDataType::Float3, "a_position"},
			{ShaderDataType::Float4, "a_color"},
			{ShaderDataType::Float2, "a_TextCoord"},
			{ShaderDataType::Float, "a_texIndex"},
			{ShaderDataType::Float, "a_tilingFactor"}
		});
		s_Data.quadVertexArray->addVertexBuffer(s_Data.quadVertexBuffer);

		s_Data.quadVertexBufferBase = new QuadVertex[s_Data.maxVertices];
		
		uint32_t* quadIndices = new uint32_t[s_Data.maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.maxIndices; i+=6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		auto quadIB = IndexBuffer::create(quadIndices, s_Data.maxIndices);
		s_Data.quadVertexArray->setIndexBuffer(quadIB);

		s_Data.whiteTexture = Texture2D::create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		int samplers[s_Data.maxtextureSlots];
		for (uint32_t i = 0; i < s_Data.maxtextureSlots; i++)
			samplers[i] = i;

		s_Data.textureShader->setIntArray("u_Textures", samplers, s_Data.maxtextureSlots);

		s_Data.textureSlots[0] = s_Data.whiteTexture;
	}
	void Renderer2D::shutdown()
	{
		LG_PROFILE_FUNCTION();
	}

	void Renderer2D::endScene()
	{
		LG_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase;
		s_Data.quadVertexBuffer->setData(s_Data.quadVertexBufferBase, dataSize);

		flush();
	}

	void Renderer2D::beginScene(const OrthoCamera& camera)
	{
		LG_PROFILE_FUNCTION();

		s_Data.textureShader->bind();
		s_Data.textureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.textureSlotIndex = 1;
	}

	void Renderer2D::flush()
	{
		s_Data.textureShader->bind();
		for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
			s_Data.textureSlots[i]->bind(i);

		RenderCommand::drawIndexed(s_Data.quadVertexArray, s_Data.quadIndexCount);
	}

	void Renderer2D::drawQuad(const QuadSpecs& specs)
	{
		LG_PROFILE_FUNCTION();

		uint32_t textureIndex = 0;
		if (specs.texture != nullptr)
		{
			for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
			{
				if (s_Data.textureSlots[i]->compare(*specs.texture.get()))
				{
					textureIndex = i;
					break;
				}
			}

			if (textureIndex == 0)
			{
				textureIndex = s_Data.textureSlotIndex;
				s_Data.textureSlots[s_Data.textureSlotIndex] = specs.texture;
				s_Data.textureSlotIndex++;
			}
		}

		s_Data.quadVertexBufferPtr->position = specs.position;
		s_Data.quadVertexBufferPtr->color = specs.color;
		s_Data.quadVertexBufferPtr->texCoord = { 0.0f, 0.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr->tilingFactor = specs.tiling;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->position = { specs.position.x + specs.size.x, specs.position.y, specs.position.z };
		s_Data.quadVertexBufferPtr->color = specs.color;
		s_Data.quadVertexBufferPtr->texCoord = { 1.0f, 0.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr->tilingFactor = specs.tiling;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->position = { specs.position.x + specs.size.x, specs.position.y +  specs.size.y, specs.position.z };
		s_Data.quadVertexBufferPtr->color = specs.color;
		s_Data.quadVertexBufferPtr->texCoord = { 1.0f, 1.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr->tilingFactor = specs.tiling;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadVertexBufferPtr->position = { specs.position.x, specs.position.y + specs.size.y, specs.position.z };
		s_Data.quadVertexBufferPtr->color = specs.color;
		s_Data.quadVertexBufferPtr->texCoord = { 0.0f, 1.0f };
		s_Data.quadVertexBufferPtr->texIndex = textureIndex;
		s_Data.quadVertexBufferPtr->tilingFactor = specs.tiling;
		s_Data.quadVertexBufferPtr++;

		s_Data.quadIndexCount += 6;
	}
}