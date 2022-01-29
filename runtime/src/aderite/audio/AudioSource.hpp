#pragma once

#include <fmod_studio.hpp>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"
#include "aderite/io/ISerializable.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Audio source object used to denote a point in the world where audio is emitted from
 */
class AudioSource final : public io::ISerializable {
public:
    AudioSource(scene::GameObject* gObject);
    virtual ~AudioSource();

    /**
     * @brief Update the audio source properties
     * @param delta Delta time of last frame
     */
    void update(float delta);

    /**
     * @brief Set the audio clip of this source
     * @param clip New clip of the audio source
     */
    void setAudioClip(asset::AudioAsset* clip);

    /**
     * @brief Returns the audio clip of this source
     */
    asset::AudioAsset* getAudioClip() const;

    /**
     * @brief Start playing audio
     */
    void start() const;

    /**
     * @brief Stop playing immediately
     */
    void stop() const;

    /**
     * @brief Mutes the source
     */
    void mute();

    /**
     * @brief Unmutes the source
     */
    void unmute();

    /**
     * @brief Returns the volume of the source
     */
    float getVolume() const;

    /**
     * @brief Sets the volume of the source
     * @param volume New volume for this source
     */
    void setVolume(const float volume);

    // Inherited via ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    scene::GameObject* m_gObject = nullptr;
    FMOD::Studio::EventInstance* m_instance = nullptr;
    asset::AudioAsset* m_audioClip = nullptr;
    bool m_muted = false;

    // Properties
    float m_volume = 1.0f;
};

} // namespace audio
} // namespace aderite
