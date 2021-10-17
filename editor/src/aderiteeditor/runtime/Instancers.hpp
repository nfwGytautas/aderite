#pragma once

#include "aderite/Aderite.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/io/Instancer.hpp"
#include "aderiteeditor/runtime/EditorSerializables.hpp"

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
#include "aderiteeditor/asset/RenderingPipeline.hpp"
#include "aderiteeditor/node/Graph.hpp"

namespace aderite {
namespace utility {

/**
 * @brief Utility function to link instancers
*/
void linkInstancers() {
	aderite::io::Serializer* serializer = aderite::Engine::getSerializer();

	// Material
	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::AddNode),
		new ::aderite::io::Instancer<node::AddNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::MaterialInputNode),
		new ::aderite::io::Instancer<node::MaterialInputNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::MaterialOutputNode),
		new ::aderite::io::Instancer<node::MaterialOutputNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::Sampler2DNode),
		new ::aderite::io::Instancer<node::Sampler2DNode>()
	);

	// Pipeline
	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::CameraProviderNode),
		new ::aderite::io::Instancer<node::CameraProviderNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::ConcatObjectsNode),
		new ::aderite::io::Instancer<node::ConcatObjectsNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::EditorCameraNode),
		new ::aderite::io::Instancer<node::EditorCameraNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::EditorRenderNode),
		new ::aderite::io::Instancer<node::EditorRenderNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::EditorTargetNode),
		new ::aderite::io::Instancer<node::EditorTargetNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::EntitiesNode),
		new ::aderite::io::Instancer<node::EntitiesNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::RenderNode),
		new ::aderite::io::Instancer<node::RenderNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::RequireLockNode),
		new ::aderite::io::Instancer<node::RequireLockNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::ScreenNode),
		new ::aderite::io::Instancer<node::ScreenNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::SelectObjectNode),
		new ::aderite::io::Instancer<node::SelectObjectNode>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::TargetProviderNode),
		new ::aderite::io::Instancer<node::TargetProviderNode>()
	);

	// Shared
	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::ConvertNode),
		new ::aderite::io::Instancer<node::ConvertNode>()
	);

	// Assets
	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::RenderingPipelineAsset),
		new ::aderite::io::Instancer<aderite::asset::RenderingPipeline>()
	);

	serializer->linkInstancer(
		static_cast<::aderite::io::SerializableType>(::aderite::io::EditorSerializables::GraphAsset),
		new ::aderite::io::Instancer<node::Graph>()
	);
}

}
}
