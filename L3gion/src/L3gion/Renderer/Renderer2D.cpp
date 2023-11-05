#include "lgpch.h"
#include "L3gion/Renderer/Renderer2D.h"

#include "L3gion/Renderer/VertexArray.h"
#include "L3gion/Renderer/Shader.h"
#include "L3gion/Renderer/UniformBuffer.h"
#include "L3gion/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace L3gion
{
	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float tilingFactor;

		// Editor Only
		int entityId;
	};

	struct Renderer2DData
	{
		static const uint32_t maxQuads = 10'000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxtextureSlots = 32;

		ref<VertexArray> quadVertexArray;
		ref<VertexBuffer> quadVertexBuffer;
		ref<Shader> textureShader;
		ref<Texture2D> whiteTexture;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr; // TODO: Realy dont like this raw pointer
		QuadVertex* quadVertexBufferPtr = nullptr;  // TODO: Realy dont like this raw pointer
		std::vector<Renderer2D::QuadSpecs> preBuffer;

		std::array<ref<Texture2D>, maxtextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1;

		glm::vec4 quadVertexPositions[4];

		Renderer2D::Statistics stats;

		struct CameraData
		{
			glm::mat4 viewProjection;
		};
		CameraData cameraBuffer;
		ref<UniformBuffer> cameraUniformBuffer;

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

		s_Data.quadVertexArray = VertexArray::create();
		s_Data.quadVertexBuffer = VertexBuffer::create(s_Data.maxVertices * sizeof(QuadVertex));

		s_Data.quadVertexBuffer->setLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TextCoord"},
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Float, "a_TilingFactor"},
			{ShaderDataType::Int, "a_EntityID"}
		});
		s_Data.quadVertexArray->addVertexBuffer(s_Data.quadVertexBuffer);

		s_Data.quadVertexBufferBase = new QuadVertex[s_Data.maxVertices];
		s_Data.preBuffer.reserve(s_Data.maxVertices);

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
		delete[] quadIndices;

		s_Data.whiteTexture = Texture2D::create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		int samplers[s_Data.maxtextureSlots];
		for (uint32_t i = 0; i < s_Data.maxtextureSlots; i++)
			samplers[i] = i;

		s_Data.textureShader = Shader::create("resources/shaders/Texture.glsl");

		s_Data.textureSlots[0] = s_Data.whiteTexture;

		s_Data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.cameraUniformBuffer = UniformBuffer::create(sizeof(Renderer2DData::CameraData), 0);
	}
	void Renderer2D::shutdown()
	{
		LG_PROFILE_FUNCTION();
	}

	static void prepareBuffers()
	{
		s_Data.quadIndexCount = 0;
		s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		s_Data.textureSlotIndex = 1;
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform)
	{
		LG_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.getProjection();
		viewProj *= glm::inverse(transform);

		s_Data.cameraBuffer.viewProjection = camera.getProjection() * glm::inverse(transform);
		s_Data.cameraUniformBuffer->setData(&s_Data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		prepareBuffers();
	}
	void Renderer2D::beginScene(const EditorCamera& editorCamera)
	{
		LG_PROFILE_FUNCTION();

		s_Data.cameraBuffer.viewProjection = editorCamera.getViewProjection();
		s_Data.cameraUniformBuffer->setData(&s_Data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		prepareBuffers();
	}

	void Renderer2D::endScene()
	{
		LG_PROFILE_FUNCTION();

		flush();
	}

	void Renderer2D::flush()
	{
		if (s_Data.quadIndexCount == 0)
			return; // Nothing to draw

		fillBuffer();
		s_Data.preBuffer.clear();

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase);
		s_Data.quadVertexBuffer->setData(s_Data.quadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
			s_Data.textureSlots[i]->bind(i);

		s_Data.textureShader->bind();
		RenderCommand::drawIndexed(s_Data.quadVertexArray, s_Data.quadIndexCount);

		s_Data.stats.drawCalls++;
	}

	void Renderer2D::flushAndReset()
	{
		flush();
		prepareBuffers();
	}

	void Renderer2D::drawQuad(const QuadSpecs& specs)
	{
		// TODO: Fix rendering order!!!

		LG_PROFILE_FUNCTION();

		if (s_Data.quadIndexCount >= Renderer2DData::maxIndices)
			flushAndReset();

		if (s_Data.textureSlotIndex >= Renderer2DData::maxtextureSlots)
			flushAndReset();

		QuadSpecs quad = specs;

		auto inverseVP = glm::inverse(s_Data.cameraBuffer.viewProjection);

		quad.distanceFromCamera = glm::distance(inverseVP[3], quad.transform[3]);
		s_Data.preBuffer.push_back(quad);

		s_Data.quadIndexCount += 6;
		s_Data.stats.quadCount++;
	}

	bool compareDepth(const Renderer2D::QuadSpecs& a, Renderer2D::QuadSpecs& b)
	{
		return a.distanceFromCamera > b.distanceFromCamera;
	}

	void Renderer2D::fillBuffer()
	{
		std::sort(s_Data.preBuffer.begin(), s_Data.preBuffer.end(), compareDepth);

		for (auto& specs : s_Data.preBuffer)
		{
			int textureIndex = 0;
			glm::vec2 texCoords[4] = {
				{ 0.0f, 0.0f, },
				{ 1.0f, 0.0f, },
				{ 1.0f, 1.0f, },
				{ 0.0f, 1.0f, } };

			if (specs.subTexture != nullptr)
			{
				for (uint32_t i = 1; i < s_Data.textureSlotIndex; i++)
				{
					if (s_Data.textureSlots[i]->compare(*specs.subTexture->getTexture().get()))
					{
						textureIndex = i;
						break;
					}
				}

				if (textureIndex == 0)
				{
					textureIndex = s_Data.textureSlotIndex;
					s_Data.textureSlots[s_Data.textureSlotIndex] = specs.subTexture->getTexture();
					s_Data.textureSlotIndex++;
				}

				texCoords[0] = specs.subTexture->getTexCoords()[0];
				texCoords[1] = specs.subTexture->getTexCoords()[1];
				texCoords[2] = specs.subTexture->getTexCoords()[2];
				texCoords[3] = specs.subTexture->getTexCoords()[3];
			}


			for (int i = 0; i < 4; i++)
			{
				s_Data.quadVertexBufferPtr->position = specs.transform * s_Data.quadVertexPositions[i];
				s_Data.quadVertexBufferPtr->color = specs.color;
				s_Data.quadVertexBufferPtr->texCoord = texCoords[i];
				s_Data.quadVertexBufferPtr->texIndex = textureIndex;
				s_Data.quadVertexBufferPtr->tilingFactor = specs.tiling;
				s_Data.quadVertexBufferPtr->entityId = specs.id;
				s_Data.quadVertexBufferPtr++;
			}
		}
	}

	void Renderer2D::resetStats()
	{
		memset(&s_Data.stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::getStats()
	{
		return s_Data.stats;
	}
}