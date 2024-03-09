#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <tuple>

#include "Core/Timer.h"

#include "World/Block.h"
#include "World/WorldData.h"

#include "Renderer/Shader.h"

namespace KuchCraft {

	// Constances
	constexpr bool opengl_logs = true;

	// GPU
	constexpr uint32_t max_texture_slots     = 32;
	constexpr uint32_t default_texture_slot  = 0;

	// Text rendering
	constexpr float default_font_size      = 24.0f;
	constexpr float default_font_spacing   = 1.3f;
	constexpr glm::vec4 default_font_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	constexpr uint32_t max_text_uniform_array_limit = 400;
	constexpr uint32_t font_characters_count        = 128;
	constexpr uint32_t font_texture_size            = 256;

	// Vertex data
	constexpr uint32_t triangle_index_count = 6;
	constexpr uint32_t quad_index_count     = 6;
	constexpr uint32_t quad_vertex_count    = 4;
	constexpr uint32_t quad_vertex_count_a  = 6;
	constexpr uint32_t cube_vertex_count    = 24;
	constexpr uint32_t cube_face_cout       = 6;

	// Utils
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

	// Renderer data
	struct RendererStatistics
	{
		uint32_t DrawCalls = 0;
		uint32_t Quads     = 0;

		Timer RenderTimer;
		Timer ChunkTimer;
		Timer SkyboxTimer;
		Timer WaterTimer;
		Timer TextTimer;
	};

	struct UniformWorldBuffer
	{
		glm::mat4 ViewProjection;
		glm::mat4 AbsoluteViewProjection;
		glm::mat4 OrthoProjection;
		glm::vec4 TintColor;
	};

	struct UniformTextBuffer
	{
		struct Text
		{
			glm::mat4 Transform;
			glm::vec4 Letter{ 0.0f }; // weird alignment
		} Text[max_text_uniform_array_limit];

		glm::vec4 Color{ default_font_color };
	};

	struct RendererData
	{
		uint32_t Textures[absolute_number_of_blocks];
		uint32_t QuadIndexBuffer = 0;
		uint32_t UniformBuffer   = 0;
		bool     TintStatus      = false;

		Shader   Shader;
		uint32_t VertexBuffer = 0;
		uint32_t VertexArray  = 0;

		struct FrameBuffer
		{
			uint32_t RendererID = 0;
			uint32_t ColorAttachment = 0;
			uint32_t DepthAttachment = 0;
		} RenderOutputFrameBuffer;
		uint32_t DefaultFrameBufferRendererID = 0;
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

	struct RendererWaterData
	{
		uint32_t VertexArray  = 0;
		uint32_t VertexBuffer = 0;
		Shader   Shader;
	};

	struct RendererTextCharacter
	{
		uint32_t   ID;
		uint32_t   Advance;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
	};

	struct RendererTextData
	{
		const char* FontPath  = "assets/fonts/Roboto-Regular.ttf";
		std::vector<std::tuple<std::string, glm::vec2, glm::vec4, float, float>> Data;

		uint32_t UniformBuffer   = 0;
		uint32_t VertexArray     = 0;
		uint32_t VertexBuffer    = 0;
		uint32_t Texture         = 0;
		Shader   Shader;
	
		std::map<char, RendererTextCharacter> Characters;
	};

	// Renderer data utils
	constexpr Vertex_P2C2 screen_vertices[quad_vertex_count_a]
	{
		Vertex_P2C2{{ -1.0f, -1.0f }, { 0.0f, 0.0f }},
		Vertex_P2C2{{  1.0f, -1.0f }, { 1.0f, 0.0f }},
		Vertex_P2C2{{  1.0f,  1.0f }, { 1.0f, 1.0f }},

		Vertex_P2C2{{ -1.0f, -1.0f }, { 0.0f, 0.0f }},
		Vertex_P2C2{{  1.0f,  1.0f }, { 1.0f, 1.0f }},
		Vertex_P2C2{{ -1.0f,  1.0f }, { 0.0f, 1.0f }},
	};

	// Cube - quad vertices
	constexpr Vertex vertices_bottom[quad_vertex_count]
	{
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.0f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 0.0f,  0.0f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.0f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.25f, 0.5f }, 0.0f }
	};
	constexpr Vertex vertices_top[quad_vertex_count]
	{
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 0.25f, 0.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.5f,  0.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.25f, 0.5f }, 0.0f }
	};
	constexpr Vertex vertices_front[quad_vertex_count]
	{
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 0.0f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.5f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.25f, 1.0f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 0.0f,  1.0f }, 0.0f }
	};
	constexpr Vertex vertices_right[quad_vertex_count]
	{
		Vertex{{ 1.0f, 0.0f, 1.0f }, { 0.25f, 0.5f }, 0.0f },
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  1.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 1.0f }, { 0.25f, 1.0f }, 0.0f }
	};
	constexpr Vertex vertices_behind[quad_vertex_count]
	{
		Vertex{{ 1.0f, 0.0f, 0.0f }, { 0.5f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.75f, 0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.75f, 1.0f }, 0.0f },
		Vertex{{ 1.0f, 1.0f, 0.0f }, { 0.5f,  1.0f }, 0.0f }
	};
	constexpr Vertex vertices_left[quad_vertex_count]
	{
		Vertex{{ 0.0f, 0.0f, 0.0f }, { 0.75f, 0.5f }, 0.0f },
		Vertex{{ 0.0f, 0.0f, 1.0f }, { 1.0f,  0.5f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 1.0f }, { 1.0f,  1.0f }, 0.0f },
		Vertex{{ 0.0f, 1.0f, 0.0f }, { 0.75f, 1.0f }, 0.0f }
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