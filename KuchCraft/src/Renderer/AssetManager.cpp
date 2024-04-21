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
		TextureSpecification textureSpec = { 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true };

		for (uint32_t i = 1; i < item_types_count; i++)
		{
			Item block = Item((ItemType)i);

			std::string blockName = block.GetName();
			std::replace(blockName.begin(), blockName.end(), ' ', '_');

			const std::string path = s_BlockData.Path + blockName + s_BlockData.Extension;
			s_BlockData.Textures[block.Type].Create(path, textureSpec);
		}
	}

	void AssetManager::PrepareUI()
	{
		TextureSpecification textureSpec = { 0, 0, ImageFormat::RGBA8, TextureFilter::NEAREST, true };

		auto uiElements = Utils::CreateEnumStringMap<UIElement>();
		uiElements.erase(uiElements.find(UIElement::None));

		for (auto& [type, name] : uiElements)
		{
			std::string uiElementName = name;
			std::replace(uiElementName.begin(), uiElementName.end(), ' ', '_');

			const std::string path = s_UIData.Path + uiElementName + s_UIData.Extension;
			s_UIData.Textures[type].Create(path, textureSpec);
		}
	}

}