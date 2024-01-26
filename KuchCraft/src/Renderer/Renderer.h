#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

#include "Renderer/Camera.h"

namespace KuchCraft {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		float     TexIndex;

	};

	constexpr size_t quad_index_count = 6;
	constexpr size_t cube_vertex_count = 24;

	constexpr Vertex vertices_bottom[4]
	{
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.0f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 0.0f,  0.0f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.0f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.25f, 0.5f }, 0.0f }
	};
	constexpr Vertex vertices_top[4]
	{
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 0.25f, 0.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.5f,  0.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.25f, 0.5f }, 0.0f }
	};
	constexpr Vertex vertices_front[4]
	{
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 0.0f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.5f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.25f, 1.0f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 0.0f,  1.0f }, 0.0f }
	};
	constexpr Vertex vertices_right[4]
	{
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  1.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.25f, 1.0f }, 0.0f }
	};
	constexpr Vertex vertices_behind[4]
	{
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.75f, 0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.75f, 1.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  1.0f }, 0.0f }
	};
	constexpr Vertex vertices_left[4]
	{
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.75f, 0.5f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 1.0f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 1.0f,  1.0f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.75f, 1.0f }, 0.0f }
	};

	enum class BlockType;
	class Block;

	struct RendererStatistics
	{
		uint32_t DrawCalls = 0;
		uint32_t Quads = 0;
	};

	class Renderer
	{
	public:
		static void Init();
		static void ShutDown();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);

		static uint32_t GetTexture(const BlockType& type);
		static void     LoadTextureAtlas();
		static uint32_t LoadTextureToAtals(const std::string& path);

		static void Flush();
		static void StartBatch();
		static void NextBatch();

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void ResetStats();
		static RendererStatistics& GetStats() { return s_Stats; }

		static void DrawList(std::vector<Vertex>& vertices, const std::vector<BlockType>& textures);

	private:
		Renderer() = default;

		static RendererStatistics s_Stats;
		static std::unordered_map<BlockType, unsigned int> s_BlockTextureAtlas;
		static std::unordered_map<BlockType, std::string>  s_BlockTexturePathsAtlas;

	};

}