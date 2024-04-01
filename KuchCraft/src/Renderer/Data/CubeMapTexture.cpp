#include "kcpch.h"
#include "Renderer/Data/CubeMapTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace KuchCraft {

	static std::string GetPath(uint32_t i, const CubeMapFacesInfo& cubeMapFacesInfo)
	{
		enum class CubeMapFace
		{
			Xpos = 0,
			Xneg = 1,
			Ypos = 2,
			Yneg = 3,
			Zpos = 4,
			Zneg = 5
		};

		CubeMapFace face = (CubeMapFace)i;
		switch (face)
		{
			case CubeMapFace::Xpos: return cubeMapFacesInfo.PathXpos; break;
			case CubeMapFace::Xneg: return cubeMapFacesInfo.PathXneg; break;
			case CubeMapFace::Ypos: return cubeMapFacesInfo.PathYpos; break;
			case CubeMapFace::Yneg: return cubeMapFacesInfo.PathYneg; break;
			case CubeMapFace::Zpos: return cubeMapFacesInfo.PathZpos; break;
			case CubeMapFace::Zneg: return cubeMapFacesInfo.PathZneg; break;
		}
	}

	Ref<CubeMapTexture> CubeMapTexture::Create(const CubeMapFacesInfo& cubeMapFacesInfo)
	{
		return CreateRef<CubeMapTexture>(cubeMapFacesInfo);
	}

	CubeMapTexture::CubeMapTexture(const CubeMapFacesInfo& cubeMapFacesInfo)
	{
		constexpr GLenum types[6] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		for (uint32_t i = 0; i < 6; i++)
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(0);
			unsigned char* data = stbi_load(GetPath(i, cubeMapFacesInfo).c_str(), &width, &height, &channels, 4);

			glTexImage2D(types[i], 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			stbi_image_free(data);
		}
	}

	CubeMapTexture::~CubeMapTexture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void CubeMapTexture::Bind() const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void CubeMapTexture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}
