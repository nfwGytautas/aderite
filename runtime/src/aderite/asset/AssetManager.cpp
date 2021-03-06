#include "AssetManager.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace asset {

bool AssetManager::init() {
    ADERITE_LOG_BLOCK;
    LOG_DEBUG("[Asset] Initializing asset manager, current version");
    LOG_INFO("[Asset] Asset manager initialized");
    return true;
}

void AssetManager::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Asset] Shutting down asset manager");

    // Free memory
    // 3 Update cycles should free any data during shutdown

    // 1 update will delete 0 outstanding reference
    // 2 update will delete any left over dependencies
    // 3 update will delete every other nested dependency
    this->update();
    this->update();
    this->update();

    // Verify
    for (auto& obj : m_registry) {
        if (obj.Asset != nullptr) {
            LOG_WARN("[Asset] Asset memory leak for {1}({0}) it has {2} references", obj.Handle, obj.Asset->getName(), obj.Asset->getRefCount());
        }
    }

    // Save registry
    this->saveRegistry();

    LOG_INFO("[Asset] Asset manager shutdown");
}

void AssetManager::update() {
    for (auto& entry : m_registry) {
        if (entry.Asset != nullptr) {
            // Check references
            if (entry.Asset->getRefCount() > 0) {
                // Check if loaded
                if (entry.Asset->needsLoading()) {
                    // Load
                    ::aderite::Engine::getLoaderPool()->enqueue(entry.Asset);
                }
            } else {
                // No outstanding ref count, can be freed
                entry.Asset->unload();

                // And delete and flag as no meta
                delete entry.Asset;
                entry.Asset = nullptr;
            }
        }
    }
}

bool AssetManager::loadRegistry() {
    ADERITE_DYNAMIC_ASSERT(m_registry.size() == 0, "Cannot load a registry when an asset manager was already modified");

    LOG_TRACE("[Asset] Loading registry");

    // Open loadable
    io::DataChunk chunk = aderite::Engine::getFileHandler()->openReservedLoadable(aderite::io::FileHandler::Reserved::AssetRegistry);
    YAML::Node data = YAML::Load(reinterpret_cast<const char*>(chunk.Data.data()));

    // Parse data
    const YAML::Node& registryNode = data["Registry"];

    m_nextFreeHandle = registryNode["NextHandle"].as<io::SerializableHandle>();

    for (const YAML::Node& entryNode : registryNode["Entries"]) {
        io::SerializableHandle handle = entryNode["Handle"].as<io::SerializableHandle>();
        m_registry.push_back({nullptr, handle});
    }

    LOG_INFO("[Asset] Registry loaded it contains {0} elements", m_registry.size());
    return true;
}

void AssetManager::saveRegistry() {
    LOG_TRACE("[Asset] Saving registry");

    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "Registry" << YAML::BeginMap;
    out << YAML::Key << "NextHandle" << YAML::Value << m_nextFreeHandle;
    out << YAML::Key << "Entries" << YAML::BeginSeq;

    for (auto& entry : m_registry) {
        out << YAML::BeginMap;
        out << YAML::Key << "Handle" << YAML::Value << entry.Handle;
        out << YAML::EndMap;
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;
    out << YAML::EndMap;

    // Save to file
    io::DataChunk chunk = aderite::Engine::getFileHandler()->openReservedLoadable(aderite::io::FileHandler::Reserved::AssetRegistry);
    chunk.Data.resize(out.size());
    std::memcpy(chunk.Data.data(), out.c_str(), chunk.Data.size());
    aderite::Engine::getFileHandler()->commit(chunk);

    LOG_INFO("[Asset] Registry saved");
}

bool AssetManager::has(io::SerializableHandle handle) const {
    ADERITE_DYNAMIC_ASSERT(handle != io::SerializableAsset::c_InvalidHandle, "Invalid handle passed to asset manager has method");

    // Try to find the entry
    auto it = std::find_if(m_registry.begin(), m_registry.end(), [handle](const AssetRegistryEntry& entry) {
        return entry.Handle == handle;
    });

    return it != m_registry.end();
}

io::SerializableAsset* AssetManager::get(io::SerializableHandle handle) {
    ADERITE_DYNAMIC_ASSERT(handle != io::SerializableAsset::c_InvalidHandle, "Invalid handle passed to asset manager get method");

    // Try to find the entry
    auto it = std::find_if(m_registry.begin(), m_registry.end(), [handle](const AssetRegistryEntry& entry) {
        return entry.Handle == handle;
    });

    ADERITE_DYNAMIC_ASSERT(it != m_registry.end(), "Tried to query asset that has not been tracked with the asset manager");

    if (it->Asset == nullptr) {
        // Resolve path and load
        io::DataChunk chunk = aderite::Engine::getFileHandler()->openSerializable(handle);
        YAML::Node data = YAML::Load(reinterpret_cast<const char*>(chunk.Data.data()));
        it->Asset = static_cast<io::SerializableAsset*>(aderite::Engine::getSerializer()->parseObject(data));
        it->Asset->m_handle = it->Handle;
    }

    return it->Asset;
}

void AssetManager::save(io::SerializableAsset* object) const {
    // Verify that the object is valid
    ADERITE_DYNAMIC_ASSERT(object != nullptr, "Nullptr object passed to save");
    ADERITE_DYNAMIC_ASSERT(object->getHandle() != c_InvalidHandle, "SerializableAsset with invalid handle passed");

    YAML::Emitter out;

    LOG_TRACE("[Asset] Saving {0}", object->getHandle());

    // Common
    aderite::Engine::getSerializer()->writeObject(out, object);

    // Resolve where to store this object
    LOG_TRACE("[Asset] Emitting {0} to DataChunk", object->getHandle());
    io::DataChunk chunk = aderite::Engine::getFileHandler()->openSerializable(object->getHandle());
    chunk.Data.resize(out.size());
    std::memcpy(chunk.Data.data(), out.c_str(), chunk.Data.size());
    aderite::Engine::getFileHandler()->commit(chunk);
}

void AssetManager::track(io::SerializableAsset* object) {
    m_registry.push_back({object, m_nextFreeHandle++});
    object->m_handle = m_nextFreeHandle - 1;

    // Save registry
    this->saveRegistry();
}

void AssetManager::untrack(io::SerializableAsset* asset) {
    ADERITE_DYNAMIC_ASSERT(asset != nullptr, "Tried to untrack nullptr");

    // Remove from list
    auto it = std::find_if(m_registry.begin(), m_registry.end(), [asset](auto& entry) {
        return entry.Asset != nullptr && entry.Asset == asset;
    });

    if (it != m_registry.end()) {
        ADERITE_DYNAMIC_ASSERT(it->Asset->getRefCount() == 0, "Tried to untrack asset with outstanding references");
        m_registry.erase(it);
    }
}

void AssetManager::saveAllTrackedObjects() const {
    LOG_TRACE("[Asset] Saving all objects");
    for (auto& obj : m_registry) {
        if (obj.Asset != nullptr) {
            this->save(obj.Asset);
        }
    }
    LOG_INFO("[Asset] All objects saved");
}

} // namespace asset
} // namespace aderite
