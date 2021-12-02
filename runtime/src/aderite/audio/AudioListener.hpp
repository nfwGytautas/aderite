#pragma once

#include <string>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/io/SerializableAsset.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Audio listener object used to denote a point in the world where audio is heard from
 */
class AudioListener final : public io::SerializableAsset {
public:
    AudioListener();
    virtual ~AudioListener();

    /**
     * @brief Update audio source
     */
    void update() const;

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
    bool isEnabled() const;

    /**
     * @brief Set the name of the source
     * @param name Name of the source
     */
    void setName(const std::string& name);

    /**
     * @brief Returns the name of the source
     */
    std::string getName() const;

    /**
     * @brief Returns the position of the source
     */
    glm::vec3 getPosition() const;

    /**
     * @brief Returns the rotation of the source
     */
    glm::quat getRotation() const;

    /**
     * @brief Returns the velocity of the source
     */
    glm::vec3 getVelocity() const;

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

    // Inherited via SerializableAsset
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

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
