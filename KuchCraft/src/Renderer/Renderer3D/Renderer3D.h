#pragma once

#include "Renderer/Renderer3D/Renderer3DData.h"

namespace KuchCraft {

	class Renderer3D
	{
	public:
		static void DrawChunk(Chunk* chunk);
		static void DrawChunks(const std::vector<Chunk*>& chunks);
		static void DrawOutlinedBlock(const glm::vec3& position);

	private:
		static void Init();
		static void ShutDown();

		static void Render();
		static void Clear();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);

		static Ref<FrameBuffer> GetFrameBuffer() { return s_Data.MainFrameBuffer; }

	private:
		static void PrepareChunks();
		static void PrepareSkybox();
		static void PrepareWater();
		static void PrepareOutlinedBlock();

		static void RenderChunks();
		static void RenderSkybox();
		static void RenderWater();
		static void RenderOutlinedBlock();

	private:
		Renderer3D() = default;

	private:
		static Renderer3DData            s_Data;
		static Renderer3DChunkData       s_ChunkData;
		static Renderer3DSkyboxData      s_SkyboxData;
		static Renderer3DWaterData       s_WaterData;
		static RendererOutlinedBlockData s_OutlinedBlockData;

	public:
		friend class Renderer;

	};

}