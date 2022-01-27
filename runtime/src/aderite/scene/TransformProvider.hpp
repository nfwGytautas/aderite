#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "aderite/io/ISerializable.hpp"

namespace aderite {
namespace scene {

/**
 * @brief TransformProvider is an interface for objects that have transform information
 */
class TransformProvider : public io::ISerializable {
public:
    virtual ~TransformProvider() = default;

    virtual const glm::vec3& getPosition() const = 0;
    virtual const glm::quat& getRotation() const = 0;
    virtual const glm::vec3& getScale() const = 0;

    virtual void setPosition(const glm::vec3& position) = 0;
    virtual void setRotation(const glm::quat& rotation) = 0;
    virtual void setScale(const glm::vec3& scale) = 0;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
};

} // namespace scene
} // namespace aderite
