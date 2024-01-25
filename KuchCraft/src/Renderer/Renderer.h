#pragma once

#include <glm/glm.hpp>
#include "Renderer/Camera.h"
#include "World/World.h"
#include "Shader.h"

#include <unordered_map>

namespace KuchCraft {

	struct RendererStatistics
	{
		uint32_t DrawCalls = 0;
		uint32_t Triangles = 0;
	};

	class Renderer
	{
	public:
		static void Init();
		static void ShutDown();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);

		static uint32_t GetTexture(const Block& block);
		static void     LoadTextureAtlas();
		static uint32_t LoadTextureToAtals(const std::string& path);

		static void Flush();
		static void StartBatch();
		static void NextBatch();

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void ResetStats();
		static RendererStatistics& GetStats() { return s_Stats; }

		static void DrawCube(const glm::vec3& position, const Block& block);

	private:
		Renderer() = default;

		static RendererStatistics s_Stats;
		static std::unordered_map<BlockType, unsigned int> s_BlockTextureAtlas;
		static std::unordered_map<BlockType, std::string>  s_BlockTexturePathsAtlas;

	};

}