#include "kcpch.h"
#include "Renderer/Data/Texture2D.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace KuchCraft {

	static GLenum ImageFormatToOpenGLDataFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RGB8:  return GL_RGB;
		case ImageFormat::RGBA8: return GL_RGBA;
		}

		return 0;
	}

	static GLenum ImageFormatToOpenGLInternalFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::RGB8:  return GL_RGB8;
		case ImageFormat::RGBA8: return GL_RGBA8;
		}

		return 0;
	}

	Texture2D::Texture2D()
	{

	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2D::Create(const TextureSpecification& textureSpecification)
	{
		m_TextureSpecification = textureSpecification;

		GLenum internalFormat = ImageFormatToOpenGLInternalFormat(m_TextureSpecification.Format);
		GLenum dataFormat = ImageFormatToOpenGLDataFormat(m_TextureSpecification.Format);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTextureStorage2D(m_RendererID, 1, internalFormat, m_TextureSpecification.Width, m_TextureSpecification.Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, m_TextureSpecification.Filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, m_TextureSpecification.Filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	void Texture2D::Create(const std::string& path, const TextureSpecification& textureSpecification)
	{
		m_TextureSpecification = textureSpecification;

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data)
		{
			KC_ERROR("Failed to load texture: {0}", path.c_str());
		}

		m_TextureSpecification.Width = width;
		m_TextureSpecification.Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_TextureSpecification.Width, m_TextureSpecification.Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, m_TextureSpecification.Filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, m_TextureSpecification.Filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_TextureSpecification.Width, m_TextureSpecification.Height, dataFormat, GL_UNSIGNED_BYTE, data);
		if (m_TextureSpecification.Mips)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		stbi_image_free(data);
	}	

	void Texture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void Texture2D::Setdata(void* data, uint32_t size)
	{
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_TextureSpecification.Width, m_TextureSpecification.Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	void Texture2D::Bind(uint32_t rendererID, uint32_t slot)
	{
		glBindTextureUnit(slot, rendererID);
	}

}