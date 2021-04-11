/**
 * @file transformations.h
 * @brief Convenience transformation matrices written in 3D homogeneous coordinates (i.e. matrices shape is 4x4).
 *        Under the hood, Eigen matrices are used.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#pragma once

#include "simple_eigen.h"

namespace Grafica
{
namespace Transformations
{

Matrix4f identity();

Matrix4f uniformScale(Coord s);

Matrix4f scale(Coord sx, Coord sy, Coord sz);

Matrix4f rotationX(Coord theta_radians);

Matrix4f rotationY(Coord theta_radians);

Matrix4f rotationZ(Coord theta_radians);

Matrix4f rotationA(Coord theta_radians, Vector3f axis);

Matrix4f translate(Coord tx, Coord ty, Coord tz);

Matrix4f shearing(Coord xy, Coord yx, Coord xz, Coord zx, Coord yz, Coord zy);

Matrix4f frustum(Coord left, Coord right, Coord bottom, Coord top, Coord near, Coord far);

Matrix4f perspective(Coord fovy, Coord aspect, Coord near, Coord far);

Matrix4f ortho(Coord left, Coord right, Coord bottom, Coord top, Coord near, Coord far);

Matrix4f lookAt(Vector3f const& eye, Vector3f const& at, Vector3f const& up);

} // Transformations
} // Grafica