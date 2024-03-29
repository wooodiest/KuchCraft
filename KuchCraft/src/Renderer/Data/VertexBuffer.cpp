#include "kcpch.h"
#include "Renderer/Data/VertexBuffer.h"

#include <glad/glad.h>

namespace KuchCraft {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, const void* data, bool staticDraw)
	{
		return CreateRef<VertexBuffer>(size ,data, staticDraw);
	}

	VertexBuffer::VertexBuffer(uint32_t size, const void* data, bool staticDraw)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void VertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}
