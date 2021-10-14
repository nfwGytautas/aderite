#include "OperationArray.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

OperationArray::OperationArray()
{}

void OperationArray::addOperation(rendering::OperationBase* op) {
	m_operations.push_back(op);
}

size_t OperationArray::size() const {
	return m_operations.size();
}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
