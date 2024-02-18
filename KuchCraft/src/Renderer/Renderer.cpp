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

	RendererStatistics Renderer::s_Stats;

	// Renderer data
	struct RendererData
	{

		// Rendering blocks
		uint32_t VertexArray;
		uint32_t VertexBuffer;
		uint32_t IndexBuffer;
		Shader   DefaultShader;

		std::array<uint32_t, number_of_blocks> Textures;
		uint32_t TextureSlotIndex = 1;

		// Rendering skybox
		uint32_t SkyboxVertexArray;
		uint32_t SkyboxVertexBuffer;
		uint32_t SkyboxTexture;
		Shader   SkyboxShader;
	};
	static RendererData s_Data;

	// Declarations
	void Renderer::BeginScene(const Camera& camera)
	{
		// Set uniform: view-projection matrix
		s_Data.DefaultShader.Bind();
		s_Data.DefaultShader.SetMat4("u_ViewProjection", camera.GetViewProjection());
		s_Data.DefaultShader.SetFloat4("u_TintColor", { 1.0f, 1.0f, 1.0f, 1.0f });
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::BeginRenderingWater()
	{
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glDepthFunc(GL_LEQUAL);
	}

	void Renderer::EndRenderingWater()
	{
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDepthFunc(GL_LESS);
	}

	void Renderer::ResetStats()
	{
		s_Stats.DrawCalls = 0;
		s_Stats.Quads     = 0;
	}

	void Renderer::DrawChunk(Chunk* chunk)
	{
		uint32_t vertexOffset = 0;
		auto& vertices  = chunk->GetDrawList();
		auto& chunkDrawList = chunk->GetChunkDrawList(); // rename to drawlist

		for (uint32_t i = 0; i < chunkDrawList.GetDrawCallCount(); i++)
		{
			uint32_t indexCount = chunkDrawList.GetIndexCount(i);
			if (indexCount != 0)
			{
				uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;

				glBindVertexArray(s_Data.VertexArray);
				glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBuffer);
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), &vertices[vertexOffset]);

				vertexOffset += vertexCount;

				// Bind textures
				uint32_t textures = chunkDrawList.GetTextureCount(i);
				for (uint32_t j = 0; j < textures; j++)
					glBindTextureUnit(j, chunkDrawList.GetTexture(i, j));

				// Draw elements		
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IndexBuffer);
				glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

				// Update stats
				s_Stats.DrawCalls++;
				s_Stats.Quads += vertexCount / 4;
			}
		}
	}

	void Renderer::DrawChunkWater(Chunk* chunk)
	{
		auto&    vertices   = chunk->GetDrawListWater();
		uint32_t indexCount = vertices.size() * triangle_index_count;


		if (indexCount != 0)
		{
			uint32_t vertexCount = (uint32_t)(indexCount / quad_index_count) * quad_vertex_count;
			glBindVertexArray(s_Data.VertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), &vertices[0]);

			s_Stats.Quads += vertexCount / 4;
			// Bind textures
			glBindTextureUnit((uint32_t)water_texture_slot, GetTexture(BlockType::Water));

			// Draw elements		
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IndexBuffer);
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

			// Update stats
			Renderer::s_Stats.DrawCalls++;
		}
	}

	void Renderer::DrawSkybox(const Camera& camera)
	{
		s_Data.SkyboxShader.Bind();
		s_Data.SkyboxShader.SetMat4("u_ViewProjection0", camera.GetSkyboxProjection());
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);

		glBindVertexArray(s_Data.SkyboxVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.SkyboxVertexBuffer);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_Data.SkyboxTexture);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IndexBuffer);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

		glCullFace(GL_BACK);
		glDepthFunc(GL_LESS);
	}

	void Renderer::Init()
	{
		// Setup viewport
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);

		glEnable(GL_CULL_FACE);	
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		// Skybox
		glGenVertexArrays(1, &s_Data.SkyboxVertexArray);
		glBindVertexArray(s_Data.SkyboxVertexArray);

		glGenBuffers(1, &s_Data.SkyboxVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.SkyboxVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		s_Data.SkyboxShader.Create("assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
		s_Data.SkyboxShader.Bind();
		s_Data.SkyboxShader.SetInt("u_CubemapTexture", 0);
		s_Data.SkyboxTexture = LoadSkyboxTexture();
		// Create vertex array
		glGenVertexArrays(1, &s_Data.VertexArray);
		glBindVertexArray(s_Data.VertexArray);

		// Create vertex buffer and layout setup
		constexpr int max_quads    = chunk_size_XZ * chunk_size_XZ * chunk_size_Y * 6;
		constexpr int max_indices  = max_quads * 6;
		constexpr int max_vertices = max_quads * 4;

		glGenBuffers(1, &s_Data.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, max_vertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// Index buffer
		uint32_t* indices = new uint32_t[max_indices];
		uint32_t  offset = 0;
		for (uint32_t i = 0; i < max_indices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}
		glGenBuffers(1, &s_Data.IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		delete[] indices;

		// Shader
		s_Data.DefaultShader.Create("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");
		s_Data.DefaultShader.Bind();

		// Textures
		int samplers[max_texture_slots];
		for (int i = 0; i < max_texture_slots; i++)
			samplers[i] = i;
		s_Data.DefaultShader.SetIntArray("u_Textures", samplers, max_texture_slots);

		LoadTextureAtlas();
	}

	void Renderer::ShutDown()
	{
		glDeleteBuffers(1, &s_Data.IndexBuffer);
		glDeleteBuffers(1, &s_Data.VertexBuffer);
		glDeleteVertexArrays(1, &s_Data.VertexArray);

		glDeleteBuffers(1, &s_Data.SkyboxVertexBuffer);
		glDeleteVertexArrays(1, &s_Data.SkyboxVertexArray);
	}

	uint32_t Renderer::GetTexture(BlockType type)
	{
		return s_Data.Textures[(int)type];
	}

	void Renderer::SetTrianglesVisibility(bool status)
	{
		if (status)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
			s_Data.Textures[(int)texture.first] = LoadTextureToAtals(path);
		}
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	void Renderer::SetTintColor(const glm::vec4& color)
	{
		s_Data.DefaultShader.SetFloat4("u_TintColor", color);
	}

	uint32_t Renderer::LoadSkyboxTexture()
	{
		const GLenum types[6] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};
		const std::string paths[6] =
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

		for (uint32_t i = 0; i < 6; i++)
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