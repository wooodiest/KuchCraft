#include "kcpch.h"
#include "Renderer/Renderer.h"

#include "Renderer/Text/TextRenderer.h"
#include "Renderer/Renderer3D/Renderer3D.h"
#include "Renderer/AssetManager.h"

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

		PrepareShaders();
		PrepareRenderer();
		
		AssetManager::Init();
		TextRenderer::Init();
		Renderer3D  ::Init();
	}

	void Renderer::ShutDown()
	{
		KC_PROFILE_FUNCTION();

		// RendererData
		glDeleteFramebuffers(1, &s_RendererData.RenderOutputFrameBuffer.RendererID);
		glDeleteTextures(1, &s_RendererData.RenderOutputFrameBuffer.ColorAttachment);
		glDeleteTextures(1, &s_RendererData.RenderOutputFrameBuffer.DepthAttachment);

		TextRenderer::ShutDown();
		Renderer3D  ::ShutDown();
	}

	void Renderer::OnUpdate(float dt)
	{
		KC_PROFILE_FUNCTION();
	}

	void Renderer::BeginFrame()
	{
		KC_PROFILE_FUNCTION();

		// Clear text buffer
		TextRenderer::Clear();
		Renderer3D  ::Clear();
	}

	void Renderer::EndFrame()
	{
		KC_PROFILE_FUNCTION();
		TextRenderer::Render();
	}

	void Renderer::BeginWorld(const Camera& camera)
	{
		KC_PROFILE_FUNCTION();

		// Set uniform buffer
		UniformWorldBuffer buffer{
			camera.GetViewProjection(),
			camera.GetAbsoluteViewProjection(),
			camera.GetOrthoProjection(),
			s_RendererData.TintStatus ? water_tint_color : white_color
		};
		s_RendererData.WorldDataUniformBufferrrrrrr->SetData(&buffer, sizeof(buffer));

		// Setup main frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, s_RendererData.RenderOutputFrameBuffer.RendererID);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::EndWorld()
	{
		KC_PROFILE_FUNCTION();

		Renderer3D::Render();

		// Setup deafult frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, s_RendererData.DefaultFrameBufferRendererID);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render main frame buffer data to default frame buffer 
		s_RendererData.Shader      ->Bind();
		s_RendererData.VertexArray ->Bind();
		s_RendererData.VertexBuffer->Bind();
		glBindTextureUnit(default_texture_slot, s_RendererData.RenderOutputFrameBuffer.ColorAttachment);
		glDrawArrays(GL_TRIANGLES, 0, quad_vertex_count_a);
	}

	void Renderer::PrepareShaders()
	{
		KC_PROFILE_FUNCTION();

		s_RendererData.ShaderStrData["##world_data_uniform_buffer"] =
			R"(layout(std140, binding = ##world_data_uniform_buffer_binding) uniform UniformWorldData
			   {
			   	   mat4 u_ViewProjection;
			   	   mat4 u_AbsoluteViewProjection;
			   	   mat4 u_OrthoProjection;
			   	   vec4 u_TintColor;
			   };)";
		s_RendererData.ShaderStrData["##text_data_uniform_buffer"] =
			R"(struct TextData
			   {
					mat4 Transform;
					vec4 Letter;
			   };
			   layout(std140, binding = ##text_data_uniform_buffer_binding) uniform UniformTextData
			   {
					TextData u_Text[##max_text_uniform_array_limit];
			   };)";

		s_RendererData.ShaderVarData["##max_texture_slots"]                 = std::to_string(max_texture_slots);
		s_RendererData.ShaderVarData["##world_data_uniform_buffer_binding"] = std::to_string(0);
		s_RendererData.ShaderVarData["##text_data_uniform_buffer_binding"]  = std::to_string(1); // TODO
		s_RendererData.ShaderVarData["##max_text_uniform_array_limit"]      = std::to_string(400); //TODO
		s_RendererData.ShaderVarData["##outlined_block_border_radius"]      = std::to_string(outlined_block_border_radius);
		s_RendererData.ShaderVarData["##outlined_block_border_color"]       = VecToString(outlined_block_border_color);
	}

	void Renderer::PrepareRenderer()
	{
		KC_PROFILE_FUNCTION();

		// Setup
		if (opengl_logs)
		{
			glDebugMessageCallback(OpenGLLogMessage, nullptr);
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}

		auto [width, height] = Application::Get().GetWindow().GetWindowSize();
		OnViewportSizeChanged(width, height);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);

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
		s_RendererData.WorldDataUniformBufferrrrrrr = UniformBuffer::Create(sizeof(UniformWorldBuffer), 0);

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
		InvalidateMainFrameBuffer(width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, s_RendererData.RenderOutputFrameBuffer.RendererID);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, s_RendererData.DefaultFrameBufferRendererID);
		glViewport(0, 0, width, height);
	}

	void Renderer::InvalidateMainFrameBuffer(uint32_t width, uint32_t height)
	{
		KC_PROFILE_FUNCTION();

		if (s_RendererData.RenderOutputFrameBuffer.RendererID)
		{
			glDeleteFramebuffers(1, &s_RendererData.RenderOutputFrameBuffer.RendererID);
			glDeleteTextures(1, &s_RendererData.RenderOutputFrameBuffer.ColorAttachment);
			glDeleteTextures(1, &s_RendererData.RenderOutputFrameBuffer.DepthAttachment);
		}

		glCreateFramebuffers(1, &s_RendererData.RenderOutputFrameBuffer.RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, s_RendererData.RenderOutputFrameBuffer.RendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &s_RendererData.RenderOutputFrameBuffer.ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, s_RendererData.RenderOutputFrameBuffer.ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_RendererData.RenderOutputFrameBuffer.ColorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &s_RendererData.RenderOutputFrameBuffer.DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, s_RendererData.RenderOutputFrameBuffer.DepthAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, s_RendererData.RenderOutputFrameBuffer.DepthAttachment, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	void Renderer::OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			KC_ERROR("[OpenGL] : {0}", (char*)message);
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
		case GL_DEBUG_SEVERITY_LOW:
			KC_WARN("[OpenGL] : {0}", (char*)message);
			break;

		case GL_DEBUG_SEVERITY_NOTIFICATION:
			KC_INFO("[OpenGL] : {0}", (char*)message);
			break;
		}
	}

}