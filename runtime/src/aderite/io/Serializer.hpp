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
    void writeObject(YAML::Emitter& emitter, const SerializableObject* object) const;

private:
    Serializer() {}
    friend Engine;
};

} // namespace io
} // namespace aderite
