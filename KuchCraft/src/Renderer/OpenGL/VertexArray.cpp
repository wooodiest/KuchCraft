#include "kcpch.h"
#include "Renderer/OpenGL/VertexArray.h"

#include <glad/glad.h>

namespace KuchCraft {

	Ref<VertexArray> VertexArray::Create()
	{
		return CreateRef<VertexArray>();
	}

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
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

	void VertexArray::SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		Bind();
		vertexBuffer->Bind();

		const auto& bufferLayout = vertexBuffer->GetBufferLayout();
		for (const auto& element : bufferLayout)
		{
			uint32_t index = 0;

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
				case ShaderDataType::Float :
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

	void VertexArray::UnBind() const
	{
		glBindVertexArray(0);
	}

}
