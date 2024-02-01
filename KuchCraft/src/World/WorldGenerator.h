#pragma once

#include "World.h"

#include "perlin_noise.hpp"

namespace KuchCraft {

	class WorldGenerator
	{
	public:
		static void Init(uint64_t seed);
		static void ShutDown();

		static void FillWithRandomBlocks(Chunk* chunk);
		static void Generate(Chunk* chunk);

	private:
		static siv::PerlinNoise::seed_type s_Seed;
		static siv::PerlinNoise s_Noise;
	};



}