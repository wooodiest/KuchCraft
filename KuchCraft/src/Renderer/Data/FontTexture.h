#pragma once

namespace KuchCraft {

	struct FontCharacter
	{
		uint32_t   ID;
		uint32_t   Advance;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
	};

	class FontTexture
	{
	public:
		FontTexture();
		~FontTexture();

		void Create(const std::string& fontPath, uint32_t size, uint32_t charactersCount);

		const FontCharacter& GetCharacter(char c) { return m_Characters[c]; }

		void Bind()   const;
		void Unbind() const;

	private:
		uint32_t m_RendererID = 0;
		std::map<char, FontCharacter> m_Characters;
	};

}