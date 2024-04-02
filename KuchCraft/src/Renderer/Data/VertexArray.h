#pragma once

#include "Renderer/Data/VertexBuffer.h"

namespace KuchCraft {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void Create();

		void SetVertexBuffer(const VertexBuffer& vertexBuffer);

		void Bind()   const;
		void Unbind() const;

	private:
		uint32_t m_RendererID = 0;
	};

}