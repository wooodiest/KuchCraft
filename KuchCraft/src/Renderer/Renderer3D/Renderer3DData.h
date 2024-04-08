#pragma once

#include "Renderer/Data/Shader.h"
#include "Renderer/Data/IndexBuffer.h"
#include "Renderer/Data/VertexBuffer.h"
#include "Renderer/Data/VertexArray.h"
#include "Renderer/Data/FrameBuffer.h"
#include "Renderer/Data/CubeMapTexture.h"
#include "Renderer/Renderer2D/Renderer2DData.h"
#include "World/Chunk.h"

namespace KuchCraft {

	struct Renderer3DInfo
	{
		uint32_t MaxQuads = 20000;
		uint32_t MaxVertices = MaxQuads * 4;
		uint32_t MaxIndices = MaxQuads * 6;
	};

	struct Renderer3DData
	{
		IndexBuffer QuadIndexBuffer;
		FrameBuffer RenderFrameBuffer;
	};

	struct Renderer3DChunkData
	{
		Shader       Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;

		std::vector<Chunk*> Chunks;
		std::vector<Chunk*> ChunksToRender;
	};

	struct Renderer3DSkyboxData
	{
		Shader         Shader;
		VertexArray    VertexArray;
		VertexBuffer   VertexBuffer;
		CubeMapTexture Texture;
	};

	struct Renderer3DWaterData
	{
		Shader       Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
	};

	struct Renderer3DOutlinedBlockData
	{
		bool Status = false;
		glm::vec3 Position{ 0.0f };
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float BorderRadius = 0.005f;

		Shader       Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
	};

	struct Renderer3DTintedData
	{
		bool Tinted = false;
		glm::vec4 Color{ 0.0f, 0.0f, 0.75f, 1.0f };

		Shader       Shader;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;
		FrameBuffer  FrameBuffer;
	};

	struct Renderer3DQuadData
	{
		Shader		 Shader;
		IndexBuffer  IndexBuffer;
		VertexArray  VertexArray;
		VertexBuffer VertexBuffer;

		Texture2D WhiteTexture;

		std::vector<QuadVertex> Vertices;
		uint32_t* TextureSlots;
		uint32_t  TextureSlotIndex = 1;
		uint32_t  IndexCount       = 0;
		uint32_t  VertexOffset     = 0;
	};

}