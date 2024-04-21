#pragma once

#include <magic_enum/magic_enum.hpp>

namespace KuchCraft {

	enum class ItemType : uint8_t
	{
		Air = 0,

		/* SOLID BLOCKS */
		AcaciaLog,
		AcaciaPlanks,
		Amethyst,
		AncientDebris,
		Andesite,
		Bedrock,
		Bricks,
		Bambo,
		BamboPlanks,
		BamboMosaic,
		Barrel,
		Basalt,
		BeeNest,
		Beehive,
		BirchLog,
		BirchPlanks,
		BlackConcrete,
		BlackConcretePowder, 
		BlackWool,
		BlackStone,
		BlastFurnace,
		BlueConcrete,
		BlueConcretePowder,
		BlueIce,
		BlueWool,
		BoneBlock,
		BookShelf,
		BrainCoralBlock,
		BrownConcrete,
		BrownConcretePowder,
		CoalOre, 
		Cobblestone,
		CraftingTable,
		CarvedPumpkin,
		CherryLog, 
		CherryPlanks,
		ChiseledCopper,
		ChiseledDeepslate,
		ChiseledNetherBricks, 
		ChiseledPolishedBlackstone,
		ChiseledQuartz,
		ChiseledRedSandstone,
		ChiseledSandstone,
		ChiseledStoneBricks,
		ChiseledTuff,
		ChiseledTuffBricks,
		ClayBlock, 
		CoalBlock,
		CorseDirt, 
		CobbledDeepslate, 
		CopperBlock,
		CopperOre,
		CrackedDeepslateBricks,
		CrackedDeepslateTiles, 
		CrackedNetherBricks,
		CrackedPolishedBlackstoneBricks,
		CrackedStoneBricks,
		CrimsonLog, 
		CrimsonPlanks,
		CrimsonLogStriped,
		CryingObsidian,
		CutCopper,
		CyanConcrete,
		CyanConcretePowder,
		CyanWool,
		DarkOakLog, 
		DarkOakPlanks,
		DarkPrismarine,
		Deepslate,
		DeepslateBricks,
		DeepslateCoalOre,
		DeepslateIronOre,
		DeepslateCopperOre,
		DeepslateDiamondOre,
		DeepslateEmeraldOre,
		DeepslateGoldOre,
		DeepslateLapisOre,
		DeepslateTiles,
		DiamondBlock,
		DirtPath, 
		Dripstone,
		DiamondOre,
		Dioryte, 
		Dirt,
		EmeraldOre, 
		EmeraldBlock,
		EndStone,
		EndStoneBrick,
		Furnace,
		Farmland, 
		FireCoralBlock,
		Granite,
		GrassBlock,
		Gravel, 
		GildedBlackstone,
		Glowstone,
		GoldBlock, 
		GoldOre, 
		GrassSnowBlock, 
		GreyConcrete,
		GreyConcretePowder,
		GreyWool, 
		GreenConcrete,
		GreenConcretePowder, GreenWool,
		HayBlock,
		IronOre,
		Ice,
		IronBlock,
		JungleLog,
		JunglePlanks,
		LapisBlock,
		LapisOre,
		LightBlueConcrete,
		LightBlueConcretePowder,
		LightBlueWool,
		LightGreyConcrete, 
		LightGreyConcretePowder,
		LightGreyWool,
		LimeConcrete,
		LimeConcretePowder, 
		LimeWool,
		Lamp, 
		Leaves,
		MagentaConcrete, 
		MagentaConcretePowder, 
		MagentaWool,
		MelonBlock, 
		Mud,
		MudBricks,
		NetherBricks,
		Netherrack,
		OakLog, 
		OakPlanks,
		OrangeConcrete, 
		OrangeConcretePowder,
		OrangeWool,
		PumpkinBlock,
		PinkConcrete,
		PinkConcretePowder,
		PinkWool, 
		PolishedAndesite,
		PolishedBasalt, 
		PrismarineBricks,
		PurpleConcrete, 
		PurpleConcretePowder,
		PurpleWool, 
		PurpurPillar,
		QuartzBlock,
		QuartzBricks,
		QuartzPillar,
		RedConcrete,
		RedConcretePowder,
		RedWool,
		Sand, 
		Sandstone,
		Stone, 
		StoneBricks, 
		SnowBlock, 
		SoulSand, 
		SpruceLog, 
		SprucePlanks,
		Tnt,
		TubeCoralBlock,
		WhiteConcrete,
		WhiteConcretePowder,
		WhiteWool, 
		YellowConcrete,
		YellowConcretePowder,
		YellowWool,
		NoneBlock,

		/* TRANSPARENT BLOCKS */
		Glass,
		Water,

		/* FOLIAGE QUADS */
		/* - saplings */
		AcaciaSapling,
		BirchSapling,
		CherrySapling,
		DeadBush,
		DarkOakSapling,
		JungleSapling,
		OakSapling,
		SpruceSapling,

		/* - crops */
		BeetrootsCrops,
		CarrotsCrops,
		PotatoesCrops,
		WheatCrops,

		/* - flowers */
		AzureBluet,
		Allium,
		BlueOrchid,
		Cornflower,
		Dandelion,
		LilyOfTheValley,
		OrangeTulip,
		OxeyeDaisy,
		PinkTulip,
		Poppy,
		RedTulip,
		Torchflower,
		WhiteTulip,
		WitherRose,

		/* - mushrooms */
		BrownMushroom,
		CrimsonFungus,
		RedMushroom,

		/* - other */
		BrainCoral,
		Cobweb,
		CrimsonRoots,
		FireCoral,
		SugarCane,
		SweetBerryBush,
		TubeCoral,
		Grass,

		/* FOLIAGE cube */


		/* FOOD */


		/* TOOLS */


		None
	};

	enum class ItemRotation : uint8_t
	{
		Deg0   = 0,
		Deg90  = 1,
		Deg180 = 2,
		Deg270 = 3
	};

	inline constexpr uint32_t item_types_count = (uint32_t)ItemType::None;

	inline constexpr uint32_t item_type_solid_block_begin = (uint32_t)ItemType::AcaciaLog;
	inline constexpr uint32_t item_type_solid_block_end   = (uint32_t)ItemType::NoneBlock;

	inline constexpr uint32_t item_type_transparent_block_begin = (uint32_t)ItemType::Glass;
	inline constexpr uint32_t item_type_transparent_block_end   = (uint32_t)ItemType::Water;

	inline constexpr uint32_t item_foliage_quad_begin           = (uint32_t)ItemType::AcaciaSapling;
	inline constexpr uint32_t item_foliage_quad_saplings_begin  = (uint32_t)ItemType::AcaciaSapling;
	inline constexpr uint32_t item_foliage_quad_saplings_end    = (uint32_t)ItemType::SpruceSapling;
	inline constexpr uint32_t item_foliage_quad_crops_begin     = (uint32_t)ItemType::BeetrootsCrops;
	inline constexpr uint32_t item_foliage_quad_crops_end       = (uint32_t)ItemType::WheatCrops;
	inline constexpr uint32_t item_foliage_quad_flowers_begin   = (uint32_t)ItemType::AzureBluet;
	inline constexpr uint32_t item_foliage_quad_flowers_end     = (uint32_t)ItemType::WitherRose;
	inline constexpr uint32_t item_foliage_quad_mushrooms_begin = (uint32_t)ItemType::BrownMushroom;
	inline constexpr uint32_t item_foliage_quad_mushrooms_end   = (uint32_t)ItemType::RedMushroom;
	inline constexpr uint32_t item_foliage_quad_end             = (uint32_t)ItemType::Grass;

}

template <>
struct magic_enum::customize::enum_range<KuchCraft::ItemType> {
	static constexpr int min = 0;
	static constexpr int max = (int)KuchCraft::ItemType::None;
};
