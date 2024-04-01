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
		static Ref<CubeMapTexture> Create(const CubeMapFacesInfo& cubeMapFacesInfo);

		CubeMapTexture(const CubeMapFacesInfo& cubeMapFacesInfo);
		~CubeMapTexture();

		void Bind()   const;
		void Unbind() const;

	private:
		uint32_t m_RendererID = 0;

	};

}