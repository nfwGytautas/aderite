#pragma once

#include <string>

#include "aderite/audio/AudioSource.hpp"

namespace aderite {
namespace audio {

/**
 * @brief Extension to runtime AudioSource class, to provide name functionality
 */
class EditorAudioSource : public AudioSource {
public:
    EditorAudioSource();

    /**
     * @brief Returns the name of the source
    */
    std::string getName() const;

    /**
     * @brief Set the name of the source
     * @param name Name of the source
    */
    void setName(const std::string& name);

    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    std::string m_name = "";
};

} // namespace audio
} // namespace aderite
