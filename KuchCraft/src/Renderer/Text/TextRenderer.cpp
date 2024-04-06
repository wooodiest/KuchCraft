#include "kcpch.h"
#include "Renderer/Text/TextRenderer.h"

#include "Renderer/Renderer.h"

#include "Renderer/RendererCommand.h"

#include "Core/Application.h"

namespace KuchCraft {

	TextRendererInfo TextRenderer::s_Info;
	TextRendererData TextRenderer::s_Data;

	void TextRenderer::Init()
	{
		KC_PROFILE_FUNCTION();

		LoadTextInfo();

		s_Data.VertexArray.Create();
		s_Data.VertexArray.Bind();

		float vertices[] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
		};

		s_Data.VertexBuffer.Create(sizeof(vertices), vertices, true);
		s_Data.VertexBuffer.SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" }
		});

		s_Data.VertexArray.SetVertexBuffer(s_Data.VertexBuffer);

		std::unordered_map<std::string, std::string> textShaderData;
		textShaderData["##max_text_uniform_array_limit"] = std::to_string(s_Info.MaxCharacterUniformArrayLimit);

		s_Data.Texture.Create(s_Info.FontPath, s_Info.FontTextureSize, s_Info.FontCharactersCount);

		s_Data.Shader.Create("assets/shaders/text.vert.glsl", "assets/shaders/text.frag.glsl", textShaderData);
		s_Data.Shader.Bind();

		s_Data.UniformBuffer.Create(s_Info.MaxCharacterUniformArrayLimit * sizeof(UniformText), 1);

		s_Data.VertexArray .Unbind();
		s_Data.VertexBuffer.Unbind();
		s_Data.Shader      .Unbind();
	}

	void TextRenderer::ShutDown()
	{
		KC_PROFILE_FUNCTION();
	}

	void TextRenderer::Render()
	{
		KC_PROFILE_FUNCTION();

		Renderer::s_Stats.TextTimer.Start();

		RendererCommand::EnableBlending();
		RendererCommand::DisableFaceCulling();
		RendererCommand::DisableDepthTesting();

		s_Data.Shader.Bind();
		s_Data.Texture.Bind();

		s_Data.VertexArray.Bind();
		s_Data.VertexBuffer.Bind();

		UniformText* textBuffer = new UniformText[s_Info.MaxCharacterUniformArrayLimit];

		for (const auto& [text, textStyle] : s_Data.TextData)
		{
			s_Data.Shader.SetFloat4("u_Color", textStyle.Color);

			glm::vec2 currentPosition = textStyle.Position;
			float scale               = textStyle.FontSize / s_Info.FontTextureSize;

			uint32_t currentIndex = 0;
			for (auto c = text.begin(); c != text.end(); c++)
			{
				const FontCharacter& character = s_Data.Texture.GetCharacter(*c);

				if (*c == '\n')
				{
					currentPosition.y -= (character.Size.y) * textStyle.FontSpacing * scale;
					currentPosition.x  = textStyle.Position.x;
				}
				else if (*c == ' ')
				{
					currentPosition.x += (character.Advance >> 6) * scale;
				}
				else
				{
					float xpos = currentPosition.x + character.Bearing.x * scale;
					float ypos = currentPosition.y - (textStyle.FontSize - character.Bearing.y) * scale;

					textBuffer[currentIndex].Transform = glm::translate(glm::mat4(1.0f), { xpos, ypos, 0 }) *
						glm::scale(glm::mat4(1.0f), { s_Info.FontTextureSize * scale, s_Info.FontTextureSize * scale, 0 });
					textBuffer[currentIndex].Letter.x = (float)character.ID;

					currentPosition.x += (character.Advance >> 6) * scale;
					currentIndex++;

					if (currentIndex == s_Info.MaxCharacterUniformArrayLimit)
					{
						s_Data.UniformBuffer.SetData(textBuffer, s_Info.MaxCharacterUniformArrayLimit * sizeof(UniformText));
						RendererCommand::DrawStripArraysInstanced(4, currentIndex);
						currentIndex = 0;

						Renderer::s_Stats.DrawCalls++;
						Renderer::s_Stats.Quads += currentIndex;
					}
				}
			}
			if (currentIndex)
			{
				s_Data.UniformBuffer.SetData(textBuffer, s_Info.MaxCharacterUniformArrayLimit * sizeof(UniformText));
				RendererCommand::DrawStripArraysInstanced(4, currentIndex);

				Renderer::s_Stats.DrawCalls++;
				Renderer::s_Stats.Quads += currentIndex;
			}

		}
		delete[] textBuffer;

		Renderer::s_Stats.TextTimer.Finish();
	}

	void TextRenderer::Clear()
	{
		KC_PROFILE_FUNCTION();

		s_Data.TextData.clear();
	}

	void TextRenderer::Text(const std::string& text, const TextStyle& textStyle)
	{
		s_Data.TextData.emplace_back(text, textStyle);
	}

	void TextRenderer::Text(const std::string& text, const glm::vec2& position)
	{
		Text(text, TextStyle{
			s_Info.DefaultFontColor,
			position,
			s_Info.DefaultFontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::Text(const std::string& text, const glm::vec2& position, float fontSize)
	{
		Text(text, TextStyle{
			s_Info.DefaultFontColor,
			position,
			fontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::Text(const std::string& text, const glm::vec2& position, const glm::vec4& color)
	{
		Text(text, TextStyle{
			color,
			position,
			s_Info.DefaultFontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::Text(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize)
	{
		Text(text, TextStyle{
			color,
			position,
			fontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextTopLeft(const std::string& text, const TextStyle& textStyle)
	{
		Text(text, TextStyle{
			s_Info.DefaultFontColor,
			glm::vec2(textStyle.Position.x, Application::Get().GetWindow().GetHeight() - textStyle.Position.y),
			s_Info.DefaultFontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextTopLeft(const std::string& text, const glm::vec2& position)
	{
		Text(text, TextStyle{
			s_Info.DefaultFontColor,
			glm::vec2(position.x, Application::Get().GetWindow().GetHeight() - position.y),
			s_Info.DefaultFontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextTopLeft(const std::string& text, const glm::vec2& position, float fontSize)
	{
		Text(text, TextStyle{
			s_Info.DefaultFontColor,
			glm::vec2(position.x, Application::Get().GetWindow().GetHeight() - position.y),
			fontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextTopLeft(const std::string& text, const glm::vec2& position, const glm::vec4& color)
	{
		Text(text, TextStyle{
			color,
			glm::vec2(position.x, Application::Get().GetWindow().GetHeight() - position.y),
			s_Info.DefaultFontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextTopLeft(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize)
	{
		Text(text, TextStyle{
			color,
			glm::vec2(position.x, Application::Get().GetWindow().GetHeight() - position.y),
			fontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextN(const std::string& text, const TextStyle& textStyle)
	{
		Text(text, TextStyle{
			s_Info.DefaultFontColor,
			glm::vec2(textStyle.Position.x * Application::Get().GetWindow().GetWidth(), textStyle.Position.y * Application::Get().GetWindow().GetHeight()),
			s_Info.DefaultFontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextN(const std::string& text, const glm::vec2& position)
	{
		Text(text, TextStyle{
			s_Info.DefaultFontColor,
			glm::vec2(position.x * Application::Get().GetWindow().GetWidth(), position.y * Application::Get().GetWindow().GetHeight()),
			s_Info.DefaultFontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextN(const std::string& text, const glm::vec2& position, float fontSize)
	{
		Text(text, TextStyle{
			s_Info.DefaultFontColor,
			glm::vec2(position.x * Application::Get().GetWindow().GetWidth(), position.y * Application::Get().GetWindow().GetHeight()),
			fontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextN(const std::string& text, const glm::vec2& position, const glm::vec4& color)
	{
		Text(text, TextStyle{
			color,
			glm::vec2(position.x * Application::Get().GetWindow().GetWidth(), position.y * Application::Get().GetWindow().GetHeight()),
			s_Info.DefaultFontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::TextN(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize)
	{
		Text(text, TextStyle{
			color,
			glm::vec2(position.x * Application::Get().GetWindow().GetWidth(), position.y * Application::Get().GetWindow().GetHeight()),
			fontSize,
			s_Info.DefaultFontSpacing
		});
	}

	void TextRenderer::LoadTextInfo()
	{
		s_Info = TextRendererInfo();
	}

}

