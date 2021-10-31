#include "File.hpp"

#include "aderiteeditor/vfs/Directory.hpp"

namespace aderite {
namespace vfs {

File::File(const std::string& name, io::SerializableHandle handle, Directory* parent) : m_name(name), m_handle(handle), m_parent(parent) {
    m_parent->m_files.push_back(this);
}

io::SerializableHandle File::getHandle() const {
    return m_handle;
}

Directory* File::getParent() const {
    return m_parent;
}

std::string File::getName() const {
    return m_name;
}

std::string File::getPath() const {
    return m_parent->getPath() + m_name;
}

} // namespace vfs
} // namespace aderite
