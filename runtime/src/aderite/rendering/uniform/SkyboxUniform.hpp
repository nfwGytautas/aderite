#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/uniform/Uniform.hpp"

ADERITE_UNIFORM_NAMESPACE_BEGIN

/**
 * @brief Skybox uniform
*/
class SkyboxUniform final : public Uniform {
public:
	static constexpr uint8_t c_Count = 5;

	// Inherited via Uniform
	virtual bgfx::UniformType::Enum getType() const override;
	virtual const char* getName() const override;
	virtual uint8_t getECount() const override;
	virtual const void* getData() const override;

	union
	{
		struct
		{
			union
			{
				float mtx[16];

				/* 0*/ struct { float mtx0[4]; };
				/* 1*/ struct { float mtx1[4]; };
				/* 2*/ struct { float mtx2[4]; };
				/* 3*/ struct { float mtx3[4]; };
			};
			/* 4*/ struct { float exposure, bgType, unused[2]; };
		};

		float Params[c_Count * 4];
	};
};

ADERITE_UNIFORM_NAMESPACE_END
