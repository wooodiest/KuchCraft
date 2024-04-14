#include "kcpch.h"
#include "Core/Utils.h"

namespace KuchCraft {

	std::string VecToString(const glm::vec2& vec, uint8_t precision)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(precision);
		stream << vec.x << ", " << vec.y << ", ";
		return stream.str();
	}

	std::string VecToString(const glm::vec3& vec, uint8_t precision)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(precision);
		stream << vec.x << ", " << vec.y << ", " << vec.z;
		return stream.str();
	}

	std::string VecToString(const glm::vec4& vec, uint8_t precision)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(precision);
		stream << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
		return stream.str();
	}

	std::string VecToString(const glm::ivec2& vec)
	{
		return std::to_string(vec.x) + ", " + std::to_string(vec.y);
	}

	std::string VecToString(const glm::ivec3& vec)
	{
		return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
	}

	std::string VecToString(const glm::ivec4& vec)
	{
		return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w);
	}

	std::string BoolToString(bool value)
	{
		return value ? "true" : "false";
	}

}