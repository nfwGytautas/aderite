#pragma once

/**
 * @brief All components supported natively by aderite are described in this file
*/

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderite/utility/Macros.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/physics/Forward.hpp"

ADERITE_COMPONENTS_NAMESPACE_BEGIN

/**
 * @brief Component containing all MetaComponent information about an Entity, such as it's name, tags, etc.
*/
struct MetaComponent {
	std::string Name;

	MetaComponent() = default;
	MetaComponent(const MetaComponent&) = default;
	MetaComponent(const std::string& name)
		: Name(name) {}
};

/**
 * @brief Transform component, used to situate an Entity in world space
*/
struct TransformComponent {
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
		: Position(position), Rotation(rotation), Scale(scale) {}

	/**
	 * @brief Computes the transformation matrix of the component
	 * @param t Transform component
	 * @return Computed transformation matrix
	*/
	static glm::mat4 compute_transform(const TransformComponent& t) {
		glm::mat4 rotation = glm::toMat4(glm::quat(t.Rotation));
		return glm::translate(glm::mat4(1.0f), t.Position) * rotation * glm::scale(glm::mat4(1.0f), t.Scale);
	}
};

/**
 * @brief Mesh rendering component, this component contains information needed for rendering meshes
*/
struct MeshRendererComponent {
	asset::MeshAsset* MeshHandle = nullptr;
	asset::MaterialAsset* MaterialHandle = nullptr;

	MeshRendererComponent() = default;
	MeshRendererComponent(const MeshRendererComponent&) = default;
	MeshRendererComponent(asset::MeshAsset* mesh, asset::MaterialAsset* material)
		: MeshHandle(mesh), MaterialHandle(material) {}
};

/**
 * @brief Camera component, which references ICamera object
*/
struct CameraComponent {
	EntityCamera* Camera = nullptr;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};

/**
 * @brief Rigid body component used for physics
*/
struct RigidbodyComponent {
	physics::Rigidbody* Body = nullptr;

	RigidbodyComponent() = default;
	RigidbodyComponent(const RigidbodyComponent&) = default;
};

/**
 * @brief Component containing all colliders attached to this object
*/
struct CollidersComponent {
	physics::ColliderList* Colliders = nullptr;

	CollidersComponent() = default;
	CollidersComponent(const CollidersComponent&) = default;
};

ADERITE_COMPONENTS_NAMESPACE_END
