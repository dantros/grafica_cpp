/**
 * @file simple_eigen.h
 * @brief Definition of common eigen vectors and matrices types used in Grafica.
 *        Eigen::DontAlign is used to these matrices can be used properly when interfacing with OpenGL.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>

namespace Grafica
{

using Coord = float;

// Special types to prevent eigen alignment problem
using Matrix4f = Eigen::Matrix<Coord,4,4,Eigen::DontAlign>;
using Matrix34f = Eigen::Matrix<Coord,3,4,Eigen::DontAlign>;
using Vector2f = Eigen::Matrix<Coord,2,1,Eigen::DontAlign>;
using Vector3f = Eigen::Matrix<Coord,3,1,Eigen::DontAlign>;
using Vector4f = Eigen::Matrix<Coord,4,1,Eigen::DontAlign>;

} // Grafica