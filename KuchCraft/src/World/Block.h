#pragma once

namespace KuchCraft {

	enum class BlockType : uint8_t
	{
		Air = 0,
		AcaciaLog, AcaciaPlanks, Amethyst, AncientDebris, Andesite,
		Bedrock, Bricks, Bambo, BamboPlanks, BamboMosaic, Barrel, Basalt, BeeNest, BeeHive, BirchLog,
		BirchPlanks, BlackConcrete, BlackConcretePowder, BlackWool, BlackStone, BlastFurnace,
		BlueConcrete, BlueConcretePowder, BlueIce, BlueWool, Bone, BookShelf, BrainCoral,
		BrownConcrete, BrownConcretePowder,
		CoalOre, Cobblestone, CraftingTable, CarvedPumpkin, CherryLog, CherryPlanks,
		ChiseledCopper, ChiseledDeepslate, ChiseledNetherBricks, ChiseledPolishedBlackstone, 
		ChiseledQuartz, ChiseledRedSandstone, ChiseledSandstone, ChiseledStoneBricks, ChiseledTuff, ChiseledTuffBricks,
		Clay, Coal, CorseDirt, CobbledDeepslate, Copper, CopperOre, CrackedDeepslateBricks,
		CrackedDeepslateTiles, CrackedNetherBricks, CrackedPolishedBlackstoneBricks, CrackedStoneBricks,
		DiamondOre, Dioryte, Dirt,
		Furnace,
		Granite, Grass, Gravel,
		IronOre,
		OakLog, OakPlanks,
		Pumpkin,
		Sand, Stone, StoneBrick,
		Leaves,
		Water,

		None // No element and/or last element
	};
	constexpr uint32_t first_index_of_block_types     = (uint32_t)BlockType::Air  + 1;
	constexpr uint32_t total_number_of_block_types    = (uint32_t)BlockType::None - 1;
	constexpr uint32_t absolute_number_of_block_types = (uint32_t)BlockType::None;

	enum class BlockRotation : uint8_t
	{
		Deg0   = 0,
		Deg90  = 1,
		Deg180 = 2,
		Deg270 = 3
	};

	struct Block
	{
		BlockType     Type     = BlockType::Air;
		BlockRotation Rotation = BlockRotation::Deg0;

		Block() = default;

		Block(BlockType type)
			: Type(type) {}

		Block(BlockType type, BlockRotation rotation)
			: Type(type), Rotation(rotation) {}

		operator BlockType()                     const { return  Type;               }
		bool operator ==(const Block& other)     const { return  Type == other.Type; }
		bool operator !=(const Block& other)     const { return !(*this == other);   }
		bool operator ==(const BlockType& other) const { return  Type == other;      }
		bool operator !=(const BlockType& other) const { return !(*this == other);   }

		bool IsTransparent() const { return IsTransparent(Type); }
		bool IsSolid()       const { return IsSolid(Type);       }

		const std::string& GetName() const { return GetName(*this); };

		static bool IsTransparent(const Block& block);
		static bool IsSolid(const Block& block);
		static const std::string& GetName(const Block& block);

	private:
		static std::unordered_map<BlockType, std::string> s_BlockNames;
	};

}