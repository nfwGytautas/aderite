#include "RenderOperation.hpp"

#include <bgfx/bgfx.h>
#include <glm/gtc/type_ptr.hpp>

#include "aderite/utility/Log.hpp"
#include "aderite/rendering/DrawCall.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"
#include "aderite/rendering/operation/EyeProvideOperation.hpp"
#include "aderite/rendering/operation/EntityProvideOperation.hpp"
#include "aderite/scene/components/Components.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

static uint8_t nextViewId = 0;

RenderOperation::RenderOperation(EntityProvideOperation* entities, EyeProvideOperation* eye, TargetProvideOperation* target)
	: m_entities(entities), m_eye(eye), m_target(target)
{}

void RenderOperation::initialize() {
	m_viewId = nextViewId++;

	ADERITE_DEBUG_SECTION
	(
		bgfx::setViewName(m_viewId, p_debugName.c_str());
	)
}

void RenderOperation::execute() {
	if (!validateOperations()) {
		return;
	}

	// Render
	bgfx::setViewFrameBuffer(m_viewId, m_target->getHandle());
	bgfx::touch(m_viewId);

	// Set persistent matrices
	bgfx::setViewTransform(
		m_viewId,
		glm::value_ptr(m_eye->getViewMatrix()),
		glm::value_ptr(m_eye->getProjectionMatrix()));

	// Execute draw calls

	for (const DrawCall& dc : m_entities->getDrawCalls()) {
		//bgfx::discard(BGFX_DISCARD_ALL);
		executeDrawCall(dc);
	}
}

bool RenderOperation::validateOperations() {
	if (m_entities == nullptr ||
		m_eye == nullptr ||
		m_target == nullptr) {
		LOG_ERROR("Some operations are nullptr for render operation");
		return false;
	}

	if (!bgfx::isValid(m_target->getHandle())) {
		LOG_ERROR("Invalid target handle for render operation");
		return false;
	}

	if (!m_eye->isValid()) {
		LOG_ERROR("Invalid eye for render operation");
		return false;
	}

	return true;
}

void RenderOperation::executeDrawCall(const DrawCall& dc) {
	// Check if valid draw call
	if (dc.Skip) {
		return;
	}

	if (dc.Transformations.size() == 0) {
		// Nothing to render
		return;
	}
	
	// Uniform
	bgfx::setUniform(dc.MaterialUniform, dc.UniformData, UINT16_MAX);
	
	// Samplers
	for (size_t i = 0; i < dc.Samplers.size(); i++) {
		bgfx::setTexture(i, dc.Samplers[i].first, dc.Samplers[i].second);
	}
	
	// Bind buffers
	bgfx::setVertexBuffer(0, dc.VBO);
	bgfx::setIndexBuffer(dc.IBO);
	
	uint64_t state = 0
		| BGFX_STATE_WRITE_R
		| BGFX_STATE_WRITE_G
		| BGFX_STATE_WRITE_B
		| BGFX_STATE_WRITE_A
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_CULL_CCW
		| BGFX_STATE_MSAA
		;
	
	bgfx::setState(state);
	
	for (auto& transform : dc.Transformations) {
		bgfx::setTransform(glm::value_ptr(scene::components::TransformComponent::computeTransform(*transform)));
	
		// Submit draw call
		uint8_t flags = BGFX_DISCARD_ALL & ~(BGFX_DISCARD_BINDINGS | BGFX_DISCARD_INDEX_BUFFER | BGFX_DISCARD_VERTEX_STREAMS | BGFX_DISCARD_STATE);
		bgfx::submit(m_viewId, dc.Shader, 0, flags);
	}
}

ADERITE_RENDERING_NAMESPACE_END
