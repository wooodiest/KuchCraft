#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Game/Camera.h"
#include "World/Chunk.h"

namespace KuchCraft {

	class Renderer
	{
	public:
		static void Init();
		static void ShutDown();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);
		static void OnUpdate(float dt);

		// Scene
		static void BeginWorld(const Camera& camera);
		static void EndWorld();

		// Rendering chunks
		static void BeginChunk();
		static void RenderChunk(Chunk* chunk);
		static void EndChunk();
		
		// Rendering skybox
		static void BeginSkybox();
		static void RenderSkybox();
		static void EndSkybox();

		// Rendering water
		static void BeginWater();
		static void RenderWater(Chunk* chunk);
		static void EndWater();

		// Utils
		static void SetWaterTintStatus(bool status);
		static void ShowTriangles(bool status);
		static uint32_t GetTexture(BlockType type);

		// Stats
		static void ResetStats();
		static RendererStatistics& GetStats() { return s_Stats; }

	private:
		// Utils
		static void InvalidateMainFrameBuffer(uint32_t width, uint32_t height);
		static void RenderDebugText(const std::string& text);
		static void RenderDubugTest(uint32_t length);

		// Loading textures
		static void     LoadTextureAtlas();
		static uint32_t LoadTextureToAtals(const std::string& path);
		static uint32_t LoadSkyboxTexture();

		// Prepare renderer for...
		static void PrepareRenderer();
		static void PrepareChunkRendering();
		static void PrepareSkyboxRendering();
		static void PrepareWaterRendering();
		static void PrepareTextRendering();

		// Renderer data
		static RendererData       s_RendererData;
		static RendererStatistics s_Stats;
		static RendererChunkData  s_ChunkData;
		static RendererSkyboxData s_SkyboxData;
		static RendererWaterData  s_WaterData;
		static RendererTextData s_TextData;

	private:
		Renderer() = default;
	};

}