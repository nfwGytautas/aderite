#include "OperationArray.hpp"

#include "aderiteeditor/runtime/EditorTypes.hpp"

namespace aderite {
namespace editor {

OperationArray::OperationArray() {}

void OperationArray::addOperation(rendering::OperationBase* op) {
    m_operations.push_back(op);
}

size_t OperationArray::size() const {
    return m_operations.size();
}

reflection::Type OperationArray::getType() const {
    return 0;
}

bool OperationArray::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    return true;
}

bool OperationArray::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    return true;
}

} // namespace editor
} // namespace aderite
