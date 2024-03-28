#include "kcpch.h"
#include "Renderer/Renderer.h"

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
	RendererTextData    Renderer::s_TextData;
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
		PrepareTextRendering();
		
		LoadTextures();
	}

	void Renderer::ShutDown()
	{
		KC_PROFILE_FUNCTION();

		// RendererData
		glDeleteBuffers(1, &s_RendererData.QuadIndexBuffer);
		glDeleteBuffers(1, &s_RendererData.WorldDataUniformBuffer);
		glDeleteFramebuffers(1, &s_RendererData.RenderOutputFrameBuffer.RendererID);
		glDeleteTextures(1, &s_RendererData.RenderOutputFrameBuffer.ColorAttachment);
		glDeleteTextures(1, &s_RendererData.RenderOutputFrameBuffer.DepthAttachment);

		// Utils data
		glDeleteBuffers(1, &s_UtilsData.OutlinedBlockVertexBuffer);
		glDeleteVertexArrays(1, &s_UtilsData.OutlinedBlockVertexArray);

		// RendererChunkData
		glDeleteBuffers(1, &s_ChunkData.VertexBuffer);
		glDeleteVertexArrays(1, &s_ChunkData.VertexArray);

		// WaterData
		glDeleteBuffers(1, &s_WaterData.VertexBuffer);
		glDeleteVertexArrays(1, &s_WaterData.VertexArray);

		// Text data
		glDeleteBuffers(1, &s_TextData.VertexBuffer);
		glDeleteVertexArrays(1, &s_TextData.VertexArray);

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
		s_TextData.Data.clear();
	}

	void Renderer::EndFrame()
	{
		KC_PROFILE_FUNCTION();

		// Render text
		glEnable(GL_BLEND);
		RenderText();
		glDisable(GL_BLEND);
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
		s_RendererData.Shader.Bind();
		glBindVertexArray(s_RendererData.VertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_RendererData.VertexBuffer);
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
		s_ChunkData.Shader.Bind();

		for (const auto& chunk : chunks)
		{
			uint32_t vertexOffset = 0;
			const auto& drawList  = chunk->GetDrawList();
			s_ChunkData.Shader.SetFloat3("u_ChunkPosition", chunk->GetPosition());

			for (uint32_t i = 0; i < drawList.GetDrawCallCount(); i++)
			{
				uint32_t indexCount = drawList.GetIndexCount(i);
				if (indexCount != 0)
				{
					uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;

					glBindVertexArray(s_ChunkData.VertexArray);
					glBindBuffer(GL_ARRAY_BUFFER, s_ChunkData.VertexBuffer);
					glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(uint32_t), drawList.GetVerticesPtr(vertexOffset));

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

		s_Stats.ChunkTimer.End();
	}

	void Renderer::RenderSkybox()
	{
		KC_PROFILE_FUNCTION();

		s_Stats.SkyboxTimer.Begin();
		s_SkyboxData.Shader.Bind();

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);

		glBindVertexArray(s_SkyboxData.VertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_SkyboxData.VertexBuffer);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_SkyboxData.Texture);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIndexBuffer);
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
		s_WaterData.Shader.Bind();

		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);

		for (const auto& chunk : chunks)
		{
			const auto& drawList = chunk->GetDrawList();
			uint32_t indexCount  = drawList.GetWaterVertices().size() / quad_vertex_count * quad_index_count;

			if (indexCount != 0)
			{
				uint32_t vertexCount = indexCount / quad_index_count * quad_vertex_count;

				glBindVertexArray(s_WaterData.VertexArray);
				glBindBuffer(GL_ARRAY_BUFFER, s_WaterData.VertexBuffer);
				glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(Vertex_P3C2), drawList.GetWaterVerticesPtr());

				glBindTextureUnit(default_texture_slot, GetTexture(BlockType::Water));

				// Draw elements		
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIndexBuffer);
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
					vec4 u_Color;
			   };)";

		s_RendererData.ShaderVarData["##max_texture_slots"]                 = std::to_string(max_texture_slots);
		s_RendererData.ShaderVarData["##world_data_uniform_buffer_binding"] = std::to_string(s_RendererData.WorldDataUniformBufferBinding);
		s_RendererData.ShaderVarData["##text_data_uniform_buffer_binding"]  = std::to_string(s_TextData.TextDataUniformBufferBinding);
		s_RendererData.ShaderVarData["##max_text_uniform_array_limit"]      = std::to_string(max_text_uniform_array_limit);
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
		glGenBuffers(1, &s_RendererData.QuadIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices_in_chunk * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		delete[] indices;

		// Uniform buffer
		glCreateBuffers(1, &s_RendererData.WorldDataUniformBuffer);
		glNamedBufferData(s_RendererData.WorldDataUniformBuffer, sizeof(UniformWorldBuffer), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, s_RendererData.WorldDataUniformBufferBinding, s_RendererData.WorldDataUniformBuffer);

		// Prepare for rendering to screen
		glGenVertexArrays(1, &s_RendererData.VertexArray);
		glBindVertexArray(s_RendererData.VertexArray);

		glGenBuffers(1, &s_RendererData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_RendererData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex_P2C2), screen_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_P2C2), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_P2C2), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		s_RendererData.Shader.Create("assets/shaders/default.vert.glsl", "assets/shaders/default.frag.glsl");
		s_RendererData.Shader.Bind();
		s_RendererData.Shader.SetInt("u_Texture", default_texture_slot);
	}

	void Renderer::PrepareChunkRendering()
	{
		KC_PROFILE_FUNCTION();

		glGenVertexArrays(1, &s_ChunkData.VertexArray);
		glBindVertexArray(s_ChunkData.VertexArray);

		glGenBuffers(1, &s_ChunkData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_ChunkData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, max_vertices_in_chunk * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(uint32_t), (void*)0);
		glEnableVertexAttribArray(0);

		s_ChunkData.Shader.Create("assets/shaders/chunk.vert.glsl", "assets/shaders/chunk.frag.glsl");
		s_ChunkData.Shader.Bind();

		int samplers[max_texture_slots];
		for (int i = 0; i < max_texture_slots; i++)
			samplers[i] = i;
		s_ChunkData.Shader.SetIntArray("u_Textures", samplers, max_texture_slots);
	}

	void Renderer::PrepareSkyboxRendering()
	{
		KC_PROFILE_FUNCTION();

		glGenVertexArrays(1, &s_SkyboxData.VertexArray);
		glBindVertexArray(s_SkyboxData.VertexArray);

		glGenBuffers(1, &s_SkyboxData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_SkyboxData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		s_SkyboxData.Shader.Create("assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
		s_SkyboxData.Shader.Bind();
		s_SkyboxData.Shader.SetInt("u_CubemapTexture", 0);

		s_SkyboxData.Texture = LoadSkyboxTexture();
	}

	void Renderer::PrepareWaterRendering()
	{
		KC_PROFILE_FUNCTION();

		glGenVertexArrays(1, &s_WaterData.VertexArray);
		glBindVertexArray(s_WaterData.VertexArray);

		glGenBuffers(1, &s_WaterData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_WaterData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, max_vertices_in_chunk * sizeof(Vertex_P3C2), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_P3C2), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_P3C2), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		s_WaterData.Shader.Create("assets/shaders/water.vert.glsl", "assets/shaders/water.frag.glsl");
		s_WaterData.Shader.Bind();
		s_ChunkData.Shader.SetInt("u_Texture", default_texture_slot);
	}

	void Renderer::PrepareTextRendering()
	{
		KC_PROFILE_FUNCTION();

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			KC_ERROR("Failed to init freetype");
		}

		FT_Face face;
		if (FT_New_Face(ft, s_TextData.FontPath, 0, &face))
		{
			KC_ERROR("Failed to load font: {0}", s_TextData.FontPath);
		}

		FT_Set_Pixel_Sizes(face, font_texture_size, font_texture_size);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &s_TextData.Texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, s_TextData.Texture);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, font_texture_size, font_texture_size, font_characters_count, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

		for (unsigned char c = 0; c < font_characters_count; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				KC_ERROR("Failed to load glyph: {0}", (uint32_t)c);
			}

			glTexSubImage3D(
				GL_TEXTURE_2D_ARRAY,
				0, 0, 0, int(c),
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				1,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			s_TextData.Characters.emplace(c, RendererTextCharacter{
				(uint32_t)c,
				(uint32_t)face->glyph->advance.x,
				{face->glyph->bitmap.width, face->glyph->bitmap.rows},
				{face->glyph->bitmap_left, face->glyph->bitmap_top}
			});
		}

		float vertices[] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
		};

		glGenVertexArrays(1, &s_TextData.VertexArray);
		glBindVertexArray(s_TextData.VertexArray);

		glGenBuffers(1, &s_TextData.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_TextData.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		s_TextData.Shader.Create("assets/shaders/text.vert.glsl", "assets/shaders/text.frag.glsl");
		s_TextData.Shader.Bind();

		// Uniform buffer
		glCreateBuffers(1, &s_TextData.TextDataUniformBuffer);
		glNamedBufferData(s_TextData.TextDataUniformBuffer, sizeof(UniformTextBuffer), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, s_TextData.TextDataUniformBufferBinding, s_TextData.TextDataUniformBuffer);

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}

	void Renderer::PrepereUtils()
	{
		KC_PROFILE_FUNCTION();
		
		glGenVertexArrays(1, &s_UtilsData.OutlinedBlockVertexArray);
		glBindVertexArray(s_UtilsData.OutlinedBlockVertexArray);

		glGenBuffers(1, &s_UtilsData.OutlinedBlockVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, s_UtilsData.OutlinedBlockVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(block_vertices), block_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		s_UtilsData.OutlinedBlockShader.Create("assets/shaders/outlined_block.vert.glsl", "assets/shaders/outlined_block.frag.glsl");
		s_UtilsData.OutlinedBlockShader.Bind();
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

	void Renderer::RenderText()
	{
		KC_PROFILE_FUNCTION();

		s_Stats.TextTimer.Begin();

		s_TextData.Shader.Bind();
		glBindTextureUnit(default_texture_slot, s_TextData.Texture);
		glBindVertexArray(s_TextData.VertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_TextData.VertexBuffer);

		// Set uniform buffer
		UniformTextBuffer buffer;
		
		for (const auto& [text, position, color, fontSize, spacing] : s_TextData.Data)
		{
			buffer.Color = color;
			glm::vec2 currentPosition{ position };
			float scale = fontSize / font_texture_size;

			uint32_t currentArrayIndex = 0;
			for (auto c = text.begin(); c != text.end(); c++)
			{
				RendererTextCharacter ch = s_TextData.Characters[*c];
				if (*c == '\n')
				{
					currentPosition.y -= (ch.Size.y) * spacing * scale;
					currentPosition.x  = position.x;
				}
				else if (*c == ' ')
				{
					currentPosition.x += (ch.Advance >> 6) * scale;
				}
				else
				{
					float xpos = currentPosition.x + ch.Bearing.x * scale;
					float ypos = currentPosition.y - (font_texture_size - ch.Bearing.y) * scale;

					buffer.Text[currentArrayIndex].Transform = glm::translate(glm::mat4(1.0f), { xpos, ypos, 0 }) *
						glm::scale(glm::mat4(1.0f), { font_texture_size * scale, font_texture_size * scale, 0 });
					buffer.Text[currentArrayIndex].Letter.x = (float)ch.ID;

					currentPosition.x += (ch.Advance >> 6) * scale;
					currentArrayIndex++;
					if (currentArrayIndex == max_text_uniform_array_limit)
					{
						RenderText(currentArrayIndex, &buffer);
						currentArrayIndex = 0;
					}
				}
			}
			RenderText(currentArrayIndex, &buffer);
		}

		s_Stats.TextTimer.End();
	}

	void Renderer::RenderText(uint32_t length, UniformTextBuffer* buffer)
	{
		if (length != 0) 
		{
			glNamedBufferSubData(s_TextData.TextDataUniformBuffer, 0, sizeof(UniformTextBuffer), buffer);
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, length);
		}
	}

	void Renderer::RenderTextTopLeft(const std::string& text, const glm::vec2& margin, const glm::vec4& color, float fontSize, float spacing)
	{
		glm::vec2 position{ margin.x, Application::Get().GetWindow().GetHeight() - margin.y };
		RenderText(text, position, color, fontSize, spacing);
	}

	void Renderer::RenderText(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize, float spacing)
	{
		s_TextData.Data.emplace_back(text, position, color, fontSize, spacing);
	}

	void Renderer::RenderTextNorm(const std::string& text, const glm::vec2& positionNormalized, const glm::vec4& color, float fontSize, float spacing)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		glm::vec2 position{  width * positionNormalized.x, height * positionNormalized.y};
		RenderText(text, position, color, fontSize, spacing);
	}

	void Renderer::RenderOutlinedBlock(const glm::vec3& position)
	{
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

		s_UtilsData.OutlinedBlockShader.Bind();
		s_UtilsData.OutlinedBlockShader.SetMat4("u_Transform", transform);

		glBindVertexArray(s_UtilsData.OutlinedBlockVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_UtilsData.OutlinedBlockVertexBuffer);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_RendererData.QuadIndexBuffer);
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