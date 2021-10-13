#include "EntityCamera.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

ADERITE_SCENE_NAMESPACE_BEGIN

EntityCamera::EntityCamera(Entity on)
	: m_on(on)
{}

void EntityCamera::update(float delta) {
	ADERITE_UNIMPLEMENTED;
}

glm::mat4 EntityCamera::computeViewMatrix() {
	ADERITE_UNIMPLEMENTED;
	return glm::mat4();
}

glm::mat4 EntityCamera::computeProjectionMatrix() {
	ADERITE_UNIMPLEMENTED;
	return glm::mat4();
}

bool EntityCamera::isEnabled() {
	ADERITE_UNIMPLEMENTED;
	return false;
}

bool EntityCamera::serialize(YAML::Emitter& out) {
	ADERITE_UNIMPLEMENTED;
	return false;
}

bool EntityCamera::deserialize(YAML::Node& data) {
	ADERITE_UNIMPLEMENTED;
	return false;
}

ADERITE_SCENE_NAMESPACE_END
