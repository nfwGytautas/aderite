#include "mesh_source.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Logger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Exporter.hpp>

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"

namespace aderite {
	namespace asset {

		class AssimpLogSource : public Assimp::Logger
		{
			virtual void OnDebug(const char* message) override {
				LOG_DEBUG("ASSIMP: {0}", message);
			}
			virtual void OnInfo(const char* message) override {
				LOG_TRACE("ASSIMP: {0}", message);
			}
			virtual void OnWarn(const char* message) override {
				LOG_WARN("ASSIMP: {0}", message);
			}
			virtual void OnError(const char* message) override {
				LOG_ERROR("ASSIMP: {0}", message);
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

		mesh_source::mesh_source(std::filesystem::path file) 
			: m_file(file)
		{
			if (!std::filesystem::exists(m_file)) {
				LOG_WARN("Tried to load {0} but such file doesn't exist", m_file.string());
				m_error = load_error::BAD_FILE;
				return;
			}

			if (!g_LoggerSet) {
				Assimp::DefaultLogger::set(&g_LogSource);
				g_LoggerSet = true;
			}
		}

		void mesh_source::request_position_data() {
			m_willLoadPosition = true;
		}

		void mesh_source::request_indices_data() {
			m_willLoadIndices = true;
		}

		void mesh_source::load() {
			m_positionBuffer.clear();
			m_indicesBuffer.clear();

			// Check if there were any errors up until now
			if (m_error != load_error::NONE) {
				return;
			}

			Assimp::Importer importer;
			unsigned int flags = 0;

			// Default flags
			flags = aiProcessPreset_TargetRealtime_Quality |                     // some optimizations and safety checks
				aiProcess_OptimizeMeshes |                                   // minimize number of meshes
				aiProcess_PreTransformVertices |                             // apply node matrices
				aiProcess_FixInfacingNormals | aiProcess_TransformUVCoords | // apply UV transformations
				//aiProcess_FlipWindingOrder   | // we cull clock-wise, keep the default CCW winding order
				aiProcess_MakeLeftHanded | // we set GLM_FORCE_LEFT_HANDED and use left-handed bx matrix functions
				aiProcess_FlipUVs |
				0;

			//if (calculateTangents)
			//{
			//	flags = flags |
			//		aiProcess_CalcTangentSpace;
			//}

			const aiScene* scene = importer.ReadFile(
				m_file.string(),
				flags
			);

			// Check if the scene was loaded
			if (!scene) {
				m_error = load_error::BAD_FORMAT;
				return;
			}

			// Start filling the data
			if (scene->mNumMeshes > 1) {
				LOG_ERROR("Multiple mesh models not supported, first call asset manager mesh split API and then select individual");
				m_error = load_error::UNSUPPORTED_CONFIGURATION;
				return;
			}

			if (scene->mNumMeshes == 0) {
				LOG_ERROR("File {0} contains no meshes", m_file.string());
				m_error = load_error::BAD_FORMAT;
				return;
			}

			// There should only be one mesh
			aiMesh* mesh = scene->mMeshes[0];

			// Reserve memory for the mesh
			if (m_willLoadPosition) {
				m_positionBuffer.reserve(mesh->mNumVertices * 3);

				// Load vertices
				for (unsigned int verticeIdx = 0; verticeIdx < mesh->mNumVertices; verticeIdx++) {
					m_positionBuffer.push_back(mesh->mVertices[verticeIdx].x);
					m_positionBuffer.push_back(mesh->mVertices[verticeIdx].y);
					m_positionBuffer.push_back(mesh->mVertices[verticeIdx].z);
				}
			}

			// Load indices
			if (m_willLoadIndices) {
				for (unsigned int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) {
					// Get the face
					aiFace& face = mesh->mFaces[faceIdx];

					// Add the indices of the face to the vector
					for (unsigned int indiceIdx = 0; indiceIdx < face.mNumIndices; indiceIdx++) {
						m_indicesBuffer.push_back(face.mIndices[indiceIdx]);
					}
				}
			}
		}

		std::vector<float>& mesh_source::position_data() {
			return m_positionBuffer;
		}

		std::vector<unsigned int>& mesh_source::indices_data() {
			return m_indicesBuffer;
		}
	}
}
