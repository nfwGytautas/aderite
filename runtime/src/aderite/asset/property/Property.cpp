#include "Property.hpp"

#include "aderite/utility/Log.hpp"

ADERITE_PROPERTY_NAMESPACE_BEGIN

uint8_t getElementCountForType(PropertyType type) {
	switch (type) {
	case PropertyType::FLOAT:
		return 1;
	case PropertyType::TEXTURE_2D:
	case PropertyType::TEXTURE_CUBE:
		return 0;
	case PropertyType::VEC2:
		return 2;
	case PropertyType::VEC3:
		return 3;
	case PropertyType::VEC4:
		return 4;
	default:
		LOG_ERROR("Unknown property type passed to getElementCountForType");
		return 0;
	}
}

bool isSampler(PropertyType type) {
	switch (type) {
	case PropertyType::TEXTURE_2D:
	case PropertyType::TEXTURE_CUBE:
		return true;
	default:
		return false;
	}
}

const char* getNameForType(PropertyType type) {
	switch (type) {
	case PropertyType::FLOAT:
		return "Float";
	case PropertyType::TEXTURE_2D:
		return "Texture 2D";
	case PropertyType::TEXTURE_CUBE:
		return "Texture Cube";
	case PropertyType::VEC2:
		return "Vec2";
	case PropertyType::VEC3:
		return "Vec3";
	case PropertyType::VEC4:
		return "Vec4";
	default:
		LOG_ERROR("Unknown property type passed to getElementCountForType");
		return "";
	}
}

Property::Property(PropertyType type, const std::string& name)
	: m_name(name), m_type(type)
{}

PropertyType Property::getType() const {
	return m_type;
}

float* Property::getValue(float* data) const {
	return &data[m_offset];
}

void Property::setValue(float* data, float* value) {
	std::memcpy(data + m_offset, value, getElementCountForType(m_type) * sizeof(float));
}

uint16_t Property::getOffset() const {
	return m_offset;
}

void Property::setOffset(uint16_t offset) {
	m_offset = offset;
}

std::string Property::getName() const {
	return m_name;
}

void Property::setName(const std::string& name) {
	m_name = name;
}

ADERITE_PROPERTY_NAMESPACE_END
