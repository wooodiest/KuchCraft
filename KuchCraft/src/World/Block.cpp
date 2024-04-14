#include "kcpch.h"
#include "World/Block.h"

namespace KuchCraft {

	bool Block::IsTransparent(const Block& block)
	{
		switch (block.Type)
		{
			case BlockType::Air:    return true;
			case BlockType::Water:  return true;
		}
		return false;
	}

	bool Block::IsSolid(const Block& block)
	{
		switch (block.Type)
		{
			case BlockType::Air:    return false;
			case BlockType::Water:  return false;
		}
		return true;
	}

	const std::string& Block::GetName(const Block& block)
	{
		return s_BlockNames[block];
	}

	std::unordered_map<BlockType, std::string> Block::s_BlockNames = {
		{ BlockType::Air,                             "air"                          },
		{ BlockType::AcaciaLog,                       "acacia log"                   },
		{ BlockType::AcaciaPlanks,                    "acacia planks"                },
		{ BlockType::Amethyst,                        "amethyst"                     },
		{ BlockType::AncientDebris,                   "ancient debris"               },
		{ BlockType::Andesite,                        "andesite"                     },
		{ BlockType::Bedrock,                         "bedrock"                      },
		{ BlockType::Bricks,                          "bricks"                       },
		{ BlockType::Bambo,                           "bambo"                        },
		{ BlockType::BamboPlanks,                     "bambo planks"                 },
		{ BlockType::BamboMosaic,                     "bambo mosaic"                 },
		{ BlockType::Barrel,                          "barrel"                       },
		{ BlockType::Basalt,                          "basalt"                       },
		{ BlockType::BeeNest,                         "bee nest"                     },
		{ BlockType::BeeHive,                         "beehive"                      },
		{ BlockType::BirchLog,                        "birch log"                    },
		{ BlockType::BirchPlanks,                     "birch planks"                 },
		{ BlockType::BlackConcrete,                   "black concrete"               },
		{ BlockType::BlackConcretePowder,             "black concrete powder"        },
		{ BlockType::BlackWool,                       "black wool"                   },
		{ BlockType::BlackStone,                      "black stone"                  },
		{ BlockType::BlastFurnace,                    "blast furnace"                },
		{ BlockType::BlueConcrete,                    "blue concrete"                },
		{ BlockType::BlueConcretePowder,              "blue concrete powder"         },
		{ BlockType::BlueIce,                         "blue ice"                     },
		{ BlockType::BlueWool,                        "blue wool"                    },
		{ BlockType::Bone,                            "bone"                         },
		{ BlockType::BookShelf,                       "book shelf"                   },
		{ BlockType::BrainCoral,                      "brain coral"                  },
		{ BlockType::BrownConcrete,                   "brown concrete"               },
		{ BlockType::BrownConcretePowder,             "brown concrete powder"        },
		{ BlockType::CoalOre,                         "coal ore"                     },
		{ BlockType::CarvedPumpkin,                   "carved pumpkin"               },
		{ BlockType::Cobblestone,                     "cobblestone"                  },
		{ BlockType::CraftingTable,                   "crafting table"               },
		{ BlockType::CherryLog,                       "cherry log"                   },
		{ BlockType::CherryPlanks,                    "cherry planks"                },
		{ BlockType::ChiseledCopper,                  "chiseled copper"              },
		{ BlockType::ChiseledDeepslate,               "chiseled deepslate"           },
		{ BlockType::ChiseledNetherBricks,            "chiseled nether bricks"       },
		{ BlockType::ChiseledPolishedBlackstone,      "chiseled polished blackstone" },
		{ BlockType::ChiseledQuartz,                  "chiseled quartz"              },
		{ BlockType::ChiseledRedSandstone,            "chiseled red sandstone"       },
		{ BlockType::ChiseledSandstone,               "chiseled sandstone"           },
		{ BlockType::ChiseledStoneBricks,             "chiseled stone bricks"        },
		{ BlockType::ChiseledTuff,                    "chiseled tuff"                },
		{ BlockType::ChiseledTuffBricks,              "chiseled tuff bricks"         },
		{ BlockType::Clay,                            "clay"                                },
		{ BlockType::Coal,                            "coal"                                },
		{ BlockType::CorseDirt,                       "corse dirt"                          },
		{ BlockType::CobbledDeepslate,                "cobbled deepslate"                   },
		{ BlockType::Copper,                          "copper"                              },
		{ BlockType::CopperOre,                       "copper ore"                          },
		{ BlockType::CrackedDeepslateBricks,          "cracked deepslate bricks"            },
		{ BlockType::CrackedDeepslateTiles,           "cracked deepslate tiles"             },
		{ BlockType::CrackedNetherBricks,             "cracked nether bricks"               },
		{ BlockType::CrackedPolishedBlackstoneBricks, "cracked polished blackstone bricks"  },
		{ BlockType::CrackedStoneBricks,              "cracked stone bricks"                },
		{ BlockType::DiamondOre,                      "diamond ore"                  },
		{ BlockType::Dioryte,                         "dioryte"                      },
		{ BlockType::Dirt,                            "dirt"                         },
		{ BlockType::Furnace,                         "furnace"                      },
		{ BlockType::Granite,                         "granite"                      },
		{ BlockType::Grass,                           "grass"                        },
		{ BlockType::Gravel,                          "gravel"                       },
		{ BlockType::IronOre,                         "iron ore"                     },
		{ BlockType::OakLog,                          "oak log"                      },
		{ BlockType::OakPlanks,                       "oak planks"                   },
		{ BlockType::Pumpkin,                         "pumpkin"                      },
		{ BlockType::Sand,                            "sand"                         },
		{ BlockType::Stone,                           "stone"                        },
		{ BlockType::StoneBrick,                      "stone brick"                  },
		{ BlockType::Leaves,                          "leaves"                       },
		{ BlockType::Water,                           "water"                        },
		{ BlockType::None,                            "none"                         }
	};
}