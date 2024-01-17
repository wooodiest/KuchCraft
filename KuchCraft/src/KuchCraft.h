#pragma once

#include "Renderer/Shader.h"

namespace KuchCraft {

	class KuchCraft
	{
	public:
		KuchCraft();
		~KuchCraft();

		void Init();
		void OnUpdate(float dt);

		void OnViewportSizeChanged(uint32_t width, uint32_t height);

	private:
		Shader m_Shader;
		unsigned int m_VBO, m_VAO;

	};

}