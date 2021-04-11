/**
 * @file scene_graph.cpp
 * @brief Convenience struct SceneGraphNode to store a hierarchy of transformations and shapes meant to be drawn with OpenGL.
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include "scene_graph.h"

namespace Grafica
{

void SceneGraphNode::clear()
{
    for (auto& childPtr : childs)
        childPtr->clear();
}

std::optional<SceneGraphNodePtr> findNode(
    SceneGraphNodePtr nodePtr,
    const std::string& name)
{
    // This is the requested node
    if (nodePtr->name == name)
        return nodePtr;

    // No child of this node had the requested name
    for (auto& childPtr : nodePtr->childs)
    {
        auto nodeMaybe = findNode(childPtr, name);
        if (nodeMaybe.has_value())
            return nodeMaybe.value();
    }

    // No child of this node had the requested name
    return std::nullopt;
}

std::optional<Matrix4f> findTransform(
    SceneGraphNodePtr nodePtr,
    const std::string& name,
    const Matrix4f& parentTransform)
{
    Matrix4f newTransform = parentTransform * nodePtr->transform;

    // This is the requested node
    if (nodePtr->name == name)
        return newTransform;
    
    // All childs are checked for the requested name
    for (const auto& child : nodePtr->childs)
    {
        auto foundTransformMaybe = findTransform(child, name, newTransform);
        if (foundTransformMaybe.has_value())
            return foundTransformMaybe.value();
    }
    
    // No child of this node had the requested name
    return std::nullopt;
}

std::optional<Vector4f> findPosition(
    SceneGraphNodePtr nodePtr,
    const std::string& name,
    const Matrix4f& parentTransform)
{
    auto transformMaybe = findTransform(nodePtr, name, parentTransform);

    if (not transformMaybe.has_value())
        return std::nullopt;

    auto& transform = transformMaybe.value();
    return transform * Vector4f(0,0,0,1);
}

} // Grafica