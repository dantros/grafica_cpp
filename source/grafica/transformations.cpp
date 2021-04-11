/**
 * @file transformations.cpp
 * @brief Convenience transformation matrices written in 3D homogeneous coordinates (i.e. matrices shape is 4x4).
 *        Under the hood, Eigen matrices are used.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include "transformations.h"

#include <cmath>
#include <numbers>
#include <Eigen/Geometry>

namespace Grafica
{
namespace Transformations
{

Matrix4f identity()
{
    return Matrix4f::Identity();
}

Matrix4f uniformScale(Coord s)
{
    return (Matrix4f() <<
        s, 0, 0, 0,
        0, s, 0, 0,
        0, 0, s, 0,
        0, 0, 0, 1).finished();
}

Matrix4f scale(Coord sx, Coord sy, Coord sz)
{
    return (Matrix4f() <<
        sx, 0, 0, 0,
        0, sy, 0, 0,
        0, 0, sz, 0,
        0, 0, 0, 1).finished();
}

Matrix4f rotationX(Coord theta_radians)
{
    Coord sin_theta = std::sin(theta_radians);
    Coord cos_theta = std::cos(theta_radians);

    return (Matrix4f() <<
        1,         0,          0, 0,
        0, cos_theta, -sin_theta, 0,
        0, sin_theta,  cos_theta, 0,
        0,         0,          0, 1).finished();
}

Matrix4f rotationY(Coord theta_radians)
{
    Coord sin_theta = std::sin(theta_radians);
    Coord cos_theta = std::cos(theta_radians);

    return (Matrix4f() <<
         cos_theta, 0, sin_theta, 0,
                 0, 1,         0, 0,
        -sin_theta, 0, cos_theta, 0,
                 0, 0,         0, 1).finished();
}

Matrix4f rotationZ(Coord theta_radians)
{
    Coord sin_theta = std::sin(theta_radians);
    Coord cos_theta = std::cos(theta_radians);

    return (Matrix4f() <<
        cos_theta, -sin_theta, 0, 0,
        sin_theta,  cos_theta, 0, 0,
                0,          0, 1, 0,
                0,          0, 0, 1).finished();
}

Matrix4f rotationA(Coord theta_radians, Vector3f axis)
{
    Coord s = std::sin(theta_radians);
    Coord c = std::cos(theta_radians);

    axis.normalize();

    Coord x = axis[0];
    Coord y = axis[1];
    Coord z = axis[2];

    return (Matrix4f() <<
        // First row
        c + (1 - c) * x * x,
        (1 - c) * x * y - s * z,
        (1 - c) * x * z + s * y,
        0,
        // Second row
        (1 - c) * x * y + s * z,
        c + (1 - c) * y * y,
        (1 - c) * y * z - s * x,
        0,
        // Third row
        (1 - c) * x * z - s * y,
        (1 - c) * y * z + s * x,
        c + (1 - c) * z * z,
        0,
        // Fourth row
        0,0,0,1).finished();
}

Matrix4f translate(Coord tx, Coord ty, Coord tz)
{
    return (Matrix4f() <<
        1, 0, 0, tx,
        0, 1, 0, ty,
        0, 0, 1, tz,
        0, 0, 0,  1).finished();
}

Matrix4f shearing(Coord xy, Coord yx, Coord xz, Coord zx, Coord yz, Coord zy)
{
    return (Matrix4f() <<
         1, xy, xz, 0,
        yx,  1, yz, 0,
        zx, zy,  1, 0,
         0,  0,  0, 1).finished();
}

Matrix4f frustum(Coord left, Coord right, Coord bottom, Coord top, Coord near, Coord far)
{
    Coord r_l = right - left;
    Coord t_b = top - bottom;
    Coord f_n = far - near;

    return (Matrix4f() <<
        // First row
        2 * near / r_l,
        0,
        (right + left) / r_l,
        0,
        // Second row
        0,
        2 * near / t_b,
        (top + bottom) / t_b,
        0,
        // Third row
        0,
        0,
        -(far + near) / f_n,
        -2 * near * far / f_n,
        // Fourth row
        0, 0, -1, 0).finished();
}

Matrix4f perspective(Coord fovy, Coord aspect, Coord near, Coord far)
{
    Coord halfHeight = std::tan(std::numbers::pi * fovy / 360) * near;
    Coord halfWidth = halfHeight * aspect;

    return frustum(-halfWidth, halfWidth, -halfHeight, halfHeight, near, far);
}


Matrix4f ortho(Coord left, Coord right, Coord bottom, Coord top, Coord near, Coord far)
{
    Coord r_l = right - left;
    Coord t_b = top - bottom;
    Coord f_n = far - near;

    return (Matrix4f() <<
        // First row
        2 / r_l,
        0,
        0,
        -(right + left) / r_l,
        // Second row
        0,
        2 / t_b,
        0,
        -(top + bottom) / t_b,
        // Third row
        0,
        0,
        -2 / f_n,
        -(far + near) / f_n,
        // Fourth row
        0, 0, 0, 1).finished();
}


Matrix4f lookAt(Vector3f const& eye, Vector3f const& at, Vector3f const& up)
{
    Vector3f forward = at - eye;
    forward.normalize();

    Vector3f side = forward.cross(up);
    side.normalize();

    Vector3f newUp = side.cross(forward);
    newUp.normalize();

    return (Matrix4f() <<
            side[0],       side[1],    side[2], -side.dot(eye),
            newUp[0],     newUp[1],   newUp[2], -newUp.dot(eye),
            -forward[0], -forward[1], -forward[2], forward.dot(eye),
            0,0,0,1).finished();
}

} // Transformations
} // Grafica