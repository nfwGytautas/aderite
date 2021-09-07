#pragma once

#include <functional>
#include <filesystem>
#include <vector>
#include "aderite/utility/Macros.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Class containing various information about a mesh
*/
class MeshSource {
public:
	/**
	 * @brief Possible load errors
	*/
	enum class LoadError : size_t {
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
	void requestPositionData();

	/**
	 * @brief Will flag the source to load indices data
	*/
	void requestIndicesData();

	/**
	 * @brief Loads data into source buffers
	*/
	void load();

	/**
	 * @brief Data buffer
	*/
	std::vector<float>& getData();

	/**
	 * @brief Indices data buffer
	*/
	std::vector<unsigned int>& getIndicesData();

	/**
	 * @brief Return load error
	*/
	LoadError error() const {
		return m_error;
	}

private:
	std::filesystem::path m_file;
	LoadError m_error = LoadError::NONE;

	bool m_willLoadPosition = false;
	bool m_willLoadIndices = false;

	std::vector<float> m_buffer;
	std::vector<unsigned int> m_indicesBuffer;
};

ADERITE_ASSET_NAMESPACE_END
