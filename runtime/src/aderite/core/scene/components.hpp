#pragma once

/**
 * @brief All components supported natively by aderite are described in this file
*/

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace aderite {
	namespace asset {
		class mesh_asset;
		class material_asset;
	}

	namespace scene {
		namespace components {

			/**
			 * @brief Component containing all meta information about an entity, such as it's name, tags, etc.
			*/
			struct meta {
				std::string Name;

				meta() = default;
				meta(const meta&) = default;
				meta(const std::string& name)
					: Name(name) {}
			};

			/**
			 * @brief Transform component, used to situate an entity in world space
			*/
			struct transform {
				glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
				glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
				glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

				transform() = default;
				transform(const transform&) = default;
				transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
					: Position(position), Rotation(rotation), Scale(scale) {}

				/**
				 * @brief Computes the transformation matrix of the component
				 * @param t Transform component
				 * @return Computed transformation matrix
				*/
				static glm::mat4 compute_transform(const transform& t) {
					glm::mat4 rotation = glm::toMat4(glm::quat(t.Rotation));
					return glm::translate(glm::mat4(1.0f), t.Position) * rotation * glm::scale(glm::mat4(1.0f), t.Scale);
				}
			};

			/**
			 * @brief Mesh rendering component, this component contains information needed for rendering meshes
			*/
			struct mesh_renderer {
				asset::mesh_asset* MeshHandle = nullptr;
				asset::material_asset* MaterialHandle = nullptr;

				mesh_renderer() = default;
				mesh_renderer(const mesh_renderer&) = default;
				mesh_renderer(asset::mesh_asset* mesh, asset::material_asset* material)
					: MeshHandle(mesh), MaterialHandle(material) {}
			};

			/**
			 * @brief Camera component, which basically references a specific camera settings asset
			*/
			struct camera {
				glm::vec3 Position = { 0.0f, 0.0f, 0.0f };

				camera() = default;
				camera(const camera&) = default;
			};
		}
	}
}
