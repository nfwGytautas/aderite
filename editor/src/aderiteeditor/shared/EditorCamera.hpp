#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "aderiteeditor/utility/Macros.hpp"
#include "aderite/scene/ICamera.hpp"

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

class EditorCamera : public interfaces::ICamera {
public:
	EditorCamera();
	virtual ~EditorCamera();

	/**
	 * @brief Called when scene view is resized
	 * @param size New size
	*/
	void onViewportResize(const glm::uvec2& size);

	// Inherited via ICamera
	virtual bgfx::FrameBufferHandle getOutputHandle() override;
	virtual glm::mat4 computeViewMatrix() override;
	virtual glm::mat4 computeProjectionMatrix() override;
	virtual void update(float delta) override;
	virtual bool isEnabled() override;

private:
	glm::vec3 getUpDirection() const;
	glm::vec3 getRightDirection() const;
	glm::vec3 getForwardDirection() const;
	glm::vec3 calculatePosition() const;
	glm::quat getOrientation() const;

	void updateViewMatrix();

private:
	bgfx::FrameBufferHandle m_target = BGFX_INVALID_HANDLE;
	glm::uvec2 m_viewportSize = {};
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	glm::vec3 m_focalpoint = {};
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;
	float m_distance = 10.0f;
};

ADERITE_EDITOR_SHARED_NAMESPACE_END
