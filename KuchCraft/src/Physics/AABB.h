#pragma once

namespace KuchCraft {

	struct AABB
	{
		glm::vec3 Min{ 0.0f };
		glm::vec3 Max{ 0.0f };

		AABB() = default;
		AABB(const glm::vec3& min, const glm::vec3& max)
			: Min(min), Max(max) {}

		glm::vec3 GetPosition() const { return (Max + Min) * 0.5f; }
		glm::vec3 GetSize()     const { return (Max - Min) * 0.5f; }

		bool IsColliding(const AABB& other) const;

		AABB MoveTo(const glm::vec3& position) const;

		glm::vec3 GetOverlaping(const AABB& other) const;

	};

}