#pragma once

namespace KuchCraft {

	struct CubeMapFacesInfo
	{
		std::string PathXpos;
		std::string PathXneg;
		std::string PathYpos;
		std::string PathYneg;
		std::string PathZpos;
		std::string PathZneg;
	};

	class CubeMapTexture
	{
	public:
		CubeMapTexture();
		~CubeMapTexture();

		void Create(const CubeMapFacesInfo& cubeMapFacesInfo);

		void Bind()   const;
		void Unbind() const;

	private:
		uint32_t m_RendererID = 0;

	};

}