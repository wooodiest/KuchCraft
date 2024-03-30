#include "kcpch.h"
#include "Renderer/Text/TextRenderer.h"

#include "Core/Application.h"

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace KuchCraft {

	TextRendererInfo TextRenderer::s_Info;
	TextRendererData TextRenderer::s_Data;

	void TextRenderer::Init()
	{
		KC_PROFILE_FUNCTION();

		LoadTextInfo();

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			KC_ERROR("Failed to init freetype");
		}

		FT_Face face;
		if (FT_New_Face(ft, s_Info.FontPath.c_str(), 0, &face))
		{
			KC_ERROR("Failed to load font: {0}", s_Info.FontPath.c_str());
		}

		FT_Set_Pixel_Sizes(face, s_Info.FontTextureSize, s_Info.FontTextureSize);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &s_Data.Texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, s_Data.Texture);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, s_Info.FontTextureSize, s_Info.FontTextureSize, s_Info.FontCharactersCount, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

		for (unsigned char c = 0; c < s_Info.FontCharactersCount; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				KC_ERROR("Failed to load glyph: {0}", c);
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

			s_Data.TextCharacters.emplace(c, TextCharacter{
				(uint32_t)c,
				(uint32_t)face->glyph->advance.x,
				{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
				{ face->glyph->bitmap_left, face->glyph->bitmap_top   }
			});
		}

		s_Data.VertexArray = VertexArray::Create();
		s_Data.VertexArray->Bind();

		float vertices[] = {
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
		};

		s_Data.VertexBuffer = VertexBuffer::Create(sizeof(vertices), vertices, true);
		s_Data.VertexBuffer->SetBufferLayout({
			{ ShaderDataType::Float2, "a_Position" }
		});

		s_Data.VertexArray->SetVertexBuffer(s_Data.VertexBuffer);

		s_Data.Shader = Shader::Create("assets/shaders/text.vert.glsl", "assets/shaders/text.frag.glsl");
		s_Data.Shader->Bind();

		s_Data.UniformBuffer = UniformBuffer::Create(s_Info.MaxCharacterUniformArrayLimit * sizeof(UniformText), 1);

		s_Data.VertexArray ->Unbind();
		s_Data.VertexBuffer->Unbind();
		s_Data.Shader      ->Unbind();
	}

	void TextRenderer::ShutDown()
	{
		KC_PROFILE_FUNCTION();
	}

	void TextRenderer::Render()
	{
		KC_PROFILE_FUNCTION();

		glEnable(GL_BLEND);

		s_Data.Shader->Bind();
		constexpr uint32_t default_texture_slott = 0;
		glBindTextureUnit(default_texture_slott, s_Data.Texture);

		s_Data.VertexArray->Bind();
		s_Data.VertexBuffer->Bind();

		UniformText* textBuffer = new UniformText[s_Info.MaxCharacterUniformArrayLimit];

		for (const auto& [text, textStyle] : s_Data.TextData)
		{
			s_Data.Shader->SetFloat4("u_Color", textStyle.Color);

			glm::vec2 currentPosition = textStyle.Position;
			float scale               = textStyle.FontSize / s_Info.FontTextureSize;

			uint32_t currentIndex = 0;
			for (auto c = text.begin(); c != text.end(); c++)
			{
				TextCharacter character = s_Data.TextCharacters[*c];

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
						s_Data.UniformBuffer->SetData(textBuffer, s_Info.MaxCharacterUniformArrayLimit * sizeof(UniformText));
						glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, currentIndex);
						currentIndex = 0;

					}
				}
			}
			if (currentIndex)
			{
				s_Data.UniformBuffer->SetData(textBuffer, s_Info.MaxCharacterUniformArrayLimit * sizeof(UniformText));
				glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, currentIndex);
			}

		}
		delete[] textBuffer;

		glDisable(GL_BLEND);
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
		// TODO: load TextRendererInfo from file
	}

}

