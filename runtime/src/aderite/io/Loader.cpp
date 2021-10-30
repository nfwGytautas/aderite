#include "Loader.hpp"

#include <functional>

// Mesh loading
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Logger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Exporter.hpp>

// Texture loading
#include <bgfx/bgfx.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/FileHandler.hpp"

class AssimpLogSource : public Assimp::Logger {
	virtual void OnDebug(const char* message) override {
		LOG_DEBUG("ASSIMP {0}", message);
	}
	virtual void OnInfo(const char* message) override {
		LOG_TRACE("ASSIMP {0}", message);
	}
	virtual void OnWarn(const char* message) override {
		LOG_WARN("ASSIMP {0}", message);
	}
	virtual void OnError(const char* message) override {
		LOG_ERROR("ASSIMP {0}", message);
	}
	virtual bool attachStream(Assimp::LogStream* pStream, unsigned int severity) override {
		delete pStream;
		return true;
	}
	virtual bool detachStream(Assimp::LogStream* pStream, unsigned int severity = Debugging | Err | Warn | Info) override {
		return false;
	}
	virtual void OnVerboseDebug(const char* message) override {}
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

Loader::Loader(LoaderPool* pool) 
	: m_pool(pool), m_impl(new LoaderImpl())
{
	if (!g_LoggerSet) {
		Assimp::DefaultLogger::set(&g_LogSource);
		g_LoggerSet = true;
	}
}

Loader::~Loader() {
	m_thread.join();
	delete m_impl;
}

void Loader::startup() {
	m_thread = std::thread([&]() {
		while (!m_terminated) {
			ILoadable* loadable = m_pool->getNextLoadable();
			if (loadable != nullptr) {
				m_impl->Current = loadable;
				loadable->load(this);
				m_impl->Current = nullptr;
			}
		}
	});
}

void Loader::terminate() {
	m_terminated = true;
}

ILoadable* Loader::current() const {
	return m_impl->Current;
}

Loader::MeshLoadResult Loader::loadMesh(LoadableHandle handle) const {
	DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
	if (chunk.Data.size() == 0) {
		return {
			"File doesn't exist"
		};
	}

	unsigned int flags = 0;

	// Default flags
	flags = aiProcessPreset_TargetRealtime_Quality |                 // some optimizations and safety checks
		aiProcess_OptimizeMeshes |                                   // minimize number of meshes
		aiProcess_PreTransformVertices |                             // apply node matrices
		aiProcess_FixInfacingNormals | aiProcess_TransformUVCoords | // apply UV transformations
		//aiProcess_FlipWindingOrder   | // we cull clock-wise, keep the default CCW winding order
		aiProcess_MakeLeftHanded | // we set GLM_FORCE_LEFT_HANDED and use left-handed bx matrix functions
		aiProcess_FlipUVs |
		//aiProcess_GenNormals |
		0;

	const aiScene* scene = m_impl->Importer.ReadFileFromMemory(
		chunk.Data.data(),
		chunk.Data.size(),
		flags,
		"obj"
	);

	// Sanity checks
	if (scene == nullptr) {
		return {
			m_impl->Importer.GetErrorString()
		};
	}

	if (scene->mNumMeshes > 1) {
		return {
			"File contains more than 1 mesh"
		};
	}

	if (scene->mNumMeshes == 0) {
		return {
			"File contains no meshes"
		};
	}

	// There should only be one mesh
	aiMesh* mesh = scene->mMeshes[0];

	MeshLoadResult result = {};

	// Reserve memory for the mesh
	result.Vertices.reserve(mesh->mNumVertices * 3 * 3 * 2);

	// Load vertices
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
	for (unsigned int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) {
		// Get the face
		aiFace& face = mesh->mFaces[faceIdx];

		// Add the indices of the face to the vector
		for (unsigned int indiceIdx = 0; indiceIdx < face.mNumIndices; indiceIdx++) {
			result.Indices.push_back(face.mIndices[indiceIdx]);
		}
	}

	return result;
}

Loader::TextureLoadResult<unsigned char> Loader::loadTexture(LoadableHandle handle) const {
	DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
	if (chunk.Data.size() == 0) {
		return {
			"File doesn't exist"
		};
	}

	TextureLoadResult<unsigned char> result = {};
	unsigned char* data = stbi_load_from_memory(chunk.Data.data(), chunk.Data.size(), &result.Width, &result.Height, &result.BPP, 0);

	if (data == nullptr) {
		result.Error = stbi_failure_reason();
		return result;
	}

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
	return result;
}

Loader::TextureLoadResult<float> Loader::loadHdrTexture(LoadableHandle handle) const {
	DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
	if (chunk.Data.size() == 0) {
		return {
			"File doesn't exist"
		};
	}

	TextureLoadResult<float> result = {};
	float* data = stbi_loadf_from_memory(chunk.Data.data(), chunk.Data.size(), &result.Width, &result.Height, &result.BPP, 4);

	if (data == nullptr) {
		result.Error = stbi_failure_reason();
		return result;
	}

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
		LOG_FATAL("3 BPP HDR not supported");
		break;
	}
	case 4: {
		result.Format = bgfx::TextureFormat::RGBA32F;
		break;
	}
	}

	result.Data = std::unique_ptr<float>(data);
	return result;
}

Loader::ShaderLoadResult Loader::loadShader(LoadableHandle handle) const {
	DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
	if (chunk.Data.size() == 0) {
		return {
			"File doesn't exist"
		};
	}

	// First std::uint64_t is the size of vertex shader, following it is fragment shader to the end
	std::uint64_t vertexSize = 0;
	std::memcpy(&vertexSize, chunk.Data.data(), sizeof(std::uint64_t));

	ShaderLoadResult result = {};
	const size_t fragmentSize =  chunk.Data.size() - sizeof(std::uint64_t) - vertexSize;
	result.VertexSource.resize(vertexSize);
	result.FragmentSource.resize(fragmentSize);

	std::memcpy(result.VertexSource.data(), chunk.Data.data() + sizeof(std::uint64_t), vertexSize);
	std::memcpy(result.FragmentSource.data(), chunk.Data.data() + sizeof(std::uint64_t) + vertexSize, fragmentSize);

	return result;
}

Loader::BinaryLoadResult Loader::loadBinary(LoadableHandle handle) const {
	DataChunk chunk = ::aderite::Engine::getFileHandler()->openLoadable(handle);
	if (chunk.Data.size() == 0) {
		return {
			"File doesn't exist"
		};
	}

	return {
		"",
		chunk.Data
	};
}
	
}
}
