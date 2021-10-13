#include "PipelineEvaluator.hpp"

#include "aderite/rendering/operation/OperationBase.hpp"

ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN

PipelineEvaluator::~PipelineEvaluator() {
	for (rendering::OperationBase* op : m_operations) {
		delete op;
	}
}

EvaluatorValue PipelineEvaluator::addOperation(rendering::OperationBase* operation) {
	m_operations.push_back(operation);
	return m_operations.size() - 1;
}

rendering::OperationBase* PipelineEvaluator::getOperation(EvaluatorValue value) {
	return m_operations[value];
}

void PipelineEvaluator::logOrder() {
	for (rendering::OperationBase* op : m_operations) {
		op->execute();
	}
}

ADERITE_EDITOR_COMPILER_NAMESPACE_END
