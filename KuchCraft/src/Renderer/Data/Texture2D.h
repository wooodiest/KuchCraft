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
		static Ref<Texture2D> Create(const TextureSpecification& textureSpecification);
		static Ref<Texture2D> Create(const std::string& path, const TextureSpecification& textureSpecification = TextureSpecification());

		Texture2D(const TextureSpecification& textureSpecification);
		Texture2D(const std::string& path, const TextureSpecification& textureSpecification);
		~Texture2D();

		uint32_t GetRendererID() const { return m_RendererID; }
		void Bind(uint32_t slot) const;

	private:
		uint32_t m_RendererID = 0;
		TextureSpecification m_TextureSpecification;
	};
}