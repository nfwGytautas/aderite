#pragma once

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace compiler {

/**
 * @brief Enum containing possible evaluator types
 */
enum class EvaluatorType : size_t {
    Shader = 0,
    Pipeline = 1,
};

/**
 * @brief Evaluator value
 */
using EvaluatorValue = size_t;

/**
 * @brief Base class for evaluators
 */
class GraphEvaluator {
public:
    virtual ~GraphEvaluator() {}
};

} // namespace compiler
} // namespace aderite
