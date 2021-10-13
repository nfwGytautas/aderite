#pragma once

#include <vector>
#include "aderite/rendering/operation/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/compiler/Forward.hpp"
#include "aderiteeditor/compiler/GraphEvaluator.hpp"

ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN

/**
 * @brief Class used to evaluate a render pipeline graph
*/
class PipelineEvaluator : public GraphEvaluator
{
public:
	~PipelineEvaluator();

	/**
	 * @brief Adds operation to the pipeline evaluator, takes ownership
	 * @param operation Operation to add
	*/
	EvaluatorValue addOperation(rendering::OperationBase* operation);

	/**
	 * @brief Returns operation that is tied to the evaluator value
	 * @param value Value of the operation
	 * @return OperationBase instance
	*/
	rendering::OperationBase* getOperation(EvaluatorValue value);

	/**
	 * @brief Outputs the order of the operations to the log
	*/
	void logOrder();
private:
	std::vector<rendering::OperationBase*> m_operations;
};

ADERITE_EDITOR_COMPILER_NAMESPACE_END
