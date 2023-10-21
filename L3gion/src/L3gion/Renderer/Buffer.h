#pragma once

namespace L3gion
{
	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};
	
	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case L3gion::ShaderDataType::Float:		return 4;
			case L3gion::ShaderDataType::Float2:	return 4 * 2;
			case L3gion::ShaderDataType::Float3:	return 4 * 3;
			case L3gion::ShaderDataType::Float4:	return 4 * 4;
			case L3gion::ShaderDataType::Mat3:		return 4 * 3 * 3;
			case L3gion::ShaderDataType::Mat4:		return 4 * 4 * 4;
			case L3gion::ShaderDataType::Int:		return 4;
			case L3gion::ShaderDataType::Int2:		return 4 * 2;
			case L3gion::ShaderDataType::Int3:		return 4 * 3;
			case L3gion::ShaderDataType::Int4:		return 4 * 4;
			case L3gion::ShaderDataType::Bool:		return 1;
		}
		
		LG_CORE_ASSERT(false, "In ShaderDataTypeSize(): Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		uint32_t offset;
		uint32_t size;
		bool normalized;

		BufferElement() {}

		BufferElement(const ShaderDataType type, const std::string& name, bool normalized = false)
			: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized)
		{
		}

		uint32_t getComponentCount() const 
		{
			switch (type)
			{
				case L3gion::ShaderDataType::Float:		return 1;
				case L3gion::ShaderDataType::Float2:	return 2;
				case L3gion::ShaderDataType::Float3:	return 3;
				case L3gion::ShaderDataType::Float4:	return 4;
				case L3gion::ShaderDataType::Mat3:		return 3 * 3;
				case L3gion::ShaderDataType::Mat4:		return 4 * 4;
				case L3gion::ShaderDataType::Int:		return 1;
				case L3gion::ShaderDataType::Int2:		return 2;
				case L3gion::ShaderDataType::Int3:		return 3;
				case L3gion::ShaderDataType::Int4:		return 4;
				case L3gion::ShaderDataType::Bool:		return 1;
			}

			LG_CORE_ASSERT(false, "In getComponentCount(): Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements) 
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}
		
		inline const uint32_t getStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& getElements() const
		{
			return m_Elements;
		}

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator const begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator const end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.offset = offset;
				offset += element.size;
				m_Stride += element.size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual const BufferLayout& getLayout() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;

		static ref<VertexBuffer> create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual uint32_t getCount() const = 0;

		static ref<IndexBuffer> create(uint32_t* vertices, uint32_t count);
	};
}