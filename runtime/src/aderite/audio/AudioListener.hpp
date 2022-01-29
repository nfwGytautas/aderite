#pragma once

#include <string>

#include "aderite/io/ISerializable.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Audio listener object used to denote a point in the world where audio is heard from
 */
class AudioListener final : public io::ISerializable {
public:
    AudioListener(scene::GameObject* gObject);
    virtual ~AudioListener();

    /**
     * @brief Update the audio listener properties
     * @param delta Delta time of last frame
     */
    void update(float delta);

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

private:
    scene::GameObject* m_gObject = nullptr;

    // Properties
    bool m_enabled = false;
};

} // namespace audio
} // namespace aderite
