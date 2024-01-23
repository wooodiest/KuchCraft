#include "Renderer.h"

#include <memory>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <stb_image.h>
#include "Core/Application.h"

namespace KuchCraft {

	Renderer* Renderer::s_Instance = nullptr;

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		float     TexIndex;
	};

	struct RendererData
	{
		static const uint32_t MaxQuads = 20004; // % 6 == 0
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 48;

		uint32_t QuadVertexArray;
		uint32_t QuadVertexBuffer;
		uint32_t QuadIndexBuffer;
		Shader   QuadShader;

		Vertex*  VertexBufferBase = nullptr;
		Vertex*  VertexBufferPtr = nullptr;
		uint32_t IndexCount = 0;

		uint32_t TextureSlots[MaxTextureSlots];
		uint32_t TextureSlotIndex = 0;

		glm::vec4 VertexPositions[24];
	};
	static RendererData s_Data;

	Renderer::Renderer()
	{
		s_Instance = this;

		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Cube vertex array and buffer
		glGenVertexArrays(1, &s_Data.QuadVertexArray);
		glBindVertexArray(s_Data.QuadVertexArray);

		glGenBuffers(1, &s_Data.QuadVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, s_Data.MaxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// Cube vertices
		//  Bottom
		s_Data.VertexPositions[0]  = { 0.0f, 0.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[1]  = { 1.0f, 0.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[2]  = { 1.0f, 0.0f, 0.0f, 1.0f };
		s_Data.VertexPositions[3]  = { 0.0f, 0.0f, 0.0f, 1.0f };
		//  Top
		s_Data.VertexPositions[4]  = { 0.0f, 1.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[5]  = { 1.0f, 1.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[6]  = { 1.0f, 1.0f, 0.0f, 1.0f };
		s_Data.VertexPositions[7]  = { 0.0f, 1.0f, 0.0f, 1.0f };
		//  Front
		s_Data.VertexPositions[8]  = { 0.0f, 0.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[9]  = { 1.0f, 0.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[10] = { 1.0f, 1.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[11] = { 0.0f, 1.0f, 1.0f, 1.0f };
		//  Right
		s_Data.VertexPositions[12] = { 1.0f, 0.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[13] = { 1.0f, 0.0f, 0.0f, 1.0f };
		s_Data.VertexPositions[14] = { 1.0f, 1.0f, 0.0f, 1.0f };
		s_Data.VertexPositions[15] = { 1.0f, 1.0f, 1.0f, 1.0f };
		//  Behind
		s_Data.VertexPositions[16] = { 1.0f, 0.0f, 0.0f, 1.0f };
		s_Data.VertexPositions[17] = { 0.0f, 0.0f, 0.0f, 1.0f };
		s_Data.VertexPositions[18] = { 0.0f, 1.0f, 0.0f, 1.0f };
		s_Data.VertexPositions[19] = { 1.0f, 1.0f, 0.0f, 1.0f };
		//  Left
		s_Data.VertexPositions[20] = { 0.0f, 0.0f, 0.0f, 1.0f };
		s_Data.VertexPositions[21] = { 0.0f, 0.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[22] = { 0.0f, 1.0f, 1.0f, 1.0f };
		s_Data.VertexPositions[23] = { 0.0f, 1.0f, 0.0f, 1.0f };

		s_Data.VertexBufferBase = new Vertex[s_Data.MaxVertices];

		// Cube index buffer
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

		glGenBuffers(1, &s_Data.QuadIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		delete[] indices;

		// Shader
		s_Data.QuadShader.Create("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");
		s_Data.QuadShader.Bind();

		// Textures
		int samplers[s_Data.MaxTextureSlots];
		for (int i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;
		s_Data.QuadShader.SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		m_Texture = LoadTexture("assets/textures/grass.png");
	}

	Renderer::~Renderer()
	{
		ShutDown();
	}

	void Renderer::Flush()
	{
		if (s_Data.IndexCount == 0) // Nothing to draw
			return;

		// Check size of elements
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.VertexBufferPtr - (uint8_t*)s_Data.VertexBufferBase);
		
		for (int i = 0; i < 36; i++)
		{
			std::cout << s_Data.VertexBufferBase[i].Position.x << " " << s_Data.VertexBufferBase[i].Position.y << " " << s_Data.VertexBufferBase[i].Position.z << std::endl;
		}

		// Upload data
		glBindVertexArray(s_Data.QuadVertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, s_Data.VertexBufferBase);

		// Bind textures : TODO
		s_Data.QuadShader.Bind();
		glBindTextureUnit(0, m_Texture);

		// Draw elements		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadIndexBuffer);
		glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr);

		// Update stats
		m_Stats.DrawCalls++;
	}

	void Renderer::StartBatch()
	{
		s_Data.IndexCount = 0;
		s_Data.VertexBufferPtr = s_Data.VertexBufferBase;
		s_Data.TextureSlotIndex = 0;
	}

	void Renderer::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer::BeginScene(Camera& camera)
	{
		s_Data.QuadShader.Bind();
		s_Data.QuadShader.SetMat4("u_ViewProjection", camera.GetViewProjection());

		StartBatch();
	}

	void Renderer::EndScene()
	{
		Flush();

		std::cout << "Draw calls: " << m_Stats.DrawCalls << ", Triangles: " << m_Stats.Triangles << std::endl;
		m_Stats.DrawCalls = 0;
		m_Stats.Triangles = 0;
	}

	void Renderer::DrawCube(const glm::vec3& position, const Block& block)
	{
		if (s_Data.IndexCount >= s_Data.MaxIndices)
			NextBatch();

		// Check textures : TODO
		float textureIndex = 0.0f;

		constexpr size_t vertexCount = 24;
		constexpr glm::vec2 textureCords[] = {
			{ 0.0f,  0.0f },
			{ 0.25f, 0.0f },
			{ 0.25f, 0.5f },
			{ 0.0f,  0.5f },

			{ 0.25f, 0.0f },
			{ 0.5f,  0.0f },
			{ 0.5f,  0.5f },
			{ 0.25f, 0.5f },

			{ 0.0f,  0.5f },
			{ 0.25f, 0.5f },
			{ 0.25f, 1.0f },
			{ 0.0f,  1.0f },

			{ 0.25f, 0.5f },
			{ 0.5f,  0.5f },
			{ 0.5f,  1.0f },
			{ 0.25f, 1.0f },

			{ 0.5f,  0.5f },
			{ 0.75f, 0.5f },
			{ 0.75f, 1.0f },
			{ 0.5f,  1.0f },

			{ 0.75f, 0.5f },
			{ 1.0f,  0.5f },
			{ 1.0f,  1.0f },
			{ 0.75f, 1.0f },
		};

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		for (size_t i = 0; i < vertexCount; i++)
		{
			s_Data.VertexBufferPtr->Position = transform * s_Data.VertexPositions[i];
			s_Data.VertexBufferPtr->TexCoord = textureCords[i];
			s_Data.VertexBufferPtr->TexIndex = textureIndex;
			
			s_Data.VertexBufferPtr++;
		}
		s_Data.IndexCount += 36;

		// Update stats
		m_Stats.Triangles += 12;
	}

	void Renderer::ShutDown()
	{
		delete[] s_Data.VertexBufferBase;

		glDeleteBuffers(1, &s_Data.QuadIndexBuffer);
		glDeleteBuffers(1, &s_Data.QuadVertexBuffer);
		glDeleteVertexArrays(1, &s_Data.QuadVertexArray);
			
		glDeleteTextures(1, &m_Texture);
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