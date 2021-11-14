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
    virtual void load(const io::Loader* loader) override;
    virtual void unload() override;
    virtual bool needsLoading() override;

    // Inherited via SerializableObject
    virtual reflection::Type getType() const override;
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    std::string m_name = "";
};

} // namespace asset
} // namespace aderite
