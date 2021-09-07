#pragma once

#include <functional>
#include <filesystem>
#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Class containing various information about a texture
*/
class TextureSource {
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
	TextureSource(std::filesystem::path file);
	~TextureSource();

	/**
	 * @brief Loads data as a normal image
	*/
	void load();

	/**
	 * @brief Loads data as a hdr image
	*/
	void loadHdr();

	/**
	 * @brief Returns the bits per pixel of the image
	*/
	int getBpp() const;

	/**
	 * @brief Returns the width of the loaded image
	*/
	int getWidth() const;

	/**
	 * @brief Returns the height of the loaded image
	*/
	int getHeight() const;

	/**
	 * @brief Data buffer
	*/
	const void* getData() const;

	/**
	 * @brief Returns the size of the data buffer, same as getWidth() * getHeight() * getBpp()
	*/
	size_t getSize() const;

	/**
	 * @brief Returns bgfx format of the source
	*/
	bgfx::TextureFormat::Enum getFormat() const;

	/**
	 * @brief Return load error
	*/
	LoadError error() const {
		return m_error;
	}

private:
	std::filesystem::path m_file;
	LoadError m_error = LoadError::NONE;

	void* m_data = nullptr;
	bool m_hdr = false;

	int m_bpp;
	int m_width;
	int m_height;
};

ADERITE_ASSET_NAMESPACE_END
