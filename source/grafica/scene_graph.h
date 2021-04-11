/**
 * @file scene_graph.h
 * @brief Convenience struct SceneGraphNode to store a hierarchy of transformations and shapes meant to be drawn with OpenGL.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <ciso646>
#include "gpu_shape.h"
#include "shape.h"
#include "simple_eigen.h"
#include "transformations.h"

namespace Grafica
{

struct SceneGraphNode;

using GPUShapePtr = std::shared_ptr<GPUShape>;
using SceneGraphNodePtr = std::shared_ptr<SceneGraphNode>;

class SceneGraphNode
{
public:
    std::string name;
    Matrix4f transform;
    std::optional<GPUShapePtr> gpuShapeMaybe;
    std::vector<SceneGraphNodePtr> childs;
    
    SceneGraphNode(
        const std::string& name_) : 
        name(name_),
        transform(Transformations::identity()),
        gpuShapeMaybe(std::nullopt),
        childs()
    {}

    SceneGraphNode(
        const std::string& name_,
        const Matrix4f& transform_) : 
        name(name_),
        transform(transform_),
        gpuShapeMaybe(std::nullopt),
        childs()
    {}

    SceneGraphNode(
        const std::string& name_,
        const Matrix4f& transform_,
        GPUShapePtr gpuShapePtr_) : 
        name(name_),
        transform(transform_),
        gpuShapeMaybe(gpuShapePtr_),
        childs()
    {}

    void clear();
};

std::optional<SceneGraphNodePtr> findNode(
    SceneGraphNodePtr nodePtr,
    const std::string& name);

std::optional<Matrix4f> findTransform(
    SceneGraphNodePtr nodePtr,
    const std::string& name,
    const Matrix4f& parentTransform = Transformations::identity());

std::optional<Vector4f> findPosition(
    SceneGraphNodePtr nodePtr,
    const std::string& name,
    const Matrix4f& parentTransform = Transformations::identity());

template <typename PipelineType>
void drawSceneGraphNode(
    SceneGraphNodePtr nodePtr,
    const PipelineType& pipeline,
    const std::string& transformName,
    const Matrix4f& parentTransform = Transformations::identity())
{
    // Composing the transformations through this path
    Matrix4f newTransform = parentTransform * nodePtr->transform;

    if (nodePtr->gpuShapeMaybe.has_value())
    {
        auto const shapePtr = nodePtr->gpuShapeMaybe.value();
        auto const& shape = *shapePtr;
        glUniformMatrix4fv(glGetUniformLocation(pipeline.shaderProgram, transformName.c_str()), 1, GL_FALSE, newTransform.data());
        pipeline.drawCall(shape);
    }

    // If the child node is not a leaf, it MUST be a SceneGraphNode,
    // so this draw function is called recursively
    for (auto childPtr : nodePtr->childs)
        drawSceneGraphNode(childPtr, pipeline, transformName, newTransform);
}

} // Grafica