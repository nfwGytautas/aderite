#pragma once

#include <string>
#include <vector>

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief Dialog for selecting files, directories, etc. Currently uses PFD
 */
class FileDialog {
public:
    /**
     * @brief Opens a select directory dialog and returns selected directory
     * @return Directory path
     */
    static std::string selectDirectory();

    /**
     * @brief Select file with an optional filter
     * @param title Title of the dialog
     * @param filter Vector of filter to apply for example {"Aderite project", "*.aproj"}
     * @return Path to selected file
     */
    static std::string selectFile(const std::string& title, const std::vector<std::string>& filter = {"All files", "*"});
};

} // namespace editor_ui
} // namespace aderite
