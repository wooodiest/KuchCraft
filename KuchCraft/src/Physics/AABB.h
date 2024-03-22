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

		bool IsColliding(const AABB& other) const;

		AABB MoveTo(const glm::vec3& position);

		glm::vec3 GetOverlaping(const AABB& other) const;

	};

}