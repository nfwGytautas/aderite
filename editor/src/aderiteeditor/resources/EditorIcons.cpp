#include "EditorIcons.hpp"

#include <stb_image.h>

#include "aderite/io/Loader.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace editor {

bool EditorIcons::load() {
    LOG_TRACE("[Editor] Loading editor icons");

    bool result = true;
    size_t found = 0;
    if (std::filesystem::exists("res/icons/editor/")) {
        // Load icons
        for (const auto& icon : std::filesystem::recursive_directory_iterator("res/icons/editor/")) {
            found++;
            const std::string name = icon.path().stem().string();
            if (!this->loadIcon(name, icon.path())) {
                LOG_WARN("[Editor] Failed to load icon {0} from {1}", name, icon.path().string());
                result = false;
            }
        }
    } else {
        LOG_ERROR("[Editor] Icon directory was not found");
        result = false;
    }

    const size_t failedToLoad = found - m_icons.size();

    if (failedToLoad == 0) {
        LOG_INFO("[Editor] Found {1} icons. Loaded {0}, failed to load {2}", m_icons.size(), found, found - m_icons.size());
    } else {
        LOG_WARN("[Editor] Found {1} icons. Loaded {0}, failed to load {2}", m_icons.size(), found, found - m_icons.size());
    }

    return result;
}

void EditorIcons::unload() {
    LOG_TRACE("[Editor] Unloading editor icons");

    for (auto kvp : m_icons) {
        bgfx::destroy(kvp.second);
    }
    m_icons.clear();
    m_nullHandle = BGFX_INVALID_HANDLE;

    LOG_INFO("[Editor] Editor icons unloaded");
}

EditorIcons& EditorIcons::getInstance() {
    static EditorIcons icons;
    return icons;
}

bgfx::TextureHandle EditorIcons::getIcon(const std::string& name) const {
    auto it = m_icons.find(name);

    if (it != m_icons.end()) {
        return it->second;
    }

    return m_nullHandle;
}

EditorIcons::EditorIcons() {}

bool EditorIcons::loadIcon(const std::string& name, const std::filesystem::path& path) {
    int width = 0;
    int height = 0;
    int bpp = 0;
    bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Count;

    LOG_TRACE("[Editor] Loading icon {0}", name);

    // Load
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &bpp, 0);

    if (data == nullptr) {
        LOG_ERROR("[Editor] {0} stbi load error {1}", stbi_failure_reason());
        return false;
    }

    // Get format
    switch (bpp) {
    case 1: {
        format = bgfx::TextureFormat::R8;
        break;
    }
    case 2: {
        format = bgfx::TextureFormat::RG8;
        break;
    }
    case 3: {
        format = bgfx::TextureFormat::RGB8;
        break;
    }
    case 4: {
        format = bgfx::TextureFormat::RGBA8;
        break;
    }
    default: {
        LOG_ERROR("[Editor] Failed to resolve format");
        return false;
    }
    }

    // Create handle
    bgfx::TextureHandle handle = bgfx::createTexture2D(static_cast<uint16_t>(width), static_cast<uint16_t>(height), false, 1,
                                                       static_cast<bgfx::TextureFormat::Enum>(format),
                                                       BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP,
                                                       bgfx::copy(data, width * height * bpp));

    if (!bgfx::isValid(handle)) {
        LOG_ERROR("[Editor] Failed to create handle");
        return false;
    }

    // Register name
    m_icons[name] = handle;

    if (name == "null") {
        m_nullHandle = handle;
    }

    return true;
}

} // namespace editor
} // namespace aderite
