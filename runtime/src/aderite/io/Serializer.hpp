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
#include <map>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "aderite/io/Forward.hpp"
#include "aderite/io/Versions.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
class Engine;

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
     * @brief Parses the type inside the current data scope and returns it
     * @param data Data node, must have Type and Handle keys
     * @return SerializableObject instance
     */
    SerializableObject* parseType(const YAML::Node& data);

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
    ISerializable* parseUntrackedType(const YAML::Node& data);

    /**
     * @brief Fills the specified object with data from the node. This method is similar to parseUntrackedType, but is used when an instance
     * already exists and should just be filled with data form disk
     * @param object Object to fill
     * @param data Data to fill with
     */
    void fillData(ISerializable* object, const YAML::Node& data);

    /**
     * @brief Write an untracked object to the specified emitter
     * @param emitter Emitter to write to
     * @param object Object to write
     */
    void writeUntrackedType(YAML::Emitter& emitter, const ISerializable* object) const;

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

    /**
     * @brief Utility method for saving all objects
     */
    void saveAll();

    /**
     * @brief Allows to pass any data to subsequent callers
     * @param data Data to be passed
     */
    void setData(void* data);

    /**
     * @brief Returns the argument of setData
     */
    void* getData() const;

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

private:
    Serializer() {}
    friend Engine;

private:
    /**
     * @brief Returns the next available handle for an object, expanding the object vector as needed
     * @return Next available handle
     */
    SerializableHandle nextAvailableHandle();

private:
    std::vector<SerializableObject*> m_objects;
    bool m_hasNull = false;
    void* m_data = nullptr;
};

} // namespace io
} // namespace aderite
