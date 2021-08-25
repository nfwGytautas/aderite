#pragma once

/**
 * @brief File containing various configuration variables and constants
*/

#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

/**
 * @brief Drag and drop id passed when the object that is being dragged is a mesh object
*/
constexpr char* DDPayloadID__MeshAsset = "shared::DDPayloadID__MeshAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a material object
*/
constexpr char* DDPayloadID__MaterialAsset = "shared::DDPayloadID__MaterialAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a shader object
*/
constexpr char* DDPayloadID__ShaderAsset = "shared::DDPayloadID__ShaderAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a generic object
*/
constexpr char* DDPayloadID__GenericAsset = "shared::DDPayloadID__GenericAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a scene object
*/
constexpr char* DDPayloadID__SceneAsset = "shared::DDPayloadID__SceneAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a directory
*/
constexpr char* DDPayloadID__Directory = "shared::DDPayloadID__Directory";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a raw file
*/
constexpr char* DDPayloadID__RawData = "shared::DDPayloadID__RawData";

ADERITE_EDITOR_SHARED_NAMESPACE_END
