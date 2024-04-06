#pragma once

#include "Renderer/Data/Shader.h"
#include "Renderer/Data/IndexBuffer.h"
#include "Renderer/Data/VertexBuffer.h"
#include "Renderer/Data/VertexArray.h"
#include "Renderer/Data/UniformBuffer.h"

namespace KuchCraft {

	struct Renderer2DInfo
	{
		uint32_t MaxQuads    = 20000;
		uint32_t MaxVertices = MaxQuads * 4;
		uint32_t MaxIndices  = MaxQuads * 6;
	};

	struct Renderer2DQuadData
	{
		Shader		 Shader;
		IndexBuffer  IndexBuffer;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
	};

	struct Renderer2DFullScreenQuadData
	{
		Shader		 Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
	};

}