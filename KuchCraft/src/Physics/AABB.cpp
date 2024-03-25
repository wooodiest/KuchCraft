#include "kcpch.h"
#include "Physics/AABB.h"

namespace KuchCraft {

	bool AABB::IsColliding(const AABB& other) const
	{
		return ( Min.x < other.Max.x && Max.x > other.Min.x &&
				 Min.y < other.Max.y && Max.y > other.Min.y &&
				 Min.z < other.Max.z && Max.z > other.Min.z    );
	}

	AABB AABB::MoveTo(const glm::vec3& position)
	{
		return AABB {
			Min + position,
			Max + position
		};
	}

	glm::vec3 AABB::GetOverlaping(const AABB& other) const
	{
		return glm::vec3 {
			glm::min(Max.x, other.Max.x) - glm::max(Min.x, other.Min.x),
			glm::min(Max.y, other.Max.y) - glm::max(Min.y, other.Min.y),
			glm::min(Max.z, other.Max.z) - glm::max(Min.z, other.Min.z)
		};
	}

}