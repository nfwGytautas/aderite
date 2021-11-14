#pragma once

#include <stack>
#include <vector>

#include <bgfx/bgfx.h>

#include "aderite/rendering/DrawCall.hpp"

namespace aderite {
namespace rendering {

using DrawCallList = std::vector<DrawCall>;

/**
 * @brief Struct containing data about an Eye object
 */
struct EyeInformation {
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    bool Valid = true;
};

/**
 * @brief Object used to hold the current state of the pipeline
 */
class PipelineState final {
public:
    /**
     * @brief Sets the draw call list of the pipeline
     * @param dcl DrawCallList instance
     */
    void setDrawCallList(DrawCallList* dcl);

    /**
     * @brief Returns the draw call list of the pipeline
     */
    DrawCallList* getDrawCallList() const;

    /**
     * @brief Pushes a target to the pipeline state
     * @param target Target to render
     */
    void pushTarget(bgfx::FrameBufferHandle target);

    /**
     * @brief Pushes an eye to the pipeline
     * @param eye Eye to push
     */
    void pushEye(EyeInformation eye);

    /**
     * @brief Pops a target and returns it
     * @return Target handle
     */
    bgfx::FrameBufferHandle popTarget();

    /**
     * @brief Pops an eye from the stack and returns it
     */
    EyeInformation popEye();

    /**
     * @brief Resets the pipeline state, preparing it for next frame
     */
    void reset();

    /**
     * @brief Returns true if a render operation can be executed, false otherwise
    */
    bool canExecuteRender() const;

private:
    DrawCallList* m_drawCalls = nullptr;
    std::stack<bgfx::FrameBufferHandle> m_targets;
    std::stack<EyeInformation> m_eyes;
};

} // namespace rendering
} // namespace aderite
