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

	void WorldGenerator::FillWithRandomBlocks(Chunk* chunk)
	{
		int count = (int)BlockType::None - 1;
		int block = Random::Int(1, count);

		for (int x = 0; x < chunk_size_XZ; x++)
		{
			for (int y = 0; y < chunk_size_Y; y++)
			{
				for (int z = 0; z < chunk_size_XZ; z++)
				{
					if (y == 0)
						chunk->Block[x][y][z].Type = BlockType::Bedrock;
					else
					{
						if (y < 60)
						{
							if (Random::Int(0, 1000) == 0)
								chunk->Block[x][y][z].Type = BlockType::Air;
							else
								chunk->Block[x][y][z].Type = static_cast<BlockType>(block);
						}
						else
							chunk->Block[x][y][z].Type = BlockType::Air;
					}

				}
			}
		}
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
									chunk->Block[x][y][z] = Block(BlockType::Grass);
								else
									chunk->Block[x][y][z] = Block(BlockType::Sand);
							}
							else
							{
								Block block = s_SeeLevel - y >= 6 ? Block(BlockType::Gravel) : Block(BlockType::Sand);
								if (y == height - 1)
									chunk->Block[x][y][z] = block;
								else if (y == height - 2)
									chunk->Block[x][y][z] = block;
								else if (y == height - 3)
									chunk->Block[x][y][z] = block;
								else if (y == height - 4)
									chunk->Block[x][y][z] = block;
								else
									chunk->Block[x][y][z] = Block(BlockType::Stone);
							}
						}
						else
						{
							if (y == height - 1)
								chunk->Block[x][y][z] = Block(BlockType::Grass);
							else if (y == height - 2)
								chunk->Block[x][y][z] = Block(BlockType::Dirt);
							else if (y == height - 3)
								chunk->Block[x][y][z] = Block(BlockType::Dirt);
							else if (y == height - 4)
								chunk->Block[x][y][z] = Block(BlockType::Dirt);
							else
								chunk->Block[x][y][z] = Block(BlockType::Stone);
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
				chunk->Block[x][0][z] = Block(BlockType::Bedrock);
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
						if (chunk->Block[x][y + 1][z].Type == BlockType::Air)
						{
							chunk->Block[x][y + 1][z] = Block(BlockType::Water);
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
					if (treeProb && chunk->Block[x][y][z].Type == BlockType::Grass
						&& chunk->Block[x][y + 1][z].Type == BlockType::Air)
					{
						if (chunk->Block[x - 1][y + 1][z] == Block(BlockType::OakLog) ||
							chunk->Block[x + 1][y + 1][z] == Block(BlockType::OakLog) ||
							chunk->Block[x][y + 1][z - 1] == Block(BlockType::OakLog) ||
							chunk->Block[x][y + 1][z + 1] == Block(BlockType::OakLog) ||
							chunk->Block[x - 1][y + 1][z - 1] == Block(BlockType::OakLog) ||
							chunk->Block[x - 1][y + 1][z + 1] == Block(BlockType::OakLog) ||
							chunk->Block[x + 1][y + 1][z - 1] == Block(BlockType::OakLog) ||
							chunk->Block[x + 1][y + 1][z + 1] == Block(BlockType::OakLog))
							break;

						// tree
						chunk->Block[x][y + 1][z] = Block(BlockType::OakLog);
						chunk->Block[x][y + 2][z] = Block(BlockType::OakLog);
						chunk->Block[x][y + 3][z] = Block(BlockType::OakLog);
						chunk->Block[x][y + 4][z] = Block(BlockType::OakLog);

						chunk->Block[x - 1][y + 4][z] = Block(BlockType::Leaves);
						chunk->Block[x + 1][y + 4][z] = Block(BlockType::Leaves);
						chunk->Block[x][y + 4][z - 1] = Block(BlockType::Leaves);
						chunk->Block[x][y + 4][z + 1] = Block(BlockType::Leaves);

						chunk->Block[x][y + 5][z]     = Block(BlockType::Leaves);
						chunk->Block[x - 1][y + 5][z] = Block(BlockType::Leaves);
						chunk->Block[x + 1][y + 5][z] = Block(BlockType::Leaves);
						chunk->Block[x][y + 5][z - 1] = Block(BlockType::Leaves);
						chunk->Block[x][y + 5][z + 1] = Block(BlockType::Leaves);

						chunk->Block[x - 1][y + 5][z - 1] = Block(BlockType::Leaves);
						chunk->Block[x - 1][y + 5][z + 1] = Block(BlockType::Leaves);
						chunk->Block[x + 1][y + 5][z - 1] = Block(BlockType::Leaves);
						chunk->Block[x + 1][y + 5][z + 1] = Block(BlockType::Leaves);

						chunk->Block[x - 2][y + 5][z] = Block(BlockType::Leaves);
						chunk->Block[x + 2][y + 5][z] = Block(BlockType::Leaves);
						chunk->Block[x][y + 5][z - 2] = Block(BlockType::Leaves);
						chunk->Block[x][y + 5][z + 2] = Block(BlockType::Leaves);

						chunk->Block[x][y + 6][z]     = Block(BlockType::Leaves);
						chunk->Block[x - 1][y + 6][z] = Block(BlockType::Leaves);
						chunk->Block[x + 1][y + 6][z] = Block(BlockType::Leaves);
						chunk->Block[x][y + 6][z - 1] = Block(BlockType::Leaves);
						chunk->Block[x][y + 6][z + 1] = Block(BlockType::Leaves);

						chunk->Block[x][y + 7][z] = Block(BlockType::Leaves);

						break;
					}

				}
			}
		}

	}

}
