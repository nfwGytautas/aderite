#pragma once

/**
 * @brief This file defines the Serializer class. Serializer is responsible for handling
 * all META data io operations.
 * 
 * The serializer works in a pretty simple way and utilizes a mini reflection system.
 * Every serializable file (for this instance yaml files are used, but for most production products)
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
class Serializer {
	using SerializableMapKey = SerializableHandle;
	using SerializableMapVal = SerializableObject*;
	using SerializableMap = std::map<SerializableMapKey, SerializableMapVal>;
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
	 * @brief Set the resolver of the serializer, caller remains the owner of the instance
	 * @param resolver Resolver instance
	*/
	void setResolver(PathResolver* resolver);

	/**
	 * @brief Links a type to an instancer, if a type already has a linked instancer, then
	 * the instancer is overridden and a INFO message is logged
	 * @param type Type to link
	 * @param instancer Instancer instance to use (takes ownership)
	*/
	void linkInstancer(SerializableType type, InstancerBase* instancer);

	/**
	 * @brief Parses the type inside the current data scope and returns it
	 * @param data Data node, must have Type and Handle keys
	 * @return SerializableObject instance or nullptr
	*/
	SerializableObject* parseType(const YAML::Node& data);

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
	 * @brief Serializes object into a file
	 * @param object Object to serialize
	 * @param file File where to store result
	*/
	void save(SerializableObject* object, std::filesystem::path file);

	auto begin() {
		return m_serializables.begin();
	}

	auto begin() const {
		return m_serializables.begin();
	}

	auto end() {
		return m_serializables.end();
	}

	auto end() const {
		return m_serializables.end();
	}

	ADERITE_DEBUG_SECTION(
		/**
		 * @brief Debug functionality to print what type is linked to what instancer
		*/
		void printInstancers();
	);

	ADERITE_MIDDLEWARE_SECTION(
		/**
		 * @brief Sets the next available handle, done once when loading a project from middleware
		*/
		void setNextId(SerializableHandle handle);
	);
private:
	Serializer() {}
	friend class Engine;
private:
	/**
	 * @brief Register all runtime serializables and their instancers
	*/
	void registerRuntimeTypes();

	/**
	 * @brief Resolves an instancer for a type, asserts if a type doesn't have a type
	 * @param type Type to get instancer for
	 * @return Instancer instance
	*/
	InstancerBase* resolveInstancer(SerializableType type);

	/**
	 * @brief Use the resolver to resolve the path to a serializable
	 * @param handle Handle to resolve
	 * @return Path
	*/
	Path resolvePath(SerializableHandle handle);
private:
	std::map<SerializableType, InstancerBase*> m_instancers;
	SerializableMap m_serializables;

	SerializableHandle m_nextAvailableHandle = {};
	PathResolver* m_resolver = nullptr;
};

}
}
