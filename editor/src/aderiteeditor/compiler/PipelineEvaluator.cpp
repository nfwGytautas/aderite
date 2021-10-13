#include "PipelineEvaluator.hpp"

#include "aderite/utility/Log.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"

ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN

PipelineEvaluator::~PipelineEvaluator() {
	if (!m_pipelineConstructed) {
		for (rendering::OperationBase* op : m_operations) {
			delete op;
		}
	}
}

EvaluatorValue PipelineEvaluator::addOperation(rendering::OperationBase* operation) {
	m_operations.push_back(operation);
	return m_operations.size() - 1;
}

rendering::OperationBase* PipelineEvaluator::getOperation(EvaluatorValue value) {
	return m_operations[value];
}

rendering::Pipeline* PipelineEvaluator::constructPipeline() {
	if (m_pipelineConstructed) {
		LOG_ERROR("Tried to construct pipeline a second time from the same evaluator");
		return nullptr;
	}
	rendering::Pipeline* pipeline = new rendering::Pipeline();
	for (rendering::OperationBase* op : m_operations) {
		pipeline->addOperation(op);
	}
	m_pipelineConstructed = true;
	return pipeline;
}

ADERITE_EDITOR_COMPILER_NAMESPACE_END
