/**
 * @file easy_shaders.h
 * @brief Many simple shader programs for quick prototyping
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#pragma once

#include <filesystem>
#include <glad/glad.h>
#include "load_shaders.h"
#include "gpu_shape.h"

namespace Grafica
{

/** Loads the texture indicated by imgPath with the wrap modes and filters speficied.
 * A texture reference in GPU is returned.
 */
GLuint textureSimpleSetup(
    const std::filesystem::path& imgPath,
    GLuint sWrapMode,
    GLuint tWrapMode,
    GLuint minFilterMode,
    GLuint maxFilterMode);

struct PositionColorVAO
{
    GLuint shaderProgram;

    void setupVAO(GPUShape& gpuShape) const;

    void drawCall(const GPUShape& gpuShape, GLuint mode = GL_TRIANGLES) const;
};

struct SimpleShaderProgram : public PositionColorVAO
{
    SimpleShaderProgram();
};

struct TransformShaderProgram : public PositionColorVAO
{
    TransformShaderProgram();
};

struct ModelViewProjectionShaderProgram : public PositionColorVAO
{
    ModelViewProjectionShaderProgram();
};

struct PositionTextureVAO
{
    GLuint shaderProgram;

    void setupVAO(GPUShape& gpuShape) const;

    void drawCall(const GPUShape& gpuShape, GLuint mode = GL_TRIANGLES) const;
};

struct TextureTransformShaderProgram : public PositionTextureVAO
{
    TextureTransformShaderProgram();
};

struct PositionColorNormalVAO
{
    GLuint shaderProgram;

    void setupVAO(GPUShape& gpuShape) const;

    void drawCall(const GPUShape& gpuShape, GLuint mode = GL_TRIANGLES) const;
};

struct PhongColorShaderProgram : public PositionColorNormalVAO
{
    PhongColorShaderProgram();
};

struct PositionTextureNormalVAO
{
    GLuint shaderProgram;

    void setupVAO(GPUShape& gpuShape) const;

    void drawCall(const GPUShape& gpuShape, GLuint mode = GL_TRIANGLES) const;
};

struct PhongTextureShaderProgram : public PositionTextureNormalVAO
{
    PhongTextureShaderProgram();
};
    
} //Grafica