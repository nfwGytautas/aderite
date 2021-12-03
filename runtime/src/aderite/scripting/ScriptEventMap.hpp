#pragma once

#include <vector>

#include "aderite/io/ISerializable.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief ScriptEventMap class is used to establish a connection between scriptable C# objects and aderite engine events such as input key
 * press or timed events such as update
 */
class ScriptEventMap : public io::ISerializable {
    // Generic function
public:
    virtual ~ScriptEventMap() = default;

protected:
    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    // Input events
public:
};

} // namespace scripting
} // namespace aderite
