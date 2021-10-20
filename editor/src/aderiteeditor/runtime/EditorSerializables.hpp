#pragma once

#include "aderite/io/Forward.hpp"
#include "aderite/io/RuntimeSerializables.hpp"

namespace aderite {
namespace io {

/**
 * @brief Enum containing all serializable types that the editor has
*/
enum class EditorSerializables : SerializableType {
	// Nodes
	AddNode = static_cast<int>(RuntimeSerializables::END),
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

	RenderingPipelineAsset = 150,
	GraphAsset,
};

}
}
