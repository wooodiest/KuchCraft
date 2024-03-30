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

		// Stats
		static bool GetShowStatsStatus()  { return s_RendererData.ShowStats;                      }
		static void FlipShowStatsStatus() { s_RendererData.ShowStats = !s_RendererData.ShowStats; }

	private:
		static RendererData s_RendererData;

		static void PrepareRenderer();
		 
	private:
		Renderer() = default;

	};

}