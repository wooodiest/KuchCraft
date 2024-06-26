#pragma once

#include "AABB.h"

namespace KuchCraft
{
	enum class PlaneDirection
	{
		Left   = 0, Right = 1, 
		Bottom = 2, Top   = 3,
		Behind = 4, Front = 5
	};

	struct TargetedItemInfo
	{
		bool Targeted = false;
		PlaneDirection Plane;
		glm::vec3 Position { 0.0f };
		glm::vec3 Size     { 0.5f };

		TargetedItemInfo() = default;

		TargetedItemInfo(const glm::vec3& position, PlaneDirection plane)
			: Targeted(true), Plane(plane), Position(position){}
	};

	struct Ray
	{
		glm::vec3 Origin   { 0.0f };
		glm::vec3 Direction{ 0.0f };
		float Distance = 0.0f;

		Ray(const glm::vec3& origin, const glm::vec3& direction, float distance = std::numeric_limits<float>::infinity());

		TargetedItemInfo IsColliding(const AABB& aabb) const;
	};

}