#include "TextureSource.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "aderite/utility/Log.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

TextureSource::TextureSource(std::filesystem::path file) 
	: m_file(file)
{
	if (!std::filesystem::exists(m_file)) {
		LOG_WARN("Tried to load {0} but such file doesn't exist", m_file.string());
		m_error = LoadError::BAD_FILE;
		return;
	}
}

TextureSource::~TextureSource() {
	stbi_image_free(m_data);
}

void TextureSource::load() {
	m_data = stbi_load(m_file.string().c_str(), &m_width, &m_height, &m_bpp, 0);

	if (m_data == nullptr) {
		LOG_WARN("STB failed loading normal image {0}, {1}", m_file.string(), stbi_failure_reason());
		m_error = LoadError::BAD_FORMAT;
	}
}

void TextureSource::loadHdr() {
	m_hdr = true;
	m_data = stbi_loadf(m_file.string().c_str(), &m_width, &m_height, &m_bpp, 4);

	if (m_data == nullptr) {
		LOG_WARN("STB failed loading HDR image {0}, {1}", m_file.string(), stbi_failure_reason());
		m_error = LoadError::BAD_FORMAT;
	}
}

int TextureSource::getBpp() const {
	return m_bpp;
}

int TextureSource::getWidth() const {
	return m_width;
}

int TextureSource::getHeight() const {
	return m_height;
}

const void* TextureSource::getData() const {
	return m_data;
}

size_t TextureSource::getSize() const {
	return m_width * m_height * m_bpp * (m_hdr ? sizeof(float) : sizeof(unsigned char));
}

bgfx::TextureFormat::Enum TextureSource::getFormat() const {
	if (m_hdr) {
		switch (m_bpp) {
		case 1:
			return bgfx::TextureFormat::R32F;
		case 2:
			return bgfx::TextureFormat::RG32F;
		case 3:
			LOG_FATAL("3 BPP HDR not supported");
		case 4:
			return bgfx::TextureFormat::RGBA32F;
		}
	}
	else {
		switch (m_bpp) {
		case 1:
			return bgfx::TextureFormat::R8;
		case 2:
			return bgfx::TextureFormat::RG8;
		case 3:
			return bgfx::TextureFormat::RGB8;
		case 4:
			return bgfx::TextureFormat::RGBA8;
		}
	}
}

ADERITE_ASSET_NAMESPACE_END
