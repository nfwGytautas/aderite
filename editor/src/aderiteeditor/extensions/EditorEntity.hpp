#pragma once

#include "aderite/scene/Entity.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Extension for editor entity
 */
class EditorEntity : public Entity {
public:
    EditorEntity();

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

} // namespace scene
} // namespace aderite
