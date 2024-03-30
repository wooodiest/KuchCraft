#pragma once

#include "Renderer/Data/Texture2D.h"
#include "World/Block.h"

namespace KuchCraft {

	struct AssetManagerBlockData
	{
		std::string Path      = "assets/textures/";
		std::string Extension = ".png";
		std::map<BlockType, Ref<Texture2D>> Textures;
	};

}