#include "kcpch.h"
#include "Physics/Ray.h"

namespace KuchCraft {

	Ray::Ray(const glm::vec3& origin, const glm::vec3& direction, float distance)
		: Origin(origin), Direction(direction), Distance(distance)
	{
	}

    TargetedBlockInfo Ray::IsColliding(const AABB& aabb) const
	{
        float tMin = 0.0f;
        float tMax = Distance;

        int intersectedAxis = -1;

        for (uint32_t i = 0; i < 3; i++)
        {
            float invDir = 1.0f / Direction[i];
            float tNear  = (aabb.Min[i] - Origin[i]) * invDir;
            float tFar   = (aabb.Max[i] - Origin[i]) * invDir;

            if (tNear > tFar)
                std::swap(tNear, tFar);

            if (tNear > tMin)
            {
                tMin = tNear;
                intersectedAxis = i;
            }

            if (tFar < tMax) 
                tMax = tFar;

            if (tMin > tMax)
                return TargetedBlockInfo();
        }

        if (intersectedAxis != -1) 
        {
            PlaneDirection plane = static_cast<PlaneDirection>(intersectedAxis * 2);
            if (Direction[intersectedAxis] < 0) 
                plane = static_cast<PlaneDirection>(aabb.Min, (int)plane + 1);
            
            return TargetedBlockInfo(aabb.Min, plane);
        }

        return TargetedBlockInfo();
	}

}


