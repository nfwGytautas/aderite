#pragma once

/**
 * @brief File containing various configuration variables and constants
*/

namespace aderite {
	namespace editor {

		/**
		 * @brief Drag and drop id passed when the object that is being dragged is a mesh object
		*/
		constexpr char* DDPayloadID_MeshAsset = "DDPayloadID_MeshAsset";

		/**
		 * @brief Drag and drop id passed when the object that is being dragged is a material object
		*/
		constexpr char* DDPayloadID_MaterialAsset = "DDPayloadID_MaterialAsset";

		/**
		 * @brief Drag and drop id passed when the object that is being dragged is a generic object
		*/
		constexpr char* DDPayloadID_GenericAsset = "DDPayloadID_GenericAsset";

		/**
		 * @brief Drag and drop id passed when the object that is being dragged is a scene object
		*/
		constexpr char* DDPayloadID_SceneAsset = "DDPayloadID_SceneAsset";

		/**
		 * @brief Drag and drop id passed when the object that is being dragged is a directory
		*/
		constexpr char* DDPayloadID_Directory = "DDPayloadID_Directory";
	}
}
