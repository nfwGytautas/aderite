#include "ScriptComponents.hpp"

#include <mono/jit/jit.h>
#include "aderite/utility/Log.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/components/Components.hpp"

namespace aderite {
namespace scripting {

scene::components::TransformComponent* GetTransform(scene::Scene* scene, entt::entity entity) {
	return &scene::Entity(entity, scene).getComponent<scene::components::TransformComponent>();
}

glm::vec3 GetPosition(scene::components::TransformComponent* transform) {
	return transform->Position;
}

glm::quat GetRotation(scene::components::TransformComponent* transform) {
	return transform->Rotation;
}

glm::vec3 GetScale(scene::components::TransformComponent* transform) {
	return transform->Scale;
}

void SetPosition(scene::components::TransformComponent* transform, glm::vec3 position) {
	transform->Position = position;
}

void SetRotation(scene::components::TransformComponent* transform, glm::quat rotation) {
	transform->Rotation = rotation;
}

void SetScale(scene::components::TransformComponent* transform, glm::vec3 scale) {
	transform->Scale = scale;
}

void componentInternals() {
	mono_add_internal_call("Aderite.Transform::__GetInstance(intptr,uintptr)", GetTransform);

	mono_add_internal_call("Aderite.Transform::__GetPosition(intptr)", GetPosition);
	mono_add_internal_call("Aderite.Transform::__GetRotation(intptr)", GetRotation);
	mono_add_internal_call("Aderite.Transform::__GetScale(intptr)", GetScale);

	mono_add_internal_call("Aderite.Transform::__SetPosition(intptr,Aderite.Vector3)", SetPosition);
	mono_add_internal_call("Aderite.Transform::__SetRotation(intptr,Aderite.Quaternion)", SetRotation);
	mono_add_internal_call("Aderite.Transform::__SetScale(intptr,Aderite.Vector3)", SetScale);
}

}
}
