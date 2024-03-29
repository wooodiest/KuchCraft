#pragma once

#include "Renderer/OpenGL/VertexBuffer.h"

namespace KuchCraft {

	class VertexArray
	{
	public:
		static Ref<VertexArray> Create();

		VertexArray();
		~VertexArray();

		void SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);

		void Bind()   const;
		void UnBind() const;

	private:
		uint32_t m_RendererID = 0;
	};

}