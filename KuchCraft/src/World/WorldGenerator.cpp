#include "kcpch.h"
#include "World/WorldGenerator.h"

#include "Core/Random.h"

namespace KuchCraft {

	siv::PerlinNoise::seed_type WorldGenerator::s_Seed;
	siv::PerlinNoise WorldGenerator::s_Noise;
	static int s_SeeLevel = 60;

	void WorldGenerator::Init(uint32_t seed)
	{
		s_Seed = seed;
		s_Noise = siv::PerlinNoise{ s_Seed };
	}

	void WorldGenerator::ShutDown()
	{
		// Clear 
	}

	void WorldGenerator::Generate(Chunk* chunk)
	{
		auto& position = chunk->GetPosition();
		constexpr double zoomFactor = 256.0f; //chunk_size_XZ * chunk_size_XZ;
		constexpr double zoom1      = 1.0 / zoomFactor;
		constexpr double zoom2      = 1.0 / (zoomFactor / 2.0);

		// Terain shape
		// I dont know what I created here
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int z = 0; z < chunk_size_XZ; z++)
			{
				double peresistance = s_Noise.noise2D_01((position.x + x) * zoom1, (position.z + z) * zoom1) * 2.0;
				double H = s_Noise.octave2D((position.x + x) * zoom1, (position.z + z) * zoom1, 3, peresistance) + 
					s_Noise.octave2D((position.x + x) * zoom2, (position.z + z) * zoom2, 4);

				int height = s_SeeLevel + 10 + 10.0 * H; 
			
				for (int y = 0; y < chunk_size_Y; y++)
				{	
					if (y < height)
					{
						if (y <= s_SeeLevel)
						{
							if (y == s_SeeLevel - 1 || y == s_SeeLevel)
							{
								if (peresistance > 1.0)
									chunk->Items[x][y][z] = Item(ItemType::GrassBlock);								
								else
									chunk->Items[x][y][z] = Item(ItemType::Sand);
							}
							else
							{
								Item block = s_SeeLevel - y >= 6 ? Item(ItemType::Gravel) : Item(ItemType::Sand);
								if (y == height - 1)
									chunk->Items[x][y][z] = block;
								else if (y == height - 2)
									chunk->Items[x][y][z] = block;
								else if (y == height - 3)
									chunk->Items[x][y][z] = block;
								else if (y == height - 4)
									chunk->Items[x][y][z] = block;
								else
									chunk->Items[x][y][z] = Item(ItemType::Stone);
							}
						}
						else
						{
							if (y == height - 1)
							{
								chunk->Items[x][y][z] = Item(ItemType::GrassBlock);
								if (y < chunk_size_Y - 1 && Random::Float(0.0f, 100.0f) < 2.0f)
								{
									if (Random::Float(0.0f, 100.0f) < 90.0f)
										chunk->Items[x][y + 1][z] = Item(ItemType::Grass);
									else
										chunk->Items[x][y + 1][z] = Item((ItemType)Random::UInt(item_foliage_quad_flowers_begin, item_foliage_quad_mushrooms_end));
								}
							}
							else if (y == height - 2)
								chunk->Items[x][y][z] = Item(ItemType::Dirt);
							else if (y == height - 3)
								chunk->Items[x][y][z] = Item(ItemType::Dirt);
							else if (y == height - 4)
								chunk->Items[x][y][z] = Item(ItemType::Dirt);
							else
								chunk->Items[x][y][z] = Item(ItemType::Stone);
						}

					}
				}
			}
		}

		// Fill with bedrock and water
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int z = 0; z < chunk_size_XZ; z++)
			{
				chunk->Items[x][0][z] = Item(ItemType::Bedrock);
			}
		}
		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y - 1; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
				{
					if (y < s_SeeLevel - 1)
					{
						if (chunk->Items[x][y + 1][z].Type == ItemType::Air)
						{
							chunk->Items[x][y + 1][z] = Item(ItemType::Water);
						}
					}
				}
			}

		}

		// Foliage
		for (int x = 2; x < chunk_size_XZ - 2; x++)
		{
			for (int z = 2; z < chunk_size_XZ - 2; z++)
			{
				double peresistance = s_Noise.noise2D_01((position.x + x) * zoom1, (position.z + z) * zoom1) / 2.0;
				double tree = s_Noise.octave2D((position.x + x), (position.z + z), 3, peresistance); 
				bool treeProb = tree >= 0.5;
				for (int y = 0; y < chunk_size_Y - 7; y++)
				{
					// Max height trees can spaw
					if (y > 80)
						break;

					// We dont want trees to be next to rach other
					if (treeProb && chunk->Items[x][y][z].Type == ItemType::GrassBlock
						&& chunk->Items[x][y + 1][z].Type == ItemType::Air)
					{
						if (chunk->Items[x - 1][y + 1][z].Type == Item(ItemType::OakLog).Type ||
							chunk->Items[x + 1][y + 1][z].Type == Item(ItemType::OakLog).Type ||
							chunk->Items[x][y + 1][z - 1].Type == Item(ItemType::OakLog).Type ||
							chunk->Items[x][y + 1][z + 1].Type == Item(ItemType::OakLog).Type ||
							chunk->Items[x - 1][y + 1][z - 1].Type == Item(ItemType::OakLog).Type ||
							chunk->Items[x - 1][y + 1][z + 1].Type == Item(ItemType::OakLog).Type ||
							chunk->Items[x + 1][y + 1][z - 1].Type == Item(ItemType::OakLog).Type ||
							chunk->Items[x + 1][y + 1][z + 1].Type == Item(ItemType::OakLog).Type)
							break;

						// tree
						chunk->Items[x][y + 1][z] = Item(ItemType::OakLog);
						chunk->Items[x][y + 2][z] = Item(ItemType::OakLog);
						chunk->Items[x][y + 3][z] = Item(ItemType::OakLog);
						chunk->Items[x][y + 4][z] = Item(ItemType::OakLog);

						chunk->Items[x - 1][y + 4][z] = Item(ItemType::Leaves);
						chunk->Items[x + 1][y + 4][z] = Item(ItemType::Leaves);
						chunk->Items[x][y + 4][z - 1] = Item(ItemType::Leaves);
						chunk->Items[x][y + 4][z + 1] = Item(ItemType::Leaves);

						chunk->Items[x][y + 5][z]     =Item(ItemType::Leaves);
						chunk->Items[x - 1][y + 5][z] =Item(ItemType::Leaves);
						chunk->Items[x + 1][y + 5][z] =Item(ItemType::Leaves);
						chunk->Items[x][y + 5][z - 1] =Item(ItemType::Leaves);
						chunk->Items[x][y + 5][z + 1] =Item(ItemType::Leaves);

						chunk->Items[x - 1][y + 5][z - 1] = Item(ItemType::Leaves);
						chunk->Items[x - 1][y + 5][z + 1] = Item(ItemType::Leaves);
						chunk->Items[x + 1][y + 5][z - 1] = Item(ItemType::Leaves);
						chunk->Items[x + 1][y + 5][z + 1] = Item(ItemType::Leaves);

						chunk->Items[x - 2][y + 5][z] = Item(ItemType::Leaves);
						chunk->Items[x + 2][y + 5][z] = Item(ItemType::Leaves);
						chunk->Items[x][y + 5][z - 2] = Item(ItemType::Leaves);
						chunk->Items[x][y + 5][z + 2] = Item(ItemType::Leaves);

						chunk->Items[x][y + 6][z]     = Item(ItemType::Leaves);
						chunk->Items[x - 1][y + 6][z] = Item(ItemType::Leaves);
						chunk->Items[x + 1][y + 6][z] = Item(ItemType::Leaves);
						chunk->Items[x][y + 6][z - 1] = Item(ItemType::Leaves);
						chunk->Items[x][y + 6][z + 1] = Item(ItemType::Leaves);

						chunk->Items[x][y + 7][z] = Item(ItemType::Leaves);

						break;
					}

				}
			}
		}

	}

}
