#include "kcpch.h"
#include "Renderer/Data/FontTexture.h"

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace KuchCraft {

	Ref<FontTexture> FontTexture::Create(const std::string& fontPath, uint32_t size, uint32_t charactersCount)
	{
		return CreateRef<FontTexture>(fontPath, size, charactersCount);
	}

	FontTexture::FontTexture(const std::string& fontPath, uint32_t size, uint32_t charactersCount)
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			KC_ERROR("Failed to init freetype");
		}

		FT_Face face;
		if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
		{
			KC_ERROR("Failed to load font: {0}", fontPath.c_str());
		}

		FT_Set_Pixel_Sizes(face, size, size);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, size, size, charactersCount, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

		for (unsigned char c = 0; c < charactersCount; c++)
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

			m_Characters.emplace(c, FontCharacter{
				(uint32_t)c,
				(uint32_t)face->glyph->advance.x,
				{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
				{ face->glyph->bitmap_left, face->glyph->bitmap_top   }
			});
		}
	}

	FontTexture::~FontTexture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void FontTexture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	void FontTexture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

}

