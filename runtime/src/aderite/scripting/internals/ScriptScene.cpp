#include "ScriptScene.hpp"

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

//MonoString* entityGetName(scene::Entity* entity) {
//    return ::aderite::Engine::getScriptManager()->string(entity->getName().c_str());
//}

//void entityDestroy(scene::Entity* entity) {
//    entity->markForDeletion();
//}

void sceneInternals() {
    // Entity
    /*mono_add_internal_call("Aderite.Entity::__GetName(intptr)", reinterpret_cast<void*>(entityGetName));
    mono_add_internal_call("Aderite.Entity::__Destroy(intptr)", reinterpret_cast<void*>(entityDestroy));*/
}

} // namespace scripting
} // namespace aderite
