#pragma once

#include <mono/jit/jit.h>

#include "aderite/io/ISerializable.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Class used to wrap around a scripted behavior component
 */
class ScriptedBehavior final : public io::ISerializable {
public:
    ScriptedBehavior(BehaviorBase* behavior, scene::GameObject* gObject);

    /**
     * @brief Returns the behavior base
     */
    BehaviorBase* getBase() const;

    /**
     * @brief Returns the C# instance of this object
     */
    MonoObject* getInstance() const;

    /**
     * @brief Returns the game object that this script is attached to
     */
    scene::GameObject* getGameObject() const;

    // Inherited via ISerializable
    virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    scene::GameObject* m_gameObject = nullptr;
    BehaviorBase* m_behaviorBase = nullptr;
    MonoObject* m_instance = nullptr;
};

} // namespace scripting
} // namespace aderite
