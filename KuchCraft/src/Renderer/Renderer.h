#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Renderer/Camera.h"
#include "World/Chunk.h"

namespace KuchCraft {

	class Renderer
	{
	public:
		static void Init();
		static void ShutDown();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);

		static void     LoadTextureAtlas();
		static uint32_t LoadTextureToAtals(const std::string& path);
		static uint32_t LoadSkyboxTexture();

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void BeginRenderingWater();
		static void EndRenderingWater();

		static void ResetStats();
		static RendererStatistics& GetStats() { return s_Stats; }
		static uint32_t GetTexture(BlockType type);

		static void DrawChunk(Chunk* chunk);
		static void DrawChunkWater(Chunk* chunk);

		static void DrawSkybox(const Camera& camera);
		static void SetTintColor(const glm::vec4& color);

		static void SetTrianglesVisibility(bool status);

	private:
		Renderer() = default;

		static RendererStatistics s_Stats;

	};

}