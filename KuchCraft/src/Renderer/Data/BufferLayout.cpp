#include "kcpch.h"
#include "Renderer/Data/BufferLayout.h"

namespace KuchCraft {

	BufferElement::BufferElement(ShaderDataType type, const std::string& name)
		: Name(name), Type(type), Size(GetSize())
	{

	}

	uint32_t BufferElement::GetCount() const
	{
		switch (Type)
		{
			case ShaderDataType::Uint:   return 1;
			case ShaderDataType::Int:    return 1;
			case ShaderDataType::Float:  return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
		}

		return 0;
	}

	uint32_t BufferElement::GetSize() const
	{
		switch (Type)
		{
			case ShaderDataType::Uint:   return 1 * sizeof(uint32_t);
			case ShaderDataType::Int:    return 1 * sizeof(int32_t);
			case ShaderDataType::Float:  return 1 * sizeof(float);
			case ShaderDataType::Float2: return 2 * sizeof(float);
			case ShaderDataType::Float3: return 3 * sizeof(float);
			case ShaderDataType::Float4: return 4 * sizeof(float);
		}

		return 0;
	}

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& bufferElements)
		: m_BufferElements(bufferElements)
	{
		CalculateOffsetsAndStride();
	}

	void BufferLayout::CalculateOffsetsAndStride()
	{
		uint32_t offset = 0;
		m_Stride        = 0;

		for (auto& element : m_BufferElements)
		{
			element.Offset = offset;
			offset   += element.Size;
			m_Stride += element.Size;
		}
	}

}