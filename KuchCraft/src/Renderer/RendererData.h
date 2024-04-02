#pragma once

#include "World/Block.h"
#include "World/WorldData.h"

#include "Renderer/Data/UniformBuffer.h"

#include "Renderer/Data/Camera.h"

namespace KuchCraft {

	// Constances
	constexpr bool opengl_logs = true;

	// GPU
	constexpr uint32_t max_texture_slots     = 32; // Max value 128
	constexpr uint32_t default_texture_slot  = 0;

	// Vertex data
	constexpr uint32_t triangle_index_count = 6;
	constexpr uint32_t quad_index_count     = 6;
	constexpr uint32_t quad_vertex_count    = 4;
	constexpr uint32_t quad_vertex_count_a  = 6;
	constexpr uint32_t cube_vertex_count    = 24;
	constexpr uint32_t cube_index_count     = 36;
	constexpr uint32_t cube_face_cout       = 6;

	constexpr glm::vec4 white_color     { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr glm::vec4 water_tint_color{ 0.0f, 0.0f, 0.7f, 1.0f };
	constexpr glm::vec4 clear_color     { 0.8f, 0.8f, 1.0f, 1.0f };

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

	struct RendererData
	{
		Camera* CurrentCamera = nullptr;
		std::shared_ptr<Camera> SpareCamera; // spare camera just in case, only care about ortho projection
		UniformBuffer WorldDataUniformBuffer;
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