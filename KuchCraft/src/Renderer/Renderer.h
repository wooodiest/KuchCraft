#pragma once

#include "Game/Camera.h"
#include "World/Chunk.h"

#include <glad/glad.h>

namespace KuchCraft {

	class Renderer
	{
	public:
		static void Init();
		static void ShutDown();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);
		static void OnUpdate(float dt);

		// Renderer
		static void BeginFrame();
		static void EndFrame();

		// Scene
		static void BeginWorld(const Camera& camera);
		static void EndWorld();

		// Utils
		static void SetWaterTintStatus(bool status);
		static void ShowTriangles(bool status);
		static uint32_t GetTexture(BlockType type);

		static std::unordered_map<std::string, std::string>& GetShaderStrMap() { return s_RendererData.ShaderStrData; }
		static std::unordered_map<std::string, std::string>& GetShaderVarMap() { return s_RendererData.ShaderVarData; }

		// Stats
		static bool GetShowStatsStatus()  { return s_RendererData.ShowStats;                      }
		static void FlipShowStatsStatus() { s_RendererData.ShowStats = !s_RendererData.ShowStats; }

		static RendererData s_RendererData;
	private:
		// Utils
		static void InvalidateMainFrameBuffer(uint32_t width, uint32_t height);
		static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

		// Loading textures
		static void LoadTextures();

		// Prepare renderer for...
		static void PrepareShaders();
		static void PrepareRenderer();
		 
	private:
		Renderer() = default;

	};

}