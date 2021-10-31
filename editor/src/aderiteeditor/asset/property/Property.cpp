#include "Property.hpp"

#include "aderite/utility/Log.hpp"

namespace aderite {
namespace asset {

uint8_t getElementCountForType(PropertyType type) {
    switch (type) {
    case PropertyType::FLOAT:
        return 1;
    case PropertyType::VEC2:
        return 2;
    case PropertyType::VEC3:
        return 3;
    case PropertyType::VEC4:
        return 4;
    case PropertyType::NONE:
        return 0;
    default:
        LOG_ERROR("Unknown property type passed to getElementCountForType");
        return 0;
    }
}

const char* getNameForType(PropertyType type) {
    switch (type) {
    case PropertyType::FLOAT:
        return "float";
    case PropertyType::VEC2:
        return "vec2";
    case PropertyType::VEC3:
        return "vec3";
    case PropertyType::VEC4:
        return "vec4";
    case PropertyType::NONE:
        return "None";
    default:
        LOG_ERROR("Unknown property type passed to getNameForType");
        return "";
    }
}

const char* getNameForType(SamplerType type) {
    switch (type) {
    case SamplerType::TEXTURE_2D:
        return "Texture 2D";
    case SamplerType::TEXTURE_CUBE:
        return "Texture Cube";
    default:
        LOG_ERROR("Unknown sampler type passed to getNameForType");
        return "";
    }
}

Property::Property(PropertyType type, const std::string& name) : m_name(name), m_type(type) {}

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

Sampler::Sampler(SamplerType type, const std::string& name) : m_type(type), m_name(name) {}

SamplerType Sampler::getType() const {
    return m_type;
}

std::string Sampler::getName() const {
    return m_name;
}

void Sampler::setName(const std::string& name) {
    m_name = name;
}

} // namespace asset
} // namespace aderite
