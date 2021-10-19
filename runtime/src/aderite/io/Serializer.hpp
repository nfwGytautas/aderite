#pragma once

/**
 * @brief This file defines the Serializer class. Serializer is responsible for handling
 * all META data io operations.
 * 
 * The serializer works in a pretty simple way and utilizes a mini reflection system.
 * Every serializable file (for this example instance yaml files are used, but for most compiled projects)
 * a binary format will be used.
 * 
 * The file starts with a version definition, then the Type and Name specifier
 * Version: xxxxxx
 * Type: 0
 * Name: nameOfObject
 * 
 * Then a data section, this section is passed to the object
 * Data: {}
 * 
 * Instancers are used to resolve Type to instance of an object, because of this every serializable object
 * needs to have an empty constructor implementation.
 * 
 * In the event that the user has nested objects e.g. :
 * 
 * Version: xxxxx
 * Type: 0
 * Name: example1
 * Data:
 *	- SomeData: 0
 *	  Objects:
 *	   - Type: 1
 *       Name: example2
 *		 Data: {}
 *	   - Type 2
 *       Name: example3
 *       Data: {}
 * 
 * The serializer can be used to parse a node by calling Serializer::parseType(const YAML::Node& node)
 * Where the Node would be Objects[idx] item, and the return will be a SerializableObject, which can be casted
 * to necessary type or used as is.
*/

#include <filesystem>
#include <vector>
#include <map>
#include <yaml-cpp/yaml.h>
#include "aderite/utility/Macros.hpp" 
#include "aderite/io/Forward.hpp"
#include "aderite/io/Versions.hpp"

namespace aderite {
namespace io {

/**
 * @brief Serializer class that is responsible for serializing and deserializing asset meta objects
*/
class Serializer final {
public:
	/**
	 * @brief Initializes the serializer
	 * @return True if initialized without error, false otherwise
	*/
	bool init();

	/**
	 * @brief Shutdown serializer
	*/
	void shutdown();

	/**
	 * @brief Links a type to an instancer, if a type already has a linked instancer, then
	 * the instancer is overridden and a DEBUG message is logged
	 * @param type Type to link
	 * @param instancer Instancer instance to use (takes ownership)
	*/
	void linkInstancer(SerializableType type, InstancerBase* instancer);

	/**
	 * @brief Parses the type inside the current data scope and returns it
	 * @param data Data node, must have Type and Handle keys
	 * @return SerializableObject instance
	*/
	SerializableObject* parseType(const YAML::Node& data) const;

	/**
	 * @brief Write an object to the specified emitter
	 * @param emitter Emitter to write to
	 * @param object Object to write
	*/
	void writeType(YAML::Emitter& emitter, SerializableObject* object) const;

	/**
	 * @brief Parses the type inside the current data scope, but DOES NOT track the object, meaning
	 * the user is responsible for calling delete on it and this object cannot be received through the 
	 * serializer. This is mainly used when having nested objects who are managed by their parents, example
	 * editor graphs. This method allows the user to interact with the mini reflection system that serializer
	 * uses internally.
	 * @param data Data node, must have Type key
	 * @return ISerializable instance
	*/
	ISerializable* parseUntrackedType(const YAML::Node& data) const;

	/**
	 * @brief Write an untracked object to the specified emitter
	 * @param emitter Emitter to write to
	 * @param object Object to write
	*/
	void writeUntrackedType(YAML::Emitter& emitter, ISerializable* object) const;

	/**
	 * @brief Returns object associated with the serializable handle
	 * @param handle Handle of the object
	 * @return Serializable object instance
	*/
	SerializableObject* get(SerializableHandle handle);

	/**
	 * @brief Adds an object to the Serializer look up table, if already existing object is added
	 * an assert will be triggered.
	 * @param object Object to add
	*/
	void add(SerializableObject* object);

	/**
	 * @brief Removes an object with the specified handle, this frees the object and any references to it
	 * are then undefined behavior
	 * @param handle Handle of object to remove
	*/
	void remove(SerializableHandle handle);

	/**
	 * @brief Get or read a serializable object with given handle (a resolver must have been set for this to work)
	 * @param handle Handle of the object
	 * @return SerializableObject instance
	*/
	SerializableObject* getOrRead(SerializableHandle handle);

	/**
	 * @brief Rereads the meta data of the specified object
	 * @param object Object whose meta data to reread
	*/
	void reread(SerializableObject* object);

	/**
	 * @brief Serializes object into a file
	 * @param object Object to serialize
	*/
	void save(SerializableObject* object);

	auto begin() {
		return m_objects.begin();
	}

	auto begin() const {
		return m_objects.begin();
	}

	auto end() {
		return m_objects.end();
	}

	auto end() const {
		return m_objects.end();
	}

	ADERITE_DEBUG_SECTION(
		/**
		 * @brief Debug functionality to print what type is linked to what instancer
		*/
		void printInstancers();
	);
private:
	Serializer() {}
	friend class Engine;
private:
	/**
	 * @brief Resolves an instancer for a type, asserts if a type doesn't have a type
	 * @param type Type to get instancer for
	 * @return Instancer instance
	*/
	InstancerBase* resolveInstancer(SerializableType type) const;

	/**
	 * @brief Returns the next available handle for an object, expanding the object vector as needed
	 * @return Next available handle
	*/
	SerializableHandle nextAvailableHandle();
private:
	std::map<SerializableType, InstancerBase*> m_instancers;
	std::vector<SerializableObject*> m_objects;
	bool m_hasNull = false;
};

}
}
