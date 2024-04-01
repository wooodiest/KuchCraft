#pragma once

#include "Renderer/Data/Shader.h"
#include "Renderer/Data/IndexBuffer.h"
#include "Renderer/Data/VertexBuffer.h"
#include "Renderer/Data/VertexArray.h"
#include "Renderer/Data/FrameBuffer.h"
#include "Renderer/Data/CubeMapTexture.h"
#include "World/Chunk.h"

namespace KuchCraft {

	struct Renderer3DData
	{
		Ref<IndexBuffer> QuadIndexBuffer;
		Ref<FrameBuffer> FrameBuffer;
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
		Ref<Shader>         Shader;
		Ref<VertexArray>    VertexArray;
		Ref<VertexBuffer>   VertexBuffer;
		Ref<CubeMapTexture> Texture;
	};

	struct Renderer3DWaterData
	{
		Ref<Shader>       Shader;
		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;
	};

	struct RendererOutlinedBlockData
	{
		bool Status = false;
		glm::vec3 Position{ 0.0f };
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float BorderRadius = 0.005f;

		Ref<Shader>       Shader;
		Ref<VertexArray>  VertexArray;
		Ref<VertexBuffer> VertexBuffer;
	};

}