#pragma once

#include "aderite/io/SerializableObject.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/ITransformProvider.h"

namespace aderite {
namespace scene {

/**
 * @brief Visual class is used to define an object in a scene that is just that a visual. These objects only have a mesh, material and
 * transform information. They are used to detail the world
 */
class Visual final : public io::SerializableObject, public ITransformProvider, public rendering::Renderable {
public:
    Visual();

    // Inherited via SerializableObject
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    // Inherited via ITransformProvider
    virtual const glm::vec3& getPosition() const override;
    virtual const glm::quat& getRotation() const override;
    virtual const glm::vec3& getScale() const override;
    virtual void setPosition(const glm::vec3& position) override;
    virtual void setRotation(const glm::quat& rotation) override;
    virtual void setScale(const glm::vec3& scale) override;

private:
    // Transform
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::quat m_rotation = glm::quat({1.0f, 0.0f, 0.0f, 0.0f});
    glm::vec3 m_scale = {1.0f, 1.0f, 1.0f};
};

} // namespace scene
} // namespace aderite
