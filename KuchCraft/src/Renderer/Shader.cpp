#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>

namespace KuchCraft {

	Shader::Shader()
	{
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::SetMat4(const std::string& name, const glm::mat4& v)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
	}

	void Shader::SetInt(const std::string& name, int v)
	{
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, v);
	}

	void Shader::Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		std::string vertex   = ReadFile(vertexShaderPath);
		std::string fragment = ReadFile(fragmentShaderPath);
		const char* vertex_cstr   = vertex.c_str();
		const char* fragment_cstr = fragment.c_str();

		int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertex_cstr, NULL);
		glCompileShader(vertexShader);
		{
			int success;
			char infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION FAILED\n" << infoLog << std::endl;
			}
		}

		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragment_cstr, NULL);
		glCompileShader(fragmentShader);
		{
			int success;
			char infoLog[512];
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION FAILED\n" << infoLog << std::endl;
			}
		}

		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glLinkProgram(m_RendererID);

		{ 
			int success;
			char infoLog[512];
			glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(m_RendererID, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::LINKING FAILED\n" << infoLog << std::endl;
			}
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	std::string Shader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				in.close();
			}
			else
				std::cout << "Could not read from file: " << filepath << std::endl;
		}
		else
			std::cout << "Could not open file: " << filepath << std::endl;

		return result;
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

}