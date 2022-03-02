#pragma once

#include <filesystem>
#include <vector>

#include "aderite/io/SerializableAsset.hpp"

namespace aderite {
class Engine;

namespace asset {

/**
 * @brief AssetManager class is used to hold references and manage asset that are currently loaded
 */
class AssetManager final {
private:
    struct AssetRegistryEntry {
        io::SerializableAsset* Asset = nullptr;
        io::SerializableHandle Handle = io::SerializableAsset::c_InvalidHandle;
    };

public:
    /**
     * @brief Initializes the asset manager
     * @return True if initialized without error, false otherwise
     */
    bool init();

    /**
     * @brief Shutdown asset manager
     */
    void shutdown();

    /**
     * @brief Updates the asset manager, queueing necessary assets, freeing unneeded, etc.
     */
    void update();

    /**
     * @brief Loads the asset manager registry using the FileHandler
     * @return True if loaded and valid, false otherwise
     */
    bool loadRegistry();

    /**
     * @brief Saves the asset manager registry to file
     */
    void saveRegistry();

    /**
     * @brief Returns true if the asset manager has the specified asset tracked, false otherwise
     */
    bool has(io::SerializableHandle handle) const;

    /**
     * @brief Returns object associated with the serializable handle
     * @param handle Handle of the object
     * @return Serializable object instance
     */
    io::SerializableAsset* get(io::SerializableHandle handle);

    /**
     * @brief Serializes object into a file
     * @param object Object to serialize
     */
    void save(io::SerializableAsset* object) const;

    /**
     * @brief Adds an object to the asset manager look up table, if already existing object is added
     * an assert will be triggered. This method will set the object handle
     * @param object Object to track
     */
    void track(io::SerializableAsset* object);

    /**
     * @brief Removes an object from the asset manager look up table, if a non tracked object is untracked
     * an assert will be triggered.
     * @param object Object to untrack
     */
    void untrack(io::SerializableAsset* asset);

    /**
     * @brief Utility method for saving all objects that are currently tracked
     */
    void saveAllTrackedObjects() const;

    auto begin() {
        return m_registry.begin();
    }

    auto begin() const {
        return m_registry.begin();
    }

    auto end() {
        return m_registry.end();
    }

    auto end() const {
        return m_registry.end();
    }

private:
    AssetManager() {}
    friend Engine;

private:
    // Registry
    io::SerializableHandle m_nextFreeHandle = 0;
    std::vector<AssetRegistryEntry> m_registry;
};

} // namespace asset
} // namespace aderite
