#pragma once

#include <functional>
#include <filesystem>
#include <vector>
#include "aderite/utility/macros.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Class containing various information about a mesh
*/
class MeshSource {
public:
	/**
	 * @brief Possible load errors
	*/
	enum class load_error : size_t {
		// Loaded successfully
		NONE,

		// Incorrect file path
		BAD_FILE,

		// File was in an incorrect format
		BAD_FORMAT,

		// Requested action is not valid
		UNSUPPORTED_CONFIGURATION,
	};

public:
	MeshSource(std::filesystem::path file);

	/**
	 * @brief Will flag the source to load position data
	*/
	void request_position_data();

	/**
	 * @brief Will flag the source to load indices data
	*/
	void request_indices_data();

	/**
	 * @brief Loads data into source buffers
	*/
	void load();

	/**
	 * @brief Position data buffer
	*/
	std::vector<float>& position_data();

	/**
	 * @brief Indices data buffer
	*/
	std::vector<unsigned int>& indices_data();

	/**
	 * @brief Return load error
	*/
	load_error error() const {
		return m_error;
	}

private:
	std::filesystem::path m_file;
	load_error m_error = load_error::NONE;

	bool m_willLoadPosition = false;
	bool m_willLoadIndices = false;

	std::vector<float> m_positionBuffer;
	std::vector<unsigned int> m_indicesBuffer;
};

ADERITE_ASSET_NAMESPACE_END
