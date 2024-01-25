#pragma once

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Game/Player.h"
#include "World/World.h"

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
		Renderer m_Renderer;
		World m_World;
		Player m_Player;

	};

}