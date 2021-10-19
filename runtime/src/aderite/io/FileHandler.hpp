#pragma once

#include <filesystem>
#include <unordered_map>
#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief DataChunk is a piece of memory that the file handler works with
*/
class DataChunk {
public:
	DataChunk(size_t offset, size_t size, const char* name, std::vector<unsigned char> data);

	// Data of the chunk
	std::vector<unsigned char> Data;

	// Offset from start of file
	const size_t Offset;

	// Original size used to track changes
	const size_t OriginalSize;

	// Name of the data chunk
	const char* Name;
};

/**
 * @brief Class that is responsible for reading and writing to device memory
*/
class FileHandler final {
public:
	/**
	 * @brief Struct containing reserved serializable and loadable ids
	*/
	struct Reserved {
		static constexpr LoadableHandle MasterAudioBank = 0;
		static constexpr LoadableHandle StringsAudioBank = 1;
	};

public:
	/**
	 * @brief Resolves the handle file and chunk, loads it and returns it, don't forget to call close
	 * @param handle Handle to read
	 * @return DataChunk instance
	*/
	DataChunk openSerializable(SerializableHandle handle) const;

	/**
	 * @brief Resolves the handle file and chunk, loads it and returns it, don't forget to call close
	 * @param handle Handle to read
	 * @return DataChunk instance
	*/
	DataChunk openLoadable(LoadableHandle handle) const;

	/**
	 * @brief Commit changes to the data chunk to it's respective file
	 * @param chunk Chunk to commit
	*/
	void commit(const DataChunk& chunk) const;

	/**
	 * @brief Set the root directory of FileHandler
	 * @param root New root directory
	*/
	void setRoot(const std::filesystem::path& root);
private:
	struct ChunkLocator {
		size_t Offset;
		size_t Size;
	};

	std::filesystem::path m_rootDir;
};

}
}
