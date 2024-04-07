#include "kcpch.h"
#include "Renderer/Renderer2D/Renderer2D.h"

#include "Renderer/Renderer.h"
#include "Renderer/RendererData.h"
#include "Renderer/RendererCommand.h"
#include "Renderer/Data/Texture2D.h"

#include "Renderer/AssetManager.h"

#include "Core/Application.h"

namespace KuchCraft {

	Renderer2DInfo               Renderer2D::s_Info;
	Renderer2DQuadData           Renderer2D::s_QuadData;
	Renderer2DFullScreenQuadData Renderer2D::s_FullScreenQuadData;

	void Renderer2D::Init()
	{
		LoadRenderer2DInfo();

		PrepareQuadRendering();
		PrepareFullScreenQuadRendering();
	}

	void Renderer2D::ShutDown()
	{
		delete[] s_QuadData.TextureSlots;
	}

	void Renderer2D::LoadRenderer2DInfo()
	{
		s_Info = Renderer2DInfo();
	}

	void Renderer2D::Clear()
	{
		s_QuadData.Vertices.clear();
	}

	void Renderer2D::Render()
	{
		Renderer::s_Stats.Renderer2DTimer.Start();

		RenderQuads();

		Renderer::s_Stats.Renderer2DTimer.Finish();
	}

	void Renderer2D::RenderQuads()
	{
		RendererCommand::EnableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::EnableLessEqualDepthTesting();

		s_QuadData.Shader.      Bind();
		s_QuadData.VertexArray. Bind();
		s_QuadData.VertexBuffer.Bind();
		s_QuadData.IndexBuffer. Bind();

		s_QuadData.VertexOffset  = 0;
		uint32_t maxTextureSlots = Renderer::GetInfo().MaxTextureSlots;

		StartQuadsBatch();

		for (uint32_t i = 0; i < s_QuadData.Vertices.size(); i += quad_vertex_count)
		{
			if (s_QuadData.IndexCount == s_Info.MaxIndices)
				NextQuadsBatch();

			if (s_QuadData.Vertices[i].TexIndex == 0.0f) // just color, TexIndex temporarily holds the texture rendererID
			{
				s_QuadData.Vertices[i + 0].TexIndex = 0.0f;
				s_QuadData.Vertices[i + 1].TexIndex = 0.0f;
				s_QuadData.Vertices[i + 2].TexIndex = 0.0f;
				s_QuadData.Vertices[i + 3].TexIndex = 0.0f;

				s_QuadData.IndexCount += quad_index_count;
			}
			else // textures
			{	
				float textureIndex = 0.0f;
				for (uint32_t j = 1; j < s_QuadData.TextureSlotIndex; j++)
				{
					if (s_QuadData.TextureSlots[j] == s_QuadData.Vertices[i].TexIndex) // TexIndex temporarily holds the texture rendererID
					{
						textureIndex = (float)j;
						break;
					}
				}

				if (textureIndex == 0.0f)
				{
					if (s_QuadData.TextureSlotIndex >= maxTextureSlots)
						NextQuadsBatch();

					textureIndex = (float)s_QuadData.TextureSlotIndex;
					s_QuadData.TextureSlots[s_QuadData.TextureSlotIndex] = s_QuadData.Vertices[i].TexIndex;

					s_QuadData.TextureSlotIndex++;
				}

				s_QuadData.Vertices[i + 0].TexIndex = textureIndex;
				s_QuadData.Vertices[i + 1].TexIndex = textureIndex;
				s_QuadData.Vertices[i + 2].TexIndex = textureIndex;
				s_QuadData.Vertices[i + 3].TexIndex = textureIndex;

				s_QuadData.IndexCount += quad_index_count;
			}
		}

		FlushQuads();
	}

	void Renderer2D::FlushQuads()
	{
		if (s_QuadData.IndexCount == 0)
			return;

		uint32_t vertexCount = s_QuadData.IndexCount / quad_index_count * quad_vertex_count;

		s_QuadData.VertexBuffer.SetData(&s_QuadData.Vertices[s_QuadData.VertexOffset], vertexCount * sizeof(QuadVertex));
		s_QuadData.VertexOffset = vertexCount;

		for (uint32_t i = 0; i < s_QuadData.TextureSlotIndex; i++)
			Texture2D::Bind(s_QuadData.TextureSlots[i], i);
		
		RendererCommand::DrawElements(s_QuadData.IndexCount);

		Renderer::s_Stats.DrawCalls++;
		Renderer::s_Stats.Quads += vertexCount / quad_vertex_count;
	}

	void Renderer2D::StartQuadsBatch()
	{
		s_QuadData.IndexCount       = 0;
		s_QuadData.TextureSlotIndex = 1;
	}

	void Renderer2D::NextQuadsBatch()
	{
		FlushQuads();
		StartQuadsBatch();
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
		s_QuadData.VertexArray.Create();
		s_QuadData.VertexArray.Bind();

		s_QuadData.VertexBuffer.Create(s_Info.MaxVertices * sizeof(QuadVertex));
		s_QuadData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoor"      },
			{ ShaderDataType::Float,  "a_TexIndex"     }
		});

		s_QuadData.VertexArray.SetVertexBuffer(s_QuadData.VertexBuffer);

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

		{
			TextureSpecification spec;
			spec.Width = 1;
			spec.Height = 1;
			s_QuadData.WhiteTexture.Create(spec);

			uint32_t white = 0xffffffff;
			s_QuadData.WhiteTexture.Setdata(&white, sizeof(uint32_t));
		}
		
		std::unordered_map<std::string, std::string> quadShaderData;
		uint32_t maxTextureSlots = Renderer::GetInfo().MaxTextureSlots;
		quadShaderData["##max_texture_slots"] = std::to_string(maxTextureSlots);

		s_QuadData.Shader.Create("assets/shaders/quad2D.vert.glsl", "assets/shaders/quad2D.frag.glsl", quadShaderData);
		s_QuadData.Shader.Bind();

		int* samplers = new int[maxTextureSlots];
		for (int i = 0; i < maxTextureSlots; i++)
			samplers[i] = i;
		s_QuadData.Shader.SetIntArray("u_Textures", samplers, maxTextureSlots);
		delete[] samplers;

		s_QuadData.TextureSlots    = new uint32_t[maxTextureSlots];
		s_QuadData.TextureSlots[0] = s_QuadData.WhiteTexture.GetRendererID();

		s_QuadData.Vertices.reserve(s_Info.MaxVertices);

		s_QuadData.IndexBuffer .Unbind();
		s_QuadData.VertexArray. Unbind();
		s_QuadData.VertexBuffer.Unbind();
		s_QuadData.Shader.      Unbind();
	}

	void Renderer2D::PrepareFullScreenQuadRendering()
	{
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

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuadN(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawQuad({ position.x * width, position.y * height, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuadNN(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawQuad({ position.x * width, position.y * height, 0.0f }, { size.x * width, size.y * height }, color);
	}

	void Renderer2D::DrawQuadN(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawQuad({ position.x * width, position.y * height, position.z }, size, color);
	}

	void Renderer2D::DrawQuadNN(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawQuad({ position.x * width, position.y * height, position.z }, { size.x * width, size.y * height }, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			QuadVertex vertex;
			vertex.Position = transform * quad_vertex_positions[i];
			vertex.Color    = color;
			vertex.TexCoord = quad_vertex_tex_coords[i];

			s_QuadData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuadN(const glm::vec2& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawQuad({ position.x * width, position.y * height,  0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuadNN(const glm::vec2& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawQuad({ position.x * width, position.y * height,  0.0f }, { size.x * width, size.y * height }, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			QuadVertex vertex;
			vertex.Position     = transform * quad_vertex_positions[i];
			vertex.Color        = tintColor;
			vertex.TexCoord     = quad_vertex_tex_coords[i] * tilingFactor;
			vertex.TexIndex     = (float)texture.GetRendererID(); // TexIndex temporarily holds the texture rendererID

			s_QuadData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer2D::DrawQuadN(const glm::vec3& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawQuad({ position.x * width, position.y * height, position.z }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuadNN(const glm::vec3& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawQuad({ position.x * width, position.y * height, position.z }, { size.x * width, size.y * height }, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, { 0.0f, 0.0f, rotation }, color);
	}

	void Renderer2D::DrawRotatedQuadN(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, 0.0f }, size, { 0.0f, 0.0f, rotation }, color);
	}

	void Renderer2D::DrawRotatedQuadNN(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, 0.0f }, { size.x * width, size.y * height }, { 0.0f, 0.0f, rotation }, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad(position, size, { 0.0f, 0.0f, rotation }, color);
	}

	void Renderer2D::DrawRotatedQuadN(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, position.z }, size, { 0.0f, 0.0f, rotation }, color);
	}

	void Renderer2D::DrawRotatedQuadNN(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, position.z }, { size.x * width, size.y * height }, { 0.0f, 0.0f, rotation }, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color)
	{
		DrawRotatedQuad(position, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuadN(const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuadNN(const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, 0.0f }, { size.x * width, size.y * height }, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			QuadVertex vertex;
			vertex.Position = transform * quad_vertex_positions[i];
			vertex.Color    = color;
			vertex.TexCoord = quad_vertex_tex_coords[i];

			s_QuadData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer2D::DrawRotatedQuadN(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, position.z }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuadNN(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, position.z }, { size.x * width, size.y * height }, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, { 0.0f, 0.0f, rotation }, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuadN(const glm::vec2& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, 0.0f }, size, { 0.0f, 0.0f, rotation }, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuadNN(const glm::vec2& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, 0.0f }, { size.x * width, size.y * height }, { 0.0f, 0.0f, rotation }, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad(position, size, { 0.0f, 0.0f, rotation }, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuadN(const glm::vec3& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, position.z }, size, { 0.0f, 0.0f, rotation }, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuadNN(const glm::vec3& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, position.z }, { size.x * width, size.y * height }, { 0.0f, 0.0f, rotation }, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuadN(const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuadNN(const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, 0.0f }, { size.x * width, size.y * height }, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			QuadVertex vertex;
			vertex.Position     = transform * quad_vertex_positions[i];
			vertex.Color        = tintColor;
			vertex.TexCoord     = quad_vertex_tex_coords[i] * tilingFactor;
			vertex.TexIndex     = (float)texture.GetRendererID(); // TexIndex temporarily holds the texture rendererID

			s_QuadData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer2D::DrawRotatedQuadN(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, position.z }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuadNN(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		auto& [width, height] = Application::Get().GetWindow().GetWindowSize();
		DrawRotatedQuad({ position.x * width, position.y * height, position.z }, { size.x * width, size.y * height }, rotation, texture, tilingFactor, tintColor);
	}


}
