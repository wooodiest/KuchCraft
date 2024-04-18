#pragma once

#include "World/World.h"

#include "perlin_noise.hpp"

namespace KuchCraft {

	class WorldGenerator
	{
	public:
		static void Init(uint32_t seed);
		static void ShutDown();

		static void Generate(Chunk* chunk);

		static siv::PerlinNoise::seed_type GetSeed() { return s_Seed; }

	private:
		static siv::PerlinNoise::seed_type s_Seed;
		static siv::PerlinNoise s_Noise;
	};



}