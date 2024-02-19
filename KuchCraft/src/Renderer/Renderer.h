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
		static void BeginFrame();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);

		// Scene
		static void BeginScene(const Camera& camera);
		static void EndScene();

		// Rendering chunks
		static void BeginChunk();
		static void RenderChunk(Chunk* chunk);
		static void EndChunk();
		
		// Rendering skybox
		static void BeginSkybox();
		static void RenderSkybox();
		static void EndSkybox();
			
		// Utils
		static void SetWaterTintStatus(bool status);
		static void ShowTriangles(bool status);
		static uint32_t GetTexture(BlockType type);

		// Stats
		static void ResetStats();
		static RendererStatistics& GetStats() { return s_Stats; }

	private:
		// Loading textures
		static void     LoadTextureAtlas();
		static uint32_t LoadTextureToAtals(const std::string& path);
		static uint32_t LoadSkyboxTexture();

		// Prepare renderer for...
		static void PrepareRenderer();
		static void PrepareChunkRendering();
		static void PrepareSkyboxRendering();

		// Renderer data
		static RendererData       s_RendererData;
		static RendererStatistics s_Stats;
		static RendererChunkData  s_ChunkData;
		static RendererSkyboxData s_SkyboxData;

		// Temporary
		static Camera* s_Camera;

	private:
		Renderer() = default;
	};

}