#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <filesystem>
#include <memory>

#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief Class to specify that the class uses loader
*/
class ILoadable {
public:
	virtual ~ILoadable() {};

	/**
	 * @brief Load the object data, this function is executed in a separate thread
	 * @param loader Loader instance
	*/
	virtual void load(const Loader* loader) = 0;

	/**
	 * @brief Unload the object
	*/
	virtual void unload() = 0;

private:
	bool m_loading = false;
	friend class Loader;
	friend class LoaderPool;
};

/**
 * @brief A general purpose data loader, each loader runs on it's own thread
*/
class Loader final {
public: // Thread handling
	Loader(LoaderPool* pool);
	~Loader();

	/**
	 * @brief Start the worker thread
	*/
	void startup();

	/**
	 * @brief Terminate the worker thread
	*/
	void terminate();

public: // Loading operations
	template<class T>
	struct TextureLoadResult {
		std::string Error = "";

		int BPP = 0;
		int Width = 0;
		int Height = 0;

		// BGFX internal format
		int Format = 0;

		std::unique_ptr<T> Data;
	};

	struct MeshLoadResult {
		std::string Error = "";

		std::vector<float> Vertices;
		std::vector<unsigned int> Indices;
	};

	struct ShaderLoadResult {
		std::string Error = "";

		std::vector<unsigned char> VertexSource;
		std::vector<unsigned char> FragmentSource;
	};

	struct BinaryLoadResult {
		std::string Error = "";

		std::vector<unsigned char> Content;
	};

	/**
	 * @brief Loads a mesh from specified file
	 * @param handle Loadable handle
	 * @return MeshLoadResult object
	*/
	MeshLoadResult loadMesh(LoadableHandle handle) const;

	/**
	 * @brief Loads a texture from specified file
	 * @param handle Loadable handle
	 * @return ImageLoadResult object
	*/
	TextureLoadResult<unsigned char> loadTexture(LoadableHandle handle) const;

	/**
	 * @brief Loads a HDR texture from specified file
	 * @param handle Loadable handle
	 * @return ImageLoadResult object
	*/
	TextureLoadResult<float> loadHdrTexture(LoadableHandle handle) const;

	/**
	 * @brief Loads shader content from specified file
	 * @param handle Loadable handle
	 * @return ShaderLoadResult object
	*/
	ShaderLoadResult loadShader(LoadableHandle handle) const;

	/**
	 * @brief Loads binary content from specified file
	 * @param handle Loadable handle
	 * @return BinaryLoadResult object
	*/
	BinaryLoadResult loadBinary(LoadableHandle handle) const;
private:
	LoaderPool* m_pool;
	std::thread m_thread;
	bool m_terminated = false;

	class LoaderImpl;
	LoaderImpl* m_impl = nullptr;
};

}
}
