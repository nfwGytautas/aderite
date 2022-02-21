#include "ScriptedBehavior.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/scripting/BehaviorBase.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

ScriptedBehavior::ScriptedBehavior(BehaviorBase* behavior) : m_behaviorBase(behavior) {
    m_instance = ::aderite::Engine::getScriptManager()->instantiate(m_behaviorBase->getClass());
}

BehaviorBase* ScriptedBehavior::getBase() const {
    return m_behaviorBase;
}

MonoObject* ScriptedBehavior::getInstance() const {
    return m_instance;
}

} // namespace scripting
} // namespace aderite
