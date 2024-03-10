#pragma once

#include "KuchCraftData.h"
#include "World/World.h"
#include "Core/Events/Event.h"

namespace KuchCraft {

	class KuchCraft
	{
	public:
		KuchCraft();
		~KuchCraft();

		void OnUpdate(float dt);
		void OnEvent(Event& event);
		
		void LoadWorld(const std::string& path);

	private:
		World* m_World = nullptr;
		
	};

}