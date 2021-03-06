#include "FileHandler.hpp"
#include <fstream>

#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

DataChunk::DataChunk(size_t offset, size_t size, std::string name, std::vector<unsigned char> data) :
    Offset(offset),
    OriginalSize(size),
    Name(name),
    Data(std::move(data)) {}

DataChunk FileHandler::openSerializable(SerializableHandle handle) const {
    LOG_TRACE("[IO] Opening serializable {0}", handle);
    const std::filesystem::path path = m_rootDir / "Asset" / (std::to_string(handle) + ".asset");

    if (!std::filesystem::exists(path)) {
        return DataChunk(0, 0, ("Asset/" + std::to_string(handle) + ".asset").c_str(), {});
    }

    std::ifstream in(path, std::ios::binary);
    size_t offset = 0;
    size_t size = in.seekg(0, std::ios::end).tellg();
    in.seekg(0, std::ios::beg);
    std::vector<unsigned char> data;
    data.resize(size);
    in.read(reinterpret_cast<char*>(data.data()), data.size());
    data.push_back('\0');
    LOG_INFO("[IO] Serializable {0} opened and loaded", handle);
    return DataChunk(offset, size, ("Asset/" + std::to_string(handle) + ".asset").c_str(), data);
}

DataChunk FileHandler::openReservedLoadable(LoadableHandle handle) const {
    LOG_TRACE("[IO] Opening RESERVED loadable {0}", handle);
    const std::filesystem::path path = m_rootDir / "Data" / ("_" + std::to_string(handle) + ".data");

    if (!std::filesystem::exists(path)) {
        return DataChunk(0, 0, ("Data/_" + std::to_string(handle) + ".data").c_str(), {});
    }

    std::ifstream in(path, std::ios::binary);
    size_t offset = 0;
    size_t size = in.seekg(0, std::ios::end).tellg();
    in.seekg(0, std::ios::beg);
    std::vector<unsigned char> data;
    data.resize(size);
    in.read(reinterpret_cast<char*>(data.data()), data.size());
    data.push_back('\0');
    LOG_INFO("[IO] Reserved loadable {0} opened and loaded", handle);
    return DataChunk(offset, size, ("Data/_" + std::to_string(handle) + ".data").c_str(), data);
}

std::filesystem::path FileHandler::pathToReserved(LoadableHandle handle) const {
    return m_rootDir / "Data" / ("_" + std::to_string(handle) + ".data");
}

DataChunk FileHandler::openLoadable(SerializableHandle handle) const {
    LOG_TRACE("[IO] Opening loadable {0}", handle);
    const std::filesystem::path path = m_rootDir / "Data" / (std::to_string(handle) + ".data");

    if (!std::filesystem::exists(path)) {
        return DataChunk(0, 0, ("Data/" + std::to_string(handle) + ".data").c_str(), {});
    }

    std::ifstream in(path, std::ios::binary);
    size_t offset = 0;
    size_t size = in.seekg(0, std::ios::end).tellg();
    in.seekg(0, std::ios::beg);
    std::vector<unsigned char> data;
    data.resize(size);
    in.read(reinterpret_cast<char*>(data.data()), data.size());
    data.push_back('\0');
    LOG_INFO("[IO] Loadable {0} opened and loaded", handle);
    return DataChunk(offset, size, ("Data/" + std::to_string(handle) + ".data").c_str(), data);
}

void FileHandler::commit(const DataChunk& chunk) const {
    LOG_TRACE("[IO] Commiting chunk of size {0}(Was: {3}) to {1} at offset {2}", chunk.Data.size(), chunk.Name, chunk.Offset,
              chunk.OriginalSize);

    // TODO: Resize and move depending on size change
    std::filesystem::path outPath = m_rootDir / std::string(chunk.Name);
    std::ofstream of(outPath, std::ios::binary);
    of.seekp(chunk.Offset);
    of.write(reinterpret_cast<const char*>(chunk.Data.data()), chunk.Data.size());
    LOG_INFO("[IO] Chunk {0} commited", chunk.Name);
}

void FileHandler::setRoot(const std::filesystem::path& root) {
    LOG_TRACE("[IO] Setting root to {0}", root.string());
    ADERITE_DYNAMIC_ASSERT(std::filesystem::exists(root / "Asset/"), "Asset directory doesn't exist in root");
    ADERITE_DYNAMIC_ASSERT(std::filesystem::exists(root / "Data/"), "Data directory doesn't exist in root");
    m_rootDir = root;
}

void FileHandler::writePhysicalFile(LoadableHandle handle, const std::filesystem::path& file) const {
    LOG_TRACE("[IO] Writing physical file to {0} from {1}", handle, file.string());
    // Load chunk
    DataChunk chunk = this->openLoadable(handle);

    // Read all contents
    std::ifstream in(file, std::ios::binary);
    size_t size = in.seekg(0, std::ios::end).tellg();
    in.seekg(0, std::ios::beg);
    chunk.Data.resize(size);
    in.read(reinterpret_cast<char*>(chunk.Data.data()), chunk.Data.size());

    // Commit new chunk
    this->commit(chunk);
    LOG_INFO("[IO] Physical file {0} written to {1}", file.string(), handle);
}

bool FileHandler::exists(LoadableHandle handle) const {
    ADERITE_DYNAMIC_ASSERT(handle != c_InvalidHandle, "Invalid handle passed to exists");
    const std::filesystem::path path = m_rootDir / "Data" / (std::to_string(handle) + ".data");

    if (!std::filesystem::exists(path)) {
        return false;
    }

    return true;
}

} // namespace io
} // namespace aderite
