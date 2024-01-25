#pragma once

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
		World m_World;
		Player m_Player;

	};

}