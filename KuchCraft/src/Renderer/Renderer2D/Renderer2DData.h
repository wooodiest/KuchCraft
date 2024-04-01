#pragma once

#include "Renderer/Data/Shader.h"
#include "Renderer/Data/IndexBuffer.h"
#include "Renderer/Data/VertexBuffer.h"
#include "Renderer/Data/VertexArray.h"
#include "Renderer/Data/UniformBuffer.h"

namespace KuchCraft {

	struct Renderer2dInfo
	{
		uint32_t MaxQuads    = 20000;
		uint32_t MaxVertices = MaxQuads * 4;
		uint32_t MaxIndices  = MaxQuads * 6;
		uint32_t MaxTextureSlots = 32; // TODO: Get from Renderer::
	};

	struct Renderer2DQuadData
	{
		Ref<Shader>		  Shader;
		Ref<IndexBuffer>  IndexBuffer;
		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;
	};

	struct Renderer2DFullScreenQuadData
	{
		Ref<Shader>		   Shader;
		Ref<VertexArray>   VertexArray;
		Ref<VertexBuffer>  VertexBuffer;
	};

}