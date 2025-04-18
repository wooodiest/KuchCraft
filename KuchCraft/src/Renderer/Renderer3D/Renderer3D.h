#pragma once

#include "Renderer/Renderer3D/Renderer3DData.h"

// lOVE windows.h <3
#ifdef DrawText
	#undef DrawText
#endif

namespace KuchCraft {

	class Renderer3D
	{
	public:
		static void DrawChunk(Chunk* chunk);
		static void DrawChunks(const std::vector<Chunk*>& chunks);

		static void DrawItem(const glm::vec3& position, const glm::vec3& rotation, const Item& item);
		static void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const glm::vec4& color);
		static void DrawCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f), float thickness = 0.1f);

		static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec2& size, const Texture2D& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawText(const std::string& text, const TextStyle3D& textStyle);

		static void DrawWaterTinted();
		static void DrawOutlinedCube(const glm::vec3& position, const glm::vec3& size);

	private:
		static void Init();
		static void ShutDown();
		static void LoadRenderer3DInfo();

		static void Render();
		static void Clear();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);

		static uint32_t GetRendererdColorAttachmentRendererID() { return s_TintedData.FrameBuffer.GetColorAttachmentRendererID(); }

	private:
		static void PrepareChunks();
		static void PrepareSkybox();
		static void PrepareOutlinedCube();
		static void PrepareTinted();
		static void PrepareTextRendering();
		static void PrepareQuads();
		static void PrepareCubes();
		static void PrepareTransparentQuads();

		static void RenderChunks();
		static void RenderSkybox();
		static void RenderOutlinedCube();
		static void RenderTinted();
		static void RenderText();

		static void RenderQuads();
		static void FlushQuads();
		static void StartQuadsBatch();
		static void NextQuadsBatch();

		static void RenderCubes();
		static void FlushCubes();
		static void StartCubesBatch();
		static void NextCubesBatch();

		static void RenderTransparentQuads();
		static void FlushTransparentQuads();
		static void StartTransparentQuadsBatch();
		static void NextTransparentQuadsBatch();
		static void FlushSortedTransparentQuads();
		static void NextSortedTransparentQuadsBatch();

	private:
		Renderer3D() = default;

	private:
		static Renderer3DData                s_Data;
		static Renderer3DInfo                s_Info;
		static Renderer3DChunkData           s_ChunkData;
		static Renderer3DSkyboxData          s_SkyboxData;
		static Renderer3DOutlinedCubeData    s_OutlinedCubeData;
		static Renderer3DTintedData          s_TintedData;
		static Renderer3DQuadData            s_QuadData;
		static Renderer3DCubeData            s_CubeData;
		static Renderer3DTextInfo            s_TextInfo;
		static Renderer3DTextData            s_TextData;
		static Renderer3DTransparentQuadData s_TransparentQuadData;

	public:
		friend class Renderer;

	};

}