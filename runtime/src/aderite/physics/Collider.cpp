#include "Collider.hpp"

#include "aderite/utility/YAML.hpp"

ADERITE_PHYSICS_NAMESPACE_BEGIN

bool Collider::isTrigger() const {
	return p_isTrigger;
}

bool Collider::serialize(YAML::Emitter& out) {
	// TODO: Error check
	out << YAML::Key << "IsTrigger" << YAML::Value << p_isTrigger;
	return true;
}

bool Collider::deserialize(YAML::Node& data) {
	p_isTrigger = data["IsTrigger"].as<bool>();
	return true;
}

ADERITE_PHYSICS_NAMESPACE_END
