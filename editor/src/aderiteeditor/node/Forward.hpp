#pragma once

/**
 * @brief This file is used to define forward declarations for all node functionality
*/

#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_NODE_NAMESPACE_BEGIN

class Graph;
class Node;
class InputPin;
class OutputPin;
class Link;

// Node types

// Material
class MaterialInputNode;
class MaterialOutputNode;
class AddNode;
class Sampler2DNode;

// Rendering pipeline
class ScreenNode;
class EntitiesNode;
class RenderNode;
class TargetProviderNode;
class CameraProviderNode;
class EditorRenderNode;
class EditorTargetNode;
class EditorCameraNode;

ADERITE_EDITOR_NODE_NAMESPACE_END
