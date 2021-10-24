#pragma once

#include <string>
#include "aderite/utility/Macros.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/io/SerializableObject.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Rendering operation base class
*/
class OperationBase : public io::ISerializable {
public:
	virtual ~OperationBase() {};

	/**
	 * @brief Initialize the operation
	*/
	virtual void initialize() {};

	/**
	 * @brief Execute the operation
	*/
	virtual void execute(PipelineState* state) {};

	/**
	 * @brief Free all resources
	*/
	virtual void shutdown() {};

	/**
	 * @brief Sets the name of the operation, should be unique pipeline wide
	 * @param name New name of the operation
	*/
	void setName(const std::string& name);

	/**
	 * @brief Returns the name of the operation
	*/
	const std::string& getName() const;
private:
	std::string m_name = "";
};

ADERITE_RENDERING_NAMESPACE_END
