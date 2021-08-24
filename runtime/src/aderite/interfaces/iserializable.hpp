#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

#include "aderite/utility/macros.hpp"

ADERITE_INTERFACE_NAMESPACE_BEGIN

/**
 * @brief Interface for serializing objects
*/
class ISerializable {
public:
	virtual ~ISerializable() {}

	/**
	 * @brief Serialize object to the specified YAML emitter
	 * @param out YAML emitter to output into
	 * @return True if serialized without errors, false otherwise
	*/
	virtual bool serialize(YAML::Emitter& out) = 0;

	/**
	 * @brief Deserialize object from specified YAML node
	 * @param data YAML data node
	 * @return True if deserialized without errors, false otherwise
	*/
	virtual bool deserialize(YAML::Node& data) = 0;
};

ADERITE_INTERFACE_NAMESPACE_END
