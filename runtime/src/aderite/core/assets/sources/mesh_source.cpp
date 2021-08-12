#include "mesh_source.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"

namespace aderite {
	namespace asset {
		mesh_source::mesh_source(std::filesystem::path file) 
			: m_file(file)
		{
			if (!std::filesystem::exists(m_file)) {
				LOG_WARN("Tried to load {0} but such file doesn't exist", m_file.string());
				m_error = load_error::BAD_FILE;
				return;
			}
		}

		void mesh_source::request_position_data() {
			m_willLoadPosition = true;
		}

		void mesh_source::request_indices_data() {
			m_willLoadIndices = true;
		}

		void mesh_source::load() {
			// Check if there were any errors up until now
			if (m_error != load_error::NONE) {
				return;
			}

			Assimp::Importer importer;
			unsigned int flags = 0;

			// Default flags
			flags = flags |
				aiProcess_OptimizeGraph |
				aiProcess_OptimizeMeshes |
				aiProcess_Triangulate |
				aiProcess_GenNormals |
				aiProcess_JoinIdenticalVertices;

			// If the platform is windows then additional processing is needed
#if DX_BACKEND == 1
			flags = flags |
				aiProcess_MakeLeftHanded |
				aiProcess_FlipWindingOrder;
#endif

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
			}

			// Load vertices
			for (unsigned int verticeIdx = 0; verticeIdx < mesh->mNumVertices; verticeIdx++) {
				if (m_willLoadPosition) {
					m_positionBuffer.push_back(mesh->mVertices[verticeIdx].x);
					m_positionBuffer.push_back(mesh->mVertices[verticeIdx].y);
					m_positionBuffer.push_back(mesh->mVertices[verticeIdx].z);
				}
			}

			// Load indices
			if (m_willLoadIndices) {
				for (unsigned int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) {
					// Get the face
					aiFace face = mesh->mFaces[faceIdx];

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
