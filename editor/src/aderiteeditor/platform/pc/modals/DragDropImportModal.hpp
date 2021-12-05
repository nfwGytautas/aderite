#pragma once

#include <filesystem>
#include <functional>

#include "aderite/reflection/RuntimeTypes.hpp"

#include "aderiteeditor/platform/pc/modals/IModal.hpp"

namespace aderite {
namespace editor {

/**
 * @brief A modal that is used to import drag and drop assets
 */
class DragDropImportModal : public IModal {
public:
    using ImportFn = std::function<void(const std::filesystem::path&, reflection::RuntimeTypes)>;

    /**
     * @brief Create a drag and drop asset import modal
     * @param path Path of the asset being imported
     * @param callback Function to call if asset is being imported
     */
    DragDropImportModal(std::filesystem::path path, ImportFn callback);

    // Inherited via IModal
    virtual void render() override;
    virtual bool stillValid() const override;
    virtual void close() override;
    virtual void show() override;

private:
    ImportFn m_callback;
    bool m_open = true;
    std::filesystem::path m_path = "";

    int m_type = -1;
};

} // namespace editor
} // namespace aderite
