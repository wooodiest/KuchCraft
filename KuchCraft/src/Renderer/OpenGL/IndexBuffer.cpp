#include "kcpch.h"
#include "Renderer/OpenGL/IndexBuffer.h"

#include <glad/glad.h>

namespace KuchCraft {

	IndexBuffer::IndexBuffer()
	{
	}

	IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count)
	{
		Create(data, count);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::Create(uint32_t* data, uint32_t count)
	{
		m_Count = count;

		if (m_RendererID)
			glDeleteBuffers(1, &m_RendererID);

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, m_Count * sizeof(uint32_t), data, GL_STATIC_DRAW);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}
