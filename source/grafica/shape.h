/**
 * @file shape.h
 * @brief Convenience struct Shape to store vertices and indices representing a shape to be drawn later with OpenGL.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#pragma once

#include <glad/glad.h>
#include <vector>
#include <string>
#include <iostream>

namespace Grafica
{

using Coord = GLfloat;
using Index = GLuint;

using Vertices = std::vector<Coord>;
using Indices = std::vector<Index>;

struct Shape
{
    Vertices vertices;
    Indices indices;
    std::string texture = "";
    std::size_t stride;

    Shape(std::size_t stride_) : stride(stride_){}
};

Shape join(const Shape& rhs, const Shape& lhs);

std::ostream& operator<<(std::ostream& os, const Vertices& vertices);

std::ostream& operator<<(std::ostream& os, const Indices& indices);

std::ostream& operator<<(std::ostream& os, const Shape& shape);

} // Grafica