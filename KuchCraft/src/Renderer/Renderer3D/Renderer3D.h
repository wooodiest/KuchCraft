#pragma once

#include "Renderer/Renderer3D/Renderer3DData.h"

namespace KuchCraft {

	class Renderer3D
	{
	public:
		static void DrawChunk(Chunk* chunk);
		static void DrawChunks(const std::vector<Chunk*>& chunks);
		static void DrawOutlinedBlock(const glm::vec3& position);
		static void DrawWaterTinted();

	private:
		static void Init();
		static void ShutDown();
		static void LoadRenderer3DInfo();

		static void Render();
		static void Clear();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);

		static uint32_t GetRendererdColorAttachmentRendererID() { return s_TintedData.FrameBuffer.GetColorAttachmentRendererID(); }

	private:
		static void PrepareChunks();
		static void PrepareSkybox();
		static void PrepareWater();
		static void PrepareOutlinedBlock();
		static void PrepareTinted();

		static void RenderChunks();
		static void RenderSkybox();
		static void RenderWater();
		static void RenderOutlinedBlock();
		static void RenderTinted();

	private:
		Renderer3D() = default;

	private:
		static Renderer3DData              s_Data;
		static Renderer3DChunkData         s_ChunkData;
		static Renderer3DSkyboxData        s_SkyboxData;
		static Renderer3DWaterData         s_WaterData;
		static Renderer3DOutlinedBlockData s_OutlinedBlockData;
		static Renderer3DTintedData        s_TintedData;

	public:
		friend class Renderer;

	};

}