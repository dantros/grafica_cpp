/**
 * @file load_shaders.h
 * @brief Convenience functions to load shaders from strings or files
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#pragma once

#include <string>
#include <initializer_list>
#include <glad/glad.h>

namespace Grafica
{

struct ShaderCode
{
    GLenum const type;
    std::string const sourceCode;
};

struct ShaderFile
{
    GLenum const type;
    std::string const filename;
};

ShaderCode readShaderFile(GLenum type, const std::string& filename);

GLuint compileShader(GLenum type, const std::string& sourceCode);

GLuint createShaderProgramFromCode(std::initializer_list<ShaderCode> shaderCodes);

GLuint createShaderProgramFromFiles(std::initializer_list<ShaderFile> shaderFiles);

} // Grafica