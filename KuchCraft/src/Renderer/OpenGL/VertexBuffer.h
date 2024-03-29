#pragma once

#include "Renderer/OpenGL/BufferLayout.h"

namespace KuchCraft {

	class VertexBuffer
	{
	public:
		static Ref<VertexBuffer> Create(uint32_t size, const void* data = nullptr, bool staticDraw = false);

		VertexBuffer(uint32_t size, const void* data, bool staticDraw);
		~VertexBuffer();

		void SetData(const void* data, uint32_t size);

		void SetBufferLayout(const BufferLayout& bufferLayout) { m_BufferLayout = bufferLayout; }
		const BufferLayout& GetBufferLayout()            const { return m_BufferLayout;         }

		void Bind()   const;
		void UnBind() const;

	private:
		uint32_t     m_RendererID = 0;
		BufferLayout m_BufferLayout;

	};
}