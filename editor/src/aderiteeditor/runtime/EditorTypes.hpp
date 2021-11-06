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
    AddNode = 500,
    MaterialInputNode,
    MaterialOutputNode,
    Sampler2DNode,

    CameraProviderNode = 750,
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

    ConvertNode = 1250,

    // Assets
    RenderingPipelineAsset = 1500,
    GraphAsset,
    TypeAsset,

    // Operations
    EditorCameraOp = 2000,
    EditorRenderOp,
    EditorTargetOp,
    ArrayOp,

    // Extensions
    AudioSource = 3000,
};

} // namespace reflection
} // namespace aderite
