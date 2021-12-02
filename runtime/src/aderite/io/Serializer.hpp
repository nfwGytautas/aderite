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

#include <vector>

#include <yaml-cpp/yaml.h>

#include "aderite/io/Forward.hpp"
#include "aderite/io/Versions.hpp"

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
     * @brief Parses the object inside the current data scope and returns it
     * @param data Data node, must have Type, Name keys
     * @return SerializableObject instance
     */
    SerializableObject* parseObject(const YAML::Node& data);

    /**
     * @brief Write an object to the specified emitter
     * @param emitter Emitter to write to
     * @param object Object to write
     */
    void writeObject(YAML::Emitter& emitter, SerializableObject* object) const;

    /**
     * @brief Parses the asset inside the current data scope and returns it
     * @param data Data node, must have Type, Name and Handle keys
     * @return SerializableAsset instance
     */
    SerializableAsset* parseAsset(const YAML::Node& data);

    /**
     * @brief Write an asset to the specified emitter
     * @param emitter Emitter to write to
     * @param asset Asset to write
     */
    void writeAsset(YAML::Emitter& emitter, SerializableAsset* asset) const;

    /**
     * @brief Returns object associated with the serializable handle
     * @param handle Handle of the object
     * @return Serializable object instance
     */
    SerializableAsset* get(SerializableHandle handle) const;

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
     */
    void save(SerializableObject* object) const;

    /**
     * @brief Utility method for saving all objects
     */
    void saveAll() const;

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
     * @brief Returns the next available handle for an asset, expanding the asset vector as needed
     * @return Next available handle
     */
    SerializableHandle nextAvailableHandle();

private:
    std::vector<SerializableAsset*> m_objects;
    bool m_hasNull = false;
};

} // namespace io
} // namespace aderite
