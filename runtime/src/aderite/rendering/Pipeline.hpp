#pragma once

#include <vector>
#include "aderite/utility/Macros.hpp"
#include "aderite/interfaces/ISerializable.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Rendering pipeline object, the rendering engine loops through the pipeline
 * operations executing them one by one
*/
class Pipeline
{
public:
	~Pipeline();

	/**
	 * @brief Adds operation to the pipeline, the pipeline takes ownership of the operation
	 * NOTE: This adds the operation to the end of the operation array
	 *       since ordering is important the user must ensure the correct
	 *       order themselves
	 * @param operation Operation to add
	*/
	void addOperation(OperationBase* operation);

	/**
	 * @brief Initialize pipeline and all of it's operations, allocating resources
	*/
	void initialize();

	/**
	 * @brief Execute the pipeline
	*/
	void execute() const;

	/**
	 * @brief Shutdown the pipeline and all of it's operations, freeing resources
	*/
	void shutdown();

	ADERITE_DEBUG_SECTION
	(
		void logPipeline() const;
	)
private:
	std::vector<OperationBase*> m_operations;

};

ADERITE_RENDERING_NAMESPACE_END
