#include "Serializer.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/SerializableAsset.hpp"
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
    LOG_TRACE("[Asset] Shutting down serializer, deleting stored objects");
    LOG_INFO("[Asset] Serializer shutdown");
}

SerializableObject* Serializer::parseObject(const YAML::Node& data) {
    // Ensure that data is valid
    ADERITE_DYNAMIC_ASSERT(data["Type"], "No type specified in data scope");
    ADERITE_DYNAMIC_ASSERT(data["Data"], "No data specified in scope");
    ADERITE_DYNAMIC_ASSERT(data["Name"], "No name specified in scope");

    // Create object instance
    reflection::Type type = data["Type"].as<reflection::Type>();
    SerializableObject* instance = ::aderite::Engine::getReflector()->reflect<SerializableObject>(type);

    // Set data
    instance->m_name = data["Name"].as<std::string>();

    // Deserialize
    instance->deserialize(this, data["Data"]);

    return instance;
}

void Serializer::writeObject(YAML::Emitter& emitter, const SerializableObject* object) const {
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "Type" << YAML::Value << object->getType();
    emitter << YAML::Key << "Name" << YAML::Value << object->getName();
    emitter << YAML::Key << "Data" << YAML::BeginMap;
    object->serialize(this, emitter);
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
}

} // namespace io
} // namespace aderite
