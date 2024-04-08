#pragma once

#include "World/Block.h"
#include "World/WorldData.h"

#include "Renderer/Data/UniformBuffer.h"
#include "Renderer/Data/Camera.h"

#include "Core/Timer.h"

namespace KuchCraft {

	constexpr uint32_t default_texture_slot  = 0;

	// Vertex data
	constexpr uint32_t quad_index_count     = 6;
	constexpr uint32_t quad_vertex_count    = 4;
	constexpr uint32_t quad_vertex_count_a  = 6;
	constexpr uint32_t cube_vertex_count    = 24;
	constexpr uint32_t cube_index_count     = 36;
	constexpr uint32_t cube_face_cout       = 6;

	// Chunk data
	constexpr int max_quads_in_chunk    = chunk_size_XZ * chunk_size_XZ * chunk_size_Y * cube_face_cout;
	constexpr int max_indices_in_chunk  = max_quads_in_chunk * 6;
	constexpr int max_vertices_in_chunk = max_quads_in_chunk * 4;

	// Data structures
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		float     TexSlot;
	};

	struct Vertex_P2C2
	{
		glm::vec2 Position;
		glm::vec2 TexCoord;
	};

	struct Vertex_P3C2
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
	};

	struct UniformBufferCameraData
	{
		glm::mat4 ViewProjection;
		glm::mat4 AbsoluteViewProjection;
		glm::mat4 OrthoProjection;
	};

	struct RendererInfo
	{
		uint32_t MaxTextureSlots = 32;
		bool     OpenGlLogs      = true;
	};

	struct RendererData
	{
		Camera* CurrentCamera = nullptr;
		std::shared_ptr<Camera> SpareCamera; // spare camera just in case, only care about ortho projection
		UniformBuffer WorldDataUniformBuffer;
	}; 

	struct RendererStatistics
	{
		std::string DebugText;

		uint32_t DrawCalls      = 0;
		uint32_t Quads          = 0;
		uint32_t ChunksToRender = 0;

		Timer RenderTimer;
		Timer ChunkTimer;
		Timer WaterTimer;
		Timer SkyboxTimer;
		Timer Renderer2DTimer;

		void Clear()
		{
			DrawCalls      = 0;
			Quads          = 0;
			ChunksToRender = 0;
		}
	};

	// Cube - quad vertices
	constexpr uint32_t vertices_bottom_index = 0;
	constexpr uint32_t vertices_top_index    = 1;
	constexpr uint32_t vertices_front_index  = 2;
	constexpr uint32_t vertices_right_index  = 3;
	constexpr uint32_t vertices_behind_index = 4;
	constexpr uint32_t vertices_left_index   = 5;

	constexpr Vertex vertices_bottom[quad_vertex_count] {
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.0f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 0.0f,  0.0f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.0f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.25f, 0.5f }, 0.0f }
	};
	constexpr Vertex vertices_top[quad_vertex_count] {
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 0.25f, 0.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.5f,  0.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.25f, 0.5f }, 0.0f }
	};
	constexpr Vertex vertices_front[quad_vertex_count] {
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 0.0f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.5f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.25f, 1.0f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 0.0f,  1.0f }, 0.0f }
	};
	constexpr Vertex vertices_right[quad_vertex_count] {
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  1.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.25f, 1.0f }, 0.0f }
	};
	constexpr Vertex vertices_behind[quad_vertex_count] {
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.75f, 0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.75f, 1.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  1.0f }, 0.0f }
	};
	constexpr Vertex vertices_left[quad_vertex_count] {
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.75f, 0.5f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 1.0f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 1.0f,  1.0f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.75f, 1.0f }, 0.0f }
	};

}