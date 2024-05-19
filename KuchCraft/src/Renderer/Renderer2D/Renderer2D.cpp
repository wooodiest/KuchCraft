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
	Renderer2DTextInfo           Renderer2D::s_TextInfo;
	Renderer2DTextData           Renderer2D::s_TextData;
	Renderer2DMouseData			 Renderer2D::s_MouseData;

	void Renderer2D::Init()
	{
		LoadRenderer2DInfo();

		PrepareQuadRendering();
		PrepareFullScreenQuadRendering();
		PrepareTextRendering();

		s_MouseData.PrevPosition = Input::GetMousePosition();
		ResetMousePosition();
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

		s_TextData.Data             .clear();
		s_TextData.TextIndexDistance.clear();
	}

	void Renderer2D::Render()
	{
		Renderer::s_Stats.Renderer2DTimer.Start();

		glm::vec2 mousePosition  = Input::GetMousePosition();
		glm::vec2 positionDiff   = mousePosition - s_MouseData.PrevPosition;
		s_MouseData.PrevPosition = mousePosition;

		if (s_MouseData.Show)
		{
			auto [width, height] = Application::Get().GetWindow().GetWindowSize();
			s_MouseData.Position = glm::clamp(s_MouseData.Position + glm::vec2{ positionDiff.x, positionDiff.y * -1.0f }, { 0.0f, 0.0f }, { width, height });

			Renderer2DQuadInfo data;
			data.Size = { 30.0f, 30.0f };
			data.Position = glm::vec3(s_MouseData.Position, 1.0f);
			DrawQuad(data, AssetManager::GetUIElementTexture(UIElement::Cursor));
		}

		RenderQuads();
		RenderText();

		Renderer::s_Stats.Renderer2DTimer.Finish();
	}

	void Renderer2D::RenderQuads()
	{
		if (!s_QuadData.Vertices.size())
			return;

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

		s_QuadData.VertexBuffer.SetData(&s_QuadData.Vertices[s_QuadData.VertexOffset], vertexCount * sizeof(Quad2DVertex));
		s_QuadData.VertexOffset += vertexCount;

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

	void Renderer2D::RenderText()
	{
		if (!s_TextData.Data.size())
			return;

		RendererCommand::EnableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::EnableDepthTesting();
		RendererCommand::DisableDepthMask();
		RendererCommand::EnablePolygonOffset(1.0f, 1.0f);

		s_TextData.Shader      .Bind();
		s_TextData.Texture     .Bind();
		s_TextData.VertexArray .Bind();
		s_TextData.VertexBuffer.Bind();

		Renderer2DUniformText* textBuffer = new Renderer2DUniformText[s_TextInfo.MaxCharacterUniformArrayLimit];
		uint32_t currentIndex = 0;

		for (uint32_t i = 0; i < s_TextData.Data.size(); i++)
			s_TextData.TextIndexDistance.emplace_back(i, s_TextData.Data[i].second.Position.z);
		
		
		std::sort(s_TextData.TextIndexDistance.begin(), s_TextData.TextIndexDistance.end(), [](const auto& v1, const auto& v2) {
			return v1.second < v2.second;
		});

		auto [width, height] = Application::Get().GetWindow().GetWindowSize(); 

		for (const auto& [index, distance] : s_TextData.TextIndexDistance)
		{
			const auto& [text, textStyle] = s_TextData.Data[index];
			glm::vec2 currentPosition     = textStyle.Position;

			if (textStyle.PositionFromTopLeft)
				currentPosition.y = height - currentPosition.y;

			float scale = textStyle.FontSize / s_TextInfo.FontTextureSize;

			if (!text.empty())
			{
				const FontCharacter& character = s_TextData.Texture.GetCharacter(text[0]);
				currentPosition.y -= (character.Size.y) * scale * (1.0f + textStyle.FontSpacing);
			}

			float absolutePositionX = currentPosition.x;

			for (auto c = text.begin(); c != text.end(); c++)
			{
				const FontCharacter& character = s_TextData.Texture.GetCharacter(*c);

				if (*c == '\n')
				{
					currentPosition.y -= (character.Size.y) * textStyle.FontSpacing * scale;
					currentPosition.x = absolutePositionX;
				}
				else if (*c == ' ')
				{
					currentPosition.x += (character.Advance >> 6) * scale;
				}
				else
				{
					// Drawing
					float xpos = currentPosition.x + character.Bearing.x * scale;
					float ypos = currentPosition.y - (textStyle.FontSize - character.Bearing.y) * scale;

					textBuffer[currentIndex].Transform = glm::translate(glm::mat4(1.0f), { xpos, ypos, textStyle.Position.z }) *
						glm::scale(glm::mat4(1.0f), { s_TextInfo.FontTextureSize * scale, s_TextInfo.FontTextureSize * scale, 0 });

					textBuffer[currentIndex].Color = textStyle.Color;

					textBuffer[currentIndex].Letter.x = (float)character.ID;

					currentPosition.x += (character.Advance >> 6) * scale;
					currentIndex++;

					if (currentIndex == s_TextInfo.MaxCharacterUniformArrayLimit)
					{
						s_TextData.UniformBuffer.SetData(textBuffer, s_TextInfo.MaxCharacterUniformArrayLimit * sizeof(Renderer2DUniformText));
						RendererCommand::DrawStripArraysInstanced(4, currentIndex);
						currentIndex = 0;

						Renderer::s_Stats.DrawCalls++;
						Renderer::s_Stats.Quads += currentIndex;
					}
				}
			}
		}

		if (currentIndex)
		{
			s_TextData.UniformBuffer.SetData(textBuffer, s_TextInfo.MaxCharacterUniformArrayLimit * sizeof(Renderer2DUniformText));
			RendererCommand::DrawStripArraysInstanced(4, currentIndex);

			Renderer::s_Stats.DrawCalls++;
			Renderer::s_Stats.Quads += currentIndex;
		}

		delete[] textBuffer;

		RendererCommand::EnableDepthMask();
	}

	void Renderer2D::PrepareQuadRendering()
	{
		s_QuadData.VertexArray.Create();
		s_QuadData.VertexArray.Bind();

		s_QuadData.VertexBuffer.Create(s_Info.MaxVertices * sizeof(Quad2DVertex));
		s_QuadData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Float2, "a_TexCoor"  },
			{ ShaderDataType::Float,  "a_TexIndex" }
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
		s_QuadData.IndexBuffer.Create(s_Info.MaxIndices, indices, true);
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
		quadShaderData["#max_texture_slots"] = std::to_string(maxTextureSlots);

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

	void Renderer2D::PrepareTextRendering()
	{
		s_TextData.VertexArray.Create();
		s_TextData.VertexArray.Bind();

		float vertices[] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
		};

		s_TextData.VertexBuffer.Create(sizeof(vertices), vertices, true);
		s_TextData.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" }
		});

		s_TextData.VertexArray.SetVertexBuffer(s_TextData.VertexBuffer);

		std::unordered_map<std::string, std::string> textShaderData;
		textShaderData["#max_text_uniform_array_limit"] = std::to_string(s_TextInfo.MaxCharacterUniformArrayLimit);

		s_TextData.Texture.Create(s_TextInfo.FontPath, s_TextInfo.FontTextureSize, s_TextInfo.FontCharactersCount);

		s_TextData.Shader.Create("assets/shaders/text2D.vert.glsl", "assets/shaders/text2D.frag.glsl", textShaderData);
		s_TextData.Shader.Bind();

		s_TextData.UniformBuffer.Create(s_TextInfo.MaxCharacterUniformArrayLimit * sizeof(Renderer2DUniformText), 1);

		s_TextData.VertexArray.Unbind();
		s_TextData.VertexBuffer.Unbind();
		s_TextData.Shader.Unbind();
	}

	void Renderer2D::DrawQuad(const Renderer2DQuadInfo& info, const glm::vec4& color)
	{
		glm::mat4 transform = 
			glm::translate(glm::mat4(1.0f), info.Position) *
			glm::toMat4(glm::quat(info.Rotation)) * 
			glm::scale(glm::mat4(1.0f), glm::vec3(info.Size, 1.0f));

		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			Quad2DVertex vertex;
			vertex.Position = transform * quad_vertex_positions[i];
			vertex.Color    = color;
			vertex.TexCoord = quad_vertex_tex_coords[i];

			s_QuadData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer2D::DrawQuad(const Renderer2DQuadInfo& info, const Texture2D& texture)
	{
		float textureID = texture.GetRendererID();
		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), info.Position) *
			glm::toMat4(glm::quat(info.Rotation)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(info.Size, 1.0f));

		for (uint32_t i = 0; i < quad_vertex_count; i++)
		{
			Quad2DVertex vertex;
			vertex.Position = transform * quad_vertex_positions[i];
			vertex.TexCoord = quad_vertex_tex_coords[i];
			vertex.TexIndex = textureID; // TexIndex temporarily holds the texture rendererID

			s_QuadData.Vertices.emplace_back(vertex);
		}
	}

	void Renderer2D::DrawText(const std::string& text, const TextStyle2D& textStyle)
	{
		s_TextData.Data.emplace_back(text, textStyle);
	}

	void Renderer2D::ResetMousePosition(const glm::vec2& position)
	{
		if (position.x < 0.0f || position.y < 0.0f)
		{
			auto [width, height] = Application::Get().GetWindow().GetWindowSize();
			s_MouseData.Position = { width / 2.0f, height / 2.0f };
		}
		else
			s_MouseData.Position = position;
	}

}
