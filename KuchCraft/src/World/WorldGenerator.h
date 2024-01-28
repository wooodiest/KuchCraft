#pragma once

#include "World.h"

namespace KuchCraft {

	class WorldGenerator
	{
	public:
		static void Init();

		static void FillWithRandomBlocks(Chunk* chunk);
	};



}