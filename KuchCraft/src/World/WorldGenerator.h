#pragma once

#include "World.h"

namespace KuchCraft {

	class WorldGenerator
	{
	public:
		static void Init();
		static void SetSeed(uint64_t seed);

		static void FillWithRandomBlocks(Chunk* chunk);

	private:
		static uint64_t s_Seed;
	};



}