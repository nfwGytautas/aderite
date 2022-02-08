#include "CameraSettings.hpp"

namespace aderite {
namespace scene {

CameraSettings::~CameraSettings() {}

float CameraSettings::getFoV() const {
    return m_fov;
}

void CameraSettings::setFoV(float fov) {
    m_fov = fov;
}

float CameraSettings::getNearClip() const {
    return m_nearClip;
}

void CameraSettings::setNearClip(float distance) {
    m_nearClip = distance;
}

float CameraSettings::getFarClip() const {
    return m_farClip;
}

void CameraSettings::setFarClip(float distance) {
    m_farClip = distance;
}

bool CameraSettings::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Camera" << YAML::BeginMap;
    emitter << YAML::Key << "FoV" << YAML::Value << m_fov;
    emitter << YAML::EndMap;
    return true;
}

bool CameraSettings::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& camera = data["Camera"];
    if (!camera || camera.IsNull()) {
        return false;
    }

    m_fov = camera["FoV"].as<float>();
    return true;
}

CameraSettings& CameraSettings::operator=(const CameraSettings& other) {
    m_fov = other.m_fov;
    m_nearClip = other.m_nearClip;
    m_farClip = other.m_farClip;
    return *this;
}

} // namespace scene
} // namespace aderite
