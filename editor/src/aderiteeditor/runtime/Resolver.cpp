#include "Resolver.hpp"

#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Project.hpp"

namespace aderite {
namespace io {

Path MappedResolver::resolve(SerializableHandle handle) {
	//return m_paths[handle];
	return  {
		shared::State::Project->getRootDir() / "Assets" / "temp.asset",
		0, 0
	};
}

void MappedResolver::store(SerializableObject* object) {
	if (m_paths.size() < object->getHandle() + 1) {
		m_paths.resize(object->getHandle() + 1, {});
	}

	// TODO: Add object name
	// TODO: Add path to file in folders
	m_paths[object->getHandle()] = {
		 shared::State::Project->getRootDir() / "Assets" / "temp.asset",
		 0, 0
	};
}

}
}
