#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/pass/IPass.hpp"

ADERITE_PASS_NAMESPACE_BEGIN

/**
 * @brief Image based lighting pass
*/
class IblPass : public interfaces::IPass {
public:
	// Inherited via IPass
	virtual bool init(rendering::uniform::UniformManager* um) override;
	virtual void shutdown() override;
	virtual void pass(interfaces::ICamera* eye) override;
	virtual void onWindowResized(unsigned int newWidth, unsigned int newHeight) override;
private:
	const bgfx::ViewId m_iblView = 6;
};

ADERITE_PASS_NAMESPACE_END
