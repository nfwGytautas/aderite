#pragma once

#include <vector>

#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/operation/Forward.hpp"

#include "aderiteeditor/compiler/Forward.hpp"
#include "aderiteeditor/compiler/GraphEvaluator.hpp"
#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace compiler {

/**
 * @brief Class used to evaluate a render pipeline graph
 */
class PipelineEvaluator : public GraphEvaluator {
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
     * @brief Transfers operations to the specified pipeline
     * @param Pipeline to transfer to
     */
    void transferToPipeline(rendering::Pipeline* pipeline);

private:
    std::vector<rendering::OperationBase*> m_operations;
    bool m_pipelineConstructed = false;
};

} // namespace compiler
} // namespace aderite
