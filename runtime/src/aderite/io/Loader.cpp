#include "Loader.hpp"
#include <functional>

// Mesh loading
#include <assimp/DefaultLogger.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/Logger.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// Texture loading
#include <bgfx/bgfx.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/ILoadable.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/utility/Log.hpp"

class AssimpLogSource : public Assimp::Logger {
    virtual void OnDebug(const char* message) override {
        LOG_TRACE("[Asset] ASSIMP {0}", message);
    }
    virtual void OnInfo(const char* message) override {
        LOG_TRACE("[Asset] ASSIMP {0}", message);
    }
    virtual void OnWarn(const char* message) override {
        LOG_WARN("[Asset] ASSIMP {0}", message);
    }
    virtual void OnError(const char* message) override {
        LOG_ERROR("[Asset] ASSIMP {0}", message);
    }
    virtual bool attachStream(Assimp::LogStream* pStream, unsigned int severity) override {
        delete pStream;
        return true;
    }
    virtual bool detatchStream(Assimp::LogStream* pStream, unsigned int severity) override {
        return false;
    }
};

static AssimpLogSource g_LogSource;
static bool g_LoggerSet = false;

namespace aderite {
namespace io {

class Loader::LoaderImpl {
public:
    Assimp::Importer Importer;
    ILoadable* Current = nullptr;
};

Loader::Loader(LoaderPool* pool) : m_pool(pool), m_impl(new LoaderImpl()) {
    if (!g_LoggerSet) {
        LOG_DEBUG("[IO] Setting assimp logger source");
        Assimp::DefaultLogger::set(&g_LogSource);
        g_LoggerSet = true;
    }
}

Loader::~Loader() {
    LOG_TRACE("[IO] Shutting down loader instance");
    m_thread.join();
    delete m_impl;
    LOG_INFO("[IO] Loader shutdown");
}

void Loader::startup() {
    LOG_TRACE("[IO] Starting up loader instance");
    m_thread = std::thread([&]() {
        LOG_INFO("[IO] Loader instance started");
        m_ready = true;
        while (!m_terminated) {
            ILoadable* loadable = m_pool->getNextLoadable();
            if (loadable != nullptr) {
                m_impl->Current = loadable;
                loadable->load(this);
                m_impl->Current = nullptr;
            }
        }
        LOG_TRACE("[IO] Loader instance ending");
    });
}

void Loader::terminate() {
    m_terminated = true;
}

ILoadable* Loader::current() const {
    return m_impl->Current;
}

bool Loader::isReady() const {
    return m_ready;
}

Loader::MeshLoadResult Loader::loadMesh(LoadableHandle handle) const {
    LOG_TRACE("[Asset] Loading mesh from {0}", handle);
    Loader::MeshLoadResult result = {};
    DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
    if (chunk.Data.size() == 0) {
        LOG_ERROR("[Asset] {0} doesn't exist", handle);
        result.Error = "File doesn't exist";
        return result;
    }

    unsigned int flags = 0;

    // Default flags
    flags = aiProcessPreset_TargetRealtime_Quality |                     // some optimizations and safety checks
            aiProcess_OptimizeMeshes |                                   // minimize number of meshes
            aiProcess_PreTransformVertices |                             // apply node matrices
            aiProcess_FixInfacingNormals | aiProcess_TransformUVCoords | // apply UV transformations
            // aiProcess_FlipWindingOrder   | // we cull clock-wise, keep the default CCW winding order
            aiProcess_MakeLeftHanded | // we set GLM_FORCE_LEFT_HANDED and use left-handed bx matrix functions
            aiProcess_FlipUVs |
            // aiProcess_GenNormals |
            0;

    const aiScene* scene = m_impl->Importer.ReadFileFromMemory(chunk.Data.data(), chunk.Data.size(), flags, "obj");

    // Sanity checks
    if (scene == nullptr) {
        result.Error = m_impl->Importer.GetErrorString();
        return result;
    }

    if (scene->mNumMeshes > 1) {
        LOG_ERROR("[Asset] {0} has multiple defined meshes", handle);
        result.Error = "File contains more than 1 mesh";
        return result;
    }

    if (scene->mNumMeshes == 0) {
        LOG_ERROR("[Asset] {0} has no meshes", handle);
        result.Error = "File contains no meshes";
        return result;
    }

    // There should only be one mesh
    aiMesh* mesh = scene->mMeshes[0];

    // Reserve memory for the mesh
    result.Vertices.reserve(mesh->mNumVertices * 3 * 3 * 2);

    // Load vertices
    LOG_TRACE("[Asset] {0} generating vertices", handle);
    for (unsigned int verticeIdx = 0; verticeIdx < mesh->mNumVertices; verticeIdx++) {
        // Position
        result.Vertices.push_back(mesh->mVertices[verticeIdx].x);
        result.Vertices.push_back(mesh->mVertices[verticeIdx].y);
        result.Vertices.push_back(mesh->mVertices[verticeIdx].z);

        // Normal
        result.Vertices.push_back(mesh->mNormals[verticeIdx].x);
        result.Vertices.push_back(mesh->mNormals[verticeIdx].y);
        result.Vertices.push_back(mesh->mNormals[verticeIdx].z);

        // UV
        result.Vertices.push_back(mesh->mTextureCoords[0][verticeIdx].x);
        result.Vertices.push_back(mesh->mTextureCoords[0][verticeIdx].y);
    }

    // Load indices
    LOG_TRACE("[Asset] {0} generating indices", handle);
    for (unsigned int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) {
        // Get the face
        aiFace& face = mesh->mFaces[faceIdx];

        // Add the indices of the face to the vector
        for (unsigned int indiceIdx = 0; indiceIdx < face.mNumIndices; indiceIdx++) {
            result.Indices.push_back(face.mIndices[indiceIdx]);
        }
    }

    LOG_INFO("[Asset] {0} loaded ({1} vertices, {2} indices)", handle, result.Vertices.size(), result.Indices.size());

    return result;
}

Loader::TextureLoadResult<unsigned char> Loader::loadTexture(LoadableHandle handle) const {
    LOG_TRACE("[Asset] Loading texture from {0}", handle);
    TextureLoadResult<unsigned char> result = {};
    DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
    if (chunk.Data.size() == 0) {
        LOG_ERROR("[Asset] {0} doesn't exist", handle);
        result.Error = "File doesn't exist";
        return result;
    }

    unsigned char* data = stbi_load_from_memory(chunk.Data.data(), chunk.Data.size(), &result.Width, &result.Height, &result.BPP, 0);

    if (data == nullptr) {
        result.Error = stbi_failure_reason();
        LOG_ERROR("[Asset] {0} stbi load error {1}", result.Error);
        return result;
    }

    LOG_TRACE("[Asset] {0} getting pixel format", handle);
    switch (result.BPP) {
    case 1: {
        result.Format = bgfx::TextureFormat::R8;
        break;
    }
    case 2: {
        result.Format = bgfx::TextureFormat::RG8;
        break;
    }
    case 3: {
        result.Format = bgfx::TextureFormat::RGB8;
        break;
    }
    case 4: {
        result.Format = bgfx::TextureFormat::RGBA8;
        break;
    }
    }

    result.Data = std::unique_ptr<unsigned char>(data);

    LOG_INFO("[Asset] {0} loaded ({1} width, {2} height, {3} format, {4} bits per pixel)", handle, result.Width, result.Height,
             result.Format, result.BPP);
    return result;
}

Loader::TextureLoadResult<float> Loader::loadHdrTexture(LoadableHandle handle) const {
    LOG_TRACE("[Asset] Loading HDR texture from {0}", handle);
    TextureLoadResult<float> result = {};
    DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
    if (chunk.Data.size() == 0) {
        LOG_ERROR("[Asset] {0} doesn't exist", handle);
        result.Error = "File doesn't exist";
        return result;
    }

    float* data = stbi_loadf_from_memory(chunk.Data.data(), chunk.Data.size(), &result.Width, &result.Height, &result.BPP, 4);

    if (data == nullptr) {
        result.Error = stbi_failure_reason();
        LOG_ERROR("[Asset] {0} stbi load error {1}", result.Error);
        return result;
    }

    LOG_TRACE("[Asset] {0} getting pixel format", handle);
    switch (result.BPP) {
    case 1: {
        result.Format = bgfx::TextureFormat::R32F;
        break;
    }
    case 2: {
        result.Format = bgfx::TextureFormat::RG32F;
        break;
    }
    case 3: {
        LOG_ERROR("[Asset] 3 BPP HDR not supported");
        break;
    }
    case 4: {
        result.Format = bgfx::TextureFormat::RGBA32F;
        break;
    }
    }

    result.Data = std::unique_ptr<float>(data);

    LOG_INFO("[Asset] {0} loaded ({1} width, {2} height, {3} format, {4} bits per pixel)", result.Width, result.Height, result.Format,
             result.BPP);
    return result;
}

Loader::ShaderLoadResult Loader::loadShader(LoadableHandle handle) const {
    LOG_TRACE("[Asset] Loading shader from {0}", handle);
    ShaderLoadResult result = {};
    DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
    if (chunk.Data.size() == 0) {
        LOG_ERROR("[Asset] {0} doesn't exist", handle);
        result.Error = "File doesn't exist";
        return result;
    }

    // First std::uint64_t is the size of vertex shader, following it is fragment shader to the end
    std::uint64_t vertexSize = 0;
    std::memcpy(&vertexSize, chunk.Data.data(), sizeof(std::uint64_t));

    const size_t fragmentSize = chunk.Data.size() - sizeof(std::uint64_t) - vertexSize;
    result.VertexSource.resize(vertexSize);
    result.FragmentSource.resize(fragmentSize);

    std::memcpy(result.VertexSource.data(), chunk.Data.data() + sizeof(std::uint64_t), vertexSize);
    std::memcpy(result.FragmentSource.data(), chunk.Data.data() + sizeof(std::uint64_t) + vertexSize, fragmentSize);

    LOG_INFO("[Asset] {0} loaded ({1} vertex shader size, {2} fragment shader size, {3} fragment shader start offset)", handle, vertexSize,
             fragmentSize, vertexSize);

    return result;
}

Loader::BinaryLoadResult Loader::loadBinary(LoadableHandle handle) const {
    LOG_TRACE("[Asset] Loading binary file from {0}", handle);
    Loader::BinaryLoadResult result = {};
    DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
    if (chunk.Data.size() == 0) {
        LOG_ERROR("[Asset] {0} doesn't exist", handle);
        result.Error = "File doesn't exist";
        return result;
    }

    LOG_INFO("[Asset] {0} loaded ({1} bytes)", chunk.Data.size());
    result.Content = chunk.Data;

    return result;
}

} // namespace io
} // namespace aderite
