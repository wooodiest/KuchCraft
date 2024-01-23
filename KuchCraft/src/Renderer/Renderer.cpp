#include "Renderer.h"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <stb_image.h>
#include "Core/Application.h"

namespace KuchCraft {

	Renderer* Renderer::s_Instance = nullptr;

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec2 TexIndex;
	};

	CubeVertex cube_vertices[]
	{
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }
	};

	Renderer::Renderer()
	{
		s_Instance = this;

		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);

		m_DefaultCubeShader.Create("assets/shaders/test.vert.glsl", "assets/shaders/test.frag.glsl");
		m_DefaultCubeShader.Bind(); 
		m_DefaultCubeShader.SetInt("u_Texture0", 0);

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		float vertices[] = {
			// Bottom
			0.0f,  0.0f,  1.0f, /* */ 0.0f,  0.0f,
			1.0f,  0.0f,  1.0f, /* */ 0.25f, 0.0f,
			1.0f,  0.0f,  0.0f, /* */ 0.25f, 0.5f,
			0.0f,  0.0f,  0.0f, /* */ 0.0f,  0.5f,
			// Top
			0.0f,  1.0f,  1.0f, /* */ 0.25f, 0.0f,
			1.0f,  1.0f,  1.0f, /* */ 0.5f,  0.0f,
			1.0f,  1.0f,  0.0f, /* */ 0.5f,  0.5f,
			0.0f,  1.0f,  0.0f, /* */ 0.25f, 0.5f,
			// Front
			0.0f,  0.0f,  1.0f, /* */ 0.0f,  0.5f,
			1.0f,  0.0f,  1.0f, /* */ 0.25f, 0.5f,
			1.0f,  1.0,   1.0f, /* */ 0.25f, 1.0f,
			0.0f,  1.0,   1.0f, /* */ 0.0f,  1.0f,
			// Right
			1.0f,  0.0f,  1.0f, /* */ 0.25f, 0.5f,
			1.0f,  0.0f,  0.0f, /* */ 0.5f,  0.5f,
			1.0f,  1.0f,  0.0f, /* */ 0.5f,  1.0f,
			1.0f,  1.0f,  1.0f, /* */ 0.25f, 1.0f,
			// Behind
			1.0f,  0.0f,  0.0f, /* */ 0.5f,  0.5f,
			0.0f,  0.0f,  0.0f, /* */ 0.75f, 0.5f,
			0.0f,  1.0f,  0.0f, /* */ 0.75f, 1.0f,
			1.0f,  1.0f,  0.0f, /* */ 0.5f,  1.0f,
			// Left
			0.0f,  0.0f,  0.0f, /* */ 0.75f, 0.5f,
			0.0f,  0.0f,  1.0f, /* */ 1.0f,  0.5f,
			0.0f,  1.0f,  1.0f, /* */ 1.0f,  1.0f,
			0.0f,  1.0f,  0.0f, /* */ 0.75f, 1.0f
		};


		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,

			4, 5, 6,
			6, 7, 4,

			8,  9,  10,
			10, 11, 8,

			12, 13, 14,
			14, 15, 12,

			16, 17, 18,
			18, 19, 16,

			20, 21, 22,
			22, 23, 20
		};

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		{
			glGenTextures(1, &m_Texture);
			glBindTexture(GL_TEXTURE_2D, m_Texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);
			unsigned char* data = stbi_load("assets/textures/grass.png", &width, &height, &channels, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			stbi_image_free(data);
		}
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		glViewport(0, 0, width, height);
	}

	void Renderer::ShutDown()
	{
		glDeleteBuffers(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_IBO);
	}

	void Renderer::BeginScene(Camera& camera)
	{
		m_Camera = &camera;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Texture);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

		m_DefaultCubeShader.Bind();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::DrawCube(const glm::vec3& position, const Block& block)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		m_DefaultCubeShader.SetMat4("u_MVP", m_Camera->GetViewProjection() * model);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	}

}