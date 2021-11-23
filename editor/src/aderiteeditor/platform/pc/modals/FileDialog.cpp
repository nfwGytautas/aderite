#include "FileDialog.hpp"

#include <pfd/portable-file-dialogs.h>

namespace aderite {
namespace editor {

std::string FileDialog::selectDirectory() {
    std::string dir = pfd::select_folder("Select project directory").result();
    return dir;
}

std::string FileDialog::selectFile(const std::string& title, const std::vector<std::string>& filter) {
    std::vector<std::string> path = pfd::open_file(title, ".", filter, pfd::opt::none).result();

    if (path.size() > 0) {
        return path[0];
    }

    return "";
}

} // namespace editor_ui
} // namespace aderite
