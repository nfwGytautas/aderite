#pragma once

#include "aderite/io/ISerializable.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Data of audio listeners
 */
class AudioListenerData final : public io::ISerializable {
public:
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

    // Inherited via ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    AudioListenerData& operator=(const AudioListenerData& other);

private:
    bool m_enabled = false;
};

} // namespace audio
} // namespace aderite
