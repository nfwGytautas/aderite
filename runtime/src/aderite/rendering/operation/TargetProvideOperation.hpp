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
	 * @brief Returns the provider handle
	*/
	bgfx::FrameBufferHandle getHandle() const;
	
	// Inherited via OperationBase
	virtual void initialize() override;
	virtual void shutdown() override;
private:
	bgfx::FrameBufferHandle m_handle = BGFX_INVALID_HANDLE;
};

ADERITE_RENDERING_NAMESPACE_END
