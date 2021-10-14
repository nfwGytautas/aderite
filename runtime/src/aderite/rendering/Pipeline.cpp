#include "Pipeline.hpp"

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

ADERITE_RENDERING_NAMESPACE_END
