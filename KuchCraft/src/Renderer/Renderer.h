#pragma once

#include <glm/glm.hpp>
#include "Renderer/Camera.h"
#include "World/World.h"
#include "Shader.h"

namespace KuchCraft {

	struct RendererStatistics
	{
		uint32_t DrawCalls = 0;
	};

	class Renderer
	{
	public:
		Renderer();
		inline static Renderer& Get() { return *s_Instance; }
		void OnViewportSizeChanged(uint32_t width, uint32_t height);

		void ShutDown();

		void BeginScene(Camera& camera);
		void EndScene();

		void DrawCube(const glm::vec3& position, const Block& block);

		RendererStatistics m_Stats;

	private:
		Camera* m_Camera;

		Shader m_DefaultCubeShader;
		unsigned int m_VBO, m_VAO, m_IBO, m_Texture;

	private:
		static Renderer* s_Instance;

	};

}