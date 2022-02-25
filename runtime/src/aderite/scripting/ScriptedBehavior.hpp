#pragma once

#include <mono/jit/jit.h>

#include "aderite/io/ISerializable.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Class used to wrap around a scripted behavior component
 */
class ScriptedBehavior final : public io::ISerializable {
public:
    ScriptedBehavior(BehaviorBase* behavior);

    /**
     * @brief Returns the behavior base
     */
    BehaviorBase* getBase() const;

    /**
     * @brief Returns the C# instance of this object
     */
    MonoObject* getInstance() const;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    BehaviorBase* m_behaviorBase = nullptr;
    MonoObject* m_instance = nullptr;
};

} // namespace scripting
} // namespace aderite
