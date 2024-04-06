#include "kcpch.h"
#include "Renderer/Renderer2D/Renderer2D.h"

#include "Renderer/Renderer.h"
#include "Renderer/RendererData.h"
#include "Renderer/RendererCommand.h"
#include "Renderer/Data/Texture2D.h"

namespace KuchCraft {

	Renderer2DInfo               Renderer2D::s_Info;
	Renderer2DQuadData           Renderer2D::s_QuadData;
	Renderer2DFullScreenQuadData Renderer2D::s_FullScreenQuadData;

	void Renderer2D::Init()
	{
		KC_PROFILE_FUNCTION();

		LoadRenderer2DInfo();

		PrepareQuadRendering();
		PrepareFullScreenQuadRendering();
	}

	void Renderer2D::ShutDown()
	{
		KC_PROFILE_FUNCTION();
	}

	void Renderer2D::LoadRenderer2DInfo()
	{
		KC_PROFILE_FUNCTION();

		s_Info = Renderer2DInfo();
	}

	void Renderer2D::Clear()
	{
	}

	void Renderer2D::Render()
	{
		KC_PROFILE_FUNCTION();
	}

	void Renderer2D::RenderFullScreenQuad(uint32_t rendererID)
	{
		RendererCommand::DisableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::DisableDepthTesting();

		s_FullScreenQuadData.Shader      .Bind();
		s_FullScreenQuadData.VertexArray .Bind();
		s_FullScreenQuadData.VertexBuffer.Bind();

		Texture2D::Bind(rendererID, default_texture_slot);
		RendererCommand::DrawArrays(quad_vertex_count_a);

		Renderer::s_Stats.DrawCalls++;
		Renderer::s_Stats.Quads++;
	}

	void Renderer2D::PrepareQuadRendering()
	{
		KC_PROFILE_FUNCTION();

		uint32_t* indices = new uint32_t[s_Info.MaxIndices];
		uint32_t  offset  = 0;
		for (uint32_t i = 0; i < s_Info.MaxIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}
		s_QuadData.IndexBuffer.Create(indices, s_Info.MaxIndices);
		delete[] indices;

		s_QuadData.IndexBuffer.Unbind();
	}

	void Renderer2D::PrepareFullScreenQuadRendering()
	{
		KC_PROFILE_FUNCTION();

		s_FullScreenQuadData.VertexArray.Create();
		s_FullScreenQuadData.VertexArray.Bind();

		float vertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
						 
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f
		};

		s_FullScreenQuadData.VertexBuffer.Create(sizeof(vertices), vertices, true);
		s_FullScreenQuadData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_FullScreenQuadData.VertexArray.SetVertexBuffer(s_FullScreenQuadData.VertexBuffer);

		s_FullScreenQuadData.Shader.Create("assets/shaders/full_screen_quad.vert.glsl", "assets/shaders/full_screen_quad.frag.glsl");
		s_FullScreenQuadData.Shader.Bind();
		s_FullScreenQuadData.Shader.SetInt("u_Texture", default_texture_slot);

		s_FullScreenQuadData.VertexArray .Unbind();
		s_FullScreenQuadData.VertexBuffer.Unbind();
		s_FullScreenQuadData.Shader      .Unbind();
	}

}
