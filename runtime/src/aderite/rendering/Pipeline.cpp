#include "Pipeline.hpp"

#include <bgfx/bgfx.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/reflection/Reflector.hpp"
#include "aderite/rendering/PipelineState.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace rendering {

Pipeline::Pipeline() : m_state(new PipelineState()) {}

Pipeline::~Pipeline() {
    this->shutdown();

    for (OperationBase* op : m_operations) {
        delete op;
    }

    delete m_state;
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
    m_state->reset();
    bgfx::discard(BGFX_DISCARD_ALL);
    for (OperationBase* op : m_operations) {
        op->execute(m_state);
    }
}

void Pipeline::shutdown() {
    for (OperationBase* op : m_operations) {
        op->shutdown();
    }
}

reflection::Type Pipeline::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::PIPELINE);
}

bool Pipeline::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "Operations" << YAML::BeginSeq;
    for (size_t i = 0; i < m_operations.size(); i++) {
        serializer->writeUntrackedType(emitter, m_operations[i]);
    }
    emitter << YAML::EndSeq;

    return true;
}

bool Pipeline::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    for (const YAML::Node& node : data["Operations"]) {
        m_operations.push_back(static_cast<OperationBase*>(serializer->parseUntrackedType(node)));
    }
    return true;
}

ADERITE_DEBUG_SECTION(void Pipeline::logPipeline() const {
    LOG_TRACE("");
    LOG_TRACE("====================================================================================");
    LOG_TRACE("                                      PIPELINE                                      ");
    LOG_TRACE("====================================================================================");
    for (size_t i = 0; i < m_operations.size(); i++) {
        LOG_TRACE("[{0:02d}] Operation: {1:<40} Name: {2}", i, ::aderite::Engine::getReflector()->reflectName(m_operations[i]->getType()),
                  m_operations[i]->getName());
    }
    LOG_TRACE("");
})

} // namespace rendering
} // namespace aderite
