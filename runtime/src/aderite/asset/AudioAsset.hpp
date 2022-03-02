#pragma once

#include "aderite/io/SerializableAsset.hpp"
#include "aderite/audio/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Class representing an audio asset with it's own properties
 */
class AudioAsset final : public io::SerializableAsset {
public:
    ~AudioAsset();

    /**
     * @brief Set the name of the audio event
     * @param name Name of the event
     */
    void setEventName(const std::string& name);

    /**
     * @brief Returns the name of the audio event
     */
    std::string getEventName() const;

    // Inherited via SerializableAsset
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    std::string m_eventName = "";
};

} // namespace asset
} // namespace aderite
