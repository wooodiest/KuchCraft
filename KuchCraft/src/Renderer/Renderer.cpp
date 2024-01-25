#include "Renderer.h"

#include <memory>
#include <array>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <stb_image.h>
#include "Core/Application.h"

namespace KuchCraft {

	Renderer* Renderer::s_Instance = nullptr;

	// Cube vertex
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		float     TexIndex;
	};

	// Cube vertex data
	constexpr size_t cube_vertex_count  = 24;
	constexpr size_t cube_indices_count = 36;
	constexpr size_t cube_triangles     = 12 / 2; // Face culling
	constexpr glm::vec4 cube_vertex_positions[cube_vertex_count] =
	{
		// Bottom
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		// Top
		{ 0.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		// Front
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f, 1.0f },
		// Right
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		// Behind
		{ 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 0.0f, 1.0f },
		// Left
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f }
	};
	constexpr glm::vec2 cube_texture_coordinates[cube_vertex_count] = {
		// Bottom
		{ 0.25f, 0.0f }, { 0.0f,  0.0f }, { 0.0f,  0.5f }, { 0.25f, 0.5f },
		// Top
		{ 0.25f, 0.0f }, { 0.5f,  0.0f }, { 0.5f,  0.5f }, { 0.25f, 0.5f },
		// Front
		{ 0.0f,  0.5f }, { 0.25f, 0.5f }, { 0.25f, 1.0f }, { 0.0f,  1.0f },
		// Right
		{ 0.25f, 0.5f }, { 0.5f,  0.5f }, { 0.5f,  1.0f }, { 0.25f, 1.0f },
		// Behind
		{ 0.5f,  0.5f }, { 0.75f, 0.5f }, { 0.75f, 1.0f }, { 0.5f,  1.0f },
		// Left
		{ 0.75f, 0.5f }, { 1.0f,  0.5f }, { 1.0f,  1.0f }, { 0.75f, 1.0f },
	};

	// Renderer data
	struct RendererData
	{
		static const uint32_t MaxQuads        = 20004; // % 6 == 0
		static const uint32_t MaxVertices     = MaxQuads * 4;
		static const uint32_t MaxIndices      = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 48;

		uint32_t VertexArray;
		uint32_t VertexBuffer;
		uint32_t IndexBuffer;
		Shader   Shader;

		Vertex*  VertexBufferBase = nullptr;
		Vertex*  VertexBufferPtr = nullptr;
		uint32_t IndexCount = 0;

		std::array<uint32_t, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;
	};
	static RendererData s_Data;

	// Declarations
	Renderer::Renderer()
	{
		s_Instance = this;

		// Setup viewport
		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
		// Face culling (to check)
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		// Create vertex array
		glGenVertexArrays(1, &s_Data.VertexArray);
		glBindVertexArray(s_Data.VertexArray);

		// Create vertex buffer and layout setup
		glGenBuffers(1, &s_Data.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, s_Data.MaxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		s_Data.VertexBufferBase = new Vertex[s_Data.MaxVertices];

		// Index buffer
		uint32_t* indices = new uint32_t[s_Data.MaxIndices];
		uint32_t  offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, s_Data.MaxIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);
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

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	Renderer::~Renderer()
	{
		ShutDown();
	}

	void Renderer::Flush()
	{
		if (s_Data.IndexCount == 0) // Nothing to draw
			return;

		// Upload data
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.VertexBufferPtr - (uint8_t*)s_Data.VertexBufferBase);
		glBindVertexArray(s_Data.VertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, s_Data.VertexBufferBase);

		// Bind textures
		s_Data.Shader.Bind();
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
			glBindTextureUnit(i, s_Data.TextureSlots[i]);
		
		// Draw elements		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IndexBuffer);
		glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr);

		// Update stats
		m_Stats.DrawCalls++;
	}

	void Renderer::StartBatch()
	{
		// Reset current data
		s_Data.IndexCount = 0;
		s_Data.VertexBufferPtr = s_Data.VertexBufferBase;
		s_Data.TextureSlotIndex = 1;
	}

	void Renderer::NextBatch()
	{
		// Draw and reset
		Flush();
		StartBatch();
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		// Set uniform: view-projection matrix
		s_Data.Shader.Bind();
		s_Data.Shader.SetMat4("u_ViewProjection", camera.GetViewProjection());

		// Start collecting data to draw
		StartBatch();
	}

	void Renderer::EndScene()
	{
		// Render remaining data
		Flush();

		// Reset and print stats
		std::cout << "Draw calls: " << m_Stats.DrawCalls << ", Triangles: " << m_Stats.Triangles << std::endl;
		m_Stats.DrawCalls = 0;
		m_Stats.Triangles = 0;
	}

	void Renderer::DrawCube(const glm::vec3& position, const Block& block)
	{
		// If currently we store too many data to draw. Draw what we have and start collecting data again. 
		if (s_Data.IndexCount >= s_Data.MaxIndices)
			NextBatch();

		// Checking what texture slot to use
		float textureIndex = 0.0f;
		uint32_t block_texture = GetTexture(block);
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			if (s_Data.TextureSlots[i] == block_texture)
			{
				textureIndex = (float)i;
				break;
			}
		}
		if (textureIndex == 0.0f) // If still 0: Is every slot occupied or we have new texture
		{
			if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = block_texture;
			s_Data.TextureSlotIndex++;
		}

		// Applying transform to all vertices  
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		for (size_t i = 0; i < cube_vertex_count; i++)
		{
			s_Data.VertexBufferPtr->Position = transform * cube_vertex_positions[i];
			s_Data.VertexBufferPtr->TexCoord = cube_texture_coordinates[i];
			s_Data.VertexBufferPtr->TexIndex = textureIndex;

			s_Data.VertexBufferPtr++;
		}
		s_Data.IndexCount += cube_indices_count;

		// Update stats
		m_Stats.Triangles += cube_triangles;
	}

	void Renderer::ShutDown()
	{
		// free memmory
		delete[] s_Data.VertexBufferBase;

		glDeleteBuffers(1, &s_Data.IndexBuffer);
		glDeleteBuffers(1, &s_Data.VertexBuffer);
		glDeleteVertexArrays(1, &s_Data.VertexArray);
	}

	uint32_t Renderer::GetTexture(const Block& block)
	{
		return m_BlockTextureAtlas[block.m_BlockType];
	}

	void Renderer::LoadTextureAtlas()
	{
		// Pair blocktype with texture
		m_BlockTexturePathsAtlas[BlockType::Bedrock]       = "bedrock";
		m_BlockTexturePathsAtlas[BlockType::Bricks]        = "bricks";
		m_BlockTexturePathsAtlas[BlockType::CoalOre]       = "coal_ore";
		m_BlockTexturePathsAtlas[BlockType::Cobblestone]   = "cobblestone";
		m_BlockTexturePathsAtlas[BlockType::CraftingTable] = "crafting_table";
		m_BlockTexturePathsAtlas[BlockType::DiamondOre]    = "diamond_ore";
		m_BlockTexturePathsAtlas[BlockType::Dioryte]       = "dioryte";
		m_BlockTexturePathsAtlas[BlockType::Dirt]          = "dirt";
		m_BlockTexturePathsAtlas[BlockType::Furnace]       = "furnace";
		m_BlockTexturePathsAtlas[BlockType::Granite]       = "granite";
		m_BlockTexturePathsAtlas[BlockType::Grass]         = "grass";
		m_BlockTexturePathsAtlas[BlockType::Gravel]        = "gravel";
		m_BlockTexturePathsAtlas[BlockType::IronOre]       = "iron_ore";
		m_BlockTexturePathsAtlas[BlockType::OakLog]        = "oak_log";
		m_BlockTexturePathsAtlas[BlockType::OakPlanks]     = "oak_planks";
		m_BlockTexturePathsAtlas[BlockType::Sand]          = "sand";
		m_BlockTexturePathsAtlas[BlockType::Stone]         = "stone";
		m_BlockTexturePathsAtlas[BlockType::StoneBrick]    = "stone_brick";

		// Load all textures to gpu
		std::string mainPath = "assets/textures/";
		for (const auto& texture : m_BlockTexturePathsAtlas)
		{
			std::string path = mainPath + texture.second + ".png";
			m_BlockTextureAtlas[texture.first] = LoadTexture(path);
		}
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	uint32_t Renderer::LoadTexture(const std::string& path)
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