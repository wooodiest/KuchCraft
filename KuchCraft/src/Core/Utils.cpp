#include "kcpch.h"
#include "Core/Utils.h"

namespace KuchCraft {

	std::string VecToString(const glm::vec2& vec)
	{
		return std::to_string(vec.x) + ", " + std::to_string(vec.y);
	}

	std::string VecToString(const glm::vec3& vec)
	{
		return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
	}

	std::string VecToString(const glm::vec4& vec)
	{
		return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w);
	}

	std::string BoolToString(bool value)
	{
		return value ? "true" : "false";
	}

}