#include "kcpch.h"
#include "Renderer/Renderer.h"

#include "Renderer/Text/TextRenderer.h"

#include "Renderer/Shader.h"

#include "Core/Application.h"
#include "Core/Utils.h"

#include "World/World.h"

#include <stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace KuchCraft {

	RendererStatistics  Renderer::s_Stats;
	RendererChunkData   Renderer::s_ChunkData;
	RendererUtilsData   Renderer::s_UtilsData;

	RendererData        Renderer::s_RendererData;
	RendererSkyboxData  Renderer::s_SkyboxData;
	RendererWaterData   Renderer::s_WaterData;
	GraphicalInfo       Renderer::s_GraphicalInfo;

	void Renderer::Init()
	{
		KC_PROFILE_FUNCTION();

		PrepareShaders();
		PrepareRenderer();
		PrepereUtils();

		PrepareChunkRendering();
		PrepareSkyboxRendering();
		PrepareWaterRendering();
		
		LoadTextures();

		TextRenderer::Init();
	}

	void Renderer::ShutDown()
	{
		KC_PROFILE_FUNCTION();

		// RendererData
		glDeleteBuffers(1, &s_RendererData.WorldDataUniformBuffer);
		glDeleteFramebuffers(1, &s_RendererData.RenderOutputFrameBuffer.RendererID);
		glDeleteTextures(1, &s_RendererData.RenderOutputFrameBuffer.ColorAttachment);
		glDeleteTextures(1, &s_RendererData.RenderOutputFrameBuffer.DepthAttachment);

		// RendererStatistics
		ResetStats();
	}

	void Renderer::OnUpdate(float dt)
	{
		KC_PROFILE_FUNCTION();

		ResetStats();

		Timer::OnUpdate(dt);
		s_Stats.RenderTimer.End();
		s_Stats.RenderTimer.Begin();
	}

	void Renderer::BeginFrame()
	{
		KC_PROFILE_FUNCTION();

		// Clear text buffer
		TextRenderer::Clear();
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
		glNamedBufferSubData(s_RendererData.WorldDataUniformBuffer, 0, sizeof(buffer), &buffer);

		// Setup main frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, s_RendererData.RenderOutputFrameBuffer.RendererID);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::EndWorld()
	{
		KC_PROFILE_FUNCTION();

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

	std::string& Renderer::GetDebugText()
	{
		s_Stats.DebugText = 				
			"\nRenderer:"
			"\n    Draw cals: " + std::to_string(s_Stats.DrawCalls) +
			"\n    Quads: " + std::to_string(s_Stats.Quads) +
			"\n    Render time: " + std::to_string(s_Stats.RenderTimer.ElapsedMillis()) + "ms" +
			"\n      - chunks:      " + std::to_string(s_Stats.ChunkTimer.ElapsedMillis()) + "ms" +
			"\n      - skybox:      " + std::to_string(s_Stats.SkyboxTimer.ElapsedMillis()) + "ms" +
			"\n      - water:         " + std::to_string(s_Stats.WaterTimer.ElapsedMillis()) + "ms" +
			"\n      - text:            " + std::to_string(s_Stats.TextTimer.ElapsedMillis()) + "ms";

		return s_Stats.DebugText;
	}

	void Renderer::ResetStats()
	{
		s_Stats.DrawCalls = 0;
		s_Stats.Quads     = 0;
	}

	void Renderer::RenderChunks(const std::vector<Chunk*>& chunks)
	{
		KC_PROFILE_FUNCTION();

		s_Stats.ChunkTimer.Begin();
		s_ChunkData.Shader->Bind();
		s_ChunkData.VertexArray->Bind();

		for (const auto& chunk : chunks)
		{
			uint32_t vertexOffset = 0;
			const auto& drawList  = chunk->GetDrawList();
			s_ChunkData.Shader->SetFloat3("u_ChunkPosition", chunk->GetPosition());

			for (uint32_t i = 0; i < drawList.GetDrawCallCount(); i++)
			{
				uint32_t indexCount = drawList.GetIndexCount(i);
				if (indexCount != 0)
				{
					uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;
					s_ChunkData.VertexBuffer->SetData(drawList.GetVerticesPtr(vertexOffset), vertexCount * sizeof(uint32_t));

					vertexOffset += vertexCount;

					// Bind textures
					uint32_t textures = drawList.GetTextureCount(i);
					for (uint32_t j = 0; j < textures; j++)
						glBindTextureUnit(j, drawList.GetTexture(i, j));

					// Draw elements		
					s_RendererData.QuadIndexBuffer->Bind();
					glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

					// Update stats
					s_Stats.DrawCalls++;
					s_Stats.Quads += vertexCount / quad_vertex_count;
				}
			}
		}

		s_Stats.ChunkTimer.End();
	}

	void Renderer::RenderSkybox()
	{
		KC_PROFILE_FUNCTION();

		s_Stats.SkyboxTimer.Begin();
		s_SkyboxData.Shader->Bind();

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);

		s_SkyboxData.VertexArray->Bind();
		s_SkyboxData.VertexBuffer->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_SkyboxData.Texture);

		s_RendererData.QuadIndexBuffer->Bind();
		glDrawElements(GL_TRIANGLES, cube_face_cout * quad_index_count, GL_UNSIGNED_INT, nullptr);

		// Update stats
		s_Stats.DrawCalls++;
		s_Stats.Quads += cube_face_cout;

		glCullFace(GL_BACK);
		glDepthFunc(GL_LESS);

		s_Stats.SkyboxTimer.End();
	}

	void Renderer::RenderChunksWater(const std::vector<Chunk*>& chunks)
	{
		KC_PROFILE_FUNCTION();

		s_Stats.WaterTimer.Begin();
		s_WaterData.Shader->Bind();
		s_WaterData.VertexArray->Bind();

		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);

		for (const auto& chunk : chunks)
		{
			const auto& drawList = chunk->GetDrawList();
			uint32_t indexCount  = drawList.GetWaterVertices().size() / quad_vertex_count * quad_index_count;

			if (indexCount != 0)
			{
				uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;
				s_WaterData.VertexBuffer->SetData(drawList.GetWaterVerticesPtr(), vertexCount * sizeof(Vertex_P3C2));

				glBindTextureUnit(default_texture_slot, GetTexture(BlockType::Water));

				// Draw elements		
				s_RendererData.QuadIndexBuffer->Bind();
				glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

				// Update stats
				s_Stats.DrawCalls++;
				s_Stats.Quads += vertexCount / quad_vertex_count;
			}
		}

		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		s_Stats.WaterTimer.End();
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
		s_RendererData.ShaderVarData["##world_data_uniform_buffer_binding"] = std::to_string(s_RendererData.WorldDataUniformBufferBinding);
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

		s_GraphicalInfo.Vendor = (char*)glGetString(GL_VENDOR);
		s_GraphicalInfo.Renderer = (char*)glGetString(GL_RENDERER);
		s_GraphicalInfo.Version = (char*)glGetString(GL_VERSION);
		s_GraphicalInfo.ShadingLanguageVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

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
		glCreateBuffers(1, &s_RendererData.WorldDataUniformBuffer);
		glNamedBufferData(s_RendererData.WorldDataUniformBuffer, sizeof(UniformWorldBuffer), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, s_RendererData.WorldDataUniformBufferBinding, s_RendererData.WorldDataUniformBuffer);

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

	void Renderer::PrepareChunkRendering()
	{
		KC_PROFILE_FUNCTION();

		s_ChunkData.VertexArray = VertexArray::Create();
		s_ChunkData.VertexArray->Bind();

		s_ChunkData.VertexBuffer = VertexBuffer::Create(max_vertices_in_chunk * sizeof(uint32_t));
		s_ChunkData.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Uint, "a_PackedData" }
		});

		s_ChunkData.VertexArray->SetVertexBuffer(s_ChunkData.VertexBuffer);

		s_ChunkData.Shader = Shader::Create("assets/shaders/chunk.vert.glsl", "assets/shaders/chunk.frag.glsl");
		s_ChunkData.Shader->Bind();

		int samplers[max_texture_slots];
		for (int i = 0; i < max_texture_slots; i++)
			samplers[i] = i;
		s_ChunkData.Shader->SetIntArray("u_Textures", samplers, max_texture_slots);

		s_ChunkData.VertexArray ->Unbind();
		s_ChunkData.VertexBuffer->Unbind();
		s_ChunkData.Shader      ->Unbind();
	} 

	void Renderer::PrepareSkyboxRendering()
	{
		KC_PROFILE_FUNCTION();

		s_SkyboxData.VertexArray = VertexArray::Create();
		s_SkyboxData.VertexArray->Bind();

		constexpr float vertices[] = {
			 1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
		};

		s_SkyboxData.VertexBuffer = VertexBuffer::Create(sizeof(vertices), vertices, true);
		s_SkyboxData.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position"}
		});

		s_SkyboxData.VertexArray->SetVertexBuffer(s_SkyboxData.VertexBuffer);

		s_SkyboxData.Shader = Shader::Create("assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
		s_SkyboxData.Shader->Bind();
		s_SkyboxData.Shader->SetInt("u_CubemapTexture", 0);

		s_SkyboxData.Texture = LoadSkyboxTexture();

		s_SkyboxData.VertexArray ->Unbind();
		s_SkyboxData.VertexBuffer->Unbind();
		s_SkyboxData.Shader      ->Unbind();
	}

	void Renderer::PrepareWaterRendering()
	{
		KC_PROFILE_FUNCTION();

		s_WaterData.VertexArray = VertexArray::Create();
		s_WaterData.VertexArray->Bind();

		s_WaterData.VertexBuffer = VertexBuffer::Create(max_vertices_in_chunk * sizeof(Vertex_P3C2));
		s_WaterData.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_WaterData.VertexArray->SetVertexBuffer(s_WaterData.VertexBuffer);

		s_WaterData.Shader = Shader::Create("assets/shaders/water.vert.glsl", "assets/shaders/water.frag.glsl");
		s_WaterData.Shader->Bind();
		s_WaterData.Shader->SetInt("u_Texture", default_texture_slot);

		s_WaterData.VertexArray ->Unbind();
		s_WaterData.VertexBuffer->Unbind();
		s_WaterData.Shader      ->Unbind();
	}

	void Renderer::PrepereUtils()
	{
		KC_PROFILE_FUNCTION();
		
		s_UtilsData.OutlinedBlockVertexArray = VertexArray::Create();
		s_UtilsData.OutlinedBlockVertexArray->Bind();

		// Float3::pos, Float2::texCoord
		constexpr float vertices[] = {
			// bottom
			1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			// top
			0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			// front
			0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			// right
			1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			// behind
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			// left
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f, 1.0f
		};

		s_UtilsData.OutlinedBlockVertexBuffer = VertexBuffer::Create(sizeof(vertices), vertices, true);
		s_UtilsData.OutlinedBlockVertexBuffer->SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		s_UtilsData.OutlinedBlockVertexArray->SetVertexBuffer(s_UtilsData.OutlinedBlockVertexBuffer);

		s_UtilsData.OutlinedBlockShader = Shader::Create("assets/shaders/outlined_block.vert.glsl", "assets/shaders/outlined_block.frag.glsl");
		s_UtilsData.OutlinedBlockShader->Bind();

		s_UtilsData.OutlinedBlockVertexArray ->Unbind();
		s_UtilsData.OutlinedBlockVertexBuffer->Unbind();
		s_UtilsData.OutlinedBlockShader      ->Unbind();
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

	void Renderer::RenderOutlinedBlock(const glm::vec3& position)
	{
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

		s_UtilsData.OutlinedBlockShader->Bind();
		s_UtilsData.OutlinedBlockShader->SetMat4("u_Transform", transform);

		s_UtilsData.OutlinedBlockVertexArray->Bind();
		s_UtilsData.OutlinedBlockVertexBuffer->Bind();

		s_RendererData.QuadIndexBuffer->Bind();
		glDrawElements(GL_TRIANGLES, cube_index_count, GL_UNSIGNED_INT, nullptr);

		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);

		s_Stats.DrawCalls++;
		s_Stats.Quads += cube_face_cout;
	}

	void Renderer::LoadTextures()
	{
		KC_PROFILE_FUNCTION();

		const std::string path  = "assets/textures/";
		const std::string extension = ".png";

		for (uint32_t i = first_index_of_block_types; i < absolute_number_of_block_types; i++)
		{
			std::string blockName = Block::GetName((BlockType)i);
			std::replace(blockName.begin(), blockName.end(), ' ', '_');

			const std::string texturePath = path + blockName + extension;
			s_RendererData.Textures[i] = LoadTexture(texturePath);
		}
	}

	uint32_t Renderer::LoadSkyboxTexture()
	{
		KC_PROFILE_FUNCTION();

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
		if (!data)
		{
			KC_ERROR("Failed to load texture: {0}", path.c_str());
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);

		return texture;
	}

	uint32_t Renderer::GetTexture(BlockType type)
	{
		return s_RendererData.Textures[(uint32_t)type];
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