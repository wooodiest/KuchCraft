#pragma once

#include "Renderer/Text/TextRendererData.h"

namespace KuchCraft {

	// TODO: Add stats and get acces to shader preprocessing

	class TextRenderer
	{
	public:
		// position in pixels (0,0) from bottom-left corner
		static void Text(const std::string& text, const TextStyle& textStyle);
		static void Text(const std::string& text, const glm::vec2& position);
		static void Text(const std::string& text, const glm::vec2& position, float fontSize);
		static void Text(const std::string& text, const glm::vec2& position, const glm::vec4& color);
		static void Text(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize);

		// position in pixels from top-left corner
		static void TextTopLeft(const std::string& text, const TextStyle& textStyle);
		static void TextTopLeft(const std::string& text, const glm::vec2& position);
		static void TextTopLeft(const std::string& text, const glm::vec2& position, float fontSize);
		static void TextTopLeft(const std::string& text, const glm::vec2& position, const glm::vec4& color);
		static void TextTopLeft(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize);

		// position normalized from bottom-left corner
		static void TextN(const std::string& text, const TextStyle& textStyle);
		static void TextN(const std::string& text, const glm::vec2& position);
		static void TextN(const std::string& text, const glm::vec2& position, float fontSize);
		static void TextN(const std::string& text, const glm::vec2& position, const glm::vec4& color);
		static void TextN(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize);

	private:
		static void Init();
		static void ShutDown();
		static void LoadTextInfo();

		static void Render();
		static void Clear();

	private:
		static TextRendererInfo s_Info;
		static TextRendererData s_Data;

	private:
		TextRenderer() = default;

	public:
		friend class Renderer;

	};

}