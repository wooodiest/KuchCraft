#include "Renderer.h"

#include <array>
#include <iostream>
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
		static const uint32_t MaxTextureSlots = 32;

		uint32_t VertexArray;
		uint32_t VertexBuffer;
		uint32_t IndexBuffer;
		Shader   Shader;

		uint32_t VertexOffset = 0;

		std::array<uint32_t, MaxTextureSlots> TextureSlots;
		std::array<uint32_t, number_of_blocks> Textures;
		uint32_t TextureSlotIndex = 1;

	};
	static RendererData s_Data;

	// Declarations

	void Renderer::BeginScene(const Camera& camera)
	{
		// Set uniform: view-projection matrix
		s_Data.Shader.Bind();
		s_Data.Shader.SetMat4("u_ViewProjection", camera.GetViewProjection());
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::ResetStats()
	{
		s_Stats.DrawCalls = 0;
		s_Stats.Quads     = 0;
	}

	void Renderer::FlushChunk(uint32_t& indexCount, uint32_t& texturesIndex, const std::vector<Vertex>& vertices)
	{
		if (indexCount != 0)
		{
			uint32_t vertexCount = (uint32_t)(indexCount / quad_index_count) * quad_vertex_count;
			glBindVertexArray(s_Data.VertexArray);
			glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBuffer);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex), &vertices[s_Data.VertexOffset]);

			s_Data.VertexOffset += vertexCount;
			s_Stats.Quads += vertexCount / 4;
			// Bind textures
			// s_Data.Shader.Bind();
			for (uint32_t i = 1; i < texturesIndex; i++)
				glBindTextureUnit(i, s_Data.TextureSlots[i]);

			// Draw elements		
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IndexBuffer);
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

			// Update stats
			Renderer::s_Stats.DrawCalls++;
		}
		
		indexCount    = 0;
		texturesIndex = 1;
	}

	void Renderer::DrawChunk(Chunk* chunk)
	{
		auto& vertices = chunk->GetDrawList();
		auto& textures = chunk->GetTextureList();

		uint32_t indexCount       = 0;
		uint32_t textureSlotIndex = 1;

		for (int i = 0; i < vertices.size(); i += 4)
		{
			float textureIndex = 0.0f;
			uint32_t texture =  s_Data.Textures[(int)textures[i >> 2]];
			for (uint32_t j = 1; j < s_Data.MaxTextureSlots; j++)
			{
				if (j < textureSlotIndex && s_Data.TextureSlots[j] == texture)
				{
					textureIndex = (float)j;
					break;
				}
			}

			if (textureIndex == 0.0f) // Is every slot occupied or we have new texture ?
			{
				if (textureSlotIndex >= s_Data.MaxTextureSlots)
					FlushChunk(indexCount, textureSlotIndex, vertices);

				textureIndex = (float)textureSlotIndex;
				s_Data.TextureSlots[textureSlotIndex] = texture;
				textureSlotIndex++;
			}

			vertices[i + 0].TexIndex = textureIndex;
			vertices[i + 1].TexIndex = textureIndex;
			vertices[i + 2].TexIndex = textureIndex;
			vertices[i + 3].TexIndex = textureIndex;

			indexCount  += quad_index_count;
		}

		FlushChunk(indexCount, textureSlotIndex, vertices);
		s_Data.VertexOffset = 0;
	}

	void Renderer::DrawChunkTransparent(Chunk* chunk)
	{
		auto& vertices = chunk->GetDrawListTransparent();
		auto& textures = chunk->GetTextureListTransparent();

		uint32_t indexCount = 0;
		uint32_t textureSlotIndex = 1;

		for (int i = 0; i < vertices.size(); i += 4)
		{
			float textureIndex = 0.0f;
			uint32_t texture = s_Data.Textures[(int)textures[i >> 2]];
			for (uint32_t j = 1; j < s_Data.MaxTextureSlots; j++)
			{
				if (j < textureSlotIndex && s_Data.TextureSlots[j] == texture)
				{
					textureIndex = (float)j;
					break;
				}
			}

			if (textureIndex == 0.0f) // Is every slot occupied or we have new texture ?
			{
				if (textureSlotIndex >= s_Data.MaxTextureSlots)
					FlushChunk(indexCount, textureSlotIndex, vertices);

				textureIndex = (float)textureSlotIndex;
				s_Data.TextureSlots[textureSlotIndex] = texture;
				textureSlotIndex++;
			}

			vertices[i + 0].TexIndex = textureIndex;
			vertices[i + 1].TexIndex = textureIndex;
			vertices[i + 2].TexIndex = textureIndex;
			vertices[i + 3].TexIndex = textureIndex;

			indexCount += quad_index_count;
		}

		FlushChunk(indexCount, textureSlotIndex, vertices);
		s_Data.VertexOffset = 0;
	}

	void Renderer::Init()
	{
		// Setup viewport
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

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
		s_Data.Shader.Create("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");
		s_Data.Shader.Bind();

		// Textures
		int samplers[s_Data.MaxTextureSlots];
		for (int i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;
		s_Data.Shader.SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		LoadTextureAtlas();

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void Renderer::ShutDown()
	{
		// free memmory
		glDeleteBuffers(1, &s_Data.IndexBuffer);
		glDeleteBuffers(1, &s_Data.VertexBuffer);
		glDeleteVertexArrays(1, &s_Data.VertexArray);
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