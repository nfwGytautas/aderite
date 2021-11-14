#pragma once

#include <bgfx/bgfx.h>

#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace rendering {
/**
 * @brief Operation used to provide a rendering target
 */
class TargetProvideOperation ADERITE_MIDDLEWARE_FINAL : public OperationBase {
public:
    /**
     * @brief Target parameters when creating the provider
     */
    struct TargetParams {
        /**
         * @brief If true then the target will be created with BGFX_TEXTURE_BLIT_DST flag
         * which is needed for targets that will be blitted into
         */
        bool Blittable;

        /**
         * @brief If true then the target will have a depth attachment
         */
        bool DepthAttachment;

        /**
         * @brief If true then the target will have a stencil attachment
         */
        bool StencilAttachment;

        /**
         * @brief If true then the attachments will be created to support HDR values
         */
        bool HDR;

        /**
         * @brief Construct a new Target Params object
         */
        TargetParams() {
            Blittable = false;
            DepthAttachment = true;
            StencilAttachment = true;
            HDR = false;
        }
    };

    // Indexes

    /**
     * @brief Color attachment index used for bgfx::getTexture function
     */
    static constexpr uint8_t c_ColorAttachment0 = 0;

    /**
     * @brief Depth attachment index used for bgfx::getTexture function
     */
    static constexpr uint8_t c_DepthAttachment = 1;

    /**
     * @brief Depth and stencil attachment index used for bgfx::getTexture function
     */
    static constexpr uint8_t c_DepthStencilAttachment = 1;

public:
    TargetProvideOperation(const TargetParams& params = TargetParams());
    ADERITE_MIDDLEWARE_VIRTUAL ~TargetProvideOperation();

    /**
     * @brief Returns the provider handle
     */
    bgfx::FrameBufferHandle getHandle() const;

    // Inherited via OperationBase
    void initialize() override;
    void execute(PipelineState* state) override;
    void shutdown() override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

private:
    void createFramebuffer();

private:
    TargetParams m_params;
    bgfx::FrameBufferHandle m_handle = BGFX_INVALID_HANDLE;
};

} // namespace rendering
} // namespace aderite
