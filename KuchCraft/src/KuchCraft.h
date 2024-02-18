#pragma once

#include "KuchCraftData.h"
#include "World/World.h"

namespace KuchCraft {

	class KuchCraft
	{
	public:
		KuchCraft();
		~KuchCraft();

		void OnUpdate(float dt);

		void OnViewportSizeChanged(uint32_t width, uint32_t height);
		
		void LoadWorld(const std::string& path);

	private:
		World* m_World = nullptr;
		
	};

}