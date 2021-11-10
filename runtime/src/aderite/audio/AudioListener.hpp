#pragma once

#include <vector>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Audio listener object used to denote a point in the world where audio is heard from
 */
class AudioListener : public io::ISerializable {
public:
    AudioListener();
    virtual ~AudioListener();

    /**
     * @brief Update audio source
     */
    void update();

    /**
     * @brief Disables listener
     */
    void disable();

    /**
     * @brief Enables listener
     */
    void enable();

    /**
     * @brief Returns true if the listener is enabled, false otherwise
     */
    bool isEnabled() const {
        return m_enabled;
    }

    /**
     * @brief Set the name of the source
     * @param name Name of the source
     */
    void setName(const std::string& name);

    /**
     * @brief Returns the name of the source
     */
    std::string getName() const {
        return m_name;
    }

    /**
     * @brief Returns the position of the source
     */
    glm::vec3 getPosition() const {
        return m_position;
    }

    /**
     * @brief Returns the rotation of the source
     */
    glm::quat getRotation() const {
        return m_rotation;
    }

    /**
     * @brief Returns the velocity of the source
     */
    glm::vec3 getVelocity() const {
        return m_velocity;
    }

    /**
     * @brief Sets the position of the source
     * @param position New position for this source
     */
    void setPosition(const glm::vec3& position);

    /**
     * @brief Sets the rotation of the source
     * @param rotation New rotation for this source
     */
    void setRotation(const glm::quat& rotation);

    /**
     * @brief Sets the velocity of the source
     * @param velocity New velocity for this source
     */
    void setVelocity(const glm::vec3& velocity);

    // Inherited via ISerializable
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    std::string m_name = "";

    bool m_enabled = false;

    // Properties
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::quat m_rotation = glm::quat({1.0f, 0.0f, 0.0f, 0.0f});
    glm::vec3 m_velocity = {0.0f, 0.0f, 0.0f};
};

} // namespace audio
} // namespace aderite
