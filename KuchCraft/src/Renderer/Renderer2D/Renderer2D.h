#pragma once

#include "Renderer/Renderer2D/Renderer2DData.h"

#include "Renderer/Data/Texture2D.h"

// LOVE windows.h <3
#ifdef DrawText
	#undef DrawText
#endif

namespace KuchCraft {

	class Renderer2D
	{
	public:
		static void DrawQuad(const Renderer2DQuadInfo& info, const glm::vec4& color);
		static void DrawQuad(const Renderer2DQuadInfo& info, const Texture2D& texture);

		static void DrawText(const std::string& text, const TextStyle2D& textStyle);

		static void ResetMousePosition(const glm::vec2& position = { -1.0f, -1.0f });

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
		static Renderer2DMouseData			s_MouseData;

	public:
		friend class Renderer;

	private:
		Renderer2D() = default;
	};

}