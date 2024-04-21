#pragma once

#include "Renderer/AssetManagerData.h"

namespace KuchCraft {

	class AssetManager
	{
	public:
		static Texture2D& GetItemTexture(ItemType type)       { return s_BlockData.Textures[type]; }
		static Texture2D& GetUIElementTexture(UIElement type) { return s_UIData.Textures[type];    }

	private:
		static void Init();
		static void ShutDown();

		static void PrepareBlocks();
		static void PrepareUI();

	public:
		friend class Renderer;

	private:
		static AssetManagerBlockData s_BlockData;
		static AssetManagerUIData    s_UIData;

	private:
		AssetManager() = default;

	};

}