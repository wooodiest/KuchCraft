#pragma once

namespace KuchCraft
{
	enum class ImageFormat
	{
		RGB8 = 0,
		RGBA8,
	};

	enum class TextureFilter
	{
		LINEAR = 0,
		NEAREST
	};

	struct TextureSpecification
	{
		uint32_t      Width  = 1;
		uint32_t      Height = 1;
		ImageFormat   Format = ImageFormat::RGBA8;
		TextureFilter Filter = TextureFilter::LINEAR;
		bool          Mips   = true;
	};

	class Texture2D
	{
	public:
		Texture2D();
		~Texture2D();

		void Create(const TextureSpecification& textureSpecification);
		void Create(const std::string& path, const TextureSpecification& textureSpecification = TextureSpecification());

		uint32_t GetRendererID() const { return m_RendererID; }
		void Bind(uint32_t slot) const;

		// TODO: only works for rgba
		void Setdata(void* data, uint32_t size);

		static void Bind(uint32_t rendererID, uint32_t slot);

	private:
		uint32_t m_RendererID = 0;
		TextureSpecification m_TextureSpecification;
	};
}