#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "World/Block.h"
#include "World/WorldData.h"

#include "Renderer/Shader.h"

namespace KuchCraft {

	constexpr uint32_t max_texture_slots  = 32;
	constexpr uint32_t water_texture_slot = 0;

	constexpr uint32_t triangle_index_count = 6;
	constexpr uint32_t quad_index_count     = 6;
	constexpr uint32_t quad_vertex_count    = 4;
	constexpr uint32_t cube_vertex_count    = 24;
	constexpr uint32_t cube_face_cout       = 6;

	constexpr glm::vec4 white_color     { 1.0f, 1.0f, 1.0f, 1.0f };
	constexpr glm::vec4 water_tint_color{ 0.0f, 0.0f, 1.0f, 1.0f };
	constexpr glm::vec4 clear_color     { 0.8f, 0.8f, 1.0f, 1.0f };
	
	constexpr glm::vec3 quad_initial_displacement{ 0.5f, 0.5f, 0.5f };

	constexpr int max_quads_in_chunk    = chunk_size_XZ * chunk_size_XZ * chunk_size_Y * cube_face_cout;
	constexpr int max_indices_in_chunk  = max_quads_in_chunk * 6;
	constexpr int max_vertices_in_chunk = max_quads_in_chunk * 4;

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		float     TexSlot;

	};

	struct RendererStatistics
	{
		uint32_t DrawCalls = 0;
		uint32_t Quads     = 0;
	};

	struct UniformBuffer
	{
		glm::mat4 ViewProjection;
		glm::mat4 AbsoluteViewProjection;
		glm::vec4 TintColor;
	};

	struct RendererData
	{
		uint32_t Textures[absolute_number_of_blocks];
		uint32_t QuadIndexBuffer = 0;
		uint32_t UniformBuffer   = 0;
		bool     TintStatus      = false;
	}; 

	struct RendererChunkData
	{
		uint32_t VertexArray  = 0;
		uint32_t VertexBuffer = 0;
		Shader   Shader;
	};

	struct RendererSkyboxData
	{
		uint32_t VertexArray  = 0;
		uint32_t VertexBuffer = 0;
		uint32_t Texture      = 0;
		Shader   Shader;
	};

	// Cube - quad vertices
	constexpr Vertex vertices_bottom[quad_vertex_count]
	{
		Vertex{{  0.5f, -0.5f,  0.5f }, { 0.25f, 0.0f }, 0.0f },
		Vertex{{ -0.5f, -0.5f,  0.5f }, { 0.0f,  0.0f }, 0.0f },
		Vertex{{ -0.5f, -0.5f, -0.5f }, { 0.0f,  0.5f }, 0.0f },
		Vertex{{  0.5f, -0.5f, -0.5f }, { 0.25f, 0.5f }, 0.0f }
	};
	constexpr Vertex vertices_top[quad_vertex_count]
	{
		Vertex{{ -0.5f,  0.5f,  0.5f }, { 0.25f, 0.0f }, 0.0f },
		Vertex{{  0.5f,  0.5f,  0.5f }, { 0.5f,  0.0f }, 0.0f },
		Vertex{{  0.5f,  0.5f, -0.5f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ -0.5f,  0.5f, -0.5f }, { 0.25f, 0.5f }, 0.0f }
	};
	constexpr Vertex vertices_front[quad_vertex_count]
	{
		Vertex{{ -0.5f, -0.5f,  0.5f }, { 0.0f,  0.5f }, 0.0f },
		Vertex{{  0.5f, -0.5f,  0.5f }, { 0.25f, 0.5f }, 0.0f },
		Vertex{{  0.5f,  0.5f,  0.5f }, { 0.25f, 1.0f }, 0.0f },
		Vertex{{ -0.5f,  0.5f,  0.5f }, { 0.0f,  1.0f }, 0.0f }
	};
	constexpr Vertex vertices_right[quad_vertex_count]
	{
		Vertex{{  0.5f, -0.5f,  0.5f }, { 0.25f, 0.5f }, 0.0f },
		Vertex{{  0.5f, -0.5f, -0.5f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{  0.5f,  0.5f, -0.5f }, { 0.5f,  1.0f }, 0.0f },
		Vertex{{  0.5f,  0.5f,  0.5f }, { 0.25f, 1.0f }, 0.0f }
	};
	constexpr Vertex vertices_behind[quad_vertex_count]
	{
		Vertex{{  0.5f, -0.5f, -0.5f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ -0.5f, -0.5f, -0.5f }, { 0.75f, 0.5f }, 0.0f },
		Vertex{{ -0.5f,  0.5f, -0.5f }, { 0.75f, 1.0f }, 0.0f },
		Vertex{{  0.5f,  0.5f, -0.5f }, { 0.5f,  1.0f }, 0.0f }
	};
	constexpr Vertex vertices_left[quad_vertex_count]
	{
		Vertex{{ -0.5f, -0.5f, -0.5f }, { 0.75f, 0.5f }, 0.0f },
		Vertex{{ -0.5f, -0.5f,  0.5f }, { 1.0f,  0.5f }, 0.0f },
		Vertex{{ -0.5f,  0.5f,  0.5f }, { 1.0f,  1.0f }, 0.0f },
		Vertex{{ -0.5f,  0.5f, -0.5f }, { 0.75f, 1.0f }, 0.0f }
	};

	constexpr float skybox_vertices[] =
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

}