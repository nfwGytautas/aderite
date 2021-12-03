#pragma once

#include <string>

#include "aderite/io/SerializableObject.hpp"
#include "aderite/rendering/Forward.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Rendering operation base class
 */
class OperationBase : public io::SerializableObject {
public:
    virtual ~OperationBase() {};

    /**
     * @brief Initialize the operation
     */
    virtual void initialize() {};

    /**
     * @brief Execute the operation
     */
    virtual void execute(PipelineState* state) {};

    /**
     * @brief Free all resources
     */
    virtual void shutdown() {};
};

} // namespace rendering
} // namespace aderite
