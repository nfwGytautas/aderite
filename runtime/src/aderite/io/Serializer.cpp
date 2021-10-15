#include "Serializer.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/PathResolver.hpp"
#include "aderite/io/Instancer.hpp"

namespace aderite {
namespace io {

bool Serializer::init() {
	LOG_TRACE("Initializing serializer, current version {0}", c_CurrentVersion);

	// Runtime instancers
	LOG_TRACE("Setting runtime instancers");
	this->registerRuntimeTypes();

	return true;
}

void Serializer::shutdown() {
	for (auto obj : m_serializables) {
		delete obj.second;
	}

	for (auto instancer : m_instancers) {
		delete instancer.second;
	}
}

void Serializer::setResolver(PathResolver* resolver) {
	m_resolver = resolver;
}

void Serializer::linkInstancer(SerializableType type, InstancerBase* instancer) {
	auto it = m_instancers.find(type);
	if (it != m_instancers.end()) {
		// Already has mapped instancer
		LOG_DEBUG("Instancer for type {0} has been overridden");

		delete it->second;
		it->second = instancer;

		return;
	}

	m_instancers.insert_or_assign(type, instancer);
}

SerializableObject* Serializer::parseType(const YAML::Node& data) {
	ADERITE_DYNAMIC_ASSERT(data["Type"], "No type specified in data scope");
	ADERITE_DYNAMIC_ASSERT(data["Handle"], "No handle specified in data scope");

	// Get resolver for type
	SerializableType type = data["Type"].as<SerializableType>();
	SerializableHandle handle = data["Handle"].as<SerializableHandle>();

	// Get instancer
	InstancerBase* instancer = resolveInstancer(type);

	// Create object
	SerializableObject* instance = instancer->create();
	instance->m_handle = handle;

	ADERITE_DYNAMIC_ASSERT(instance->getType() == type, "Types don't match between instancer created instance and file stored type");

	// Deserialize
	instance->deserialize(this, data);

	return instance;
}

SerializableObject* Serializer::get(SerializableHandle handle) {
	if (handle == c_InvalidHandle) {
		LOG_WARN("Invalid handled passed to Serializer::get");
		return nullptr;
	}

	auto it = m_serializables.find(handle);
	if (it == m_serializables.end()) {
		return nullptr;
	}

	return it->second;
}

void Serializer::add(SerializableObject* object) {
	ADERITE_DYNAMIC_ASSERT(object->getHandle() == c_InvalidHandle, "Tried to add an already existing object to Serializer");
	object->m_handle = m_nextAvailableHandle++;
	m_serializables.insert_or_assign(object->m_handle, object);
}

void Serializer::remove(SerializableHandle handle) {
	auto it = m_serializables.find(handle);
	ADERITE_DYNAMIC_ASSERT(it != m_serializables.end(), "Tried to remove non existing object");
	it->second->m_handle = c_InvalidHandle;
	delete it->second;
	m_serializables.erase(it);
}

SerializableObject* Serializer::getOrRead(SerializableHandle handle) {
	SerializableObject* obj = this->get(handle);
	if (obj != nullptr) {
		return obj;
	}

	// Resolve path and load
	Path path = this->resolvePath(handle);
	YAML::Node data = YAML::LoadFile(path.File.string());

	// Check version
	ADERITE_DYNAMIC_ASSERT(data["Version"], "Tried to read a file without a specified version");

	// TODO: Check version and upgrade if needed
	SerializableObject* type = this->parseType(data);
	m_serializables[type->getHandle()] = type;
	return type;
}

void Serializer::save(SerializableObject* object, std::filesystem::path file) {

}

ADERITE_DEBUG_SECTION(
	void Serializer::printInstancers() {
		LOG_TRACE("");
		LOG_TRACE("====================================================================================");
		LOG_TRACE("                                    INSTANCERS                                      ");
		LOG_TRACE("====================================================================================");
		for (auto i : m_instancers) {
			if (i.second != nullptr) {
				SerializableObject* temp = nullptr;
				temp = i.second->create();
				LOG_TRACE("Type: {0:03d} Instancer: {:p} Created type: {2}", i.first, i.second, temp->getType());
			}
			else {
				LOG_TRACE("Type: {0:03d} Instancer: {:p} Created type: UNKNOWN", i.first, i.second);
			}
		}
	}
)

ADERITE_MIDDLEWARE_SECTION(
	void Serializer::setNextId(SerializableHandle handle) {
		ADERITE_DYNAMIC_ASSERT(m_nextAvailableHandle != 0, "setNextId called after serializer has already been used");
		m_nextAvailableHandle = handle;
	}
)

void Serializer::registerRuntimeTypes() {

}

InstancerBase* Serializer::resolveInstancer(SerializableType type) {
	auto it = m_instancers.find(type);
	ADERITE_DYNAMIC_ASSERT(it != m_instancers.end(), "Tried to resolve instancer for non registered type {0}", type);
	return it->second;
}

Path Serializer::resolvePath(SerializableHandle handle) {
	ADERITE_DYNAMIC_ASSERT(m_resolver != nullptr, "Tried to resolve path with no resolver");
	return m_resolver->resolve(handle);
}

}
}
