#pragma once

#include "aderite/utility/Macros.hpp"
#include "aderite/physics/Collider.hpp"

ADERITE_COLLIDER_NAMESPACE_BEGIN

/**
 * @brief Simple box collider
*/
class BoxCollider : public Collider {
public:
	BoxCollider();
	virtual ~BoxCollider() {}

	// Inherited via Collider
	virtual void setScale(const glm::vec3& scale) override;

	/**
	 * @brief Returns the size of the box collider
	 * @return Box collider size
	*/
	glm::vec3 getSize() const;

	/**
	 * @brief Sets the size of the box collider
	 * @param size New size of the box collider
	*/
	void setSize(const glm::vec3 size);

	// Inherited via Collider
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;
	virtual ColliderType getType() const override;
private:
	glm::vec3 m_size = glm::vec3(1.0f);
};

ADERITE_COLLIDER_NAMESPACE_END
