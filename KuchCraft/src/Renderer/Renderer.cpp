#include "kcpch.h"
#include "Renderer/Renderer.h"

#include "Renderer/Text/TextRenderer.h"
#include "Renderer/Renderer3D/Renderer3D.h"
#include "Renderer/AssetManager.h"
#include "Renderer/RendererCommand.h"

#include "Renderer/Data/Shader.h"

#include "Core/Application.h"
#include "Core/Utils.h"

#include "World/World.h"

#include <stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace KuchCraft {

	RendererData Renderer::s_RendererData;

	void Renderer::Init()
	{
		KC_PROFILE_FUNCTION();

		PrepareRenderer();
		
		AssetManager::Init();
		TextRenderer::Init();
		Renderer3D  ::Init();
	}

	void Renderer::ShutDown()
	{
		KC_PROFILE_FUNCTION();

		TextRenderer::ShutDown();
		Renderer3D  ::ShutDown();
		AssetManager::ShutDown();
	}

	void Renderer::BeginFrame()
	{
		KC_PROFILE_FUNCTION();

		TextRenderer::Clear();
		Renderer3D  ::Clear();
	}

	void Renderer::EndFrame(Camera* camera)
	{
		KC_PROFILE_FUNCTION();

		// Set camera uniform buffer
		{
			glm::mat4 viewProjection         = camera ? camera->GetViewProjection()         : glm::mat4(1.0f);
			glm::mat4 absoluteViewProjection = camera ? camera->GetAbsoluteViewProjection() : glm::mat4(1.0f);
			glm::mat4 orthoProjection        = camera ? camera->GetOrthoProjection()        : glm::ortho(0.0f, (float)Application::Get().GetWindow().GetWidth(), 0.0f, (float)Application::Get().GetWindow().GetHeight());

			UniformBufferCameraData buffer{
				viewProjection,
				absoluteViewProjection,
				orthoProjection
			};
			s_RendererData.WorldDataUniformBuffer->SetData(&buffer, sizeof(buffer));
		}

		Renderer3D::Render();

		// Render main frame buffer data to default frame buffer 
		DefaultFrameBuffer::Bind();
		DefaultFrameBuffer::Clear();
		RendererCommand::DisableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::DisableDepthTesting();

		s_RendererData.Shader->Bind();
		s_RendererData.VertexArray->Bind();
		s_RendererData.VertexBuffer->Bind();
		Texture2D::Bind(Renderer3D::GetFrameBuffer()->GetColorAttachmentRendererID(), default_texture_slot);
		glDrawArrays(GL_TRIANGLES, 0, quad_vertex_count_a);

		TextRenderer::Render();
	}

	void Renderer::PrepareRenderer()
	{
		KC_PROFILE_FUNCTION();

		if (opengl_logs)
			RendererCommand::EnableLogMessages();
		
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
		s_RendererData.QuadIndexBuffer = IndexBuffer::Create(indices, max_indices_in_chunk);
		delete[] indices;

		// Uniform buffer
		s_RendererData.WorldDataUniformBuffer = UniformBuffer::Create(sizeof(UniformBufferCameraData), 0);

		// Prepare for rendering to screen
		s_RendererData.VertexArray = VertexArray::Create();
		s_RendererData.VertexArray->Bind();

		s_RendererData.VertexBuffer = VertexBuffer::Create(6 * sizeof(Vertex_P2C2), screen_vertices, true);
		s_RendererData.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_RendererData.VertexArray->SetVertexBuffer(s_RendererData.VertexBuffer);

		s_RendererData.Shader = Shader::Create("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");
		s_RendererData.Shader->Bind();
		s_RendererData.Shader->SetInt("u_Texture", default_texture_slot);

		s_RendererData.VertexArray    ->Unbind();
		s_RendererData.VertexBuffer   ->Unbind();
		s_RendererData.QuadIndexBuffer->Unbind();
		s_RendererData.Shader         ->Unbind();
	}

	void Renderer::OnViewportSizeChanged(uint32_t width, uint32_t height)
	{
		Renderer3D::OnViewportSizeChanged(width, height);
	}

}