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

	RendererData       Renderer::s_RendererData;
	RendererStatistics Renderer::s_Stats;
	RendererChunkData  Renderer::s_ChunkData;
	Camera*            Renderer::s_Camera = nullptr;

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

		PrepareRenderer();
		PrepareChunkRendering();

		LoadTextureAtlas();
	}

	void Renderer::ShutDown()
	{
		// RendererData
		glDeleteBuffers(1, &s_RendererData.QuadIndexBuffer);

		// RendererChunkData
		glDeleteBuffers(1, &s_ChunkData.VertexBuffer);
		glDeleteVertexArrays(1, &s_ChunkData.VertexArray);
		
		// RendererStatistics
		ResetStats();
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		s_Camera = (Camera*)(&camera); // temporary
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
		s_ChunkData.Shader.SetMat4("u_ViewProjection", s_Camera->GetViewProjection());
	}

	void Renderer::RenderChunk(Chunk* chunk)
	{
		uint32_t vertexOffset = 0;
		auto& drawList = chunk->GetDrawList(); // rename to drawlist

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
	}

	void Renderer::RenderSkybox()
	{
	}

	void Renderer::EndSkybox()
	{
	}

	void Renderer::PrepareRenderer()
	{
		// QuadIndexBuffer
		uint32_t* indices = new uint32_t[max_indices_in_chunk];
		uint32_t  offset = 0;
		for (uint32_t i = 0; i < max_indices_in_chunk; i += 6)
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
	}

	void Renderer::PrepareChunkRendering()
	{
		// Create vertex array
		glGenVertexArrays(1, &s_ChunkData.VertexArray);
		glBindVertexArray(s_ChunkData.VertexArray);

		// Create vertex buffer and layout setup
		glGenBuffers(1, &s_ChunkData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_ChunkData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, max_vertices_in_chunk * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// Shader
		s_ChunkData.Shader.Create("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");
		s_ChunkData.Shader.Bind();

		// Set texture slots
		int samplers[max_texture_slots];
		for (int i = 0; i < max_texture_slots; i++)
			samplers[i] = i;
		s_ChunkData.Shader.SetIntArray("u_Textures", samplers, max_texture_slots);

	}

	void Renderer::PrepareSkyboxRendering()
	{
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
		s_ChunkData.Shader.Bind();
		s_ChunkData.Shader.SetFloat4("u_TintColor", status ? water_tint_color : white_color);
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