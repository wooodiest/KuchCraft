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
}