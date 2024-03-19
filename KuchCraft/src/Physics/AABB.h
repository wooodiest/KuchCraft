#pragma once

#include <glm/glm.hpp>

namespace KuchCraft {

	struct AABB
	{
		glm::vec3 Min{ 0.0f };
		glm::vec3 Max{ 0.0f };

		AABB() = default;

		AABB(const glm::vec3& min, const glm::vec3& max)
			: Min(min), Max(max) {}


		bool IsColliding(const AABB& other) const 
		{
			return Min.x < other.Max.x && Max.x > other.Min.x &&
				   Min.y < other.Max.y && Max.y > other.Min.y &&
				   Min.z < other.Max.z && Max.z > other.Min.z;
		}
			
		AABB MoveTo(const glm::vec3& position)
		{
			return AABB {
				Min + position,
				Max + position
			};
		}
	};

}