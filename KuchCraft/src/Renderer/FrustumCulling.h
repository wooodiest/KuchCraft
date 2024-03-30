#pragma once

#include "Renderer/Camera.h"

#include "World/Chunk.h"

namespace KuchCraft {

	class FrustumCulling
	{
	public:
		static void GetChunksToRender(std::vector<Chunk*>& out_ChunksToRender, std::vector<Chunk*>& activeChunks, const Camera& camera);

	};

}