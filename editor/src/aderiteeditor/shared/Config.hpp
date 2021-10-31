#pragma once

/**
 * @brief File containing various configuration variables and constants
 */

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Drag and drop id passed when the object that is being dragged is a mesh object
 */
constexpr char* DDPayloadID__MeshAsset = "DDPayloadID__MeshAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a material object
 */
constexpr char* DDPayloadID__MaterialAsset = "DDPayloadID__MaterialAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a generic object
 */
constexpr char* DDPayloadID__GenericAsset = "DDPayloadID__GenericAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a scene object
 */
constexpr char* DDPayloadID__SceneAsset = "DDPayloadID__SceneAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a texture object
 */
constexpr char* DDPayloadID__TextureAsset = "DDPayloadID__TextureAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a audio bank
 */
constexpr char* DDPayloadID__AudioBank = "DDPayloadID__AudioBank";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a directory
 */
constexpr char* DDPayloadID__Directory = "DDPayloadID__Directory";

/**
 * @brief Drag and drop ip passed when the object that is being dragged is a material type object
 */
constexpr char* DDPayloadID__MaterialType = "DDPayloadID__MaterialType";

/**
 * @brief Drag and drop ip passed when the object that is being dragged is a pipeline object
 */
constexpr char* DDPayloadID__PipelineAsset = "DDPayloadID__PipelineAsset";

/**
 * @brief Drag and drop id passed when the object that is being dragged is a raw file
 */
constexpr char* DDPayloadID__RawData = "DDPayloadID__RawData";

} // namespace editor
} // namespace aderite
