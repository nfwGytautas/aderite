#pragma once

#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"

ADERITE_UNIFORM_NAMESPACE_BEGIN

/**
 * @brief Uniform base class
*/
class Uniform {
public:
	virtual ~Uniform();

	/**
	 * @brief Create uniform
	*/
	void create();

	/**
	 * @brief Destroy uniform
	*/
	void destroy();

	/**
	 * @brief Update uniform handle with data
	*/
	void update();

	/**
	 * @brief Returns the type of the uniform
	*/
	virtual bgfx::UniformType::Enum getType() const = 0;

	/**
	 * @brief Returns the name of the uniform
	*/
	virtual const char* getName() const = 0;

	/**
	 * @brief Returns the number of elements in the uniform of type
	*/
	virtual uint8_t getECount() const = 0;

	/**
	 * @brief Returns the data of the uniform
	*/
	virtual const void* getData() const = 0;
private:
	bgfx::UniformHandle m_handle = BGFX_INVALID_HANDLE;
};

ADERITE_UNIFORM_NAMESPACE_END
