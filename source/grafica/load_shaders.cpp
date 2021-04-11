/**
 * @file load_shaders.cpp
 * @brief Convenience functions to load shaders from strings or files
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include <cassert>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "load_shaders.h"

namespace Grafica
{

ShaderCode readShaderFile(GLenum type, const std::string& filename)
{
    std::ifstream fs(filename);

    // to raise error when the file is not found.

	assert(fs.is_open());

	std::stringstream ss;
	ss << fs.rdbuf();
	fs.close();

	return {type, ss.str()};
}

GLuint compileShader(GLenum type, std::string const& sourceCode)
{
	auto shaderCodeChars = sourceCode.c_str();

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCodeChars, NULL);
	glCompileShader(shader);

	// check for shader compile errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::SHADER_SOURCE_CODE: " << std::endl;
        std::cout << sourceCode << std::endl;
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;

		throw;
	}

	return shader;
}

namespace
{
GLuint createShaderProgramCore(std::vector<ShaderCode> const& shaderCodes)
{
	GLuint shaderProgram = glCreateProgram();

	std::vector<GLuint> shaders;

	for (auto shaderCode : shaderCodes)
	{
		GLuint shader = compileShader(shaderCode.type, shaderCode.sourceCode);
		glAttachShader(shaderProgram, shader);
		shaders.push_back(shader);
	}

	glLinkProgram(shaderProgram);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		throw;
	}

	for (auto shader : shaders)
	{
		glDeleteShader(shader);
	}

    return shaderProgram;
}
} // namespace

GLuint createShaderProgramFromCode(std::initializer_list<ShaderCode> shaderCodes)
{
    std::vector<ShaderCode> shaderCodesVec;

    for (auto const shaderCode : shaderCodes)
	{
        shaderCodesVec.push_back(shaderCode);
	}

	return createShaderProgramCore(shaderCodesVec);
}

GLuint createShaderProgramFromFiles(std::initializer_list<ShaderFile> shaderFiles)
{
    std::cout << "Loading shaders ..." << std::endl;
    std::vector<ShaderCode> shaderCodesVec;

    for (auto const shaderFile : shaderFiles)
    {
        std::cout << "Loading shader: \"" << shaderFile.filename << "\" ..." << std::endl;

        ShaderCode shaderCode = readShaderFile(shaderFile.type, shaderFile.filename);
        shaderCodesVec.push_back(shaderCode);
    }
    std::cout << "Done" << std::endl;
	
    return createShaderProgramCore(shaderCodesVec);
}

} // Grafica