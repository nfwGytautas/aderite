#include "PipelineState.hpp"

#include "aderite/utility/Log.hpp"

namespace aderite {
namespace rendering {

void PipelineState::setDrawCallList(DrawCallList* dcl) {
    m_drawCalls = dcl;
}

DrawCallList* PipelineState::getDrawCallList() const {
    return m_drawCalls;
}

void PipelineState::pushTarget(bgfx::FrameBufferHandle target) {
    m_targets.push(target);
}

void PipelineState::pushEye(EyeInformation eye) {
    m_eyes.push(eye);
}

bgfx::FrameBufferHandle PipelineState::popTarget() {
    bgfx::FrameBufferHandle handle = m_targets.top();
    m_targets.pop();
    return handle;
}

EyeInformation PipelineState::popEye() {
    EyeInformation eye = m_eyes.top();
    m_eyes.pop();
    return eye;
}

void PipelineState::reset() {
    if (m_eyes.size() > 0) {
        LOG_WARN("[Rendering] Not all eyes was used in last pipeline cycle");
        m_eyes = {};
    }

    if (m_targets.size() > 0) {
        LOG_WARN("[Rendering] Not all targets was used in last pipeline cycle");
        m_targets = {};
    }
}

bool PipelineState::canExecuteRender() const {
    return m_eyes.size() > 0 && m_targets.size() > 0 && m_drawCalls != nullptr;
}

} // namespace rendering
} // namespace aderite
