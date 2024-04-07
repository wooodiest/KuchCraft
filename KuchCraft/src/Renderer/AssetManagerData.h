#pragma once

#include "Renderer/Data/Texture2D.h"
#include "World/Block.h"

namespace KuchCraft {

	struct AssetManagerBlockData
	{
		std::string Path      = "assets/textures/blocks/";
		std::string Extension = ".png";
		std::map<BlockType, Texture2D> Textures;
	};

	struct AssetManagerUIData
	{
		std::string Path = "assets/textures/ui/";
		std::string Extension = ".png";

		Texture2D HotbarTexture;
		Texture2D HotbarSelectionTexture;
		Texture2D CrosshairTexture;

		Texture2D FoodEmptyTexture;
		Texture2D FoodFullTexture;

		Texture2D HeartEmptyTexture;
		Texture2D HeartFullTexture;

		Texture2D ArmorEmptyTexture;
		Texture2D ArmorFullTexture;
	};

}