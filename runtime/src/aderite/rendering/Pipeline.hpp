#pragma once

#include <vector>

#include "aderite/io/SerializableAsset.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/operation/Forward.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Rendering pipeline object, the rendering engine loops through the pipeline
 * operations executing them one by one
 */
class Pipeline ADERITE_MIDDLEWARE_FINAL : public io::SerializableAsset {
public:
    Pipeline();
    ADERITE_MIDDLEWARE_VIRTUAL ~Pipeline();

    /**
     * @brief Adds operation to the pipeline, the pipeline takes ownership of the operation
     * NOTE: This adds the operation to the end of the operation array
     *       since ordering is important the user must ensure the correct
     *       order themselves
     * @param operation Operation to add
     */
    void addOperation(OperationBase* operation);

    /**
     * @brief Initialize pipeline and all of it's operations, allocating resources
     */
    void initialize();

    /**
     * @brief Execute the pipeline
     */
    void execute() const;

    /**
     * @brief Shutdown the pipeline and all of it's operations, freeing resources
     */
    void shutdown();

    // Inherited via SerializableObject
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    ADERITE_DEBUG_SECTION(void logPipeline() const;)
private:
    std::vector<OperationBase*> m_operations;
    PipelineState* m_state = nullptr;
};

} // namespace rendering
} // namespace aderite
