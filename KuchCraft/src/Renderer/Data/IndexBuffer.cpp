#include "kcpch.h"
#include "Renderer/Data/IndexBuffer.h"

#include <glad/glad.h>

namespace KuchCraft {

	IndexBuffer::IndexBuffer()
	{
		
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void IndexBuffer::Create(uint32_t count, uint32_t* data, bool staticDraw)
	{
		m_Count = count;
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, m_Count * sizeof(uint32_t), data, staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	void IndexBuffer::SetData(uint32_t* data, uint32_t count)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(uint32_t), data);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}
