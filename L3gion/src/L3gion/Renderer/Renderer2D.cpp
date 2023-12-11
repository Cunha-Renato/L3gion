#include "lgpch.h"
#include "L3gion/Renderer/Renderer2D.h"

#include "L3gion/Renderer/VertexArray.h"
#include "L3gion/Renderer/Shader.h"
#include "L3gion/Renderer/UniformBuffer.h"
#include "L3gion/Renderer/RenderCommand.h"
#include "L3gion/Renderer/MSDFData.h"

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

	struct CircleVertex
	{
		glm::vec3 worldPosition;
		glm::vec3 localPosition;
		glm::vec4 color;
		float thickness;
		float smoothness;

		// Editor Only
		int entityId;
	};

	struct LineVertex
	{
		glm::vec3 position;
		glm::vec4 color;

		// Editor Only
		int entityId;
	};

	struct Renderer2DData
	{
		static const uint32_t maxQuads = 1'000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxtextureSlots = 32;

		ref<VertexArray> quadVertexArray;
		ref<VertexBuffer> quadVertexBuffer;
		ref<Shader> quadShader;
		ref<Texture2D> whiteTexture;

		ref<VertexArray> circleVertexArray;
		ref<VertexBuffer> circleVertexBuffer;
		ref<Shader> circleShader;

		ref<VertexArray> lineVertexArray;
		ref<VertexBuffer> lineVertexBuffer;
		ref<Shader> lineShader;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;  
		
		uint32_t circleIndexCount = 0;
		CircleVertex* circleVertexBufferBase = nullptr; 
		CircleVertex* circleVertexBufferPtr = nullptr;  
		
		uint32_t lineVertexCount = 0;
		LineVertex* lineVertexBufferBase = nullptr;
		LineVertex* lineVertexBufferPtr = nullptr;

		float lineThickness = 2.0f;

		std::vector<Renderer2D::QuadSpecs> quadsPreBuffer;
		std::vector<Renderer2D::CircleSpecs> circlesPreBuffer;

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

			if (circleVertexBufferBase)
				delete[] circleVertexBufferBase;

			if (lineVertexBufferBase)
				delete[] lineVertexBufferBase;

			QuadVertex* quadVertexBufferBase = nullptr;
			QuadVertex* quadVertexBufferPtr = nullptr;
			CircleVertex* circleVertexBufferBase = nullptr;
			CircleVertex* circleVertexBufferPtr = nullptr;
			CircleVertex* lineVertexBufferBase = nullptr;
			CircleVertex* lineVertexBufferPtr = nullptr;
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
		s_Data.quadsPreBuffer.reserve(s_Data.maxVertices);
		s_Data.circlesPreBuffer.reserve(s_Data.maxVertices);

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

		// Circles
		s_Data.circleVertexArray = VertexArray::create();
		s_Data.circleVertexBuffer = VertexBuffer::create(s_Data.maxVertices * sizeof(CircleVertex));

		s_Data.circleVertexBuffer->setLayout({
			{ShaderDataType::Float3, "a_WorldPosition"	},
			{ShaderDataType::Float3, "a_LocalPosition"	},
			{ShaderDataType::Float4, "a_Color"			},
			{ShaderDataType::Float,  "a_Thickness"		},
			{ShaderDataType::Float,  "a_Smoothness"		},
			{ShaderDataType::Int,    "a_EntityID"		}
		});
		s_Data.circleVertexArray->addVertexBuffer(s_Data.circleVertexBuffer);
		s_Data.circleVertexArray->setIndexBuffer(quadIB); // Using quad IB
		s_Data.circleVertexBufferBase = new CircleVertex[s_Data.maxVertices];
		
		// End Circles

		// Circles
		s_Data.lineVertexArray = VertexArray::create();
		s_Data.lineVertexBuffer = VertexBuffer::create(s_Data.maxVertices * sizeof(LineVertex));

		s_Data.lineVertexBuffer->setLayout({
			{ShaderDataType::Float3, "a_Position"	},
			{ShaderDataType::Float4, "a_Color"		},
			{ShaderDataType::Int,    "a_EntityID"	}
		});
		s_Data.lineVertexArray->addVertexBuffer(s_Data.lineVertexBuffer);
		s_Data.lineVertexBufferBase = new LineVertex[s_Data.maxVertices];

		// End Circles

		s_Data.whiteTexture = Texture2D::create(TextureSpecs());
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		int samplers[s_Data.maxtextureSlots];
		for (uint32_t i = 0; i < s_Data.maxtextureSlots; i++)
			samplers[i] = i;

		s_Data.quadShader = Shader::create("resources/shaders/Renderer2D_Quad.glsl");
		s_Data.circleShader = Shader::create("resources/shaders/Renderer2D_Circle.glsl");
		s_Data.lineShader = Shader::create("resources/shaders/Renderer2D_Line.glsl");

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

		// TODO: move to a separate function
		s_Data.circleIndexCount = 0;
		s_Data.circleVertexBufferPtr = s_Data.circleVertexBufferBase;

		s_Data.lineVertexCount = 0;
		s_Data.lineVertexBufferPtr = s_Data.lineVertexBufferBase;

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

		flushAndReset();
	}

	void Renderer2D::flush()
	{
		LG_PROFILE_FUNCTION();
		
		if (s_Data.quadIndexCount)
		{
			fillQuadBuffer();
			s_Data.quadsPreBuffer.clear();

			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.quadVertexBufferPtr - (uint8_t*)s_Data.quadVertexBufferBase);
			s_Data.quadVertexBuffer->setData(s_Data.quadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
				s_Data.textureSlots[i]->bind(i);

			s_Data.quadShader->bind();
			RenderCommand::drawIndexed(s_Data.quadVertexArray, s_Data.quadIndexCount);

			s_Data.stats.drawCalls++;
		}

		if (s_Data.circleIndexCount)
		{	
			fillCircleBuffer();
			s_Data.circlesPreBuffer.clear();

			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.circleVertexBufferPtr - (uint8_t*)s_Data.circleVertexBufferBase);
			s_Data.circleVertexBuffer->setData(s_Data.circleVertexBufferBase, dataSize);

			s_Data.circleShader->bind();
			RenderCommand::drawIndexed(s_Data.circleVertexArray, s_Data.circleIndexCount);

			s_Data.stats.drawCalls++;
		}

		if (s_Data.lineVertexCount)
		{
			s_Data.circlesPreBuffer.clear();

			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.lineVertexBufferPtr - (uint8_t*)s_Data.lineVertexBufferBase);
			s_Data.lineVertexBuffer->setData(s_Data.lineVertexBufferBase, dataSize);

			s_Data.lineShader->bind();
			RenderCommand::setLineThickness(s_Data.lineThickness);
			RenderCommand::drawLines(s_Data.lineVertexArray, s_Data.lineVertexCount);

			s_Data.stats.drawCalls++;
		}
	}

	void Renderer2D::flushAndReset()
	{
		flush();
		prepareBuffers();
	}

	void Renderer2D::drawQuad(const QuadSpecs& specs)
	{
		LG_PROFILE_FUNCTION();

		if (s_Data.quadIndexCount >= Renderer2DData::maxIndices)
			flushAndReset();

		if (s_Data.textureSlotIndex >= Renderer2DData::maxtextureSlots)
			flushAndReset();

		QuadSpecs quad = specs;

		auto inverseVP = glm::inverse(s_Data.cameraBuffer.viewProjection);

		quad.distanceFromCamera = glm::distance(inverseVP[3], quad.transform[3]);
		s_Data.quadsPreBuffer.push_back(quad);

		s_Data.quadIndexCount += 6;
		s_Data.stats.quadCount++;
	}

	void Renderer2D::drawCircle(const CircleSpecs& specs)
	{
		LG_PROFILE_FUNCTION();

		if (s_Data.circleIndexCount >= Renderer2DData::maxIndices)
			flushAndReset(); // TODO: different flush for quads and circles

		CircleSpecs circle = specs;

		auto inverseVP = glm::inverse(s_Data.cameraBuffer.viewProjection);

		circle.distanceFromCamera = glm::distance(inverseVP[3], circle.transform[3]);
		s_Data.circlesPreBuffer.push_back(circle);

		s_Data.circleIndexCount += 6;
		s_Data.stats.circleCount++;
	}

	void Renderer2D::drawLine(const LineSpecs& specs)
	{
		s_Data.lineVertexBufferPtr->position = specs.p0;
		s_Data.lineVertexBufferPtr->color = specs.color;
		s_Data.lineVertexBufferPtr->entityId = specs.id;
		s_Data.lineVertexBufferPtr++;

		s_Data.lineVertexBufferPtr->position = specs.p1;
		s_Data.lineVertexBufferPtr->color = specs.color;
		s_Data.lineVertexBufferPtr->entityId = specs.id;
		s_Data.lineVertexBufferPtr++;
	
		s_Data.lineVertexCount += 2;
	}

	void Renderer2D::drawRect(const RectSpecs& specs)
	{
		LineSpecs line;
		line.id = specs.id;
		line.color = specs.color;

		glm::vec3 ps[4] = {glm::vec3(0.0f)};

		if (specs.useTransform)
		{
			for (size_t i = 0; i < 4; i++)
				ps[i] = specs.transform * s_Data.quadVertexPositions[i];
		}
		else
		{
			glm::vec3 position = specs.position;
			ps[0] = glm::vec3(position.x - specs.size.x * 0.5f, position.y - specs.size.y * 0.5f, position.z);
			ps[1] = glm::vec3(position.x + specs.size.x * 0.5f, position.y - specs.size.y * 0.5f, position.z);
			ps[2] = glm::vec3(position.x + specs.size.x * 0.5f, position.y + specs.size.y * 0.5f, position.z);
			ps[3] = glm::vec3(position.x - specs.size.x * 0.5f, position.y + specs.size.y * 0.5f, position.z);
		}

		line.p0 = ps[0];
		line.p1 = ps[1];
		drawLine(line);

		line.p0 = ps[1];
		line.p1 = ps[2];
		drawLine(line);

		line.p0 = ps[2];
		line.p1 = ps[3];
		drawLine(line);

		line.p0 = ps[3];
		line.p1 = ps[0];
		drawLine(line);
	}

	void Renderer2D::drawText(const TextSpecs& specs)
	{
		const auto& fontGeometry = specs.font->getMSDFData()->fontGeometry;
		const auto& metrics = fontGeometry.getMetrics();
		ref<SubTexture2D> fontAtlas = specs.font->getAtlasTexture();

		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		double y = 0.0;
		
		char character = 'L';
		auto glyph = fontGeometry.getGlyph(character);

		if (!glyph)
			glyph = fontGeometry.getGlyph('?');
		if (!glyph)
			return; // TODO: continue

		double al, ab, ar, at;
		glyph->getQuadAtlasBounds(al, ab, ar, at);
		glm::vec2 texCoordMin((float)al, (float)ab);
		glm::vec2 texCoordMax((float)ar, (float)at);

		double pl, pb, pr, pt;
		glyph->getQuadPlaneBounds(pl, pb, pr, pt);
		glm::vec2 quadMin((float)pl, (float)pb);
		glm::vec2 quadMax((float)pr, (float)pt);

		quadMin *= fsScale, quadMax *= fsScale;
		quadMin += glm::vec2(x, y);
		quadMax += glm::vec2(x, y);

		float texelWidth = 1.0f / fontAtlas->getTexture()->getWidth();
		float texelHeight = 1.0f / fontAtlas->getTexture()->getHeight();
		texCoordMin *= glm::vec2(texelWidth, texelHeight);
		texCoordMax *= glm::vec2(texelWidth, texelHeight);

		// Render

		double advance = glyph->getAdvance();
		char nextCharacter = 'L';
		fontGeometry.getAdvance(advance, character, nextCharacter);
	
		float kerningOffset = 0.0f;
		x += fsScale * advance + kerningOffset;
	}

	float Renderer2D::getLineThickness()
	{
		return s_Data.lineThickness;
	}
	void Renderer2D::setLineThickness(float thickness)
	{
		s_Data.lineThickness = thickness;
	}

	bool compareDepthQuad(const Renderer2D::QuadSpecs& a, Renderer2D::QuadSpecs& b)
	{
		return a.distanceFromCamera > b.distanceFromCamera;
	}
	bool compareDepthCircle(const Renderer2D::CircleSpecs& a, Renderer2D::CircleSpecs& b)
	{
		return a.distanceFromCamera > b.distanceFromCamera;
	}

	void Renderer2D::fillQuadBuffer()
	{
		LG_PROFILE_FUNCTION();
		std::sort(s_Data.quadsPreBuffer.begin(), s_Data.quadsPreBuffer.end(), compareDepthQuad);

		// Quads
		for (auto& specs : s_Data.quadsPreBuffer)
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

	void Renderer2D::fillCircleBuffer()
	{
		LG_PROFILE_FUNCTION();
		std::sort(s_Data.circlesPreBuffer.begin(), s_Data.circlesPreBuffer.end(), compareDepthCircle);

		// Circles
		for (auto& specs : s_Data.circlesPreBuffer)
		{
			for (int i = 0; i < 4; i++)
			{
				s_Data.circleVertexBufferPtr->worldPosition = specs.transform * s_Data.quadVertexPositions[i];
				s_Data.circleVertexBufferPtr->localPosition = s_Data.quadVertexPositions[i] * 2.0f;
				s_Data.circleVertexBufferPtr->color = specs.color;
				s_Data.circleVertexBufferPtr->thickness = specs.thickness;
				s_Data.circleVertexBufferPtr->smoothness = specs.smoothness;
				s_Data.circleVertexBufferPtr->entityId = specs.id;
				s_Data.circleVertexBufferPtr++;
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