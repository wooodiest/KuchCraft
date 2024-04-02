#pragma once

#include "Renderer/AssetManagerData.h"

namespace KuchCraft {

	class AssetManager
	{
	public:
		static Texture2D& GetBlockTexture(BlockType type) { return s_BlockData.Textures[type]; }

	private:
		static void Init();
		static void ShutDown();

		static void PrepareBlocks();

	public:
		friend class Renderer;

	private:
		static AssetManagerBlockData s_BlockData;

	private:
		AssetManager() = default;

	};

}