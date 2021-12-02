#include "Serializer.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/reflection/Reflector.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

bool Serializer::init() {
    ADERITE_LOG_BLOCK;
    LOG_DEBUG("[Asset] Initializing serializer, current version {0}", c_CurrentVersion);
    LOG_INFO("[Asset] Serializer initialized");
    return true;
}

void Serializer::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Asset] Shutting down serializer, deleting {0} stored objects", m_objects.size());
    for (auto obj : m_objects) {
        if (obj != nullptr) {
            delete obj;
        }
    }
    LOG_INFO("[Asset] Serializer shutdown");
}

SerializableObject* Serializer::parseType(const YAML::Node& data) {
    ADERITE_DYNAMIC_ASSERT(data["Type"], "No type specified in scope");
    ADERITE_DYNAMIC_ASSERT(data["Handle"], "No handle specified in scope");
    ADERITE_DYNAMIC_ASSERT(data["Data"], "No data specified in scope");
    ADERITE_DYNAMIC_ASSERT(data["Name"], "No name specified in scope");

    // Get resolver for type
    reflection::Type type = data["Type"].as<reflection::Type>();
    SerializableHandle handle = data["Handle"].as<SerializableHandle>();
    std::string name = data["Name"].as<std::string>();

    // Create object
    SerializableObject* instance = ::aderite::Engine::getReflector()->reflect<SerializableObject>(type);
    instance->m_handle = handle;
    instance->m_name = name;

    ADERITE_DYNAMIC_ASSERT(instance->getType() == type, "Types don't match between instancer created instance and file stored type");

    // Insert the type and then start deserializing it's contents, this way children can get the type if they need to
    if (instance->getHandle() + 1 > m_objects.size()) {
        // Resize
        m_objects.resize(instance->getHandle() + 1, nullptr);
        m_hasNull = true;
    } else {
        ADERITE_DYNAMIC_ASSERT(m_objects[instance->getHandle()] == nullptr, "Overwriting a already loaded object");
    }

    m_objects[instance->getHandle()] = instance;

    // Deserialize
    instance->deserialize(this, data["Data"]);

    return instance;
}

void Serializer::writeType(YAML::Emitter& emitter, SerializableObject* object) const {
    ADERITE_DYNAMIC_ASSERT(object->getHandle() != c_InvalidHandle, "Invalid handle passed to writeType");

    emitter << YAML::BeginMap;
    emitter << YAML::Key << "Type" << YAML::Value << object->getType();
    emitter << YAML::Key << "Handle" << YAML::Value << object->getHandle();
    emitter << YAML::Key << "Name" << YAML::Value << object->getName();
    emitter << YAML::Key << "Data" << YAML::BeginMap;
    object->serialize(this, emitter);
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
}

ISerializable* Serializer::parseUntrackedType(const YAML::Node& data) {
    ADERITE_DYNAMIC_ASSERT(data["Type"], "No type specified in data scope");
    ADERITE_DYNAMIC_ASSERT(data["Data"], "No data specified in scope");
    ADERITE_DYNAMIC_ASSERT(data["Name"], "No name specified in scope");

    reflection::Type type = data["Type"].as<reflection::Type>();
    std::string name = data["Name"].as<std::string>();

    // Create object
    ISerializable* instance = ::aderite::Engine::getReflector()->reflect<ISerializable>(type);
    instance->m_name = name;

    ADERITE_DYNAMIC_ASSERT(instance->getType() == type, "Types don't match between instancer created instance and file stored type");

    // Deserialize
    instance->deserialize(this, data["Data"]);

    return instance;
}

void Serializer::fillData(NamedSerializable* object, const YAML::Node& data) {
    ADERITE_DYNAMIC_ASSERT(object != nullptr, "Trying to fill nullptr object");

    ADERITE_DYNAMIC_ASSERT(data["Type"], "No type specified in data scope");
    ADERITE_DYNAMIC_ASSERT(data["Data"], "No data specified in scope");

    reflection::Type type = data["Type"].as<reflection::Type>();

    // Check if types match
    ADERITE_DYNAMIC_ASSERT(type == object->getType(), "Type mismatch");

    // Deserialize
    object->deserialize(this, data["Data"]);
}

void Serializer::writeUntrackedType(YAML::Emitter& emitter, const NamedSerializable* object) const {
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "Type" << YAML::Value << object->getType();
    emitter << YAML::Key << "Name" << YAML::Value << object->getName();
    emitter << YAML::Key << "Data" << YAML::BeginMap;
    object->serialize(this, emitter);
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
}

SerializableObject* Serializer::get(SerializableHandle handle) const {
    LOG_TRACE("[Asset] Getting {0}", handle);
    ADERITE_DYNAMIC_ASSERT(handle != c_InvalidHandle, "Invalid handle passed to serializer get method");
    ADERITE_DYNAMIC_ASSERT(m_objects.size() <= handle, "Requested non existing object");
    return m_objects[handle];
}

void Serializer::add(SerializableObject* object) {
    LOG_TRACE("[Asset] Adding {0}", object->getHandle());
    ADERITE_DYNAMIC_ASSERT(object->getHandle() == c_InvalidHandle, "Tried to add an already existing object to Serializer");
    SerializableHandle handle = this->nextAvailableHandle();
    object->m_handle = handle;
    m_objects[handle] = object;
}

void Serializer::remove(SerializableHandle handle) {
    LOG_TRACE("[Asset] Removing {0}", handle);
    ADERITE_DYNAMIC_ASSERT(m_objects.size() >= handle, "Tried to remove non existing object");

    if (m_objects[handle] != nullptr) {
        delete m_objects[handle];
    } else {
        LOG_WARN("[Asset] Tried to delete {0}, but it was already deleted, ignoring", handle);
    }

    m_objects[handle] = nullptr;
    m_hasNull = true;
}

SerializableObject* Serializer::getOrRead(SerializableHandle handle) {
    if (m_objects.size() > handle && m_objects[handle] != nullptr) {
        return m_objects[handle];
    }

    // Resolve path and load
    DataChunk chunk = aderite::Engine::getFileHandler()->openSerializable(handle);
    YAML::Node data = YAML::Load(reinterpret_cast<const char*>(chunk.Data.data()));

    // Check version
    //ADERITE_DYNAMIC_ASSERT(data["Version"], "Tried to read a file without a specified version");

    // TODO: Check version and upgrade if needed
    SerializableObject* type = this->parseType(data);
    return type;
}

void Serializer::reread(SerializableObject* object) {
    // Resolve path and load
    DataChunk chunk = aderite::Engine::getFileHandler()->openSerializable(object->getHandle());
    YAML::Node data = YAML::Load(reinterpret_cast<const char*>(chunk.Data.data()));
    object->deserialize(this, data["Data"]);
}

void Serializer::save(SerializableObject* object) const {
    YAML::Emitter out;

    LOG_TRACE("[Asset] Saving {0}", object->getHandle());

    ADERITE_DYNAMIC_ASSERT(object->getHandle() != c_InvalidHandle, "SerializableObject with invalid handle passed");

    // Common
    this->writeType(out, object);

    // Resolve where to store this object
    LOG_TRACE("[Asset] Emitting {0} to DataChunk", object->getHandle());
    DataChunk chunk = aderite::Engine::getFileHandler()->openSerializable(object->getHandle());
    chunk.Data.resize(out.size());
    std::memcpy(chunk.Data.data(), out.c_str(), chunk.Data.size());
    aderite::Engine::getFileHandler()->commit(chunk);
}

void Serializer::saveAll() const {
    LOG_TRACE("[Asset] Saving all objects");
    for (SerializableObject* obj : m_objects) {
        if (obj != nullptr) {
            this->save(obj);
        }
    }
    LOG_INFO("[Asset] All objects saved");
}

SerializableHandle Serializer::nextAvailableHandle() {
    LOG_TRACE("[Asset] Querying next available handle");
    if (!m_hasNull) {
        LOG_TRACE("[Asset] No null handles, adding new object");
        m_objects.push_back(nullptr);
        return m_objects.size() - 1;
    }

    // Find next nullptr
    for (size_t i = 0; i < m_objects.size(); i++) {
        if (m_objects[i] == nullptr) {
            LOG_TRACE("[Asset] Found null handle at {0}", i);
            return i;
        }
    }

    // Was a false positive null flag
    LOG_WARN("[Asset] False positive null flag, creating new object");
    m_hasNull = false;
    m_objects.push_back(nullptr);
    return m_objects.size() - 1;
}

} // namespace io
} // namespace aderite
