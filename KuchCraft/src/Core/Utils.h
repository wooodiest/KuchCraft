#pragma once

namespace Utils {

	std::string VecToString(const glm::vec2& vec, uint8_t precision = 6);

	std::string VecToString(const glm::vec3& vec, uint8_t precision = 6);

	std::string VecToString(const glm::vec4& vec, uint8_t precision = 6);

	std::string VecToString(const glm::ivec2& vec);

	std::string VecToString(const glm::ivec3& vec);

	std::string VecToString(const glm::ivec4& vec);

	std::string FloatToString(float value, uint8_t precision = 6);

	std::string BoolToString(bool value);

    template<typename T>
    std::map<T, std::string> CreateEnumStringMap()
    {
        std::map<T, std::string> map;
        for (auto value : magic_enum::enum_values<T>())
        {
            const std::string input = std::string(magic_enum::enum_name(value));
            std::string output;
            for (size_t i = 0; i < input.length(); ++i)
            {
                char currentChar = input[i];
                if (std::isupper(currentChar))
                {
                    if (i > 0 && !std::isspace(input[i - 1]) && !std::isspace(input[i]))
                        output += ' ';
                    output += std::tolower(currentChar);
                }
                else 
                    output += currentChar;
                
            }
            map[value] = output;
        }
        return map;
    }
}