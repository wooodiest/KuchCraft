#include "Block.h"

namespace KuchCraft {

	bool Block::IsTransparent(const Block& block)
	{
		switch (block.blockType)
		{
			case BlockType::Air:    return true;
			case BlockType::Water:  return true;
			case BlockType::None:   return true;
		}
		return false;
	}

	bool Block::IsSolid(const Block& block)
	{
		switch (block.blockType)
		{
			case BlockType::Air:    return false;
			case BlockType::Water:  return false;
			case BlockType::None:   return false;
		}
		return true;
	}

	std::unordered_map<BlockType, std::string> Block::s_BlockNames = {
		{ BlockType::Air,           "Air"           },
		{ BlockType::Water,         "Water"         },
		{ BlockType::Bedrock,       "Bedrock"       },
		{ BlockType::Bricks,        "Bricks"        },
		{ BlockType::CoalOre,       "CoalOre"       },
		{ BlockType::Cobblestone,   "Cobblestone"   },
		{ BlockType::CraftingTable, "CraftingTable" },
		{ BlockType::DiamondOre,    "DiamondOre"    },
		{ BlockType::Dioryte,       "Dioryte"       },
		{ BlockType::Dirt,          "Dirt"          },
		{ BlockType::Furnace,       "Furnace"       },
		{ BlockType::Granite,       "Granite"       },
		{ BlockType::Grass,         "Grass"         },
		{ BlockType::Gravel,        "Gravel"        },
		{ BlockType::IronOre,       "IronOre"       },
		{ BlockType::OakLog,        "OakLog"        },
		{ BlockType::OakPlanks,     "OakPlanks"     },
		{ BlockType::Sand,          "Sand"          },
		{ BlockType::Stone,         "Stone"         },
		{ BlockType::StoneBrick,    "StoneBrick"    },
		{ BlockType::Leaves,        "Leaves"        },
		{ BlockType::None,          "None"          }
	};
}