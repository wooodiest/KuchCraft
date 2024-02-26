#include "Renderer.h"

#include <array>
#include <iostream>
#include <unordered_map>
#include <glad/glad.h>
#include <stb_image.h>

#include "Shader.h"
#include "Core/Application.h"
#include "World/World.h"

namespace KuchCraft {

	RendererData        Renderer::s_RendererData;
	RendererStatistics  Renderer::s_Stats;
	RendererChunkData   Renderer::s_ChunkData;
	RendererSkyboxData  Renderer::s_SkyboxData;
	RendererWaterData   Renderer::s_WaterData;

	void Renderer::Init()
	{
		PrepareRenderer();

		PrepareChunkRendering();
		PrepareSkyboxRendering();
		PrepareWaterRendering();

		LoadTextureAtlas();
	}

	void Renderer::ShutDown()
	{
		// RendererData
		glDeleteBuffers(1, &s_RendererData.QuadIndexBuffer);
		glDeleteBuffers(1, &s_RendererData.UniformBuffer);

		// RendererChunkData
		glDeleteBuffers(1, &s_ChunkData.VertexBuffer);
		glDeleteVertexArrays(1, &s_ChunkData.VertexArray);
		
		// RendererStatistics
		ResetStats();
	}

	void Renderer::BeginFrame()
	{
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ResetStats();
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		// Set uniform buffer
		UniformBuffer buffer{
			camera.GetViewProjection(),
			camera.GetAbsoluteViewProjection(),
			s_RendererData.TintStatus ? water_tint_color : white_color
		};
		glNamedBufferSubData(s_RendererData.UniformBuffer, 0, sizeof(buffer), &buffer);
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::ResetStats()
	{
		s_Stats.DrawCalls = 0;
		s_Stats.Quads     = 0;
	}

	void Renderer::BeginChunk()
	{
		s_ChunkData.Shader.Bind();
	}

	void Renderer::RenderChunk(Chunk* chunk)
	{
		uint32_t vertexOffset = 0;
		const auto& drawList = chunk->GetDrawList();

		for (uint32_t i = 0; i < drawList.GetDrawCallCount(); i++)
		{
			uint32_t indexCount = drawList.GetIndexCount(i);
			if (indexCount != 0)
			{
				uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;

				glBindVertexArray(s_ChunkData.VertexArray);
				glBindBuffer(GL_ARRAY_BUFFER, s_ChunkData.VertexBuffer);
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), drawList.GetVerticesPtr(vertexOffset));
				
				vertexOffset += vertexCount;

				// Bind textures
				uint32_t textures = drawList.GetTextureCount(i);
				for (uint32_t j = 0; j < textures; j++)
					glBindTextureUnit(j, drawList.GetTexture(i, j));

				// Draw elements		
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIndexBuffer);
				glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

				// Update stats
				s_Stats.DrawCalls++;
				s_Stats.Quads += vertexCount / quad_vertex_count;
			}
		}
	}

	void Renderer::EndChunk()
	{
	}

	void Renderer::BeginSkybox()
	{
		s_SkyboxData.Shader.Bind();

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
	}

	void Renderer::RenderSkybox()
	{
		glBindVertexArray(s_SkyboxData.VertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_SkyboxData.VertexBuffer);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_SkyboxData.Texture);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIndexBuffer);
		glDrawElements(GL_TRIANGLES, cube_face_cout * quad_index_count , GL_UNSIGNED_INT, nullptr);

		// Update stats
		s_Stats.DrawCalls++;
		s_Stats.Quads += cube_face_cout;
	}

	void Renderer::EndSkybox()
	{
		glCullFace(GL_BACK);
		glDepthFunc(GL_LESS);
	}

	void Renderer::BeginWater()
	{
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);

		s_WaterData.Shader.Bind();
	}

	void Renderer::RenderWater(Chunk* chunk)
	{
		const auto& drawList = chunk->GetDrawList();
		uint32_t indexCount  = drawList.GetWaterVertices().size() / quad_vertex_count * quad_index_count;

		if (indexCount != 0)
		{
			uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;

			glBindVertexArray(s_WaterData.VertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, s_WaterData.VertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(WaterVertex), drawList.GetWaterVerticesPtr());

			glBindTextureUnit(water_texture_slot, GetTexture(BlockType::Water));

			// Draw elements		
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIndexBuffer);
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

			// Update stats
			s_Stats.DrawCalls++;
			s_Stats.Quads += vertexCount / quad_vertex_count;
		}
	}

	void Renderer::EndWater()
	{
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}

	void Renderer::PrepareRenderer()
	{
		// Setup viewport
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		glViewport(0, 0, width, height);

		// Enable...
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		// QuadIndexBuffer
		uint32_t* indices = new uint32_t[max_indices_in_chunk];
		uint32_t  offset = 0;
		for (uint32_t i = 0; i < max_indices_in_chunk; i += quad_index_count)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}
		glGenBuffers(1, &s_RendererData.QuadIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices_in_chunk * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		delete[] indices;

		// Uniform buffer
		constexpr uint32_t binding = 0;
		glCreateBuffers(1, &s_RendererData.UniformBuffer);
		glNamedBufferData(s_RendererData.UniformBuffer, sizeof(UniformBuffer), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, s_RendererData.UniformBuffer);
	}

	void Renderer::PrepareChunkRendering()
	{
		// Create vertex array
		glGenVertexArrays(1, &s_ChunkData.VertexArray);
		glBindVertexArray(s_ChunkData.VertexArray);

		// Create vertex buffer
		glGenBuffers(1, &s_ChunkData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_ChunkData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, max_vertices_in_chunk * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		// Setup layout
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// Shader
		s_ChunkData.Shader.Create("assets/shaders/chunk.vert.glsl", "assets/shaders/chunk.frag.glsl");
		s_ChunkData.Shader.Bind();

		// Set texture slots
		int samplers[max_texture_slots];
		for (int i = 0; i < max_texture_slots; i++)
			samplers[i] = i;
		s_ChunkData.Shader.SetIntArray("u_Textures", samplers, max_texture_slots);

	}

	void Renderer::PrepareSkyboxRendering()
	{
		// Create vertex array
		glGenVertexArrays(1, &s_SkyboxData.VertexArray);
		glBindVertexArray(s_SkyboxData.VertexArray);

		// Create vertex buffer and layout setup
		glGenBuffers(1, &s_SkyboxData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_SkyboxData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

		// Setup layout
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Shader
		s_SkyboxData.Shader.Create("assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
		s_SkyboxData.Shader.Bind();

		// Set texture slot
		s_SkyboxData.Shader.SetInt("u_CubemapTexture", 0);

		// Load cube map texture
		s_SkyboxData.Texture = LoadSkyboxTexture();
	}

	void Renderer::PrepareWaterRendering()
	{
		// Create vertex array
		glGenVertexArrays(1, &s_WaterData.VertexArray);
		glBindVertexArray(s_WaterData.VertexArray);

		// Create vertex buffer
		glGenBuffers(1, &s_WaterData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_WaterData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, max_vertices_in_chunk * sizeof(WaterVertex), nullptr, GL_DYNAMIC_DRAW);

		// Setup layout
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WaterVertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(WaterVertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Shader
		s_WaterData.Shader.Create("assets/shaders/water.vert.glsl", "assets/shaders/water.frag.glsl");
		s_WaterData.Shader.Bind();

		// Set texture slot
		s_ChunkData.Shader.SetInt("u_Texture", water_texture_slot);
	}

	uint32_t Renderer::GetTexture(BlockType type)
	{
		return s_RendererData.Textures[(int)type];
	}

	void Renderer::ShowTriangles(bool status)
	{
		if (status)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::SetWaterTintStatus(bool status)
	{
		s_RendererData.TintStatus = status;
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	void Renderer::LoadTextureAtlas()
	{
		// Pair blocktype with texture
		std::unordered_map<BlockType, std::string>  blockTexturePathsAtlas;
		blockTexturePathsAtlas[BlockType::Bedrock]       = "bedrock";
		blockTexturePathsAtlas[BlockType::Bricks]        = "bricks";
		blockTexturePathsAtlas[BlockType::CoalOre]       = "coal_ore";
		blockTexturePathsAtlas[BlockType::Cobblestone]   = "cobblestone";
		blockTexturePathsAtlas[BlockType::CraftingTable] = "crafting_table";
		blockTexturePathsAtlas[BlockType::DiamondOre]    = "diamond_ore";
		blockTexturePathsAtlas[BlockType::Dioryte]       = "dioryte";
		blockTexturePathsAtlas[BlockType::Dirt]          = "dirt";
		blockTexturePathsAtlas[BlockType::Furnace]       = "furnace";
		blockTexturePathsAtlas[BlockType::Granite]       = "granite";
		blockTexturePathsAtlas[BlockType::Grass]         = "grass";
		blockTexturePathsAtlas[BlockType::Gravel]        = "gravel";
		blockTexturePathsAtlas[BlockType::IronOre]       = "iron_ore";
		blockTexturePathsAtlas[BlockType::OakLog]        = "oak_log";
		blockTexturePathsAtlas[BlockType::OakPlanks]     = "oak_planks";
		blockTexturePathsAtlas[BlockType::Sand]          = "sand";
		blockTexturePathsAtlas[BlockType::Stone]         = "stone";
		blockTexturePathsAtlas[BlockType::StoneBrick]    = "stone_brick";
		blockTexturePathsAtlas[BlockType::Water]         = "water";
		blockTexturePathsAtlas[BlockType::Leaves]        = "leaves";

		// Load all textures to gpu
		const std::string mainPath  = "assets/textures/";
		const std::string extension = ".png";
		for (const auto& texture : blockTexturePathsAtlas)
		{
			std::string path = mainPath + texture.second + extension;
			s_RendererData.Textures[(int)texture.first] = LoadTextureToAtals(path);
		}
	}

	uint32_t Renderer::LoadSkyboxTexture()
	{
		const GLenum types[cube_face_cout] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};
		const std::string paths[cube_face_cout] =
		{
			"assets/skybox/xpos.png",
			"assets/skybox/xneg.png",
			"assets/skybox/ypos.png",
			"assets/skybox/yneg.png",
			"assets/skybox/zpos.png",
			"assets/skybox/zneg.png"
		}; 
		uint32_t texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

		for (uint32_t i = 0; i < cube_face_cout; i++)
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(0);
			unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &channels, 4);

			glTexImage2D(types[i], 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			stbi_image_free(data);
		}

		return texture;
	}

	uint32_t Renderer::LoadTextureToAtals(const std::string& path)
	{
		uint32_t texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

		return texture;
	}


}