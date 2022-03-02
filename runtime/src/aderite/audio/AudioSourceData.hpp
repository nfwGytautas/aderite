#pragma once

#include "aderite/asset/Forward.hpp"
#include "aderite/io/ISerializable.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Data of the audio source
 */
class AudioSourceData final : public io::ISerializable {
public:
    virtual ~AudioSourceData();

    /**
     * @brief Mutes the source
     */
    void mute();

    /**
     * @brief Unmutes the source
     */
    void unmute();

    /**
     * @brief Returns true if the source is muted, false otherwise
     */
    bool isMuted() const;

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

    AudioSourceData& operator=(const AudioSourceData& other);

private:
    bool m_muted = false;
    float m_volume = 1.0f;
};

} // namespace audio
} // namespace aderite
