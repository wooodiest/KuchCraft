#include "kcpch.h"
#include "Renderer/Utils/FrustumCulling.h"

#include "Physics/AABB.h"

namespace KuchCraft {

	constexpr uint32_t frustum_planes_count = 6;

	static bool IsAABBVisible(const AABB& aabb, const glm::vec4 frustumPlanes[6])
	{
		for (int i = 0; i < frustum_planes_count; i++)
		{
			glm::vec3 normal = glm::vec3(frustumPlanes[i]);
			float d = frustumPlanes[i].w;

			float dist = glm::dot(normal, glm::vec3(
				(normal.x < 0.0f) ? aabb.Min.x : aabb.Max.x,
				(normal.y < 0.0f) ? aabb.Min.y : aabb.Max.y,
				(normal.z < 0.0f) ? aabb.Min.z : aabb.Max.z ))
				+ d;

			if (dist < 0.0f)
				return false;
		}
		return true;
	}
	
	void FrustumCulling::Chunks(const std::vector<Chunk*>& chunks, const Camera& camera, std::vector<Chunk*>& out_chunks)
	{
		glm::mat4 viewProjection = camera.GetViewProjection();
		glm::vec4 frustumPlanes[frustum_planes_count];
		frustumPlanes[0] = glm::row(viewProjection, 3) + glm::row(viewProjection, 0);
		frustumPlanes[1] = glm::row(viewProjection, 3) - glm::row(viewProjection, 0);
		frustumPlanes[2] = glm::row(viewProjection, 3) + glm::row(viewProjection, 1);
		frustumPlanes[3] = glm::row(viewProjection, 3) - glm::row(viewProjection, 1);
		frustumPlanes[4] = glm::row(viewProjection, 3) + glm::row(viewProjection, 2);
		frustumPlanes[5] = glm::row(viewProjection, 3) - glm::row(viewProjection, 2);

		for (int i = 0; i < frustum_planes_count; i++)
		{
			float length = glm::length(glm::vec3(frustumPlanes[i]));
			frustumPlanes[i] /= length;
		}

		for (const auto& chunk : chunks)
		{
			AABB chunkAABB{ chunk->GetPosition(), chunk->GetPosition() + glm::vec3{ chunk_size_XZ, chunk_size_Y, chunk_size_XZ } };
			if (IsAABBVisible(chunkAABB, frustumPlanes) && !chunk->NeedToRecreate())
				out_chunks.push_back(chunk);
		}
	}

}