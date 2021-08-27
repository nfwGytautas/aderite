#pragma once

#include <entt/Entity/registry.hpp>
#include "aderite/utility/Macros.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/ICamera.hpp"
#include "aderite/interfaces/ISerializable.hpp"

ADERITE_SCENE_NAMESPACE_BEGIN

class EntityCamera : public interfaces::ICamera, public interfaces::ISerializable {
public:
	/**
	 * @brief Create EntityCamera instance
	 * @param on Entity on which the camera is created on
	*/
	EntityCamera(Entity on);

	// Inherited via ICamera
	virtual bgfx::FrameBufferHandle getOutputHandle() override;
	virtual glm::mat4 computeViewMatrix() override;
	virtual glm::mat4 computeProjectionMatrix() override;
	virtual void update(float delta) override;
	virtual bool isEnabled() override;

	// Inherited via ISerializable
	virtual bool serialize(YAML::Emitter& out) override;
	virtual bool deserialize(YAML::Node& data) override;

private:
	Entity m_on = Entity::null();
};

ADERITE_SCENE_NAMESPACE_END
