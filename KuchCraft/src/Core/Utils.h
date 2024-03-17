#pragma once

#include <string>
#include <glm/glm.hpp>

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


	std::string operator+(const std::string& str, unsigned int v)
	{
		return str + std::to_string(v);
	}

	std::string operator+(const std::string& str, int v)
	{
		return str + std::to_string(v);
	}

	std::string operator+(const std::string& str, float v)
	{
		return str + std::to_string(v);
	}

	std::string operator+(const std::string& str, const glm::vec2& vec)
	{
		return str + std::to_string(vec.x) + ", " + std::to_string(vec.y);
	}

	std::string operator+(const std::string& str, const glm::vec3& vec) 
	{
		return str + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
	}

	std::string operator+(const std::string& str, const glm::vec4& vec)
	{
		return str + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w);
	}

}