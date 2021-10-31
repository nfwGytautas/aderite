#pragma once

#include "aderite/io/Forward.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"

namespace aderite {
namespace reflection {

/**
 * @brief Enum containing all types that the editor has
 */
enum class EditorTypes : Type {
    // Nodes
    AddNode = 50,
    MaterialInputNode,
    MaterialOutputNode,
    Sampler2DNode,

    CameraProviderNode = 75,
    ConcatObjectsNode,
    EditorCameraNode,
    EditorRenderNode,
    EditorTargetNode,
    EntitiesNode,
    RenderNode,
    RequireLockNode,
    ScreenNode,
    SelectObjectNode,
    TargetProviderNode,

    ConvertNode = 125,

    // Assets
    RenderingPipelineAsset = 150,
    GraphAsset,
    TypeAsset,

    // Operations
    EditorCameraOp = 200,
    EditorRenderOp = 201,
    EditorTargetOp = 202,
    ArrayOp = 203,
};

} // namespace reflection
} // namespace aderite
