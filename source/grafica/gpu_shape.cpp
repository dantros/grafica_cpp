/**
 * @file gpu_shape.cpp
 * @brief Convenience struct GPUShape to reference a shape stored in GPU memory.
 *        It keeps track of an OpenGL VAO, a VBO, an EBO and a texture.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include "gpu_shape.h"

namespace Grafica
{

void GPUShape::initBuffers()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
}

void GPUShape::fillBuffers(const Shape& shape, GLuint usage)
{
    size = shape.indices.size();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, shape.vertices.size() * SIZE_IN_BYTES, shape.vertices.data(), usage);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indices.size() * SIZE_IN_BYTES, shape.indices.data(), usage);
}

void GPUShape::clear()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

std::ostream& operator<<(std::ostream& os, const GPUShape& gpuShape)
{
    os << "vao=" << gpuShape.vao
        << " vbo=" << gpuShape.vbo
        << " ebo=" << gpuShape.ebo
        << " tex=" << gpuShape.texture;

    return os;
}

} //Grafica