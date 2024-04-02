#include "kcpch.h"
#include "Renderer/AssetManager.h"

namespace KuchCraft {

	AssetManagerBlockData AssetManager::s_BlockData;

	void AssetManager::Init()
	{
		KC_PROFILE_FUNCTION();

		PrepareBlocks();
	}

	void AssetManager::ShutDown()
	{
		KC_PROFILE_FUNCTION();
	}

	void AssetManager::PrepareBlocks()
	{
		KC_PROFILE_FUNCTION();

		for (uint32_t i = first_index_of_block_types; i < absolute_number_of_block_types; i++)
		{
			Block block = Block((BlockType)i);

			std::string blockName = block.GetName();
			std::replace(blockName.begin(), blockName.end(), ' ', '_');

			const std::string path = s_BlockData.Path + blockName + s_BlockData.Extension;
			s_BlockData.Textures[block.blockType].Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}
	}

}