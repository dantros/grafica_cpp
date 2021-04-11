/**
 * @file basic_shapes.cpp
 * @brief Vertices and indices for a variety of simple shapes.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include <cmath>
#include "basic_shapes.h"

namespace Grafica
{


Shape createRainbowTriangle()
{
    Shape shape(6);
    shape.vertices = {
    //   positions           colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    shape.indices = {
        0, 1, 2
    };
    
    return shape;
}


Shape createRainbowQuad()
{
    Shape shape(6);
    shape.vertices = {
    //   positions           colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f
    };
    shape.indices = {
        0, 1, 2,
        2, 3, 0
    };
    
    return shape;
}


Shape createColorQuad(Coord r, Coord g, Coord b)
{
    Shape shape(6);
    shape.vertices = {
    //   positions           colors
        -0.5f, -0.5f, 0.0f,  r, g, b,
         0.5f, -0.5f, 0.0f,  r, g, b,
         0.5f,  0.5f, 0.0f,  r, g, b,
        -0.5f,  0.5f, 0.0f,  r, g, b
    };
    shape.indices = {
        0, 1, 2,
         2, 3, 0
    };

    return shape;
}

Shape createTextureQuad(Coord nx, Coord ny)
{
    Shape shape(5);
    shape.vertices = {
    //   positions          texture coordinates
        -0.5f, -0.5f, 0.0f, 0.0f,   ny,
         0.5f, -0.5f, 0.0f,   nx,   ny,
         0.5f,  0.5f, 0.0f,   nx, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f
    };
    shape.indices = {
        0, 1, 2,
         2, 3, 0
    };

    return shape;
}

template <typename Type>
void append(std::vector<Type>& vector, const std::vector<Type>& newElements)
{
    for (auto const& newElement : newElements)
        vector.push_back(newElement);
}

Shape createColorCircle(unsigned int steps, Coord r, Coord g, Coord b)
{
    Shape shape(6);
    shape.vertices;

    shape.vertices.reserve(6 * (steps + 1));
    shape.indices.reserve(3 * steps);
    
    /* This is vertex 0, the center of the circle */
    append(shape.vertices, {0.0f, 0.0f, 0.0f, r, g, b});
    append(shape.vertices, {0.5f, 0.0f, 0.0f, r, g, b});

    Coord dtheta = 2 * 3.1415 / steps;

    for (unsigned int i = 1; i <= steps + 1; i++)
    {
        Coord theta = i * dtheta;
        Coord x = 0.5 * std::cos(theta);
        Coord y = 0.5 * std::sin(theta);
        std::vector<Coord> newVertex{x, y, 0.0f, r, g, b};
        append(shape.vertices, newVertex);

        std::vector<Index> newTriangle{0, i-1, i};
        append(shape.indices, newTriangle);
    }

    return shape;
}

Shape createAxis(Coord length)
{
    Shape shape(6);
    shape.vertices = {
    //   positions           colors
        -length,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         length,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

         0.0f, -length,  0.0f, 0.0f, 0.0f, 0.0f,
         0.0f,  length,  0.0f, 0.0f, 1.0f, 0.0f,

         0.0f,  0.0f, -length, 0.0f, 0.0f, 0.0f,
         0.0f,  0.0f,  length, 0.0f, 0.0f, 1.0f
    };
    // This shape is meant to be drawn with GL_LINES,
    // i.e. every 2 indices, we have 1 line.
    shape.indices = {
        0, 1,
        2, 3,
        4, 5
    };

    return shape;
}

Shape createGridXY(unsigned int Nx, unsigned int Ny, Coord z, Coord r, Coord g, Coord b)
{
    Shape shape(6);
    shape.vertices.reserve(6 * Nx + 6 * Ny);
    shape.indices.reserve(2 * Nx + 2 * Ny);

    Coord xmin = 0;
    Coord xmax = Ny;

    for (unsigned int i = 0; i <= Nx; i++)
    {
        Coord y = i;
        
        shape.vertices.push_back(xmin);
        shape.vertices.push_back(y);
        shape.vertices.push_back(z);

        shape.vertices.push_back(r);
        shape.vertices.push_back(g);
        shape.vertices.push_back(b);

        shape.vertices.push_back(xmax);
        shape.vertices.push_back(y);
        shape.vertices.push_back(z);

        shape.vertices.push_back(r);
        shape.vertices.push_back(g);
        shape.vertices.push_back(b);

        shape.indices.push_back(2*i);
        shape.indices.push_back(2*i + 1);
    }

    auto Sx = shape.indices.size();

    Coord ymin = 0;
    Coord ymax = Nx;

    for (unsigned int j = 0; j <= Ny; j++)
    {
        Coord x = j;
        
        shape.vertices.push_back(x);
        shape.vertices.push_back(ymin);
        shape.vertices.push_back(z);

        shape.vertices.push_back(r);
        shape.vertices.push_back(g);
        shape.vertices.push_back(b);

        shape.vertices.push_back(x);
        shape.vertices.push_back(ymax);
        shape.vertices.push_back(z);

        shape.vertices.push_back(r);
        shape.vertices.push_back(g);
        shape.vertices.push_back(b);

        shape.indices.push_back(Sx + 2*j);
        shape.indices.push_back(Sx + 2*j + 1);
    }

    return shape;
}

Shape createRainbowCube()
{
    Shape shape(6);
    shape.vertices = {
    //   positions           colors
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
 
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f
    };
    // One face of the cube per row
    shape.indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        4, 5, 1, 1, 0, 4,
        6, 7, 3, 3, 2, 6,
        5, 6, 2, 2, 1, 5,
        7, 4, 0, 0, 3, 7
    };

    return shape;
}

Shape createColorCube(Coord r, Coord g, Coord b)
{
    Shape shape(6);
    shape.vertices = {
    //   positions           colors
        -0.5f, -0.5f,  0.5f, r, g, b,
         0.5f, -0.5f,  0.5f, r, g, b,
         0.5f,  0.5f,  0.5f, r, g, b,
        -0.5f,  0.5f,  0.5f, r, g, b,

        -0.5f, -0.5f, -0.5f, r, g, b,
         0.5f, -0.5f, -0.5f, r, g, b,
         0.5f,  0.5f, -0.5f, r, g, b,
        -0.5f,  0.5f, -0.5f, r, g, b
    };
    // One face of the cube per row
    shape.indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        4, 5, 1, 1, 0, 4,
        6, 7, 3, 3, 2, 6,
        5, 6, 2, 2, 1, 5,
        7, 4, 0, 0, 3, 7
    };

    return shape;
}

Shape createRainbowNormalsCube()
{
    Coord sq3 = 0.57735027;

    Shape shape(9);
    shape.vertices = {
    //   positions           colors
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -sq3, -sq3, sq3,
         0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  sq3, -sq3,  sq3,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  sq3,  sq3,  sq3,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, -sq3,  sq3,  sq3,

        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -sq3, -sq3, -sq3,
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  sq3, -sq3, -sq3,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f,  sq3,  sq3, -sq3,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, -sq3,  sq3, -sq3
    };
    // One face of the cube per row
    shape.indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        4, 5, 1, 1, 0, 4,
        6, 7, 3, 3, 2, 6,
        5, 6, 2, 2, 1, 5,
        7, 4, 0, 0, 3, 7
    };

    return shape;
}

Shape createColorNormalsCube(Coord r, Coord g, Coord b)
{
    Shape shape(9);
    shape.vertices = {
    //   positions         colors   normals
    // Z+
        -0.5f, -0.5f,  0.5f, r, g, b, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, r, g, b, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, r, g, b, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, r, g, b, 0.0f, 0.0f, 1.0f,

    // Z-
        -0.5f, -0.5f, -0.5f, r, g, b, 0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f, r, g, b, 0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, r, g, b, 0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f, r, g, b, 0.0f, 0.0f, -1.0f,
        
    // X+
        0.5f, -0.5f, -0.5f, r, g, b, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f, r, g, b, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f, r, g, b, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, r, g, b, 1.0f, 0.0f, 0.0f,
 
    // X-
        -0.5f, -0.5f, -0.5f, r, g, b, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, r, g, b, -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, r, g, b, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, r, g, b, -1.0f, 0.0f, 0.0f,

    // Y+
        -0.5f, 0.5f, -0.5f, r, g, b, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, -0.5f, r, g, b, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f,  0.5f, r, g, b, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f, r, g, b, 0.0f, 1.0f, 0.0f,

    // Y-
        -0.5f, -0.5f, -0.5f, r, g, b, 0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, r, g, b, 0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, r, g, b, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, r, g, b, 0.0f, -1.0f, 0.0f
    };
    // One face of the cube per row
    shape.indices = {
         0, 1, 2, 2, 3, 0, // Z+
         7, 6, 5, 5, 4, 7, // Z-
         8, 9,10,10,11, 8, // X+
        15,14,13,13,12,15, // X-
        19,18,17,17,16,19, // Y+
        20,21,22,22,23,20  // Y-
    };

    return shape;
}

} // Grafica