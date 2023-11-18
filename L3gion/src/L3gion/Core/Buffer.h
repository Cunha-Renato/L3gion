#pragma once

#include<stdint.h>
#include<cstring>

namespace L3gion
{
	class Buffer
	{
	public:
		uint8_t* data = nullptr;
		uint64_t size = 0;

		Buffer() = default;
		Buffer(uint64_t size)
		{
			allocate(size);
		}
		Buffer(const Buffer&) = default;

		static Buffer copy(Buffer other)
		{
			Buffer result(other.size);
			memcpy(result.data, other.data, other.size);
			return result;
		}

		void allocate(uint64_t _size)
		{
			if (data)
				release();

			data = new uint8_t[_size];
			size = _size;
		}

		void release()
		{
			delete[] data;
			data = nullptr;
			size = 0;
		}

		template<typename T>
		T* as()
		{
			return (T*)data;
		}

		operator bool() const
		{
			return (bool)data;
		}
	};

	class ScopedBuffer
	{
	public:
		ScopedBuffer(Buffer buffer) : m_Buffer(buffer){}
		ScopedBuffer(uint64_t size) : m_Buffer(size){}
		~ScopedBuffer()
		{
			m_Buffer.release();
		}

		uint8_t* data() { return m_Buffer.data; }
		uint64_t size() { return m_Buffer.size; }

		template<typename T>
		T* as()
		{
			return m_Buffer.as<T>();
		}

		operator bool() { return m_Buffer; }

	private:
		Buffer m_Buffer;
	};
}