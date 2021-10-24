#include "OperationArray.hpp"

#include "aderiteeditor/runtime/EditorTypes.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

OperationArray::OperationArray()
{}

void OperationArray::addOperation(rendering::OperationBase* op) {
	m_operations.push_back(op);
}

size_t OperationArray::size() const {
	return m_operations.size();
}

reflection::Type OperationArray::getType() const {
	return static_cast<reflection::Type>(reflection::EditorTypes::ArrayOp);
}

bool OperationArray::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	return true;
}

bool OperationArray::deserialize(io::Serializer* serializer, const YAML::Node& data) {
	return true;
}

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
