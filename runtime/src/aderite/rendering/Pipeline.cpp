#include "Pipeline.hpp"

#include "aderite/rendering/operation/DebugOperation.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

Pipeline::~Pipeline() {
	this->shutdown();

	for (OperationBase* op : m_operations) {
		delete op;
	}
}

void Pipeline::addOperation(OperationBase* operation) {
	m_operations.push_back(operation);
}

void Pipeline::initialize() {
	for (OperationBase* op : m_operations) {
		op->initialize();
	}
}

void Pipeline::execute() const {
	for (OperationBase* op : m_operations) {
		op->execute();
	}
}

void Pipeline::shutdown() {
	for (OperationBase* op : m_operations) {
		op->shutdown();
	}
}

rendering::DebugOperation* Pipeline::getDebugOperation() {
	for (OperationBase* ob : m_operations) {
		DebugOperation* dop = dynamic_cast<DebugOperation*>(ob);
		if (dop != nullptr) {
			return dop;
		}
	}

	return nullptr;
}

ADERITE_RENDERING_NAMESPACE_END
