#include "FileHandler.hpp"

#include <fstream>
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

DataChunk::DataChunk(size_t offset, size_t size, const char* name, std::vector<unsigned char> data)
    : Offset(offset), OriginalSize(size), Name(name), Data(std::move(data))
{}

DataChunk FileHandler::openSerializable(SerializableHandle handle) const {
    std::ifstream in(m_rootDir / "Asset" / (std::to_string(handle) + ".asset"));
    size_t offset = 0;
    size_t size = in.seekg(0, std::ios::end).tellg();
    in.seekg(0, std::ios::beg);
    std::vector<unsigned char> data;
    data.resize(size);
    in.read(reinterpret_cast<char*>(data.data()), data.size());
    return DataChunk(offset, size, ("Asset/" + std::to_string(handle) + ".asset").c_str(), data);
}

DataChunk FileHandler::openLoadable(LoadableHandle handle) const {
    std::ifstream in(m_rootDir / "Data" / (std::to_string(handle) + ".data"));
    size_t offset = 0;
    size_t size = in.seekg(0, std::ios::end).tellg();
    in.seekg(0, std::ios::beg);
    std::vector<unsigned char> data;
    data.resize(size);
    in.read(reinterpret_cast<char*>(data.data()), data.size());
    return DataChunk(offset, size, ("Data/" + std::to_string(handle) + ".data").c_str(), data);
}

void FileHandler::commit(const DataChunk& chunk) const {
    std::ofstream of(m_rootDir / std::string(chunk.Name));
    of.seekp(chunk.Offset);
    of.write(reinterpret_cast<const char*>(chunk.Data.data()), chunk.Data.size());
}

void FileHandler::setRoot(const std::filesystem::path& root) {
    m_rootDir = root;
}

}
}
