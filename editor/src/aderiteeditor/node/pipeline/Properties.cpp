#include "Properties.hpp"

#include "aderite/utility/Log.hpp"

ADERITE_EDITOR_PIPELINE_NAMESPACE_BEGIN

const char* getTypeName(PropertyType type) {
	switch (type) {
	case PropertyType::Target: {
		return "Target";
	}
	case PropertyType::Entity: {
		return "Entity";
	}
	case PropertyType::Camera: {
		return "Camera";
	}
	case PropertyType::Require: {
		return "Require";
	}
	case PropertyType::Eye: {
		return "Eye";
	}
	default: {
		LOG_ERROR("Unknown property type");
		return "null";
	}
	}
}

bool isArray(const std::string& type) {
	return type[type.length() - 2] == '[' && type[type.length() - 1] == ']';
}

bool isArrayOrType(const std::string& check, const std::string& type) {
	return check == type || check == (type + "[]");
}

ADERITE_EDITOR_PIPELINE_NAMESPACE_END