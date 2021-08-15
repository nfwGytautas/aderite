#pragma once

namespace aderite {
	/**
	 * @brief VAO layout configuration
	 */
	struct vao_layout {
		/**
		 * @brief True if the VAO contains position information
		*/
		bool HasPosition = true;

		/**
		 * @brief True if the position data won't be changed during runtime
		*/
		bool IsPositionStatic = true;

		/**
		 * @brief Position attribute start
		*/
		size_t PositionStart = 0;

		/**
		 * @brief Information about position data, ignored if HasPosition = false
		*/
		float* PositionData = nullptr;
		size_t PositionCount = 0;

		/**
		 * @brief True if VAO contains element buffer
		*/
		bool HasIndices = true;

		/**
		 * @brief Element buffer data, ignored if HasIndices = false
		*/
		unsigned int* IndicesData = nullptr;
		size_t IndicesCount = 0;
	};
}
