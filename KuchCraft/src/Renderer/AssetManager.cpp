#include "kcpch.h"
#include "Renderer/AssetManager.h"

namespace KuchCraft {

	AssetManagerBlockData AssetManager::s_BlockData;
	AssetManagerUIData    AssetManager::s_UIData;

	void AssetManager::Init()
	{
		PrepareBlocks();
		PrepareUI();
	}

	void AssetManager::ShutDown()
	{

	}

	void AssetManager::PrepareBlocks()
	{
		for (uint32_t i = first_index_of_block_types; i < absolute_number_of_block_types; i++)
		{
			Block block = Block((BlockType)i);

			std::string blockName = block.GetName();
			std::replace(blockName.begin(), blockName.end(), ' ', '_');

			const std::string path = s_BlockData.Path + blockName + s_BlockData.Extension;
			s_BlockData.Textures[block.blockType].Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}
	}

	void AssetManager::PrepareUI()
	{
		{
			const std::string path = s_UIData.Path + "hotbar" + s_UIData.Extension;
			s_UIData.HotbarTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

		{
			const std::string path = s_UIData.Path + "hotbar_selection" + s_UIData.Extension;
			s_UIData.HotbarSelectionTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

		{
			const std::string path = s_UIData.Path + "crosshair" + s_UIData.Extension;
			s_UIData.CrosshairTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

		{
			const std::string path = s_UIData.Path + "heart_empty" + s_UIData.Extension;
			s_UIData.HeartEmptyTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

		{
			const std::string path = s_UIData.Path + "heart_full" + s_UIData.Extension;
			s_UIData.HeartFullTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

		{
			const std::string path = s_UIData.Path + "food_empty" + s_UIData.Extension;
			s_UIData.FoodEmptyTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

		{
			const std::string path = s_UIData.Path + "food_full" + s_UIData.Extension;
			s_UIData.FoodFullTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

		{
			const std::string path = s_UIData.Path + "armor_empty" + s_UIData.Extension;
			s_UIData.ArmorEmptyTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

		{
			const std::string path = s_UIData.Path + "armor_full" + s_UIData.Extension;
			s_UIData.ArmorFullTexture.Create(path, TextureSpecification{ 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true });
		}

	}

}