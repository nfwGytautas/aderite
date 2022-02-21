#pragma once

#include <mono/jit/jit.h>

#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Class used to wrap around a scripted behavior component
 */
class ScriptedBehavior final {
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

private:
    BehaviorBase* m_behaviorBase = nullptr;
    MonoObject* m_instance = nullptr;
};

} // namespace scripting
} // namespace aderite
