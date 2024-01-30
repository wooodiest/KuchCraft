#pragma once

#include "World.h"

namespace KuchCraft {

	class WorldGenerator
	{
	public:
		static void Init(uint64_t seed);
		static void ShutDown();

		static void FillWithRandomBlocks(Chunk* chunk);
		static void Generate(Chunk* chunk);

	private:
		static uint64_t s_Seed;
	};



}