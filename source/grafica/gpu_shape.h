/**
 * @file gpu_shape.h
 * @brief Convenience struct GPUShape to reference a shape stored in GPU memory.
 *        It keeps track of an OpenGL VAO, a VBO, an EBO and a texture.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#pragma once

#include <string>
#include <cassert>
#include <iostream>
#include <glad/glad.h>

#include "shape.h"

namespace Grafica
{

/* We will use 32 bits data, so floats and integers have 4 bytes.
 * 1 byte = 8 bits
 */
constexpr unsigned int SIZE_IN_BYTES = 4;

struct GPUShape
{
    GLuint vao, vbo, ebo, texture;
    std::size_t size;

    /*
    Convenience function for initialization of OpenGL buffers.
    It returns itself to enable the convenience call:
    gpuShape = GPUShape().initBuffers()

    Note: this is not the default constructor as you may want
    to use some already existing buffers.
    */
    void initBuffers();

    void fillBuffers(const Shape& shape, GLuint usage);

    /* Freeing GPU memory */
    void clear();
};

std::ostream& operator<<(std::ostream& os, const GPUShape& gpuShape);

/* Convenience function to ease initialization */
template <typename PipelineT>
GPUShape toGPUShape(const PipelineT& pipeline, const Shape& shape, GLuint usage = GL_STATIC_DRAW)
{
    GPUShape gpuShape;
    gpuShape.initBuffers();
    pipeline.setupVAO(gpuShape);
    gpuShape.fillBuffers(shape, usage);
    return gpuShape;
}

} //Grafica