#pragma once

#include "Renderer/Renderer2D/Renderer2DData.h"

namespace KuchCraft {

	class Renderer2D
	{
	public:
		
	private:
		static void Init();
		static void ShutDown();
		static void LoadRenderer2DInfo();

		static void Clear();
		static void Render();

		static void RenderFullScreenQuad(uint32_t rendererID);

		static void PrepareQuadRendering();
		static void PrepareFullScreenQuadRendering();

	private:
		static Renderer2DInfo               s_Info;
		static Renderer2DQuadData           s_QuadData;
		static Renderer2DFullScreenQuadData s_FullScreenQuadData;

	public:
		friend class Renderer;

	private:
		Renderer2D() = default;
	};

}