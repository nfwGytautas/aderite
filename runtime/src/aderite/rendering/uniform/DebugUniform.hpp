#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/uniform/Uniform.hpp"

ADERITE_UNIFORM_NAMESPACE_BEGIN

/**
 * @brief Debug uniform
*/
class DebugUniform final : public Uniform {
public:
	static constexpr uint8_t c_Count = 2;

	// Inherited via Uniform
	virtual bgfx::UniformType::Enum getType() const override;
	virtual const char* getName() const override;
	virtual uint8_t getECount() const override;
	virtual const void* getData() const override;

	// Uniform data
	union
	{
		struct
		{
			/*0*/struct { float wfColor[3]; float wfOpacity; };
			/*1*/struct { float wfThickness, unused[3]; };
		};

		float wfParams[c_Count * 4]; // 2 vec4
	};
};

ADERITE_UNIFORM_NAMESPACE_END
