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
		Renderer();
		~Renderer();

		inline static Renderer& Get() { return *s_Instance; }
		inline RendererStatistics& GetStats() { return m_Stats; }

		void OnViewportSizeChanged(uint32_t width, uint32_t height);

		void ShutDown();

		uint32_t GetTexture(const Block& block);
		void LoadTextureAtlas();
		uint32_t LoadTexture(const std::string& path);

		void Flush();
		void StartBatch();
		void NextBatch();

		void BeginScene(Camera& camera);
		void EndScene();

		void DrawCube(const glm::vec3& position, const Block& block);

		RendererStatistics m_Stats;

	private:
		unsigned int m_Texture;
		std::unordered_map<BlockType, unsigned int> m_BlockTextureAtlas;
		std::unordered_map<BlockType, std::string> m_BlockTexturePathsAtlas;

	private:
		static Renderer* s_Instance;

	};

}