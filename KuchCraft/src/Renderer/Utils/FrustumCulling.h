#pragma once

#include "Renderer/Data/Camera.h"

#include "World/Chunk.h"

namespace KuchCraft {

	class FrustumCulling
	{
	public:
		static void Chunks(const std::vector<Chunk*>& chunks, const Camera& camera, std::vector<Chunk*>& out_chunks);

	};

}