#include "Serializer.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/Instancer.hpp"
#include "aderite/io/RuntimeSerializables.hpp"

// Assets, needed for linking instancers
#include "aderite/rendering/Pipeline.hpp"

#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/scene/Scene.hpp"

#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"

namespace aderite {
namespace io {

bool Serializer::init() {
	LOG_TRACE("Initializing serializer, current version {0}", c_CurrentVersion);

	// Runtime instancers
	LOG_TRACE("Setting runtime instancers");
	// Assets
	linkInstancer(static_cast<size_t>(RuntimeSerializables::MESH), new Instancer<asset::MeshAsset>());
	linkInstancer(static_cast<size_t>(RuntimeSerializables::MATERIAL), new Instancer<asset::MaterialAsset>());
	linkInstancer(static_cast<size_t>(RuntimeSerializables::TEXTURE), new Instancer<asset::TextureAsset>());
	linkInstancer(static_cast<size_t>(RuntimeSerializables::SCENE), new Instancer<scene::Scene>());
	linkInstancer(static_cast<size_t>(RuntimeSerializables::MAT_TYPE), new Instancer<asset::MaterialTypeAsset>());
	linkInstancer(static_cast<size_t>(RuntimeSerializables::PIPELINE), new Instancer<rendering::Pipeline>());

	// Colliders
	linkInstancer(static_cast<size_t>(RuntimeSerializables::CLDR_LIST), new Instancer<physics::ColliderList>());
	linkInstancer(static_cast<size_t>(RuntimeSerializables::BOX_CLDR), new Instancer<physics::BoxCollider>());

	LOG_DEBUG("Registered {0} runtime instancers", m_instancers.size());
	ADERITE_DEBUG_SECTION(
		this->printInstancers();
	);

	return true;
}

void Serializer::shutdown() {
	for (auto obj : m_objects) {
		delete obj;
	}

	for (auto instancer : m_instancers) {
		delete instancer.second;
	}
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

SerializableObject* Serializer::parseType(const YAML::Node& data) const {
	ADERITE_DYNAMIC_ASSERT(data["Type"], "No type specified in scope");
	ADERITE_DYNAMIC_ASSERT(data["Handle"], "No handle specified in scope");
	ADERITE_DYNAMIC_ASSERT(data["Data"], "No data specified in scope");

	// Get resolver for type
	SerializableType type = data["Type"].as<SerializableType>();
	SerializableHandle handle = data["Handle"].as<SerializableHandle>();

	// Get instancer
	InstancerBase* instancer = resolveInstancer(type);

	// Create object
	SerializableObject* instance = static_cast<SerializableObject*>(instancer->create());
	instance->m_handle = handle;

	ADERITE_DYNAMIC_ASSERT(instance->getType() == type, "Types don't match between instancer created instance and file stored type");

	// Deserialize
	instance->deserialize(this, data["Data"]);

	return instance;
}

void Serializer::writeType(YAML::Emitter& emitter, SerializableObject* object) const {
	ADERITE_DYNAMIC_ASSERT(object->getHandle() == c_InvalidHandle, "No handle specified in scope");

	emitter << YAML::BeginMap;
	emitter << YAML::Key << "Type" << YAML::Value << object->getType();
	emitter << YAML::Key << "Handle" << YAML::Value << object->getHandle();
	emitter << YAML::Key << "Data" << YAML::BeginMap;
	object->serialize(this, emitter);
	emitter << YAML::EndMap;
	emitter << YAML::EndMap;
}

ISerializable* Serializer::parseUntrackedType(const YAML::Node& data) const {
	ADERITE_DYNAMIC_ASSERT(data["Type"], "No type specified in data scope");
	ADERITE_DYNAMIC_ASSERT(data["Data"], "No data specified in scope");

	// Get resolver for type
	SerializableType type = data["Type"].as<SerializableType>();

	// Get instancer
	InstancerBase* instancer = resolveInstancer(type);

	// Create object
	ISerializable* instance = instancer->create();

	ADERITE_DYNAMIC_ASSERT(instance->getType() == type, "Types don't match between instancer created instance and file stored type");

	// Deserialize
	instance->deserialize(this, data["Data"]);

	return instance;
}

void Serializer::writeUntrackedType(YAML::Emitter& emitter, ISerializable* object) const {
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "Type" << YAML::Value << object->getType();
	emitter << YAML::Key << "Data" << YAML::BeginMap;
	object->serialize(this, emitter);
	emitter << YAML::EndMap;
	emitter << YAML::EndMap;
}

SerializableObject* Serializer::get(SerializableHandle handle) {
	ADERITE_DYNAMIC_ASSERT(handle != c_InvalidHandle, "Invalid handle passed to serializer get method");
	ADERITE_DYNAMIC_ASSERT(m_objects.size() <= handle, "Requested non existing object");
	return m_objects[handle];
}

void Serializer::add(SerializableObject* object) {
	ADERITE_DYNAMIC_ASSERT(object->getHandle() == c_InvalidHandle, "Tried to add an already existing object to Serializer");
	SerializableHandle handle = this->nextAvailableHandle();
	object->m_handle = handle;
	m_objects[handle] = object;
}

void Serializer::remove(SerializableHandle handle) {
	ADERITE_DYNAMIC_ASSERT(m_objects.size() >= handle, "Tried to remove non existing object");
	delete m_objects[handle];
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
	ADERITE_DYNAMIC_ASSERT(data["Version"], "Tried to read a file without a specified version");

	// TODO: Check version and upgrade if needed
	SerializableObject* type = this->parseType(data);

	if (type->getHandle() + 1 > m_objects.size()) {
		// Resize
		m_objects.resize(type->getHandle() + 1, nullptr);
		m_hasNull = true;
	}

	m_objects[type->getHandle()] = type;
	return type;
}

void Serializer::reread(SerializableObject* object) {
	// Resolve path and load
	DataChunk chunk = aderite::Engine::getFileHandler()->openSerializable(object->getHandle());
	YAML::Node data = YAML::Load(reinterpret_cast<const char*>(chunk.Data.data()));
	object->deserialize(this, data["Data"]);
}

void Serializer::save(SerializableObject* object) {
	YAML::Emitter out;

	ADERITE_DYNAMIC_ASSERT(object->getHandle() != c_InvalidHandle, "SerializableObject with invalid handle passed");

	out << YAML::BeginMap;

	// Common
	out << YAML::Key << "Version" << YAML::Value << c_CurrentVersion;
	out << YAML::Key << "Type" << YAML::Value << object->getType();
	out << YAML::Key << "Handle" << YAML::Value << object->getHandle();
	out << YAML::Key << "Data" << YAML::BeginMap;

	if (!object->serialize(this, out)) {
		LOG_ERROR("Failed to serialize object handle: {0}", object->getHandle());
		return;
	}

	out << YAML::EndMap;
	out << YAML::EndMap;

	// Resolve where to store this object
	DataChunk chunk = aderite::Engine::getFileHandler()->openSerializable(object->getHandle());
	chunk.Data.resize(out.size());
	std::memcpy(chunk.Data.data(), out.c_str(), chunk.Data.size());
	aderite::Engine::getFileHandler()->commit(chunk);
}

void Serializer::saveAll() {
	for (SerializableObject* obj : m_objects) {
		this->save(obj);
	}
}

ADERITE_DEBUG_SECTION(
	void Serializer::printInstancers() {
		LOG_TRACE("");
		LOG_TRACE("====================================================================================");
		LOG_TRACE("                                    INSTANCERS                                      ");
		LOG_TRACE("====================================================================================");
		for (auto i : m_instancers) {
			if (i.second != nullptr) {
				ISerializable* temp = nullptr;
				temp = i.second->create();
				LOG_TRACE("Type: {0:03d} Instancer: {1:p} Created type: {2}", i.first, static_cast<void*>(i.second), temp->getType());
			}
			else {
				LOG_TRACE("Type: {0:03d} Instancer: {1:p} Created type: UNKNOWN", i.first, static_cast<void*>(i.second));
			}
		}
		LOG_TRACE("");
	}
)

InstancerBase* Serializer::resolveInstancer(SerializableType type) const {
	auto it = m_instancers.find(type);
	ADERITE_DYNAMIC_ASSERT(it != m_instancers.end(), "Tried to resolve instancer for non registered type {0}", type);
	return it->second;
}

SerializableHandle Serializer::nextAvailableHandle() {
	if (!m_hasNull) {
		m_objects.push_back(nullptr);
		return m_objects.size() - 1;
	}
	
	// Find next nullptr
	for (size_t i = 0; i < m_objects.size(); i++) {
		if (m_objects[i] == nullptr) {
			return i;
		}
	}

	// Was a false positive null flag
	m_hasNull = false;
	m_objects.push_back(nullptr);
	return m_objects.size() - 1;
}

}
}
