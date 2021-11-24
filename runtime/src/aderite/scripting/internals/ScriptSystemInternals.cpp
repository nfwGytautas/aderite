#include "ScriptSystemInternals.hpp"

#include <glm/glm.hpp>
#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/PrefabAsset.hpp"
#include "aderite/physics/PhysicsActor.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Transform.hpp"
#include "aderite/scripting/LibClassLocator.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

MonoObject* InstantiateEntity(scene::Entity* entity, glm::vec3 origin) {
    scene::Entity* clone = entity->clone();
    if (clone->getActor() != nullptr) {
        clone->getActor()->moveActor(origin);
    } else {
        clone->getTransform()->position() = origin;
    }

    return ::aderite::Engine::getScriptManager()->getLocator().create(clone);
}

MonoObject* InstantiatePrefab(asset::PrefabAsset* prefab, glm::vec3 origin) {
    scene::Entity* clone = prefab->instantiate(::aderite::Engine::getSceneManager()->getCurrentScene());
    if (clone->getActor() != nullptr) {
        clone->getActor()->moveActor(origin);
    } else {
        clone->getTransform()->position() = origin;
    }

    return ::aderite::Engine::getScriptManager()->getLocator().create(clone);
}

void DestroyEntity(scene::Entity* entity) {
    entity->setScene(nullptr);
}

void systemInternals() {
    mono_add_internal_call("Aderite.ScriptSystem::__InstantiateEntity(intptr,Aderite.Vector3)", reinterpret_cast<void*>(InstantiateEntity));
    mono_add_internal_call("Aderite.ScriptSystem::__InstantiatePrefab(intptr,Aderite.Vector3)", reinterpret_cast<void*>(InstantiatePrefab));
    mono_add_internal_call("Aderite.ScriptSystem::__Destroy(intptr)", reinterpret_cast<void*>(DestroyEntity));
}

} // namespace scripting
} // namespace aderite
