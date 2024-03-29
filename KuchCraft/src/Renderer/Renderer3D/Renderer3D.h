#pragma once

#include "Renderer/Renderer3D/Renderer3DData.h"

namespace KuchCraft {

	class Renderer3D
	{
	public:
		static void DrawChunk(Chunk* chunk);
		static void DrawChunks(const std::vector<Chunk*>& chunks);

	private:
		static void Init();
		static void ShutDown();

		static void Render();
		static void Clear();

	private:
		static void PrepareChunks();
		static void PrepareSkybox();

		static void RenderChunks();
		static void RenderSkybox();

	private:
		Renderer3D() = default;

	private:
		static Renderer3DData       s_Data;
		static Renderer3DChunkData  s_ChunkData;
		static Renderer3DSkyboxData s_SkyboxData;

	public:
		friend class Renderer;

	};

}