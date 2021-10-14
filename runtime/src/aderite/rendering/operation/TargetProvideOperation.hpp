#pragma once

#include <bgfx/bgfx.h>
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to provide a rendering target
*/
class TargetProvideOperation : public OperationBase {
public:
	/**
	 * @brief Target parameters when creating the provider
	*/
	struct TargetParams {
		/**
		 * @brief If true then the target will be created with BGFX_TEXTURE_BLIT_DST flag
		 * which is needed for targets that will be blitted into
		*/
		bool Blittable = false;

		/**
		 * @brief If true then the target will have a depth attachment
		*/
		bool DepthAttachment = true;

		/**
		 * @brief If true then the target will have a stencil attachment
		*/
		bool StencilAttachment = true;

		/**
		 * @brief If true then the attachments will be created to support HDR values
		*/
		bool HDR = false;
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
	TargetProvideOperation(const TargetParams& params = {});
	virtual ~TargetProvideOperation();

	/**
	 * @brief Returns the provider handle
	*/
	bgfx::FrameBufferHandle getHandle() const;
	
	// Inherited via OperationBase
	virtual void initialize() override;
	virtual void shutdown() override;

private:
	void createFramebuffer();
private:
	TargetParams m_params;
	bgfx::FrameBufferHandle m_handle = BGFX_INVALID_HANDLE;
};

ADERITE_RENDERING_NAMESPACE_END
