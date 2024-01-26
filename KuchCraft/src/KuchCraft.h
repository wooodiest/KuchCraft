#pragma once

#include "Game/Player.h"

namespace KuchCraft {

	class KuchCraft
	{
	public:
		KuchCraft();
		~KuchCraft();

		void OnUpdate(float dt);

		void OnViewportSizeChanged(uint32_t width, uint32_t height);

	private:
		Player m_Player;

	};

}