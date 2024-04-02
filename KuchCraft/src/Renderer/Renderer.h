#pragma once

#include "Renderer/Data/Camera.h"
#include "World/Chunk.h"

#include <glad/glad.h>

namespace KuchCraft {

	class Renderer
	{
	public:
		static void Init();
		static void ShutDown();

		static void OnViewportSizeChanged(uint32_t width, uint32_t height);

		static void BeginFrame();
		static void EndFrame(Camera* camera);

	private:
		static RendererData s_RendererData;

		static void PrepareRenderer();
		static void LoadRendererInfo();
		 
	public:
		friend class Renderer3D;

	private:
		Renderer() = default;

	};

}