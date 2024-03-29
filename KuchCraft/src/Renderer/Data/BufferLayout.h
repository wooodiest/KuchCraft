#pragma once

namespace KuchCraft {

	enum class ShaderDataType
	{
		None = 0,
		Uint,
		Float, Float2, Float3, Float4
	};

	struct BufferElement
	{
		ShaderDataType Type;
		std::string    Name;
		uint32_t       Size   = 0;
		uint32_t       Offset = 0;

		BufferElement(ShaderDataType type, const std::string& name);
		
		uint32_t GetCount() const;
		uint32_t GetSize()  const;
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& bufferElements);

		uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_BufferElements.begin(); }
		std::vector<BufferElement>::iterator end()   { return m_BufferElements.end();   }
		std::vector<BufferElement>::const_iterator begin() const { return m_BufferElements.begin(); }
		std::vector<BufferElement>::const_iterator end()   const { return m_BufferElements.end();   }

	private:
		void CalculateOffsetsAndStride();

	private:
		std::vector<BufferElement> m_BufferElements;
		uint32_t m_Stride = 0;

	};
}