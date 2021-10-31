#include "PipelineEvaluator.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/reflection/Reflector.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/runtime/OperationArray.hpp"

namespace aderite {
namespace compiler {

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

void PipelineEvaluator::transferToPipeline(rendering::Pipeline* pipeline) {
    if (m_pipelineConstructed) {
        LOG_ERROR("Tried to construct pipeline a second time from the same evaluator");
        return;
    }

    LOG_TRACE("Constructing pipeline");
    int id = 0;
    for (rendering::OperationBase* op : m_operations) {
        ADERITE_DEBUG_SECTION(LOG_TRACE("[{0:02d}] Operation: {1:<40} Name: {2}", id++,
                                        ::aderite::Engine::getReflector()->reflectName(op->getType()), op->getName());)

        if (dynamic_cast<editor::OperationArray*>(op) != nullptr) {
            // Filter construction helping operations
            delete op;
            continue;
        }

        pipeline->addOperation(op);
    }

    ADERITE_DEBUG_SECTION(pipeline->logPipeline();)

    m_pipelineConstructed = true;
}

} // namespace compiler
} // namespace aderite
