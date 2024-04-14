#pragma once

namespace KuchCraft {

	std::string VecToString(const glm::vec2& vec, uint8_t precision = 6);

	std::string VecToString(const glm::vec3& vec, uint8_t precision = 6);

	std::string VecToString(const glm::vec4& vec, uint8_t precision = 6);

	std::string VecToString(const glm::ivec2& vec);

	std::string VecToString(const glm::ivec3& vec);

	std::string VecToString(const glm::ivec4& vec);

	std::string FloatToString(float value, uint8_t precision = 6);

	std::string BoolToString(bool value);

}