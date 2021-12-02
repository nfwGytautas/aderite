#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/io/SerializableAsset.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Audio source object used to denote a point in the world where audio is emitted from
 */
class AudioSource final : public io::SerializableAsset {
public:
    AudioSource();
    virtual ~AudioSource();

    /**
     * @brief Update audio source
     */
    void update() const;

    /**
     * @brief Creates an audio instance for the specified clip
     * @param clip Clip to create instance for
     * @return AudioInstance object
     */
    AudioInstance* createInstance(const asset::AudioAsset* clip);

    /**
     * @brief Creats a one shot audio instance for the specified clip
     * @param clip Clip to create instance for
     * @return AudioInstance object
     */
    AudioInstance* createOneshot(const asset::AudioAsset* clip);

    /**
     * @brief Mutes the source
     */
    void mute();

    /**
     * @brief Unmutes the source
     */
    void unmute();

    /**
     * @brief Stops all instances in the audio, this will clear oneshots if there were any
     */
    void stop();

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
     * @brief Returns the volume of the source
     */
    float getVolume() const;

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
     * @brief Sets the volume of the source
     * @param volume New volume for this source
     */
    void setVolume(const float volume);

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
    /**
     * @brief Synchronizes source parameters with the instance
     * @param instance Instance to sync
     */
    void syncInstance(AudioInstance* instance) const;

private:
    std::string m_name = "";
    std::vector<AudioInstance*> m_instances;
    std::vector<AudioInstance*> m_oneshots;

    bool m_muted = false;

    // Properties
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::quat m_rotation = glm::quat({1.0f, 0.0f, 0.0f, 0.0f});
    glm::vec3 m_velocity = {0.0f, 0.0f, 0.0f};
    float m_volume = 1.0f;
};

} // namespace audio
} // namespace aderite
