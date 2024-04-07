#pragma once

#include "Renderer/Renderer2D/Renderer2DData.h"

#include "Renderer/Data/Texture2D.h"

namespace KuchCraft {

	class Renderer2D
	{
	public:
		// N  - normalized position
		// NN - normalized position and size
		static void DrawQuad  (const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuadN (const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuadNN(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad  (const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuadN (const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuadNN(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad  (const glm::vec2& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuadN (const glm::vec2& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuadNN(const glm::vec2& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad  (const glm::vec3& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuadN (const glm::vec3& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuadNN(const glm::vec3& position, const glm::vec2& size, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad  (const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuadN (const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuadNN(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);

		static void DrawRotatedQuad  (const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color);
		static void DrawRotatedQuadN (const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color);
		static void DrawRotatedQuadNN(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color);

		static void DrawRotatedQuad  (const glm::vec2& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuadN (const glm::vec2& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuadNN(const glm::vec2& position, const glm::vec2& size, float rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad  (const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuadN (const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuadNN(const glm::vec2& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad  (const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuadN (const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuadNN(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

	private:
		static void Init();
		static void ShutDown();
		static void LoadRenderer2DInfo();

		static void Clear();
		static void Render();

		static void RenderQuads();
		static void Flush();
		static void StartBatch();
		static void NextBatch();

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