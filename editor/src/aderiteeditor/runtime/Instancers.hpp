#pragma once

#include "aderite/Aderite.hpp"
#include "aderite/reflection/Reflector.hpp"

#include "aderiteeditor/runtime/EditorTypes.hpp"

// Nodes
// Material
#include "aderiteeditor/node/material/AddNode.hpp"
#include "aderiteeditor/node/material/MaterialInputNode.hpp"
#include "aderiteeditor/node/material/MaterialOutputNode.hpp"
#include "aderiteeditor/node/material/Sampler2DNode.hpp"

// Pipeline
#include "aderiteeditor/node/pipeline/CameraProviderNode.hpp"
#include "aderiteeditor/node/pipeline/ConcatObjectsNode.hpp"
#include "aderiteeditor/node/pipeline/EditorCameraNode.hpp"
#include "aderiteeditor/node/pipeline/EditorRenderNode.hpp"
#include "aderiteeditor/node/pipeline/EditorTargetNode.hpp"
#include "aderiteeditor/node/pipeline/EntitiesNode.hpp"
#include "aderiteeditor/node/pipeline/RenderNode.hpp"
#include "aderiteeditor/node/pipeline/RequireLockNode.hpp"
#include "aderiteeditor/node/pipeline/ScreenNode.hpp"
#include "aderiteeditor/node/pipeline/SelectObjectNode.hpp"
#include "aderiteeditor/node/pipeline/TargetProviderNode.hpp"

// Shared
#include "aderiteeditor/node/shared/ConvertNode.hpp"

// Assets
#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/RenderingPipeline.hpp"
#include "aderiteeditor/node/Graph.hpp"

// Operations
#include "aderiteeditor/runtime/EditorCameraOperation.hpp"
#include "aderiteeditor/runtime/EditorRenderOperation.hpp"
#include "aderiteeditor/runtime/EditorTargetOperation.hpp"
#include "aderiteeditor/runtime/OperationArray.hpp"

// Extensions
#include "aderiteeditor/extensions/EditorAudioSource.hpp"
#include "aderiteeditor/extensions/EditorEntity.hpp"

namespace aderite {
namespace utility {

/**
 * @brief Utility function to link instancers
 */
void linkInstancers() {
    aderite::reflection::Reflector* reflector = aderite::Engine::getReflector();

    // Material
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::AddNode, reflection::EditorTypes::AddNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::MaterialInputNode, reflection::EditorTypes::MaterialInputNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::MaterialOutputNode, reflection::EditorTypes::MaterialOutputNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::Sampler2DNode, reflection::EditorTypes::Sampler2DNode);

    // Pipeline
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::CameraProviderNode, reflection::EditorTypes::CameraProviderNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::ConcatObjectsNode, reflection::EditorTypes::ConcatObjectsNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::EditorCameraNode, reflection::EditorTypes::EditorCameraNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::EditorRenderNode, reflection::EditorTypes::EditorRenderNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::EditorTargetNode, reflection::EditorTypes::EditorTargetNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::EntitiesNode, reflection::EditorTypes::EntitiesNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::RenderNode, reflection::EditorTypes::RenderNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::RequireLockNode, reflection::EditorTypes::RequireLockNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::ScreenNode, reflection::EditorTypes::ScreenNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::SelectObjectNode, reflection::EditorTypes::SelectObjectNode);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::TargetProviderNode, reflection::EditorTypes::TargetProviderNode);

    // Shared
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::ConvertNode, reflection::EditorTypes::ConvertNode);

    // Assets
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, asset::RenderingPipeline, reflection::RuntimeTypes::PIPELINE);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, asset::EditorMaterialType, reflection::RuntimeTypes::MAT_TYPE);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, node::Graph, reflection::EditorTypes::GraphAsset);

    // Operations
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, editor::EditorCameraOperation, reflection::EditorTypes::EditorCameraOp);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, editor::EditorRenderOperation, reflection::EditorTypes::EditorRenderOp);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, editor::EditorTargetOperation, reflection::EditorTypes::EditorTargetOp);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, editor::OperationArray, reflection::EditorTypes::ArrayOp);

    // Extensions
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, audio::EditorAudioSource, reflection::RuntimeTypes::AUDIO_SOURCE);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, scene::EditorEntity, reflection::RuntimeTypes::ENTITY);
}

} // namespace utility
} // namespace aderite
