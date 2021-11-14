#pragma once

#include "aderite/io/ILoadable.hpp"
#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Class representing an audio asset with it's own properties
 */
class AudioAsset final : public io::SerializableObject, public io::ILoadable {
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

    // Inherited via ILoadable
    void load(const io::Loader* loader) override;
    void unload() override;
    bool needsLoading() const override;

    // Inherited via SerializableObject
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    std::string m_name = "";
};

} // namespace asset
} // namespace aderite
