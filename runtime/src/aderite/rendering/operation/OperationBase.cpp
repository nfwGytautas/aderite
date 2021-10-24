#include "OperationBase.hpp"

namespace aderite {
namespace rendering {

void OperationBase::setName(const std::string& name) {
	m_name = name;
}

const std::string& OperationBase::getName() const {
	return m_name;
}

}
}
