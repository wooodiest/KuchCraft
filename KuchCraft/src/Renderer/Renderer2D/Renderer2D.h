#pragma once

#include "Renderer/Renderer2D/Renderer2DData.h"

#include "Renderer/Data/Texture2D.h"

// lOVE windows.h <3
#ifdef DrawText
	#undef DrawText
#endif

namespace KuchCraft {

	//TODO: Sort text by depth

	class Renderer2D
	{
	public:
		static void DrawQuad(const Renderer2DQuadInfo& info, const glm::vec4& color);
		static void DrawQuad(const Renderer2DQuadInfo& info, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		// Text
		// (0, 0) is bottom left corner
		// N - normalized position
		// TopLeft (0, 0) is top left corner
		static void DrawText(const std::string& text, const TextStyle2D& textStyle);

		static void DrawText(const std::string& text, const glm::vec2& position);
		static void DrawText(const std::string& text, const glm::vec2& position, float fontSize);
		static void DrawText(const std::string& text, const glm::vec2& position, const glm::vec4& color);
		static void DrawText(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize);

		static void DrawText(const std::string& text, const glm::vec3& position);
		static void DrawText(const std::string& text, const glm::vec3& position, float fontSize);
		static void DrawText(const std::string& text, const glm::vec3& position, const glm::vec4& color);
		static void DrawText(const std::string& text, const glm::vec3& position, const glm::vec4& color, float fontSize);

		static void DrawTextN(const std::string& text, const glm::vec2& position);
		static void DrawTextN(const std::string& text, const glm::vec2& position, float fontSize);
		static void DrawTextN(const std::string& text, const glm::vec2& position, const glm::vec4& color);
		static void DrawTextN(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize);

		static void DrawTextN(const std::string& text, const glm::vec3& position);
		static void DrawTextN(const std::string& text, const glm::vec3& position, float fontSize);
		static void DrawTextN(const std::string& text, const glm::vec3& position, const glm::vec4& color);
		static void DrawTextN(const std::string& text, const glm::vec3& position, const glm::vec4& color, float fontSize);

		static void DrawTextTopLeft(const std::string& text, const glm::vec2& position);
		static void DrawTextTopLeft(const std::string& text, const glm::vec2& position, float fontSize);
		static void DrawTextTopLeft(const std::string& text, const glm::vec2& position, const glm::vec4& color);
		static void DrawTextTopLeft(const std::string& text, const glm::vec2& position, const glm::vec4& color, float fontSize);

		static void DrawTextTopLeft(const std::string& text, const glm::vec3& position);
		static void DrawTextTopLeft(const std::string& text, const glm::vec3& position, float fontSize);
		static void DrawTextTopLeft(const std::string& text, const glm::vec3& position, const glm::vec4& color);
		static void DrawTextTopLeft(const std::string& text, const glm::vec3& position, const glm::vec4& color, float fontSize);

	private:
		static void Init();
		static void ShutDown();
		static void LoadRenderer2DInfo();

		static void Clear();
		static void Render();

		static void RenderQuads();
		static void FlushQuads();
		static void StartQuadsBatch();
		static void NextQuadsBatch();

		static void RenderFullScreenQuad(uint32_t rendererID);
		static void RenderText();

		static void PrepareQuadRendering();
		static void PrepareFullScreenQuadRendering();
		static void PrepareTextRendering();

	private:
		static Renderer2DInfo               s_Info;
		static Renderer2DQuadData           s_QuadData;
		static Renderer2DFullScreenQuadData s_FullScreenQuadData;
		static Renderer2DTextInfo           s_TextInfo;
		static Renderer2DTextData           s_TextData;

	public:
		friend class Renderer;

	private:
		Renderer2D() = default;
	};

}