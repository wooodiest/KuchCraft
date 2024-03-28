#include "kcpch.h"
#include "World/Block.h"

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

	const std::string& Block::GetName(const Block& block)
	{
		return s_BlockNames[block];
	}

	std::unordered_map<BlockType, std::string> Block::s_BlockNames = {
		{ BlockType::Air,           "air"            },
		{ BlockType::Water,         "water"          },
		{ BlockType::Bedrock,       "bedrock"        },
		{ BlockType::Bricks,        "bricks"         },
		{ BlockType::CoalOre,       "coal ore"       },
		{ BlockType::Cobblestone,   "cobblestone"    },
		{ BlockType::CraftingTable, "crafting table" },
		{ BlockType::DiamondOre,    "diamond ore"    },
		{ BlockType::Dioryte,       "dioryte"        },
		{ BlockType::Dirt,          "dirt"           },
		{ BlockType::Furnace,       "furnace"        },
		{ BlockType::Granite,       "granite"        },
		{ BlockType::Grass,         "grass"          },
		{ BlockType::Gravel,        "gravel"         },
		{ BlockType::IronOre,       "iron ore"       },
		{ BlockType::OakLog,        "oak log"        },
		{ BlockType::OakPlanks,     "oak planks"     },
		{ BlockType::Sand,          "sand"           },
		{ BlockType::Stone,         "stone"          },
		{ BlockType::StoneBrick,    "stone brick"    },
		{ BlockType::Leaves,        "leaves"         },
		{ BlockType::None,          "none"           }
	};
}