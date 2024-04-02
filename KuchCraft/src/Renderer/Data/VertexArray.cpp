#include "kcpch.h"
#include "Renderer/Data/VertexArray.h"

#include <glad/glad.h>

namespace KuchCraft {

	VertexArray::VertexArray()
	{
		
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArray::Create()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Uint:   return GL_UNSIGNED_INT;
			case ShaderDataType::Float:  return GL_FLOAT;
			case ShaderDataType::Float2: return GL_FLOAT;
			case ShaderDataType::Float3: return GL_FLOAT;
			case ShaderDataType::Float4: return GL_FLOAT;
		}

		return 0;
	}

	void VertexArray::SetVertexBuffer(const VertexBuffer& vertexBuffer)
	{
		Bind();
		vertexBuffer.Bind();

		uint32_t index = 0;
		const auto& bufferLayout = vertexBuffer.GetBufferLayout();
		for (const auto& element : bufferLayout)
		{
			switch (element.Type)
			{
				case ShaderDataType::Uint:
				{
					glVertexAttribIPointer(
						index,
						element.GetCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						bufferLayout.GetStride(),
						(const void*)element.Offset
					);

					glEnableVertexAttribArray(index);
					index++;
					break;
				}
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glVertexAttribPointer(
						index, 
						element.GetCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						GL_FALSE,
						bufferLayout.GetStride(),
						(const void*)element.Offset
					);

					glEnableVertexAttribArray(index);
					index++;
					break;
				}
			}
		}
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

}
