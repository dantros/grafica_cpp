/**
 * @file basic_shapes.h
 * @brief Vertices and indices for a variety of simple shapes.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#pragma once

#include <string>
#include "shape.h"

namespace Grafica
{

Shape createRainbowTriangle();

Shape createRainbowQuad();

Shape createColorQuad(Coord r, Coord g, Coord b);

Shape createTextureQuad(Coord nx=1, Coord ny=1);

Shape createColorCircle(unsigned int steps, Coord r, Coord g, Coord b);

Shape createAxis(Coord length = 1);

Shape createGridXY(unsigned int Nx, unsigned int Ny, Coord z=0, Coord r=0, Coord g=0, Coord b=0);

Shape createRainbowCube();

Shape createColorCube(Coord r, Coord g, Coord b);

Shape createRainbowNormalsCube();

Shape createColorNormalsCube(Coord r, Coord g, Coord b);

} // Grafica