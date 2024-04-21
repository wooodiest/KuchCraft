#pragma once

#include "Renderer/Data/Texture2D.h"
#include "Renderer/UIElement.h"
#include "World/Item.h"

namespace KuchCraft {

	struct AssetManagerBlockData
	{
		std::string Path      = "assets/textures/items/";
		std::string Extension = ".png";
		std::map<ItemType, Texture2D> Textures;
	};

	struct AssetManagerUIData
	{
		std::string Path = "assets/textures/ui/";
		std::string Extension = ".png";
		std::map<UIElement, Texture2D> Textures;
	};

}