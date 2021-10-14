#include "Pipeline.hpp"

#include "aderite/utility/Log.hpp"

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

ADERITE_DEBUG_SECTION
(
void Pipeline::logPipeline() const {
	LOG_TRACE("");
	LOG_TRACE("====================================================================================");
	LOG_TRACE("                                      PIPELINE                                      ");
	LOG_TRACE("====================================================================================");
	for (size_t i = 0; i < m_operations.size(); i++) {
		LOG_TRACE("[{0:02d}] Operation: {1:<32} Debug name: {2}", i, m_operations[i]->getOperationName(), m_operations[i]->getDebugName());
	}
	LOG_TRACE("");
}
)

ADERITE_RENDERING_NAMESPACE_END
