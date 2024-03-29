#pragma once

#include "Renderer/Data/Shader.h"
#include "Renderer/Data/IndexBuffer.h"
#include "Renderer/Data/VertexBuffer.h"
#include "Renderer/Data/VertexArray.h"

#include "World/Chunk.h"

namespace KuchCraft {

	struct Renderer3DData
	{
		Ref<IndexBuffer> QuadIndexBuffer;
	};

	struct Renderer3DChunkData
	{
		Ref<Shader>       Shader;
		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;

		std::vector<Chunk*> Chunks;
	};

	struct Renderer3DSkyboxData
	{
		Ref<Shader>       Shader;
		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;

		uint32_t Texture = 0;
	};

}