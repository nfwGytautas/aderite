#pragma once

#include <array>
#include <bgfx/bgfx.h>
#include "aderite/Config.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/uniform/Forward.hpp"

ADERITE_UNIFORM_NAMESPACE_BEGIN

/**
 * @brief Uniform manager
*/
class UniformManager {
public:
	/**
	 * @brief Initialize uniform manager
	 * @return True if initialized without problems, false otherwise
	*/
	bool init();

	/**
	 * @brief Shutdown and free all uniforms
	*/
	void shutdown();

	/**
	 * @brief Returns the uniform
	 * @tparam T Uniform type
	*/
	template<typename T>
	T* getUniform() const;
private:
	static constexpr uint8_t c_NumUniforms = 1;
	std::array<Uniform*, c_NumUniforms> m_uniforms;

#if DEBUG_RENDER == 1
	Uniform* m_debugUniform = nullptr;
#endif
};

ADERITE_UNIFORM_NAMESPACE_END
