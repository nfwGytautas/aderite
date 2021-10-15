#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief Base class for all serializable objects
*/
class SerializableObject {
public:
	virtual ~SerializableObject() {}

	/**
	 * @brief Returns the handle of this serializable object
	 * @return Object handle
	*/
	SerializableHandle getHandle() const;

	/**
	 * @brief Returns the name of the object
	 * @return Name of the object
	*/
	virtual const char* getName() = 0;

	/**
	 * @brief The type of the serializable object
	 * @return SerializableType value
	*/
	virtual SerializableType getType() = 0;

	/**
	 * @brief Serialize object to emmitter
	 * @param serializer Serializer instance
	 * @param emitter Emmitter to serialize to
	 * @return True if serialized without errors, false otherwise
	*/
	virtual bool serialize(const Serializer* serializer, YAML::Emitter& emitter) = 0;

	/**
	 * @brief Deserialize object from data node
	 * @param serializer Serializer instance
	 * @param data Data node
	 * @return True if deserialized, false otherwise
	*/
	virtual bool deserialize(const Serializer* serializer, const YAML::Node& data) = 0;
public:
	friend class Serializer; // Used to set the handle
private:
	SerializableHandle m_handle = c_InvalidHandle;
};

}
}
