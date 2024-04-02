#pragma once

#include "Renderer/Data/BufferLayout.h"

namespace KuchCraft {

	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		void Create(uint32_t size, const void* data = nullptr, bool staticDraw = false);

		void SetData(const void* data, uint32_t size);

		void SetBufferLayout(const BufferLayout& bufferLayout) { m_BufferLayout = bufferLayout; }
		const BufferLayout& GetBufferLayout()            const { return m_BufferLayout;         }

		void Bind()   const;
		void Unbind() const;

	private:
		uint32_t     m_RendererID = 0;
		BufferLayout m_BufferLayout;

	};
}