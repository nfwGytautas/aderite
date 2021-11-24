#pragma once

#include <unordered_map>

#include "aderite/rendering/Forward.hpp"
#include "aderite/rendering/PipelineState.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief Operation used to create an entity provider
 */
class EntityProvideOperation final : public OperationBase {
public:
    // Inherited via OperationBase
    void execute(PipelineState* state) override;
    reflection::Type getType() const override;
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

public:
    /**
     * @brief Cleans the drawcall list (removes transformations)
     */
    void cleanList();

private:
    std::unordered_map<size_t, size_t> m_lookup;
    DrawCallList m_drawcalls;
};

} // namespace rendering
} // namespace aderite
