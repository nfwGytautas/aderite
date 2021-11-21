#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <bgfx/bgfx.h>

#include "aderite/io/ILoadable.hpp"

namespace aderite {
namespace editor {

/**
 * @brief EditorIcons singleton contains all icons used by the engine
 */
class EditorIcons {
public:
    /**
     * @brief Loads icons
     * @return True if loaded, false otherwise
     */
    bool load();

    /**
     * @brief Unloads icons
     */
    void unload();

    /**
     * @brief Returns the instance of EditorIcons
     * @return EditorIcons instance
     */
    static EditorIcons& getInstance();

    /**
     * @brief Returns the icon with the specified name or BGFX_INVALID_HANDLE if the name wasn't loaded
     * @param name Name of the icon
     * @return bgfx::TextureHandle
     */
    bgfx::TextureHandle getIcon(const std::string& name) const;

private:
    EditorIcons();

    /**
     * @brief Loads an icon into the storage
     * @param name Name of the icon that will be used
     * @param path Path to icon
     * @return True if loaded, false otherwise
     */
    bool loadIcon(const std::string& name, const std::filesystem::path& path);

private:
    bgfx::TextureHandle m_nullHandle = BGFX_INVALID_HANDLE;
    std::unordered_map<std::string, bgfx::TextureHandle> m_icons;
};

} // namespace editor
} // namespace aderite
