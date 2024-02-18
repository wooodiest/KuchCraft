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

	constexpr uint32_t max_texture_slots    = 32;
	constexpr uint32_t triangle_index_count = 6;
	constexpr uint32_t quad_index_count     = 6;
	constexpr uint32_t quad_vertex_count    = 4;
	constexpr uint32_t cube_vertex_count    = 24;

	constexpr float water_texture_slot = 1.0f;

	class Chunk;
	class Block;
	enum class BlockType;

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

	constexpr float cube_vertices[] =
	{
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
	};

	struct RendererChunkData
	{
		uint32_t DrawCalls = 0;
		std::vector<uint32_t> IndexCount;
		std::vector<uint32_t> Textures;

		RendererChunkData()
		{
			IndexCount.push_back(0);
		}

		uint32_t GetDrawCallTextureSlots(uint32_t drawCall)
		{
			return (uint32_t)Textures.size() - drawCall * max_texture_slots;
		}

		void StartNewDrawCall()
		{
			IndexCount.push_back(0);
			DrawCalls++;
		}

		void AddTexture(uint32_t texture)
		{
			Textures.push_back(texture);
		}

		void UpdateCurrentIndexCount()
		{
			IndexCount[DrawCalls] += quad_index_count;
		}

	};

}