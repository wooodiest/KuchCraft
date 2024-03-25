#include "kcpch.h"
#include "Renderer/FrustumCulling.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace KuchCraft {

	constexpr uint32_t frustum_planes_count = 6;

	struct AABB 
	{
		glm::vec3 min;
		glm::vec3 max;

		AABB(const glm::vec3& position)
		{
			min = position;
			max = position + glm::vec3(chunk_size_XZ, chunk_size_Y, chunk_size_XZ);
		}
	};

	static bool IsAABBVisible(const AABB& aabb, const glm::vec4 frustumPlanes[6])
	{
		for (int i = 0; i < frustum_planes_count; i++)
		{
			glm::vec3 normal = glm::vec3(frustumPlanes[i]);
			float d = frustumPlanes[i].w;

			float dist = glm::dot(normal, glm::vec3(
				(normal.x < 0.0f) ? aabb.min.x : aabb.max.x,
				(normal.y < 0.0f) ? aabb.min.y : aabb.max.y,
				(normal.z < 0.0f) ? aabb.min.z : aabb.max.z ))
				+ d;

			if (dist < 0.0f)
				return false;
		}
		return true;
	}
	
	void FrustumCulling::GetChunksToRender(std::vector<Chunk*>& out_ChunksToRender, std::vector<Chunk*>& activeChunks, const Camera& camera)
	{
		KC_PROFILE_FUNCTION();

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

		for (const auto& chunk : activeChunks)
		{
			AABB chunkAABB{ chunk->GetPosition() };
			if (IsAABBVisible(chunkAABB, frustumPlanes) && !chunk->NeedToRecreate())
				out_ChunksToRender.push_back(chunk);
		}
	}

}