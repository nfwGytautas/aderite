#include "RenderOperation.hpp"

#include <bgfx/bgfx.h>
#include <glm/gtc/type_ptr.hpp>

#include "aderite/utility/Log.hpp"
#include "aderite/rendering/DrawCall.hpp"
#include "aderite/rendering/PipelineState.hpp"
#include "aderite/scene/components/Components.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

static uint8_t nextViewId = 0;

RenderOperation::RenderOperation() {}

void RenderOperation::initialize() {
	m_viewId = nextViewId++;
	bgfx::setViewClear(m_viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x252525FF, 1.0f, 0);

	ADERITE_DEBUG_SECTION
	(
		bgfx::setViewName(m_viewId, this->getName().c_str());
	)
}

void RenderOperation::execute(PipelineState* state) {
	bgfx::FrameBufferHandle target = state->popTarget();
	EyeInformation eye = state->popEye();

	if (!eye.Valid) {
		return;
	}

	if (!bgfx::isValid(target)) {
		return;
	}

	// Render
	bgfx::setViewFrameBuffer(m_viewId, target);
	bgfx::touch(m_viewId);

	// Set persistent matrices
	bgfx::setViewTransform(
		m_viewId,
		glm::value_ptr(eye.ViewMatrix),
		glm::value_ptr(eye.ProjectionMatrix));

	// Execute draw calls

	for (const DrawCall& dc : *state->getDrawCallList()) {
		//bgfx::discard(BGFX_DISCARD_ALL);
		executeDrawCall(dc);
	}
}

reflection::Type RenderOperation::getType() const {
	return static_cast<reflection::Type>(reflection::RuntimeTypes::OP_RENDER);
}

bool RenderOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "ViewId" << YAML::Value << (int)m_viewId;
	return true;
}

bool RenderOperation::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	m_viewId = data["ViewId"].as<uint8_t>();

	if (m_viewId > nextViewId) {
		nextViewId = m_viewId + 1;
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
