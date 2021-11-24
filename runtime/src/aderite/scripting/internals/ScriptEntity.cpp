#include "ScriptEntity.hpp"

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

MonoString* GetEntityName(scene::Entity* entity) {
    return ::aderite::Engine::getScriptManager()->string(entity->getName().c_str());
}

void entityInternals() {
    mono_add_internal_call("Aderite.Entity::__GetEntityName(intptr)", reinterpret_cast<void*>(GetEntityName));
}

} // namespace scripting
} // namespace aderite
