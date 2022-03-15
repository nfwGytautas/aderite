#pragma once

#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>

#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

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

    /**
     * @brief Returns the current loadable of the loader
     */
    ILoadable* current() const;

    /**
     * @brief Returns true if the loader instance is ready to load
     */
    bool isReady() const;

public: // Loading operations
    struct LoadResult {
    public:
        virtual ~LoadResult() {};
        std::string Error = "";

    protected:
        LoadResult() {}
    };

    template<class T>
    struct TextureLoadResult : public LoadResult {
        int BPP = 0;
        int Width = 0;
        int Height = 0;

        // BGFX internal format
        int Format = 0;

        std::unique_ptr<T> Data;
    };

    struct MeshLoadResult : public LoadResult {
        std::vector<float> Vertices;
        std::vector<unsigned int> Indices;
    };

    struct ShaderLoadResult : public LoadResult {
        std::vector<unsigned char> VertexSource;
        std::vector<unsigned char> FragmentSource;
    };

    struct BinaryLoadResult : public LoadResult {
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
    bool m_ready = false;

    class LoaderImpl;
    LoaderImpl* m_impl = nullptr;
};

} // namespace io
} // namespace aderite
