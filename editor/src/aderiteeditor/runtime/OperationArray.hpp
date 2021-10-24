#pragma once

#include <initializer_list>
#include <vector>
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

/**
 * @brief Special operation type used to provide a reference to N amount of T type operations
 * this is an editor only operation that is removed from the pipeline when the graph is being compiled
*/
class OperationArray : public rendering::OperationBase {
public:
	OperationArray();
	OperationArray(std::initializer_list<rendering::OperationBase*> operations) 
		: m_operations(operations)
	{}

	/**
	 * @brief Add operation to array
	 * @param op Operation to add
	*/
	void addOperation(rendering::OperationBase* op);

	/**
	 * @brief Returns the number of operations in array
	*/
	size_t size() const;

	rendering::OperationBase* operator[](size_t idx) {
		return m_operations[idx];
	}

	auto begin() {
		return m_operations.begin();
	}

	auto begin() const {
		return m_operations.begin();
	}

	auto end() {
		return m_operations.end();
	}

	auto end() const {
		return m_operations.end();
	}

	// Inherited via OperationBase
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
private:
	std::vector<rendering::OperationBase*> m_operations;
};

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
