#include "ScriptComponents.hpp"

#include <mono/jit/jit.h>

#include "aderite/scene/Transform.hpp"

namespace aderite {
namespace scripting {

glm::vec3 GetPosition(scene::Transform* transform) {
    return transform->position();
}

glm::quat GetRotation(scene::Transform* transform) {
    return transform->rotation();
}

glm::vec3 GetScale(scene::Transform* transform) {
    return transform->scale();
}

void SetPosition(scene::Transform* transform, glm::vec3 position) {
    transform->position() = position;
}

void SetRotation(scene::Transform* transform, glm::quat rotation) {
    transform->rotation() = rotation;
}

void SetScale(scene::Transform* transform, glm::vec3 scale) {
    transform->scale() = scale;
}

void componentInternals() {
    // Transform
    mono_add_internal_call("Aderite.Transform::__GetPosition(intptr)", reinterpret_cast<void*>(GetPosition));
    mono_add_internal_call("Aderite.Transform::__GetRotation(intptr)", reinterpret_cast<void*>(GetRotation));
    mono_add_internal_call("Aderite.Transform::__GetScale(intptr)", reinterpret_cast<void*>(GetScale));

    mono_add_internal_call("Aderite.Transform::__SetPosition(intptr,Aderite.Vector3)", reinterpret_cast<void*>(SetPosition));
    mono_add_internal_call("Aderite.Transform::__SetRotation(intptr,Aderite.Quaternion)", reinterpret_cast<void*>(SetRotation));
    mono_add_internal_call("Aderite.Transform::__SetScale(intptr,Aderite.Vector3)", reinterpret_cast<void*>(SetScale));
}

} // namespace scripting
} // namespace aderite
