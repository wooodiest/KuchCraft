#include "kcpch.h"
#include "Renderer/Data/UniformBuffer.h"

#include <glad/glad.h>

namespace KuchCraft {

	UniformBuffer::UniformBuffer()
	{
		
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

}

