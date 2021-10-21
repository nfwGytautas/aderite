#include "Pipeline.hpp"

#include <bgfx/bgfx.h>
#include "aderite/utility/Log.hpp"
#include "aderite/io/RuntimeSerializables.hpp"

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
	bgfx::discard(BGFX_DISCARD_ALL);
	for (OperationBase* op : m_operations) {
		op->execute();
	}
}

void Pipeline::shutdown() {
	for (OperationBase* op : m_operations) {
		op->shutdown();
	}
}

io::SerializableType Pipeline::getType() const {
	return static_cast<io::SerializableType>(io::RuntimeSerializables::PIPELINE);
}

bool Pipeline::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	emitter << YAML::Key << "Operations" << YAML::BeginSeq;
	for (size_t i = 0; i < m_operations.size(); i++) {
		emitter << YAML::BeginMap;
		
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;

	return true;
}

bool Pipeline::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	// TODO: Deserialize
	return true;
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
