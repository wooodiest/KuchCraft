#pragma once

#include "World/WorldData.h"
#include "World/World.h"

#include "Renderer/RendererData.h"
#include "Renderer/Utils/ChunkDrawList.h"

#include "Core/Events/Event.h"

namespace KuchCraft {

	struct KuchCraftSettings
	{
		uint32_t RenderDistance             = 8;
		uint32_t ChunksKeptInMemoryDistance = 16;
	};

	class KuchCraft
	{
	public:
		KuchCraft();
		~KuchCraft();

		static const KuchCraftSettings& GetSettings() { return s_Settings; }

		void OnUpdate(float dt);
		void OnEvent(Event& event);

		void LoadWorld(const std::string& path);
		void SetRenderDistance(uint32_t distance);
		void SetKeptInMemoryChunksDistance(uint32_t distance);

	private:
		World* m_World = nullptr;

		static KuchCraftSettings s_Settings;
	};

}